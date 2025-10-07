#include "afe_wake_word.h"
#include "core_management.h"
#include <esp_log.h>
#include "audio_service.h"
#include <opus_encoder.h>
#include "memory_management.h"
#include <sstream>

#define DETECTION_RUNNING_EVENT 1
#define ENCODE_RUNNING_EVENT 1

#define TAG "AfeWakeWord"

AfeWakeWord::AfeWakeWord()
    : afe_data_(nullptr),
      wake_word_pcm_(),
      wake_word_opus_() {

    event_group_ = xEventGroupCreate();
    encode_event_group_ = xEventGroupCreate();
}

AfeWakeWord::~AfeWakeWord() {
    // Stop encode task
    encode_task_running_ = false;
    xEventGroupSetBits(encode_event_group_, ENCODE_RUNNING_EVENT);
    
    // Use safeDestroyAfe to properly clean up AFE
    safeDestroyAfe();

    if (wake_word_encode_task_stack_ != nullptr) {
        MemoryManager::freeMemory(wake_word_encode_task_stack_);
    }

    if (wake_word_encode_task_buffer_ != nullptr) {
        MemoryManager::freeMemory(wake_word_encode_task_buffer_);
    }
    
    if (audio_encode_task_stack_ != nullptr) {
        MemoryManager::freeMemory(audio_encode_task_stack_);
    }

    if (audio_encode_task_buffer_ != nullptr) {
        MemoryManager::freeMemory(audio_encode_task_buffer_);
    }

    if (models_ != nullptr) {
        esp_srmodel_deinit(models_);
    }

    vEventGroupDelete(event_group_);
    vEventGroupDelete(encode_event_group_);
}

bool AfeWakeWord::Initialize(AudioCodec* codec, srmodel_list_t* models) {
    codec_ = codec;
    int ref_num = codec_->input_reference() ? 1 : 0;

    if (models == nullptr) {
        models_ = esp_srmodel_init("model");
    } else {
        models_ = models;
    }

    if (models_ == nullptr || models_->num == -1) {
        ESP_LOGE(TAG, "Failed to initialize wakenet model");
        return false;
    }
    for (int i = 0; i < models_->num; i++) {
        ESP_LOGI(TAG, "Model %d: %s", i, models_->model_name[i]);
        if (strstr(models_->model_name[i], ESP_WN_PREFIX) != NULL) {
            wakenet_model_ = models_->model_name[i];
            auto words = esp_srmodel_get_wake_words(models_, wakenet_model_);
            // split by ";" to get all wake words
            std::stringstream ss(words);
            std::string word;
            while (std::getline(ss, word, ';')) {
                wake_words_.push_back(word);
            }
        }
    }

    std::string input_format;
    for (int i = 0; i < codec_->input_channels() - ref_num; i++) {
        input_format.push_back('M');
    }
    for (int i = 0; i < ref_num; i++) {
        input_format.push_back('R');
    }
    
    // Log memory before AFE init
    ESP_LOGI(TAG, "Free internal before AFE: %d", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    
    // attempt create AFE with fallback sizes - ưu tiên kích thước nhỏ hơn trước
    const size_t ringbuf_sizes[] = {4096, 6144, 8192, 10240};
    bool created = false;
    for (size_t s : ringbuf_sizes) {
        if (safeCreateAfe(input_format, s)) {
            created = true;
            break;
        }
        // small delay before retry
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    if (!created) {
        ESP_LOGE(TAG, "AFE init failed after retries — disabling wake word");
        return false; // don't start tasks
    }
    
    // Kiểm tra ringbuffer đã được tạo đúng
    auto test_fetch_size = afe_iface_->get_fetch_chunksize(afe_data_);
    if (test_fetch_size == 0) {
        ESP_LOGE(TAG, "AFE ringbuffer not properly initialized (fetch_size=0)");
        safeDestroyAfe();
        return false;
    }

    // Create wake word detection task with configuration from core management
    BaseType_t detection_core = core_management_get_task_core(CORE_TASK_TYPE_WAKE_WORD_DETECTION);
    UBaseType_t detection_priority = core_management_get_task_priority(CORE_TASK_TYPE_WAKE_WORD_DETECTION);
    uint32_t detection_stack_size = core_management_get_task_stack_size(CORE_TASK_TYPE_WAKE_WORD_DETECTION);
    
    core_management_register_task(CORE_TASK_TYPE_WAKE_WORD_DETECTION, detection_stack_size);
    
    xTaskCreatePinnedToCore([](void* arg) {
        auto this_ = (AfeWakeWord*)arg;
        this_->AudioDetectionTask();
        vTaskDelete(NULL);
    }, "audio_detection", 
       detection_stack_size, 
       this, 
       detection_priority, 
       &wake_word_detection_task_, 
       detection_core);

    // Create audio encode task with configuration from core management
    BaseType_t encode_core = core_management_get_task_core(CORE_TASK_TYPE_WAKE_WORD_ENCODING); // Lấy core từ config
    UBaseType_t encode_priority = core_management_get_task_priority(CORE_TASK_TYPE_WAKE_WORD_ENCODING); // Sử dụng priority gốc cho encode task
    uint32_t encode_stack_size = core_management_get_task_stack_size(CORE_TASK_TYPE_WAKE_WORD_ENCODING);
    
    core_management_register_task(CORE_TASK_TYPE_WAKE_WORD_ENCODING, encode_stack_size);
    
    // Allocate stack and buffer for encode task
    audio_encode_task_stack_ = (StackType_t*)MemoryManager::allocatePsram(encode_stack_size);
    if (audio_encode_task_stack_ == nullptr) {
        ESP_LOGE(TAG, "Failed to allocate PSRAM for audio encode task stack");
        // Cleanup và trả về false
        safeDestroyAfe();
        return false;
    }
    
    audio_encode_task_buffer_ = (StaticTask_t*)MemoryManager::allocateInternal(sizeof(StaticTask_t));
    if (audio_encode_task_buffer_ == nullptr) {
        ESP_LOGE(TAG, "Failed to allocate internal memory for audio encode task buffer");
        // Cleanup và trả về false
        MemoryManager::freeMemory(audio_encode_task_stack_);
        audio_encode_task_stack_ = nullptr;
        safeDestroyAfe();
        return false;
    }
    
    encode_task_running_ = true;
    audio_encode_task_ = xTaskCreateStatic([](void* arg) {
        auto this_ = (AfeWakeWord*)arg;
        this_->AudioEncodeTask();
        vTaskDelete(NULL);
    }, "audio_encode", 
       encode_stack_size, 
       this, 
       encode_priority, 
       audio_encode_task_stack_, 
       audio_encode_task_buffer_);

    return true;
}

void AfeWakeWord::OnWakeWordDetected(std::function<void(const std::string& wake_word)> callback) {
    wake_word_detected_callback_ = callback;
}

void AfeWakeWord::Start() {
    xEventGroupSetBits(event_group_, DETECTION_RUNNING_EVENT);
    xEventGroupSetBits(encode_event_group_, ENCODE_RUNNING_EVENT);
}

void AfeWakeWord::Stop() {
    xEventGroupClearBits(event_group_, DETECTION_RUNNING_EVENT);
    xEventGroupClearBits(encode_event_group_, ENCODE_RUNNING_EVENT);
    // Không cần reset buffer nữa vì chúng ta sẽ dùng safeDestroyAfe khi cần
}

void AfeWakeWord::Feed(const std::vector<int16_t>& data) {
    // Kiểm tra afe_data_ và afe_iface_ trước khi sử dụng
    if (afe_data_ == nullptr || afe_iface_ == nullptr) {
        ESP_LOGE(TAG, "AFE interface or data is null, cannot feed data");
        return;
    }
    afe_iface_->feed(afe_data_, data.data());
}

size_t AfeWakeWord::GetFeedSize() {
    // Kiểm tra afe_data_ và afe_iface_ trước khi sử dụng
    if (afe_data_ == nullptr || afe_iface_ == nullptr) {
        ESP_LOGE(TAG, "AFE interface or data is null, cannot get feed size");
        return 0;
    }
    return afe_iface_->get_feed_chunksize(afe_data_);
}

void AfeWakeWord::AudioDetectionTask() {
    // wait until AFE ready với timeout
    int wait_count = 0;
    const int max_wait_count = 100; // 5 seconds timeout (50ms * 100)
    while (afe_iface_ == nullptr || afe_data_ == nullptr) {
        ESP_LOGW(TAG, "Waiting for AFE to be ready...");
        vTaskDelay(pdMS_TO_TICKS(50));
        wait_count++;
        if (wait_count >= max_wait_count) {
            ESP_LOGE(TAG, "Timeout waiting for AFE to be ready");
            vTaskDelete(NULL);
            return;
        }
    }

    auto fetch_size = afe_iface_->get_fetch_chunksize(afe_data_);
    auto feed_size = afe_iface_->get_feed_chunksize(afe_data_);
    ESP_LOGI(TAG, "Audio detection task started, feed size: %d fetch size: %d",
        feed_size, fetch_size);
        
    // Kiểm tra fetch_size để đảm bảo ringbuffer đã được tạo đúng cách
    if (fetch_size == 0) {
        ESP_LOGE(TAG, "AFE ringbuffer is not properly initialized, fetch size is 0");
        vTaskDelete(NULL);
        return;
    }

    int ringbuffer_full_count = 0;
    int emergency_reset_count = 0;  // Biến đếm cho emergency reset
    int64_t last_fetch_time = esp_timer_get_time();
    
    while (true) {
        xEventGroupWaitBits(event_group_, DETECTION_RUNNING_EVENT, pdFALSE, pdTRUE, portMAX_DELAY);

        // guard before calling AFE
        if (afe_iface_ == nullptr || afe_data_ == nullptr) {
            ESP_LOGW(TAG, "AFE not available, skipping fetch");
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }
        
        // Kiểm tra thêm một lần nữa fetch_size trước khi fetch
        auto current_fetch_size = afe_iface_->get_fetch_chunksize(afe_data_);
        if (current_fetch_size == 0) {
            ESP_LOGE(TAG, "AFE ringbuffer is not properly initialized during detection task, fetch size is 0");
            vTaskDelay(pdMS_TO_TICKS(100)); // Chờ một chút trước khi kiểm tra lại
            continue;
        }

        auto res = afe_iface_->fetch_with_delay(afe_data_, portMAX_DELAY);
        // Kiểm tra res == nullptr TRƯỚC KHI truy cập res->ret_value
        if (res == nullptr) {
            ESP_LOGW(TAG, "AFE fetch returned nullptr");
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }
        
        if (res->ret_value == ESP_FAIL) {
            ringbuffer_full_count++;
            emergency_reset_count++;
            ESP_LOGW(TAG, "AFE fetch error, code: %d, count: %d", res->ret_value, ringbuffer_full_count);
            
            if (ringbuffer_full_count >= 3) {
                ESP_LOGW(TAG, "Resetting AFE buffer due to continuous error state");
                afe_iface_->reset_buffer(afe_data_);
                ringbuffer_full_count = 0;
            }
            
            // Emergency reset sau 10 lỗi liên tiếp
            if (emergency_reset_count >= 10) {
                ESP_LOGW(TAG, "Emergency reset AFE due to continuous errors");
                safeDestroyAfe();

                bool reinit_ok = false;
                for (int attempt = 0; attempt < 3; ++attempt) {
                    vTaskDelay(pdMS_TO_TICKS(200 * (attempt + 1)));
                    if (safeCreateAfe("M", 4096)) { // try smallest ringbuf first
                        reinit_ok = true;
                        break;
                    }
                }
                if (!reinit_ok) {
                    ESP_LOGE(TAG, "AFE reinit failed after attempts");
                    // possibly notify upper layer or disable detection
                } else {
                    ESP_LOGI(TAG, "AFE emergency reset completed");
                    xEventGroupSetBits(event_group_, DETECTION_RUNNING_EVENT);
                }
                ringbuffer_full_count = 0;
                emergency_reset_count = 0;
            }
            continue;
        }
        
        ringbuffer_full_count = 0;
        emergency_reset_count = 0;  // Reset emergency counter khi fetch thành công
        last_fetch_time = esp_timer_get_time();

        // Store the wake word data for encoding in separate task
        {
            std::lock_guard<std::mutex> lock(encode_queue_mutex_);
            encode_queue_.emplace_back(std::vector<int16_t>(res->data, res->data + res->data_size / sizeof(int16_t)));
            encode_queue_cv_.notify_all();
        }

        if (res->wakeup_state == WAKENET_DETECTED) {
            Stop();
            last_detected_wake_word_ = wake_words_[res->wakenet_model_index - 1];

            if (wake_word_detected_callback_) {
                wake_word_detected_callback_(last_detected_wake_word_);
            }
        }
        
        // Monitor fetch interval
        int64_t current_time = esp_timer_get_time();
        int64_t time_diff_ms = (current_time - last_fetch_time) / 1000;
        if (time_diff_ms > 50) {
            ESP_LOGW(TAG, "Long time between fetches: %lld ms", time_diff_ms);
        }
        
        // Thêm delay nhỏ để giảm tần suất xử lý
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}

void AfeWakeWord::AudioEncodeTask() {
    ESP_LOGI(TAG, "Audio encode task started");
    
    auto encoder = std::make_unique<OpusEncoderWrapper>(16000, 1, OPUS_FRAME_DURATION_MS);
    encoder->SetComplexity(0); // Fastest encoding
    
    while (encode_task_running_) {
        std::unique_lock<std::mutex> lock(encode_queue_mutex_);
        encode_queue_cv_.wait(lock, [this]() {
            return !encode_queue_.empty() || !encode_task_running_;
        });
        
        if (!encode_task_running_) {
            break;
        }
        
        if (!encode_queue_.empty()) {
            auto pcm = std::move(encode_queue_.front());
            encode_queue_.pop_front();
            lock.unlock();
            
            // Encode the PCM data
            encoder->Encode(std::move(pcm), [this](std::vector<uint8_t>&& opus) {
                std::lock_guard<std::mutex> lock(wake_word_mutex_);
                wake_word_opus_.emplace_back(std::move(opus));
                wake_word_cv_.notify_all();
            });
        }
    }
    
    ESP_LOGI(TAG, "Audio encode task stopped");
}

void AfeWakeWord::StoreWakeWordData(const int16_t* data, size_t samples) {
    // This function is now handled by the separate encode task
    // Keep minimal implementation for compatibility
    wake_word_pcm_.emplace_back(std::vector<int16_t>(data, data + samples));
    while (wake_word_pcm_.size() > 2000 / 30) {
        wake_word_pcm_.pop_front();
    }
}

void AfeWakeWord::EncodeWakeWordData() {
    uint32_t encode_stack_size = core_management_get_task_stack_size(CORE_TASK_TYPE_WAKE_WORD_ENCODING);
    wake_word_opus_.clear();
    
    if (wake_word_encode_task_stack_ == nullptr) {
        wake_word_encode_task_stack_ = (StackType_t*)MemoryManager::allocatePsram(encode_stack_size);
        assert(wake_word_encode_task_stack_ != nullptr);
    }
    if (wake_word_encode_task_buffer_ == nullptr) {
        wake_word_encode_task_buffer_ = (StaticTask_t*)MemoryManager::allocateInternal(sizeof(StaticTask_t));
        assert(wake_word_encode_task_buffer_ != nullptr);
    }

    UBaseType_t priority = core_management_get_task_priority(CORE_TASK_TYPE_WAKE_WORD_ENCODING);
    core_management_register_task(CORE_TASK_TYPE_WAKE_WORD_ENCODING, encode_stack_size);
    
    wake_word_encode_task_ = xTaskCreateStatic([](void* arg) {
        auto this_ = (AfeWakeWord*)arg;
        {
            auto start_time = esp_timer_get_time();
            auto encoder = std::make_unique<OpusEncoderWrapper>(16000, 1, OPUS_FRAME_DURATION_MS);
            encoder->SetComplexity(0);

            int packets = 0;
            for (auto& pcm: this_->wake_word_pcm_) {
                encoder->Encode(std::move(pcm), [this_](std::vector<uint8_t>&& opus) {
                    std::lock_guard<std::mutex> lock(this_->wake_word_mutex_);
                    this_->wake_word_opus_.emplace_back(std::move(opus));
                    this_->wake_word_cv_.notify_all();
                });
                packets++;
            }
            this_->wake_word_pcm_.clear();

            auto end_time = esp_timer_get_time();
            ESP_LOGI(TAG, "Encode wake word opus %d packets in %ld ms", packets, (long)((end_time - start_time) / 1000));

            std::lock_guard<std::mutex> lock(this_->wake_word_mutex_);
            this_->wake_word_opus_.push_back(std::vector<uint8_t>());
            this_->wake_word_cv_.notify_all();
        }
        vTaskDelete(NULL);
    }, "encode_wake_word", 
       encode_stack_size, 
       this, 
       priority, 
       wake_word_encode_task_stack_, 
       wake_word_encode_task_buffer_);
}

bool AfeWakeWord::GetWakeWordOpus(std::vector<uint8_t>& opus) {
    std::unique_lock<std::mutex> lock(wake_word_mutex_);
    wake_word_cv_.wait(lock, [this]() {
        return !wake_word_opus_.empty();
    });
    opus.swap(wake_word_opus_.front());
    wake_word_opus_.pop_front();
    return !opus.empty();
}

void AfeWakeWord::safeDestroyAfe() {
    // Pause detection so task won't use AFE while we destroy
    xEventGroupClearBits(event_group_, DETECTION_RUNNING_EVENT);
    // small grace period
    vTaskDelay(pdMS_TO_TICKS(50));
    if (afe_data_) {
        afe_iface_->destroy(afe_data_);
        afe_data_ = nullptr;
    }
    afe_iface_ = nullptr;
}

bool AfeWakeWord::safeCreateAfe(const std::string& input_format, size_t ringbuf_size) {
    afe_config_t* afe_config = afe_config_init(input_format.c_str(), models_, AFE_TYPE_SR, AFE_MODE_HIGH_PERF);
    if (!afe_config) {
        ESP_LOGE(TAG, "afe_config_init failed");
        return false;
    }

    // configure
    afe_config->aec_init = codec_->input_reference();
    afe_config->aec_mode = AEC_MODE_SR_HIGH_PERF;
    afe_config->agc_init = true;
    afe_config->agc_mode = AFE_AGC_MODE_WAKENET;
    afe_config->agc_compression_gain_db = 9;
    afe_config->agc_target_level_dbfs = 3;
    afe_config->afe_perferred_core = 1;
    afe_config->afe_perferred_priority = 18;
    afe_config->afe_ringbuf_size = ringbuf_size;
    afe_config->memory_alloc_mode = AFE_MEMORY_ALLOC_MORE_PSRAM;
    afe_config->afe_linear_gain = 1.0f;

    afe_iface_ = esp_afe_handle_from_config(afe_config);
    if (!afe_iface_) {
        ESP_LOGE(TAG, "esp_afe_handle_from_config failed");
        return false;
    }

    afe_data_ = afe_iface_->create_from_config(afe_config);
    if (!afe_data_) {
        ESP_LOGE(TAG, "afe create_from_config failed (ringbuf_size=%d)", (int)ringbuf_size);
        afe_iface_ = nullptr;
        return false;
    }

    // Kiểm tra xem ringbuffer có được tạo thành công không bằng cách thử gọi một hàm sử dụng ringbuffer
    // Nếu ringbuffer không được tạo do hết bộ nhớ, việc gọi hàm sẽ thất bại
    auto fetch_size = afe_iface_->get_fetch_chunksize(afe_data_);
    if (fetch_size == 0) {
        ESP_LOGE(TAG, "AFE ringbuffer initialization failed, fetch size is 0 (ringbuf_size=%d)", (int)ringbuf_size);
        afe_iface_->destroy(afe_data_);
        afe_data_ = nullptr;
        afe_iface_ = nullptr;
        return false;
    }

    ESP_LOGI(TAG, "AFE created OK ringbuf_size=%d", (int)ringbuf_size);
    return true;
}
