#include "esp32_camera.h"
#include "mcp_server.h"
#include "display.h"
#include "board.h"
#include "system_info.h"
#include "lvgl_display.h"
#include "jpg/image_to_jpeg.h"

#include <esp_log.h>
#include <esp_heap_caps.h>
#include <cstring>

#define TAG "Esp32Camera"

Esp32Camera::Esp32Camera(const camera_config_t& config) {
    // camera init
    esp_err_t err = esp_camera_init(&config); // Định cấu hình các tham số đã xác định ở trên
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
        return;
    }

    sensor_t *s = esp_camera_sensor_get(); // Lấy mẫu camera
    if (s->id.PID == GC0308_PID) {
        s->set_hmirror(s, 0);  // Tại đây điều khiển gương camera ghi 1 gương ghi 0 không gương
    }
}

Esp32Camera::~Esp32Camera() {
    if (fb_) {
        esp_camera_fb_return(fb_);
        fb_ = nullptr;
    }
    esp_camera_deinit();
}

void Esp32Camera::SetExplainUrl(const std::string& url, const std::string& token) {
    explain_url_ = url;
    explain_token_ = token;
}

bool Esp32Camera::Capture() {
    if (encoder_thread_.joinable()) {
        encoder_thread_.join();
    }

    auto start_time = esp_timer_get_time();
    int frames_to_get = 2;
    // Try to get a stable frame
    for (int i = 0; i < frames_to_get; i++) {
        if (fb_ != nullptr) {
            esp_camera_fb_return(fb_);
        }
        fb_ = esp_camera_fb_get();
        if (fb_ == nullptr) {
            ESP_LOGE(TAG, "Camera capture failed");
            return false;
        }
    }
    auto end_time = esp_timer_get_time();
    ESP_LOGI(TAG, "Camera captured %d frames in %d ms", frames_to_get, int((end_time - start_time) / 1000));

    // Hiển thị hình ảnh xem trước
    auto display = dynamic_cast<LvglDisplay*>(Board::GetInstance().GetDisplay());
    if (display != nullptr) {
        auto data = (uint8_t*)heap_caps_malloc(fb_->len, MALLOC_CAP_SPIRAM);
        if (data == nullptr) {
            ESP_LOGE(TAG, "Failed to allocate memory for preview image");
            return false;
        }

        auto src = (uint16_t*)fb_->buf;
        auto dst = (uint16_t*)data;
        size_t pixel_count = fb_->len / 2;
        for (size_t i = 0; i < pixel_count; i++) {
            // Hoán đổi byte trong mỗi từ 16 bit
            dst[i] = __builtin_bswap16(src[i]);
        }

        auto image = std::make_unique<LvglAllocatedImage>(data, fb_->len, fb_->width, fb_->height, fb_->width * 2, LV_COLOR_FORMAT_RGB565);
        display->SetPreviewImage(std::move(image));
    }
    return true;
}

bool Esp32Camera::SetHMirror(bool enabled) {
    sensor_t *s = esp_camera_sensor_get();
    if (s == nullptr) {
        ESP_LOGE(TAG, "Failed to get camera sensor");
        return false;
    }
    
    esp_err_t err = s->set_hmirror(s, enabled);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set horizontal mirror: %d", err);
        return false;
    }
    
    ESP_LOGI(TAG, "Camera horizontal mirror set to: %s", enabled ? "enabled" : "disabled");
    return true;
}

bool Esp32Camera::SetVFlip(bool enabled) {
    sensor_t *s = esp_camera_sensor_get();
    if (s == nullptr) {
        ESP_LOGE(TAG, "Failed to get camera sensor");
        return false;
    }
    
    esp_err_t err = s->set_vflip(s, enabled);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set vertical flip: %d", err);
        return false;
    }
    
    ESP_LOGI(TAG, "Camera vertical flip set to: %s", enabled ? "enabled" : "disabled");
    return true;
}

/**
 * @brief Gửi hình ảnh được chụp bởi camera đến một máy chủ từ xa để phân tích và giải thích bằng AI
 * 
 * Chức năng này mã hóa hình ảnh trong bộ đệm camera hiện tại sang định dạng JPEG và gửi nó qua yêu cầu HTTP POST
 * ở định dạng multipart/form-data đến máy chủ giải thích được chỉ định. Máy chủ sẽ thực hiện phân tích AI trên hình ảnh
 * dựa trên câu hỏi được cung cấp và trả về kết quả.
 * 
 * Các tính năng triển khai:
 * - Sử dụng một luồng riêng để mã hóa JPEG, tách biệt khỏi luồng chính
 * - Sử dụng mã hóa truyền tải theo khối (chunked transfer encoding) để tối ưu hóa việc sử dụng bộ nhớ
 * - Đạt được đồng bộ hóa dữ liệu giữa luồng mã hóa và luồng gửi thông qua cơ chế hàng đợi
 * - Hỗ trợ cấu hình tiêu đề HTTP cho ID thiết bị, ID máy khách và mã thông báo xác thực
 * 
 * @param question Câu hỏi sẽ được hỏi AI về hình ảnh, sẽ được gửi dưới dạng trường biểu mẫu
 * @return std::string Chuỗi phản hồi định dạng JSON được trả về bởi máy chủ
 *         Chứa kết quả phân tích AI khi thành công, thông tin lỗi khi thất bại
 *         Ví dụ định dạng: {"success": true, "result": "Kết quả phân tích"}
 *                          {"success": false, "message": "Thông báo lỗi"}
 * 
 * @note Phải gọi SetExplainUrl() để đặt URL máy chủ trước khi gọi chức năng này
 * @note Chức năng sẽ đợi luồng mã hóa trước đó hoàn thành trước khi bắt đầu xử lý mới
 * @warning Nếu bộ đệm camera trống hoặc kết nối mạng thất bại, một thông báo lỗi sẽ được trả về
 */
std::string Esp32Camera::Explain(const std::string& question) {
    if (explain_url_.empty()) {
        throw std::runtime_error("Image explain URL or token is not set");
    }

    // Tạo một hàng đợi JPEG cục bộ, 40 mục nhập có thể lưu trữ khoảng 512 * 40 = 20480 byte dữ liệu JPEG
    QueueHandle_t jpeg_queue = xQueueCreate(40, sizeof(JpegChunk));
    if (jpeg_queue == nullptr) {
        ESP_LOGE(TAG, "Failed to create JPEG queue");
        throw std::runtime_error("Failed to create JPEG queue");
    }

    // We spawn a thread to encode the image to JPEG using optimized encoder (cost about 500ms and 8KB SRAM)
    encoder_thread_ = std::thread([this, jpeg_queue]() {
        image_to_jpeg_cb(fb_->buf, fb_->len, fb_->width, fb_->height, fb_->format, 80,
            [](void* arg, size_t index, const void* data, size_t len) -> size_t {
            auto jpeg_queue = (QueueHandle_t)arg;
            JpegChunk chunk = {
                .data = (uint8_t*)heap_caps_aligned_alloc(16, len, MALLOC_CAP_SPIRAM),
                .len = len
            };
            memcpy(chunk.data, data, len);
            xQueueSend(jpeg_queue, &chunk, portMAX_DELAY);
            return len;
        }, jpeg_queue);
    });

    auto network = Board::GetInstance().GetNetwork();
    auto http = network->CreateHttp(3);
    // 构造multipart/form-data请求体
    std::string boundary = "----ESP32_CAMERA_BOUNDARY";

    // Cấu hình máy khách HTTP, sử dụng mã hóa truyền tải theo khối
    http->SetHeader("Device-Id", SystemInfo::GetMacAddress().c_str());
    http->SetHeader("Client-Id", Board::GetInstance().GetUuid().c_str());
    if (!explain_token_.empty()) {
        http->SetHeader("Authorization", "Bearer " + explain_token_);
    }
    http->SetHeader("Content-Type", "multipart/form-data; boundary=" + boundary);
    http->SetHeader("Transfer-Encoding", "chunked");
    if (!http->Open("POST", explain_url_)) {
        ESP_LOGE(TAG, "Failed to connect to explain URL");
        // Clear the queue
        encoder_thread_.join();
        JpegChunk chunk;
        while (xQueueReceive(jpeg_queue, &chunk, portMAX_DELAY) == pdPASS) {
            if (chunk.data != nullptr) {
                heap_caps_free(chunk.data);
            } else {
                break;
            }
        }
        vQueueDelete(jpeg_queue);
        throw std::runtime_error("Failed to connect to explain URL");
    }
    
    {
        // Khối đầu tiên: trường câu hỏi
        std::string question_field;
        question_field += "--" + boundary + "\r\n";
        question_field += "Content-Disposition: form-data; name=\"question\"\r\n";
        question_field += "\r\n";
        question_field += question + "\r\n";
        http->Write(question_field.c_str(), question_field.size());
    }
    {
        // Khối thứ hai: tiêu đề trường tệp
        std::string file_header;
        file_header += "--" + boundary + "\r\n";
        file_header += "Content-Disposition: form-data; name=\"file\"; filename=\"camera.jpg\"\r\n";
        file_header += "Content-Type: image/jpeg\r\n";
        file_header += "\r\n";
        http->Write(file_header.c_str(), file_header.size());
    }

    // Khối thứ ba: dữ liệu JPEG
    size_t total_sent = 0;
    while (true) {
        JpegChunk chunk;
        if (xQueueReceive(jpeg_queue, &chunk, portMAX_DELAY) != pdPASS) {
            ESP_LOGE(TAG, "Failed to receive JPEG chunk");
            break;
        }
        if (chunk.data == nullptr) {
            break; // The last chunk
        }
        http->Write((const char*)chunk.data, chunk.len);
        total_sent += chunk.len;
        heap_caps_free(chunk.data);
    }
    // Wait for the encoder thread to finish
    encoder_thread_.join();
    // 清理队列
    vQueueDelete(jpeg_queue);

    {
        // 第四块：multipart尾部
        std::string multipart_footer;
        multipart_footer += "\r\n--" + boundary + "--\r\n";
        http->Write(multipart_footer.c_str(), multipart_footer.size());
    }
    // 结束块
    http->Write("", 0);

    if (http->GetStatusCode() != 200) {
        ESP_LOGE(TAG, "Failed to upload photo, status code: %d", http->GetStatusCode());
        throw std::runtime_error("Failed to upload photo");
    }

    std::string result = http->ReadAll();
    http->Close();

    // Get remain task stack size
    size_t remain_stack_size = uxTaskGetStackHighWaterMark(nullptr);
    ESP_LOGI(TAG, "Explain image size=%dx%d, compressed size=%d, remain stack size=%d, question=%s\n%s",
        fb_->width, fb_->height, total_sent, remain_stack_size, question.c_str(), result.c_str());
    return result;
}
