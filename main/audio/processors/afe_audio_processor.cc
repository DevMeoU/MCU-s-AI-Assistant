#include "afe_audio_processor.h"
#include "core_management.h"
#include <esp_log.h>

// Chỉ định TAG cho logging
#define TAG "AfeAudioProcessor"

// Giảm mức độ log không cần thiết
#ifdef CONFIG_AFE_DEBUG_LOG
#define AFE_LOGD ESP_LOGD
#else
#define AFE_LOGD(...) do {} while(0)  // Disable debug logs
#endif

AfeAudioProcessor::AfeAudioProcessor() : event_group_(xEventGroupCreate()), afe_data_(nullptr) {
}

void AfeAudioProcessor::Initialize(AudioCodec* codec, int frame_duration_ms, srmodel_list_t* models_list) {
    codec_ = codec;
    frame_samples_ = frame_duration_ms * 16000 / 1000;

    // Pre-allocate output buffer capacity
    output_buffer_.reserve(frame_samples_ * 2);  // Tăng dung lượng buffer dự trữ

    int ref_num = codec_->input_reference() ? 1 : 0;

    std::string input_format;
    for (int i = 0; i < codec_->input_channels() - ref_num; i++) {
        input_format.push_back('M');
    }
    for (int i = 0; i < ref_num; i++) {
        input_format.push_back('R');
    }

    srmodel_list_t *models;
    if (models_list == nullptr) {
        models = esp_srmodel_init("model");
    } else {
        models = models_list;
    }

    char* ns_model_name = esp_srmodel_filter(models, ESP_NSNET_PREFIX, NULL);
    char* vad_model_name = esp_srmodel_filter(models, ESP_VADN_PREFIX, NULL);
    
    // Log memory before AFE init - chỉ khi cần thiết
    AFE_LOGD(TAG, "Free internal before AFE: %d", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    
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
        ESP_LOGE(TAG, "AFE init failed after retries — disabling audio processor");
        return; // don't start tasks
    }
    
    // Kiểm tra ringbuffer đã được tạo đúng
    auto test_fetch_size = afe_iface_->get_fetch_chunksize(afe_data_);
    if (test_fetch_size == 0) {
        ESP_LOGE(TAG, "AFE ringbuffer not properly initialized (fetch_size=0)");
        safeDestroyAfe();
        return;
    }
    
    // Tạo task audio processor với cấu hình từ core management
    BaseType_t core = core_management_get_task_core(CORE_TASK_TYPE_AUDIO_PROCESSOR);
    UBaseType_t priority = core_management_get_task_priority(CORE_TASK_TYPE_AUDIO_PROCESSOR);
    uint32_t stack_size = core_management_get_task_stack_size(CORE_TASK_TYPE_AUDIO_PROCESSOR);
    
    core_management_register_task(CORE_TASK_TYPE_AUDIO_PROCESSOR, stack_size);
    
    xTaskCreatePinnedToCore([](void* arg) {
        auto this_ = (AfeAudioProcessor*)arg;
        this_->AudioProcessorTask();
        vTaskDelete(NULL);
    }, "audio_communication", 
       stack_size, 
       this, 
       priority, 
       NULL, 
       core);
}

AfeAudioProcessor::~AfeAudioProcessor() {
    // Use safeDestroyAfe to properly clean up AFE
    safeDestroyAfe();
    vEventGroupDelete(event_group_);
}

void AfeAudioProcessor::Feed(std::vector<int16_t>&& data) {
    // Kiểm tra afe_data_ và afe_iface_ trước khi sử dụng
    if (afe_data_ == nullptr || afe_iface_ == nullptr) {
        ESP_LOGE(TAG, "AFE interface or data is null, cannot feed data");
        return;
    }
    afe_iface_->feed(afe_data_, data.data());
}

size_t AfeAudioProcessor::GetFeedSize() {
    // Kiểm tra afe_data_ và afe_iface_ trước khi sử dụng
    if (afe_data_ == nullptr || afe_iface_ == nullptr) {
        ESP_LOGE(TAG, "AFE interface or data is null, cannot get feed size");
        return 0;
    }
    return afe_iface_->get_feed_chunksize(afe_data_);
}

void AfeAudioProcessor::Start() {
    xEventGroupSetBits(event_group_, PROCESSOR_RUNNING);
}

void AfeAudioProcessor::Stop() {
    xEventGroupClearBits(event_group_, PROCESSOR_RUNNING);
    // Không cần reset buffer nữa vì chúng ta sẽ dùng safeDestroyAfe khi cần
}

bool AfeAudioProcessor::IsRunning() {
    return xEventGroupGetBits(event_group_) & PROCESSOR_RUNNING;
}

void AfeAudioProcessor::OnOutput(std::function<void(std::vector<int16_t>&& data)> callback) {
    output_callback_ = callback;
}

void AfeAudioProcessor::OnVadStateChange(std::function<void(bool speaking)> callback) {
    vad_state_change_callback_ = callback;
}

void AfeAudioProcessor::AudioProcessorTask() {
    // wait until AFE ready với timeout
    int wait_count = 0;
    const int max_wait_count = 100; // 5 seconds timeout (50ms * 100)
    while (afe_iface_ == nullptr || afe_data_ == nullptr) {
        // Chỉ log khi cần thiết
        AFE_LOGD(TAG, "Waiting for AFE to be ready...");
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
    // Chỉ log khi cần thiết
    AFE_LOGD(TAG, "Audio communication task started, feed size: %d fetch size: %d",
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
        xEventGroupWaitBits(event_group_, PROCESSOR_RUNNING, pdFALSE, pdTRUE, portMAX_DELAY);
        
        // guard before calling AFE
        if (afe_iface_ == nullptr || afe_data_ == nullptr) {
            // Chỉ log khi cần thiết
            AFE_LOGD(TAG, "AFE not available, skipping fetch");
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }
        
        // Kiểm tra thêm một lần nữa fetch_size trước khi fetch
        auto current_fetch_size = afe_iface_->get_fetch_chunksize(afe_data_);
        if (current_fetch_size == 0) {
            ESP_LOGE(TAG, "AFE ringbuffer is not properly initialized during processor task, fetch size is 0");
            vTaskDelay(pdMS_TO_TICKS(100)); // Chờ một chút trước khi kiểm tra lại
            continue;
        }

        auto res = afe_iface_->fetch_with_delay(afe_data_, portMAX_DELAY);
        if ((xEventGroupGetBits(event_group_) & PROCESSOR_RUNNING) == 0) {
            continue;
        }
        
        // Kiểm tra res == nullptr TRƯỚC KHI truy cập res->ret_value
        if (res == nullptr) {
            // Chỉ log khi cần thiết
            AFE_LOGD(TAG, "AFE fetch returned nullptr");
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }
        
        // Check for ringbuffer full errors
        if (res->ret_value == ESP_FAIL) {
            // Kiểm tra nếu ringbuffer đầy dựa trên mã lỗi cụ thể
            ringbuffer_full_count++;
            emergency_reset_count++;
            // Chỉ log khi cần thiết
            AFE_LOGD(TAG, "AFE fetch error, code: %d, count: %d", res->ret_value, ringbuffer_full_count);
            
            // Reset buffer sau 3 lần lỗi
            if (ringbuffer_full_count >= 3) {
                ESP_LOGW(TAG, "Resetting AFE buffer due to continuous error state");
                afe_iface_->reset_buffer(afe_data_);
                ringbuffer_full_count = 0;
                
                // Thêm delay ngắn để buffer ổn định
                vTaskDelay(pdMS_TO_TICKS(5));
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
                    // possibly notify upper layer or disable processing
                } else {
                    ESP_LOGI(TAG, "AFE emergency reset completed");
                    xEventGroupSetBits(event_group_, PROCESSOR_RUNNING);
                }
                ringbuffer_full_count = 0;
                emergency_reset_count = 0;
            }
            continue;
        }
        
        // Reset counter khi fetch thành công
        ringbuffer_full_count = 0;
        emergency_reset_count = 0;  // Reset emergency counter khi fetch thành công
        last_fetch_time = esp_timer_get_time();

        // VAD state change
        if (vad_state_change_callback_) {
            if (res->vad_state == VAD_SPEECH && !is_speaking_) {
                is_speaking_ = true;
                vad_state_change_callback_(true);
            } else if (res->vad_state == VAD_SILENCE && is_speaking_) {
                is_speaking_ = false;
                vad_state_change_callback_(false);
            }
        }

        if (output_callback_) {
            size_t samples = res->data_size / sizeof(int16_t);
            
            // Add data to buffer
            output_buffer_.insert(output_buffer_.end(), res->data, res->data + samples);
            
            // Output complete frames when buffer has enough data
            while (output_buffer_.size() >= frame_samples_) {
                if (output_buffer_.size() == frame_samples_) {
                    // If buffer size equals frame size, move the entire buffer
                    output_callback_(std::move(output_buffer_));
                    output_buffer_.clear();
                    output_buffer_.reserve(frame_samples_ * 2);  // Maintain buffer capacity
                } else {
                    // If buffer size exceeds frame size, copy one frame and remove it
                    output_callback_(std::vector<int16_t>(output_buffer_.begin(), output_buffer_.begin() + frame_samples_));
                    output_buffer_.erase(output_buffer_.begin(), output_buffer_.begin() + frame_samples_);
                }
            }
        }
        
        // Monitor fetch interval - chỉ khi cần thiết
        int64_t current_time = esp_timer_get_time();
        int64_t time_diff_ms = (current_time - last_fetch_time) / 1000;
        if (time_diff_ms > 50) {  // Warning if time between fetches > 50ms
            // Chỉ log khi cần thiết
            AFE_LOGD(TAG, "Long time between fetches: %lld ms", time_diff_ms);
        }
    }
}

void AfeAudioProcessor::EnableDeviceAec(bool enable) {
    if (enable) {
#if CONFIG_USE_DEVICE_AEC
        afe_iface_->disable_vad(afe_data_);
        afe_iface_->enable_aec(afe_data_);
#else
        ESP_LOGE(TAG, "Device AEC is not supported");
#endif
    } else {
        afe_iface_->disable_aec(afe_data_);
        afe_iface_->enable_vad(afe_data_);
    }
}

void AfeAudioProcessor::safeDestroyAfe() {
    // Pause processing so task won't use AFE while we destroy
    xEventGroupClearBits(event_group_, PROCESSOR_RUNNING);
    // small grace period
    vTaskDelay(pdMS_TO_TICKS(50));
    if (afe_data_) {
        afe_iface_->destroy(afe_data_);
        afe_data_ = nullptr;
    }
    afe_iface_ = nullptr;
}

bool AfeAudioProcessor::safeCreateAfe(const std::string& input_format, size_t ringbuf_size) {
    afe_config_t* afe_config = afe_config_init(input_format.c_str(), NULL, AFE_TYPE_VC, AFE_MODE_HIGH_PERF);
    if (!afe_config) {
        ESP_LOGE(TAG, "afe_config_init failed");
        return false;
    }

    // configure
    afe_config->aec_init = true;
    afe_config->aec_mode = AEC_MODE_VOIP_HIGH_PERF;
    afe_config->vad_init = true;
    afe_config->vad_mode = VAD_MODE_0;
    afe_config->agc_init = true;
    afe_config->agc_mode = AFE_AGC_MODE_WEBRTC;
    afe_config->agc_compression_gain_db = 9;
    afe_config->agc_target_level_dbfs = 3;
    afe_config->afe_perferred_core = 1;
    afe_config->afe_perferred_priority = 20;
    afe_config->afe_ringbuf_size = ringbuf_size;
    afe_config->memory_alloc_mode = AFE_MEMORY_ALLOC_MORE_PSRAM;
    afe_config->afe_linear_gain = 1.0f;
    afe_config->vad_min_noise_ms = 1000;
    afe_config->vad_min_speech_ms = 128;
    afe_config->vad_delay_ms = 128;

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

    // Chỉ log khi cần thiết
    AFE_LOGD(TAG, "AFE created OK ringbuf_size=%d", (int)ringbuf_size);
    return true;
}