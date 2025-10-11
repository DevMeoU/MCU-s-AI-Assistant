#include "esp32_music.h"
#include "board.h"
#include "system_info.h"
#include "audio/audio_codec.h"
#include "application.h"
#include "protocols/protocol.h"
#include "display/display.h"

#include <esp_log.h>
#include <esp_heap_caps.h>
#include <esp_pthread.h>
#include <esp_timer.h>
#include <mbedtls/sha256.h>
#include <cJSON.h>
#include <cstring>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <cctype>  // cho hàm isdigit
#include <thread>   // cho so sánh ID luồng
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TAG "Esp32Music"

// ========== Các hàm xác thực ESP32 đơn giản ==========

/**
 * @brief Lấy địa chỉ MAC của thiết bị
 * @return Chuỗi địa chỉ MAC
 */
static std::string get_device_mac() {
    return SystemInfo::GetMacAddress();
}

/**
 * @brief Lấy ID chip của thiết bị
 * @return Chuỗi ID chip
 */
static std::string get_device_chip_id() {
    // Sử dụng địa chỉ MAC làm ID chip, loại bỏ dấu phân cách hai chấm
    std::string mac = SystemInfo::GetMacAddress();
    // Loại bỏ tất cả dấu hai chấm
    mac.erase(std::remove(mac.begin(), mac.end(), ':'), mac.end());
    return mac;
}

/**
 * @brief Tạo khóa động
 * @param timestamp Dấu thời gian
 * @return Chuỗi khóa động
 */
static std::string generate_dynamic_key(int64_t timestamp) {
    // Khóa bí mật (vui lòng sửa đổi để khớp với phía máy chủ)
    const std::string secret_key = "your-esp32-secret-key-2024";
    
    // Lấy thông tin thiết bị
    std::string mac = get_device_mac();
    std::string chip_id = get_device_chip_id();
    
    // Kết hợp dữ liệu: MAC:ID chip:Dấu thời gian:Khóa
    std::string data = mac + ":" + chip_id + ":" + std::to_string(timestamp) + ":" + secret_key;
    
    // Băm SHA256
    unsigned char hash[32];
    mbedtls_sha256((unsigned char*)data.c_str(), data.length(), hash, 0);
    
    // Chuyển đổi thành chuỗi thập lục phân (16 byte đầu tiên)
    std::string key;
    for (int i = 0; i < 16; i++) {
        char hex[3];
        snprintf(hex, sizeof(hex), "%02X", hash[i]);
        key += hex;
    }
    
    return key;
}

/**
 * @brief Thêm tiêu đề xác thực cho yêu cầu HTTP
 * @param http Con trỏ máy khách HTTP
 */
static void add_auth_headers(Http* http) {
    // Lấy dấu thời gian hiện tại
    int64_t timestamp = esp_timer_get_time() / 1000000;  // Chuyển đổi sang giây
    
    // Tạo khóa động
    std::string dynamic_key = generate_dynamic_key(timestamp);
    
    // Lấy thông tin thiết bị
    std::string mac = get_device_mac();
    std::string chip_id = get_device_chip_id();
    
    // Thêm tiêu đề xác thực
    if (http) {
        http->SetHeader("X-MAC-Address", mac);
        http->SetHeader("X-Chip-ID", chip_id);
        http->SetHeader("X-Timestamp", std::to_string(timestamp));
        http->SetHeader("X-Dynamic-Key", dynamic_key);
        
        ESP_LOGI(TAG, "Added auth headers - MAC: %s, ChipID: %s, Timestamp: %lld", 
                 mac.c_str(), chip_id.c_str(), timestamp);
    }
}

// Hàm mã hóa URL
static std::string url_encode(const std::string& str) {
    std::string encoded;
    char hex[4];
    
    for (size_t i = 0; i < str.length(); i++) {
        unsigned char c = str[i];
        
        if ((c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            encoded += c;
        } else if (c == ' ') {
            encoded += '+';  // Mã hóa khoảng trắng thành '+' hoặc '%20'
        } else {
            snprintf(hex, sizeof(hex), "%%%02X", c);
            encoded += hex;
        }
    }
    return encoded;
}

// Thêm một hàm trợ giúp ở đầu tệp để xử lý thống nhất việc xây dựng URL
static std::string buildUrlWithParams(const std::string& base_url, const std::string& path, const std::string& query) {
    std::string result_url = base_url + path + "?";
    size_t pos = 0;
    size_t amp_pos = 0;
    
    while ((amp_pos = query.find("&", pos)) != std::string::npos) {
        std::string param = query.substr(pos, amp_pos - pos);
        size_t eq_pos = param.find("=");
        
        if (eq_pos != std::string::npos) {
            std::string key = param.substr(0, eq_pos);
            std::string value = param.substr(eq_pos + 1);
            result_url += key + "=" + url_encode(value) + "&";
        } else {
            result_url += param + "&";
        }
        
        pos = amp_pos + 1;
    }
    
    // Xử lý tham số cuối cùng
    std::string last_param = query.substr(pos);
    size_t eq_pos = last_param.find("=");
    
    if (eq_pos != std::string::npos) {
        std::string key = last_param.substr(0, eq_pos);
        std::string value = last_param.substr(eq_pos + 1);
        result_url += key + "=" + url_encode(value);
    } else {
        result_url += last_param;
    }
    
    return result_url;
}

Esp32Music::Esp32Music() : last_downloaded_data_(), current_music_url_(), current_song_name_(), current_artist_name_(),
                         song_name_displayed_(false), current_lyric_url_(), lyrics_(), 
                         current_lyric_index_(-1), lyric_thread_(), is_lyric_running_(false),
                         display_mode_(DISPLAY_MODE_LYRICS), is_playing_(false), is_paused_(false), is_downloading_(false), 
                         play_thread_(), download_thread_(), audio_buffer_(), buffer_mutex_(), 
                         buffer_cv_(), buffer_size_(0), mp3_decoder_(nullptr), mp3_frame_info_(), 
                         mp3_decoder_initialized_(false), fft_data_size_(0), volume_(50), music_has_priority_(false) {
    ESP_LOGI(TAG, "Music player initialized with default spectrum display mode");
    
    // Phân bổ bộ nhớ FFT trong PSRAM
    fft_data_size_ = 2048; // Kích thước mẫu cho dữ liệu FFT
    final_pcm_data_fft = std::unique_ptr<int16_t[]>(new int16_t[fft_data_size_]);
    memset(final_pcm_data_fft.get(), 0, fft_data_size_ * sizeof(int16_t));
    
    InitializeMp3Decoder();
}

Esp32Music::~Esp32Music() {
    ESP_LOGI(TAG, "Destroying music player - stopping all operations");
    
    // Dừng tất cả các hoạt động
    is_downloading_ = false;
    is_playing_ = false;
    is_lyric_running_ = false;
    
    // Thông báo cho tất cả các luồng đang chờ
    {
        std::lock_guard<std::mutex> lock(buffer_mutex_);
        buffer_cv_.notify_all();
    }
    
    // Chờ luồng tải xuống kết thúc, đặt thời gian chờ 5 giây
    if (download_thread_.joinable()) {
        ESP_LOGI(TAG, "Waiting for download thread to finish (timeout: 5s)");
        auto start_time = std::chrono::steady_clock::now();
        
        // Chờ luồng kết thúc
        bool thread_finished = false;
        while (!thread_finished) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - start_time).count();
            
            if (elapsed >= 5) {
                ESP_LOGW(TAG, "Download thread join timeout after 5 seconds");
                break;
            }
            
            // Đặt lại cờ dừng một lần nữa, đảm bảo luồng có thể phát hiện
            is_downloading_ = false;
            
            // Thông báo biến điều kiện
            {
                std::lock_guard<std::mutex> lock(buffer_mutex_);
                buffer_cv_.notify_all();
            }
            
            // Kiểm tra xem luồng đã kết thúc chưa
            if (!download_thread_.joinable()) {
                thread_finished = true;
            }
            
            // In thông tin chờ đợi định kỳ
            if (elapsed > 0 && elapsed % 1 == 0) {
                ESP_LOGI(TAG, "Still waiting for download thread to finish... (%ds)", (int)elapsed);
            }
        }
        
        if (download_thread_.joinable()) {
            download_thread_.join();
        }
        ESP_LOGI(TAG, "Download thread finished");
    }
    
    // Chờ luồng phát lại kết thúc, đặt thời gian chờ 3 giây
    if (play_thread_.joinable()) {
        ESP_LOGI(TAG, "Waiting for playback thread to finish (timeout: 3s)");
        auto start_time = std::chrono::steady_clock::now();
        
        bool thread_finished = false;
        while (!thread_finished) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - start_time).count();
            
            if (elapsed >= 3) {
                ESP_LOGW(TAG, "Playback thread join timeout after 3 seconds");
                break;
            }
            
            // Đặt lại cờ dừng một lần nữa
            is_playing_ = false;
            
            // Thông báo biến điều kiện
            {
                std::lock_guard<std::mutex> lock(buffer_mutex_);
                buffer_cv_.notify_all();
            }
            
            // Kiểm tra xem luồng đã kết thúc chưa
            if (!play_thread_.joinable()) {
                thread_finished = true;
            }
        }
        
        if (play_thread_.joinable()) {
            play_thread_.join();
        }
        ESP_LOGI(TAG, "Playback thread finished");
    }
    
    // Chờ luồng lời bài hát kết thúc
    if (lyric_thread_.joinable()) {
        ESP_LOGI(TAG, "Waiting for lyric thread to finish");
        lyric_thread_.join();
        ESP_LOGI(TAG, "Lyric thread finished");
    }
    
    // Dọn dẹp bộ đệm và bộ giải mã MP3
    ClearAudioBuffer();
    CleanupMp3Decoder();
    
    ESP_LOGI(TAG, "Music player destroyed successfully");
}

bool Esp32Music::Download(const std::string& song_name, const std::string& artist_name) {
    ESP_LOGI(TAG, "Nhóm trao đổi firmware QQ: 826072986");
    ESP_LOGI(TAG, "Bắt đầu lấy thông tin chi tiết nhạc cho: %s", song_name.c_str());
    
    // Xóa dữ liệu tải xuống trước đó
    last_downloaded_data_.clear();
    
    // Lưu tên bài hát và nghệ sĩ để hiển thị sau này
    current_song_name_ = song_name;
    current_artist_name_ = artist_name;
    
    // Bước đầu tiên: Yêu cầu giao diện stream_pcm để lấy thông tin âm thanh
    std::string base_url = "http://www.xiaozhishop.xyz:5005";
    std::string full_url = base_url + "/stream_pcm?song=" + url_encode(song_name) + "&artist=" + url_encode(artist_name);
    
    ESP_LOGI(TAG, "Request URL: %s", full_url.c_str());
    
    // Sử dụng máy khách HTTP do Board cung cấp
    auto network = Board::GetInstance().GetNetwork();
    auto http = network->CreateHttp(0);
    
    // Đặt tiêu đề yêu cầu cơ bản
    http->SetHeader("User-Agent", "ESP32-Music-Player/1.0");
    http->SetHeader("Accept", "application/json");
    
    // Thêm tiêu đề xác thực ESP32
    add_auth_headers(http.get());
    
    // Mở kết nối GET
    if (!http->Open("GET", full_url)) {
        ESP_LOGE(TAG, "Failed to connect to music API");
        return false;
    }
    
    // Kiểm tra mã trạng thái phản hồi
    int status_code = http->GetStatusCode();
    if (status_code != 200) {
        ESP_LOGE(TAG, "HTTP GET failed with status code: %d", status_code);
        http->Close();
        return false;
    }
    
    // Đọc dữ liệu phản hồi
    last_downloaded_data_ = http->ReadAll();
    http->Close();
    
    ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d", status_code, last_downloaded_data_.length());
    ESP_LOGD(TAG, "Complete music details response: %s", last_downloaded_data_.c_str());
    
    // Kiểm tra phản hồi xác thực đơn giản (tùy chọn)
    if (last_downloaded_data_.find("Xác thực khóa động ESP32 thất bại") != std::string::npos) {
        ESP_LOGE(TAG, "Authentication failed for song: %s", song_name.c_str());
        return false;
    }
    
    if (!last_downloaded_data_.empty()) {
        // Phân tích JSON phản hồi để trích xuất URL âm thanh
        cJSON* response_json = cJSON_Parse(last_downloaded_data_.c_str());
        if (response_json) {
            // Trích xuất thông tin quan trọng
            cJSON* artist = cJSON_GetObjectItem(response_json, "artist");
            cJSON* title = cJSON_GetObjectItem(response_json, "title");
            cJSON* audio_url = cJSON_GetObjectItem(response_json, "audio_url");
            cJSON* lyric_url = cJSON_GetObjectItem(response_json, "lyric_url");
            
            if (cJSON_IsString(artist)) {
                ESP_LOGI(TAG, "Artist: %s", artist->valuestring);
            }
            if (cJSON_IsString(title)) {
                ESP_LOGI(TAG, "Title: %s", title->valuestring);
            }
            
            // Kiểm tra xem audio_url có hợp lệ không
            if (cJSON_IsString(audio_url) && audio_url->valuestring && strlen(audio_url->valuestring) > 0) {
                ESP_LOGI(TAG, "Audio URL path: %s", audio_url->valuestring);
                
                // Bước thứ hai: Ghép nối URL tải xuống âm thanh hoàn chỉnh, đảm bảo mã hóa URL cho audio_url
                std::string audio_path = audio_url->valuestring;
                
                // Sử dụng chức năng xây dựng URL thống nhất
                if (audio_path.find("?") != std::string::npos) {
                    size_t query_pos = audio_path.find("?");
                    std::string path = audio_path.substr(0, query_pos);
                    std::string query = audio_path.substr(query_pos + 1);
                    
                    current_music_url_ = buildUrlWithParams(base_url, path, query);
                } else {
                    current_music_url_ = base_url + audio_path;
                }
                
                ESP_LOGI(TAG, "Nhóm trao đổi firmware QQ: 826072986");
                ESP_LOGI(TAG, "Starting streaming playback for: %s", song_name.c_str());
                song_name_displayed_ = false;  // Đặt lại cờ hiển thị tên bài hát
                StartStreaming(current_music_url_);
                
                // Xử lý URL lời bài hát - Chỉ khởi động lời bài hát khi ở chế độ hiển thị lời
                if (cJSON_IsString(lyric_url) && lyric_url->valuestring && strlen(lyric_url->valuestring) > 0) {
                    // Ghép nối URL tải xuống lời bài hát hoàn chỉnh, sử dụng cùng logic xây dựng URL
                    std::string lyric_path = lyric_url->valuestring;
                    if (lyric_path.find("?") != std::string::npos) {
                        size_t query_pos = lyric_path.find("?");
                        std::string path = lyric_path.substr(0, query_pos);
                        std::string query = lyric_path.substr(query_pos + 1);
                        
                        current_lyric_url_ = buildUrlWithParams(base_url, path, query);
                    } else {
                        current_lyric_url_ = base_url + lyric_path;
                    }
                    
                    // Quyết định xem có khởi động lời bài hát hay không dựa trên chế độ hiển thị
                    if (display_mode_ == DISPLAY_MODE_LYRICS) {
                        ESP_LOGI(TAG, "Loading lyrics for: %s (lyrics display mode)", song_name.c_str());
                        
                        // Khởi động tải xuống và hiển thị lời bài hát
                        if (is_lyric_running_) {
                            is_lyric_running_ = false;
                            if (lyric_thread_.joinable()) {
                                lyric_thread_.join();
                            }
                        }
                        
                        is_lyric_running_ = true;
                        current_lyric_index_ = -1;
                        lyrics_.clear();
                        
                        lyric_thread_ = std::thread(&Esp32Music::LyricDisplayThread, this);
                    } else {
                        ESP_LOGI(TAG, "Lyric URL found but spectrum display mode is active, skipping lyrics");
                    }
                } else {
                    ESP_LOGW(TAG, "No lyric URL found for this song");
                }
                
                cJSON_Delete(response_json);
                return true;
            } else {
                // audio_url trống hoặc không hợp lệ
                ESP_LOGE(TAG, "Audio URL not found or empty for song: %s", song_name.c_str());
                ESP_LOGE(TAG, "Failed to find music: Không tìm thấy bài hát '%s'", song_name.c_str());
                cJSON_Delete(response_json);
                return false;
            }
        } else {
            ESP_LOGE(TAG, "Failed to parse JSON response");
        }
    } else {
        ESP_LOGE(TAG, "Empty response from music API");
    }
    
    return false;
}



std::string Esp32Music::GetDownloadResult() {
    return last_downloaded_data_;
}

// Bắt đầu phát trực tuyến
bool Esp32Music::StartStreaming(const std::string& music_url) {
    music_has_priority_.store(true);
    if (music_url.empty()) {
        ESP_LOGE(TAG, "Music URL is empty");
        return false;
    }
    
    ESP_LOGD(TAG, "Starting streaming for URL: %s", music_url.c_str());
    
    // Dừng phát và tải xuống trước đó
    is_downloading_ = false;
    is_playing_ = false;
    
    // Chờ các luồng trước đó kết thúc hoàn toàn
    if (download_thread_.joinable()) {
        {
            std::lock_guard<std::mutex> lock(buffer_mutex_);
            buffer_cv_.notify_all();  // Thông báo luồng thoát
        }
        download_thread_.join();
    }
    if (play_thread_.joinable()) {
        {
            std::lock_guard<std::mutex> lock(buffer_mutex_);
            buffer_cv_.notify_all();  // Thông báo luồng thoát
        }
        play_thread_.join();
    }
    
    // Xóa sạch bộ đệm
    ClearAudioBuffer();
    
    // Cấu hình kích thước ngăn xếp luồng để tránh tràn ngăn xếp
    esp_pthread_cfg_t cfg = esp_pthread_get_default_config();
    cfg.stack_size = 8192;  // Kích thước ngăn xếp 8KB
    cfg.prio = 5;           // Ưu tiên trung bình
    cfg.thread_name = "audio_stream";
    esp_pthread_set_cfg(&cfg);
    
    // Bắt đầu luồng tải xuống
    is_downloading_ = true;
    download_thread_ = std::thread(&Esp32Music::DownloadAudioStream, this, music_url);
    
    // Bắt đầu luồng phát (sẽ chờ bộ đệm có đủ dữ liệu)
    is_playing_ = true;
    play_thread_ = std::thread(&Esp32Music::PlayAudioStream, this);
    
    ESP_LOGI(TAG, "Streaming threads started successfully");
    
    return true;
}

// Dừng phát trực tuyến
bool Esp32Music::StopStreaming() {
    music_has_priority_.store(false);
    ESP_LOGI(TAG, "Stopping music streaming - current state: downloading=%d, playing=%d", 
            is_downloading_.load(), is_playing_.load());

    // Đặt lại tỷ lệ lấy mẫu về giá trị gốc
    ResetSampleRate();
    
    // Kiểm tra xem có phát trực tuyến đang diễn ra không
    if (!is_playing_ && !is_downloading_) {
        ESP_LOGW(TAG, "No streaming in progress");
        return true;
    }
    
    // Dừng cờ tải xuống và phát
    is_downloading_ = false;
    is_playing_ = false;
    
    // Xóa hiển thị tên bài hát (sử dụng SetChatMessage thay thế)
    auto& board = Board::GetInstance();
    auto display = board.GetDisplay();
    if (display) {
        display->SetChatMessage("music", "");  // Xóa hiển thị tên bài hát
        ESP_LOGI(TAG, "Cleared song name display");
    }
    
    // Thông báo cho tất cả các luồng đang chờ
    {
        std::lock_guard<std::mutex> lock(buffer_mutex_);
        buffer_cv_.notify_all();
    }
    
    // Chờ luồng kết thúc (tránh mã trùng lặp, để StopStreaming cũng có thể chờ luồng dừng hoàn toàn)
    if (download_thread_.joinable()) {
        download_thread_.join();
        ESP_LOGI(TAG, "Download thread joined in StopStreaming");
    }
    
    // Chờ luồng phát kết thúc, sử dụng cách an toàn hơn
    if (play_thread_.joinable()) {
        // Trước tiên đặt cờ dừng
        is_playing_ = false;
        
        // Thông báo biến điều kiện, đảm bảo luồng có thể thoát
        {
            std::lock_guard<std::mutex> lock(buffer_mutex_);
            buffer_cv_.notify_all();
        }
        
        // Sử dụng cơ chế thời gian chờ để chờ luồng kết thúc, tránh deadlock
        bool thread_finished = false;
        int wait_count = 0;
        const int max_wait = 100; // Chờ tối đa 1 giây
        
        while (!thread_finished && wait_count < max_wait) {
            vTaskDelay(pdMS_TO_TICKS(10));
            wait_count++;
            
            // Kiểm tra xem luồng có còn có thể join không
            if (!play_thread_.joinable()) {
                thread_finished = true;
                break;
            }
        }
        
        if (play_thread_.joinable()) {
            if (wait_count >= max_wait) {
                ESP_LOGW(TAG, "Play thread join timeout, detaching thread");
                play_thread_.detach();
            } else {
                play_thread_.join();
                ESP_LOGI(TAG, "Play thread joined in StopStreaming");
            }
        }
    }
    
    // FFT visualization is not available in the base Display class
    ESP_LOGI(TAG, "Music streaming stop signal sent");
    return true;
}

// Tải xuống âm thanh theo luồng
void Esp32Music::DownloadAudioStream(const std::string& music_url) {
    ESP_LOGD(TAG, "Starting audio stream download from: %s", music_url.c_str());
    
    // Xác minh tính hợp lệ của URL
    if (music_url.empty() || music_url.find("http") != 0) {
        ESP_LOGE(TAG, "Invalid URL format: %s", music_url.c_str());
        is_downloading_ = false;
        return;
    }
    
    auto network = Board::GetInstance().GetNetwork();
    auto http = network->CreateHttp(0);
    
    // Đặt tiêu đề yêu cầu cơ bản
    http->SetHeader("User-Agent", "ESP32-Music-Player/1.0");
    http->SetHeader("Accept", "*/*");
    http->SetHeader("Range", "bytes=0-");  // Hỗ trợ tiếp tục tải xuống từ điểm ngắt
    
    // Thêm tiêu đề xác thực ESP32
    add_auth_headers(http.get());
    
    if (!http->Open("GET", music_url)) {
        ESP_LOGE(TAG, "Failed to connect to music stream URL");
        is_downloading_ = false;
        return;
    }
    
    int status_code = http->GetStatusCode();
    if (status_code != 200 && status_code != 206) {  // 206 for partial content
        ESP_LOGE(TAG, "HTTP GET failed with status code: %d", status_code);
        http->Close();
        is_downloading_ = false;
        return;
    }
    
    ESP_LOGI(TAG, "Started downloading audio stream, status: %d", status_code);
    
    // Đọc dữ liệu âm thanh theo khối
    const size_t chunk_size = 8192;  // Tăng kích thước khối lên 8KB để cải thiện hiệu suất
    std::vector<char> buffer(chunk_size);
    size_t total_downloaded = 0;
    
    while (is_downloading_ && is_playing_) {
        int bytes_read = http->Read(buffer.data(), chunk_size);
        if (bytes_read < 0) {
            ESP_LOGE(TAG, "Failed to read audio data: error code %d", bytes_read);
            break;
        }
        if (bytes_read == 0) {
            ESP_LOGI(TAG, "Audio stream download completed, total: %d bytes", total_downloaded);
            break;
        }
        
        // Tạo khối dữ liệu âm thanh với quản lý bộ nhớ thông minh
        AudioChunk audio_chunk;
        audio_chunk.data = std::unique_ptr<uint8_t[]>(new uint8_t[bytes_read]);
        audio_chunk.size = bytes_read;
        memcpy(audio_chunk.data.get(), buffer.data(), bytes_read);
        
        // Chờ bộ đệm có không gian
        {
            std::unique_lock<std::mutex> lock(buffer_mutex_);
            buffer_cv_.wait(lock, [this] { return buffer_size_ < MAX_BUFFER_SIZE || !is_downloading_; });
            
            if (is_downloading_) {
                audio_buffer_.push(std::move(audio_chunk));
                buffer_size_ += bytes_read;
                total_downloaded += bytes_read;
                
                // Thông báo luồng phát có dữ liệu mới
                buffer_cv_.notify_one();
                
                if (total_downloaded % (512 * 1024) == 0) {  // In tiến độ mỗi 512KB
                    ESP_LOGI(TAG, "Downloaded %d bytes, buffer size: %d", total_downloaded, buffer_size_);
                }
            } else {
                // audio_chunk sẽ được giải phóng tự động
                break;
            }
        }
    }
    
    http->Close();
    is_downloading_ = false;
    
    // Thông báo luồng phát tải xuống hoàn thành
    {
        std::lock_guard<std::mutex> lock(buffer_mutex_);
        buffer_cv_.notify_all();
    }
    
    ESP_LOGI(TAG, "Audio stream download thread finished");
}

// Phát âm thanh theo luồng
void Esp32Music::PlayAudioStream() {
    ESP_LOGI(TAG, "Starting audio stream playback");
    
    // Khởi tạo các biến theo dõi thời gian
    current_play_time_ms_ = 0;
    last_frame_time_ms_ = 0;
    total_frames_decoded_ = 0;
    
    auto codec = Board::GetInstance().GetAudioCodec();
    if (!codec || !codec->output_enabled()) {
        ESP_LOGE(TAG, "Audio codec not available or not enabled");
        is_playing_ = false;
        return;
    }
    
    if (!mp3_decoder_initialized_) {
        ESP_LOGE(TAG, "MP3 decoder not initialized");
        is_playing_ = false;
        return;
    }
    
    // Chờ bộ đệm có đủ dữ liệu để bắt đầu phát
    {
        std::unique_lock<std::mutex> lock(buffer_mutex_);
        buffer_cv_.wait(lock, [this] { 
            return buffer_size_ >= MIN_BUFFER_SIZE || (!is_downloading_ && !audio_buffer_.empty()); 
        });
    }
    
    ESP_LOGI(TAG, "Nhóm trao đổi firmware QQ: 826072986");
    ESP_LOGI(TAG, "Starting playback with buffer size: %d", buffer_size_);
    
    size_t total_played = 0;
    std::unique_ptr<uint8_t[]> mp3_input_buffer(new uint8_t[MP3_INPUT_BUFFER_SIZE]);
    int bytes_left = 0;
    uint8_t* read_ptr = mp3_input_buffer.get();
    
    // Đánh dấu xem đã xử lý thẻ ID3 chưa
    bool id3_processed = false;
    
    while (is_playing_) {
        // Kiểm tra xem có đang tạm dừng không
        if (is_paused_) {
            // Chờ cho đến khi không còn tạm dừng
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        // Kiểm tra trạng thái thiết bị, chỉ phát nhạc khi ở trạng thái rảnh
        auto& app = Application::GetInstance();
        DeviceState current_state = app.GetDeviceState();
        
        // Chuyển đổi trạng thái: Đang nói -> Đang nghe -> Trạng thái chờ -> Phát nhạc
        if (current_state == kDeviceStateListening || current_state == kDeviceStateSpeaking) {
            if (current_state == kDeviceStateSpeaking) {
                ESP_LOGI(TAG, "Device is in speaking state, switching to listening state for music playback");
            }
            if (current_state == kDeviceStateListening) {
                ESP_LOGI(TAG, "Device is in listening state, switching to idle state for music playback");
            }
            // Chuyển đổi trạng thái
            app.ToggleChatState(); // Chuyển thành trạng thái chờ
            vTaskDelay(pdMS_TO_TICKS(300));
            continue;
        } else if (current_state != kDeviceStateIdle) { // Nếu không phải trạng thái chờ, sẽ bị kẹt ở đây, không cho phát nhạc
            ESP_LOGD(TAG, "Device state is %d, pausing music playback", current_state);
            // Nếu không phải trạng thái rảnh, tạm dừng phát
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }
        
        // Kiểm tra trạng thái thiết bị thông qua, hiển thị tên bài hát đang phát
        if (!song_name_displayed_ && !current_song_name_.empty()) {
            auto& board = Board::GetInstance();
            auto display = board.GetDisplay();
            if (display) {
                // Định dạng hiển thị tên bài hát thành 《tên bài hát》đang phát...
                std::string formatted_song_name = "《" + current_song_name_ + "》đang phát...";
                display->SetChatMessage("music", formatted_song_name.c_str());
                ESP_LOGI(TAG, "Displaying song name: %s", formatted_song_name.c_str());
                song_name_displayed_ = true;
            }

            // FFT visualization is not available in the base Display class
            ESP_LOGI(TAG, "Music info displayed, FFT visualization not available in base Display class");
        }
        
        // Nếu cần thêm dữ liệu MP3, đọc từ bộ đệm
        if (bytes_left < 4096) {  // Giữ ít nhất 4KB dữ liệu để giải mã
            AudioChunk chunk;
            
            // Lấy dữ liệu âm thanh từ bộ đệm
            {
                std::unique_lock<std::mutex> lock(buffer_mutex_);
                if (audio_buffer_.empty()) {
                    if (!is_downloading_) {
                        // Tải xuống hoàn thành và bộ đệm trống, phát lại kết thúc
                        ESP_LOGI(TAG, "Playback finished, total played: %d bytes", total_played);
                        break;
                    }
                    // Chờ dữ liệu mới
                    buffer_cv_.wait(lock, [this] { return !audio_buffer_.empty() || !is_downloading_; });
                    if (audio_buffer_.empty()) {
                        continue;
                    }
                }
                
                chunk = std::move(audio_buffer_.front());
                audio_buffer_.pop();
                buffer_size_ -= chunk.size;
                
                // Thông báo luồng tải xuống bộ đệm có không gian
                buffer_cv_.notify_one();
            }
            
            // Thêm dữ liệu mới vào bộ đệm đầu vào MP3
            if (chunk.data && chunk.size > 0) {
                // Di chuyển dữ liệu còn lại đến đầu bộ đệm
                if (bytes_left > 0 && read_ptr != mp3_input_buffer.get()) {
                    memmove(mp3_input_buffer.get(), read_ptr, bytes_left);
                }
                
                // Kiểm tra không gian bộ đệm
                size_t space_available = MP3_INPUT_BUFFER_SIZE - bytes_left;
                size_t copy_size = std::min(chunk.size, space_available);
                
                // Sao chép dữ liệu mới
                memcpy(mp3_input_buffer.get() + bytes_left, chunk.data.get(), copy_size);
                bytes_left += copy_size;
                read_ptr = mp3_input_buffer.get();
                
                // Kiểm tra và bỏ qua thẻ ID3 (chỉ xử lý một lần khi bắt đầu)
                if (!id3_processed && bytes_left >= 10) {
                    size_t id3_skip = SkipId3Tag(read_ptr, bytes_left);
                    if (id3_skip > 0) {
                        read_ptr += id3_skip;
                        bytes_left -= id3_skip;
                        ESP_LOGI(TAG, "Skipped ID3 tag: %u bytes", (unsigned int)id3_skip);
                    }
                    id3_processed = true;
                }
            }
        } else {
            // Đã có đủ dữ liệu trong bộ đệm đầu vào, tiếp tục giải mã
            int decode_result = MP3Decode(mp3_decoder_, &read_ptr, &bytes_left, 
                                        final_pcm_data_fft.get(), 0);
            
            if (decode_result == ERR_MP3_NONE) {
                // Giải mã thành công
                // ESP_LOGD(TAG, "MP3 decode successful, bytes left: %d", bytes_left);
                
                // Cập nhật thông tin khung
                MP3GetLastFrameInfo(mp3_decoder_, &mp3_frame_info_);
                
                if (mp3_frame_info_.samprate > 0 && mp3_frame_info_.nChans > 0) {
                    // Chuyển đổi stereo sang mono nếu cần
                    int final_sample_count = mp3_frame_info_.outputSamps;
                    int16_t* pcm_buffer = final_pcm_data_fft.get();
                    
                    if (mp3_frame_info_.nChans == 2) {
                        // Chuyển đổi stereo sang mono bằng cách lấy trung bình
                        for (int i = 0, j = 0; i < final_sample_count; i += 2, j++) {
                            pcm_buffer[j] = (pcm_buffer[i] + pcm_buffer[i+1]) / 2;
                        }
                        final_sample_count /= 2;
                    }
                    
                    // Tạo gói dữ liệu âm thanh để gửi đến dịch vụ âm thanh
                    AudioStreamPacket packet;
                    packet.sample_rate = mp3_frame_info_.samprate;
                    packet.payload.resize(final_sample_count * sizeof(int16_t));
                    memcpy(packet.payload.data(), pcm_buffer, final_sample_count * sizeof(int16_t));
                    
                    // Gửi đến hàng đợi giải mã âm thanh của Application
                    auto& app = Application::GetInstance();
                    app.AddAudioData(std::move(packet));
                    total_frames_decoded_++;
                    
                    // Cập nhật thời gian phát hiện tại
                    int frame_duration_ms = (mp3_frame_info_.outputSamps * 1000) / 
                                          (mp3_frame_info_.samprate * mp3_frame_info_.nChans);
                    current_play_time_ms_ += frame_duration_ms;
                    total_played += final_sample_count * sizeof(int16_t);
                    
                    // Cập nhật hiển thị lời bài hát
                    int buffer_latency_ms = 600; // Giá trị điều chỉnh thực tế
                    UpdateLyricDisplay(current_play_time_ms_ + buffer_latency_ms);
                    
                    // In tiến độ phát
                    if (total_played % (128 * 1024) == 0) {
                        ESP_LOGI(TAG, "Played %d bytes, buffer size: %d", total_played, buffer_size_);
                    }
                }
            } else {
                // Giải mã thất bại
                ESP_LOGW(TAG, "MP3 decode failed with error: %d", decode_result);
                
                // Bỏ qua một số byte và tiếp tục thử
                if (bytes_left > 1) {
                    read_ptr++;
                    bytes_left--;
                } else {
                    bytes_left = 0;
                }
            }
        }
    }
    
    // Xóa hiển thị tên bài hát
    if (mp3_input_buffer) {
        // Bộ nhớ sẽ được giải phóng tự động bởi unique_ptr
    }
    
    // Thực hiện dọn dẹp cơ bản khi phát xong, nhưng không gọi StopStreaming để tránh luồng tự chờ
    ESP_LOGI(TAG, "Audio stream playback finished, total played: %d bytes", total_played);
    ESP_LOGI(TAG, "Performing basic cleanup from play thread");
    
    // Dừng cờ phát
    is_playing_ = false;
    
    // Chỉ dừng hiển thị FFT khi ở chế độ hiển thị phổ
    if (display_mode_ == DISPLAY_MODE_SPECTRUM) {
        auto& board = Board::GetInstance();
        auto display = board.GetDisplay();
        if (display) {
            display->stopFft();
            ESP_LOGI(TAG, "Stopped FFT display from play thread (spectrum mode)");
        }
    } else {
        ESP_LOGI(TAG, "Not in spectrum mode, skipping FFT stop");
    }
}

// Thêm phương thức Pause
bool Esp32Music::Pause() {
    if (!is_playing_ || is_paused_) {
        return false;
    }
    
    ESP_LOGI(TAG, "Pausing music playback");
    is_paused_ = true;
    
    // FFT visualization is not available in the base Display class
    return true;
}

// Thêm phương thức Resume
bool Esp32Music::Resume() {
    if (!is_playing_ || !is_paused_) {
        return false;
    }
    
    ESP_LOGI(TAG, "Resuming music playback");
    is_paused_ = false;
    
    // FFT visualization is not available in the base Display class
    return true;
}

// Thêm phương thức SetVolume
void Esp32Music::SetVolume(int volume) {
    if (volume < 0) volume = 0;
    if (volume > 100) volume = 100;
    
    volume_ = volume;
    ESP_LOGI(TAG, "Volume set to %d", volume);
    
    // Áp dụng âm lượng cho codec nếu có
    auto& board = Board::GetInstance();
    auto codec = board.GetAudioCodec();
    if (codec) {
        codec->SetOutputVolume(volume);
        ESP_LOGI(TAG, "Applied volume %d to audio codec", volume);
    }
}

// Xóa sạch bộ đệm âm thanh
void Esp32Music::ClearAudioBuffer() {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    
    while (!audio_buffer_.empty()) {
        AudioChunk chunk = std::move(audio_buffer_.front());
        audio_buffer_.pop();
        // unique_ptr sẽ tự động giải phóng bộ nhớ, không cần gọi heap_caps_free
    }
    
    buffer_size_ = 0;
    ESP_LOGI(TAG, "Audio buffer cleared");
}

// Khởi tạo bộ giải mã MP3
bool Esp32Music::InitializeMp3Decoder() {
    mp3_decoder_ = MP3InitDecoder();
    if (mp3_decoder_ == nullptr) {
        ESP_LOGE(TAG, "Failed to initialize MP3 decoder");
        mp3_decoder_initialized_ = false;
        return false;
    }
    
    mp3_decoder_initialized_ = true;
    ESP_LOGI(TAG, "MP3 decoder initialized successfully");
    return true;
}

// Dọn dẹp bộ giải mã MP3
void Esp32Music::CleanupMp3Decoder() {
    if (mp3_decoder_ != nullptr) {
        MP3FreeDecoder(mp3_decoder_);
        mp3_decoder_ = nullptr;
    }
    mp3_decoder_initialized_ = false;
    ESP_LOGI(TAG, "MP3 decoder cleaned up");
}

// Đặt lại tỷ lệ lấy mẫu về giá trị gốc
void Esp32Music::ResetSampleRate() {
    auto& board = Board::GetInstance();
    auto codec = board.GetAudioCodec();
    if (codec && codec->original_output_sample_rate() > 0 && 
        codec->output_sample_rate() != codec->original_output_sample_rate()) {
        ESP_LOGI(TAG, "Đặt lại tỷ lệ lấy mẫu: Từ %d Hz đặt lại về giá trị gốc %d Hz", 
                codec->output_sample_rate(), codec->original_output_sample_rate());
        if (codec->SetOutputSampleRate(-1)) {  // -1 có nghĩa là đặt lại về giá trị gốc
            ESP_LOGI(TAG, "Thành công đặt lại tỷ lệ lấy mẫu về giá trị gốc: %d Hz", codec->output_sample_rate());
        } else {
            ESP_LOGW(TAG, "Không thể đặt lại tỷ lệ lấy mẫu về giá trị gốc");
        }
    }
}

// Bỏ qua thẻ ID3 ở đầu tệp MP3
size_t Esp32Music::SkipId3Tag(uint8_t* data, size_t size) {
    if (!data || size < 10) {
        return 0;
    }
    
    // Kiểm tra tiêu đề thẻ ID3v2 "ID3"
    if (memcmp(data, "ID3", 3) != 0) {
        return 0;
    }
    
    // Tính kích thước thẻ (định dạng synchsafe integer)
    uint32_t tag_size = ((uint32_t)(data[6] & 0x7F) << 21) |
                        ((uint32_t)(data[7] & 0x7F) << 14) |
                        ((uint32_t)(data[8] & 0x7F) << 7)  |
                        ((uint32_t)(data[9] & 0x7F));
    
    // Tiêu đề ID3v2 (10 byte) + Nội dung thẻ
    size_t total_skip = 10 + tag_size;
    
    // Đảm bảo không vượt quá kích thước dữ liệu có sẵn
    if (total_skip > size) {
        total_skip = size;
    }
    
    ESP_LOGI(TAG, "Found ID3v2 tag, skipping %u bytes", (unsigned int)total_skip);
    return total_skip;
}

// Tải xuống lời bài hát
bool Esp32Music::DownloadLyrics(const std::string& lyric_url) {
    ESP_LOGI(TAG, "Downloading lyrics from: %s", lyric_url.c_str());
    
    // Kiểm tra xem URL có trống không
    if (lyric_url.empty()) {
        ESP_LOGE(TAG, "Lyric URL is empty!");
        return false;
    }
    
    // Thêm logic thử lại
    const int max_retries = 3;
    int retry_count = 0;
    bool success = false;
    std::string lyric_content;
    std::string current_url = lyric_url;
    int redirect_count = 0;
    const int max_redirects = 5;  // Cho phép tối đa 5 lần chuyển hướng
    
    while (retry_count < max_retries && !success && redirect_count < max_redirects) {
        if (retry_count > 0) {
            ESP_LOGI(TAG, "Retrying lyric download (attempt %d of %d)", retry_count + 1, max_retries);
            // Tạm dừng trước khi thử lại
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        // Sử dụng máy khách HTTP do Board cung cấp
        auto network = Board::GetInstance().GetNetwork();
        auto http = network->CreateHttp(0);
        if (!http) {
            ESP_LOGE(TAG, "Failed to create HTTP client for lyric download");
            retry_count++;
            continue;
        }
        
        // Đặt tiêu đề yêu cầu cơ bản
        http->SetHeader("User-Agent", "ESP32-Music-Player/1.0");
        http->SetHeader("Accept", "text/plain");
        
        // Thêm tiêu đề xác thực ESP32
        add_auth_headers(http.get());
        
        // Mở kết nối GET
        ESP_LOGI(TAG, "Nhóm trao đổi firmware QQ: 826072986");
        if (!http->Open("GET", current_url)) {
            ESP_LOGE(TAG, "Failed to open HTTP connection for lyrics");
            // Loại bỏ delete http; vì unique_ptr sẽ tự động quản lý bộ nhớ
            retry_count++;
            continue;
        }
        
        // Kiểm tra mã trạng thái HTTP
        int status_code = http->GetStatusCode();
        ESP_LOGI(TAG, "Lyric download HTTP status code: %d", status_code);
        
        // Xử lý chuyển hướng - Do lớp Http không có phương thức GetHeader, chúng ta chỉ có thể dựa vào mã trạng thái để xác định
        if (status_code == 301 || status_code == 302 || status_code == 303 || status_code == 307 || status_code == 308) {
            // Do không thể lấy tiêu đề Location, chỉ có thể báo cáo chuyển hướng nhưng không thể tiếp tục
            ESP_LOGW(TAG, "Received redirect status %d but cannot follow redirect (no GetHeader method)", status_code);
            http->Close();
            retry_count++;
            continue;
        }
        
        // Mã trạng thái không thuộc loạt 200 được coi là lỗi
        if (status_code < 200 || status_code >= 300) {
            ESP_LOGE(TAG, "HTTP GET failed with status code: %d", status_code);
            http->Close();
            retry_count++;
            continue;
        }
        
        // Đọc phản hồi
        lyric_content.clear();
        char buffer[1024];
        int bytes_read;
        bool read_error = false;
        int total_read = 0;
        
        // Do không thể lấy tiêu đề Content-Length và Content-Type, chúng ta không biết kích thước mong đợi và loại nội dung
        ESP_LOGD(TAG, "Starting to read lyric content");
        
        while (true) {
            bytes_read = http->Read(buffer, sizeof(buffer) - 1);
            // ESP_LOGD(TAG, "Lyric HTTP read returned %d bytes", bytes_read); // Đã chú thích để giảm đầu ra nhật ký
            
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                lyric_content += buffer;
                total_read += bytes_read;
                
                // In tiến độ tải xuống định kỳ - Thay đổi thành cấp độ DEBUG để giảm đầu ra
                if (total_read % 4096 == 0) {
                    ESP_LOGD(TAG, "Downloaded %d bytes so far", total_read);
                }
            } else if (bytes_read == 0) {
                // Kết thúc bình thường, không có thêm dữ liệu
                ESP_LOGD(TAG, "Lyric download completed, total bytes: %d", total_read);
                success = true;
                break;
            } else {
                // bytes_read < 0, có thể là vấn đề đã biết của ESP-IDF
                // Nếu đã đọc được một số dữ liệu thì coi như tải xuống thành công
                if (!lyric_content.empty()) {
                    ESP_LOGW(TAG, "HTTP read returned %d, but we have data (%d bytes), continuing", bytes_read, lyric_content.length());
                    success = true;
                    break;
                } else {
                    ESP_LOGE(TAG, "Failed to read lyric data: error code %d", bytes_read);
                    read_error = true;
                    break;
                }
            }
        }
        
        http->Close();
        
        if (read_error) {
            retry_count++;
            continue;
        }
        
        // Nếu đọc dữ liệu thành công, thoát khỏi vòng lặp thử lại
        if (success) {
            break;
        }
    }
    
    // Kiểm tra xem đã vượt quá số lần thử lại tối đa chưa
    if (retry_count >= max_retries) {
        ESP_LOGE(TAG, "Failed to download lyrics after %d attempts", max_retries);
        return false;
    }
    
    // Ghi lại dữ liệu vài byte đầu tiên để hỗ trợ gỡ lỗi
    if (!lyric_content.empty()) {
        size_t preview_size = std::min(lyric_content.size(), size_t(50));
        std::string preview = lyric_content.substr(0, preview_size);
        ESP_LOGD(TAG, "Lyric content preview (%d bytes): %s", lyric_content.length(), preview.c_str());
    } else {
        ESP_LOGE(TAG, "Failed to download lyrics or lyrics are empty");
        return false;
    }
    
    ESP_LOGI(TAG, "Lyrics downloaded successfully, size: %d bytes", lyric_content.length());
    return ParseLyrics(lyric_content);
}

// Phân tích lời bài hát
bool Esp32Music::ParseLyrics(const std::string& lyric_content) {
    ESP_LOGI(TAG, "Parsing lyrics content");
    
    // Sử dụng khóa để bảo vệ truy cập mảng lyrics_
    std::lock_guard<std::mutex> lock(lyrics_mutex_);
    
    lyrics_.clear();
    
    // Chia nội dung lời bài hát theo dòng
    std::istringstream stream(lyric_content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Loại bỏ ký tự xuống dòng ở cuối dòng
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        // Bỏ qua dòng trống
        if (line.empty()) {
            continue;
        }
        
        // Phân tích định dạng LRC: [mm:ss.xx] văn bản lời bài hát
        if (line.length() > 10 && line[0] == '[') {
            size_t close_bracket = line.find(']');
            if (close_bracket != std::string::npos) {
                std::string tag_or_time = line.substr(1, close_bracket - 1);
                std::string content = line.substr(close_bracket + 1);
                
                // Kiểm tra xem có phải là thẻ siêu dữ liệu thay vì dấu thời gian không
                // Thẻ siêu dữ liệu thường là [ti:tiêu đề], [ar:nghệ sĩ], [al:album] v.v.
                size_t colon_pos = tag_or_time.find(':');
                if (colon_pos != std::string::npos) {
                    std::string left_part = tag_or_time.substr(0, colon_pos);
                    
                    // Kiểm tra xem bên trái dấu hai chấm có phải là thời gian (số) không
                    bool is_time_format = true;
                    for (char c : left_part) {
                        if (!isdigit(c)) {
                            is_time_format = false;
                            break;
                        }
                    }
                    
                    // Nếu không phải định dạng thời gian, bỏ qua dòng này (thẻ siêu dữ liệu)
                    if (!is_time_format) {
                        // Có thể xử lý siêu dữ liệu ở đây, ví dụ trích xuất tiêu đề, nghệ sĩ v.v.
                        ESP_LOGD(TAG, "Skipping metadata tag: [%s]", tag_or_time.c_str());
                        continue;
                    }
                    
                    // Là định dạng thời gian, phân tích dấu thời gian
                    try {
                        int minutes = std::stoi(tag_or_time.substr(0, colon_pos));
                        float seconds = std::stof(tag_or_time.substr(colon_pos + 1));
                        int timestamp_ms = minutes * 60 * 1000 + (int)(seconds * 1000);
                        
                        // Xử lý văn bản lời bài hát một cách an toàn, đảm bảo mã hóa UTF-8 đúng
                        std::string safe_lyric_text;
                        if (!content.empty()) {
                            // Tạo bản sao an toàn và xác minh chuỗi
                            safe_lyric_text = content;
                            // Đảm bảo chuỗi kết thúc bằng null
                            safe_lyric_text.shrink_to_fit();
                        }
                        
                        lyrics_.push_back(std::make_pair(timestamp_ms, safe_lyric_text));
                        
                        if (!safe_lyric_text.empty()) {
                            // Hạn chế độ dài đầu ra nhật ký, tránh vấn đề cắt ngắn ký tự Trung Quốc
                            size_t log_len = std::min(safe_lyric_text.length(), size_t(50));
                            std::string log_text = safe_lyric_text.substr(0, log_len);
                            ESP_LOGD(TAG, "Parsed lyric: [%d ms] %s", timestamp_ms, log_text.c_str());
                        } else {
                            ESP_LOGD(TAG, "Parsed lyric: [%d ms] (empty)", timestamp_ms);
                        }
                    } catch (const std::exception& e) {
                        ESP_LOGW(TAG, "Failed to parse time: %s", tag_or_time.c_str());
                    }
                }
            }
        }
    }
    
    // Sắp xếp theo dấu thời gian
    std::sort(lyrics_.begin(), lyrics_.end());
    
    ESP_LOGI(TAG, "Parsed %d lyric lines", lyrics_.size());
    return !lyrics_.empty();
}

// Luồng hiển thị lời bài hát
void Esp32Music::LyricDisplayThread() {
    ESP_LOGI(TAG, "Lyric display thread started");
    
    if (!DownloadLyrics(current_lyric_url_)) {
        ESP_LOGE(TAG, "Failed to download or parse lyrics");
        is_lyric_running_ = false;
        return;
    }
    
    // Định kỳ kiểm tra xem có cần cập nhật hiển thị không (tần suất có thể giảm)
    while (is_lyric_running_ && is_playing_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    ESP_LOGI(TAG, "Lyric display thread finished");
}

void Esp32Music::UpdateLyricDisplay(int64_t current_time_ms) {
    std::lock_guard<std::mutex> lock(lyrics_mutex_);
    
    if (lyrics_.empty()) {
        return;
    }
    
    // Tìm lời bài hát hiện tại nên hiển thị
    int new_lyric_index = -1;
    
    // Bắt đầu tìm kiếm từ chỉ mục lời bài hát hiện tại để cải thiện hiệu quả
    int start_index = (current_lyric_index_.load() >= 0) ? current_lyric_index_.load() : 0;
    
    // Tìm kiếm xuôi: Tìm lời bài hát cuối cùng có dấu thời gian nhỏ hơn hoặc bằng thời gian hiện tại
    for (int i = start_index; i < (int)lyrics_.size(); i++) {
        if (lyrics_[i].first <= current_time_ms) {
            new_lyric_index = i;
        } else {
            break;  // Dấu thời gian đã vượt quá thời gian hiện tại
        }
    }
    
    // Nếu không tìm thấy (có thể thời gian hiện tại sớm hơn câu lời đầu tiên), hiển thị trống
    if (new_lyric_index == -1) {
        new_lyric_index = -1;
    }
    
    // Nếu chỉ mục lời bài hát thay đổi, cập nhật hiển thị
    if (new_lyric_index != current_lyric_index_) {
        current_lyric_index_ = new_lyric_index;
        
        auto& board = Board::GetInstance();
        auto display = board.GetDisplay();
        if (display) {
            std::string lyric_text;
            
            if (current_lyric_index_ >= 0 && current_lyric_index_ < (int)lyrics_.size()) {
                lyric_text = lyrics_[current_lyric_index_].second;
            }
            
            // Hiển thị lời bài hát
            display->SetChatMessage("lyric", lyric_text.c_str());
            
            ESP_LOGD(TAG, "Lyric update at %lldms: %s", 
                    current_time_ms, 
                    lyric_text.empty() ? "(no lyric)" : lyric_text.c_str());
        }
    }
}

// Xóa các phương thức khởi tạo xác thực phức tạp, sử dụng các hàm tĩnh đơn giản

// Xóa các phương thức lớp phức tạp, sử dụng các hàm tĩnh đơn giản

/**
 * @brief Thêm tiêu đề xác thực vào yêu cầu HTTP
 * @param http_client Con trỏ máy khách HTTP
 * 
 * Các tiêu đề xác thực được thêm bao gồm:
 * - X-MAC-Address: Địa chỉ MAC thiết bị
 * - X-Chip-ID: ID chip thiết bị
 * - X-Timestamp: Dấu thời gian hiện tại
 * - X-Dynamic-Key: Khóa được tạo động
 */
// Xóa phương thức AddAuthHeaders phức tạp, sử dụng các hàm tĩnh đơn giản

// Xóa các phương thức xác minh và cấu hình xác thực phức tạp, sử dụng các hàm tĩnh đơn giản

// Cài đặt phương thức điều khiển chế độ hiển thị
void Esp32Music::SetDisplayMode(DisplayMode mode) {
    DisplayMode old_mode = display_mode_.load();
    display_mode_ = mode;
    
    ESP_LOGI(TAG, "Display mode changed from %s to %s", 
            (old_mode == DISPLAY_MODE_SPECTRUM) ? "SPECTRUM" : "LYRICS",
            (mode == DISPLAY_MODE_SPECTRUM) ? "SPECTRUM" : "LYRICS");
}
