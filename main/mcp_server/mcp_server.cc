/*
 * Triển khai máy chủ MCP
 * Tham khảo: https://modelcontextprotocol.io/specification/2024-11-05
 */

#include "mcp_server.h"
#include <esp_log.h>
#include <esp_app_desc.h>
#include <algorithm>
#include <cstring>
#include <esp_pthread.h>

#include "application.h"
#include "display.h"
#include "oled_display.h"
#include "board.h"
#include "settings.h"
#include "lvgl_theme.h"
#include "lvgl_display.h"
#include "boards/common/music.h"
#include "boards/common/alarm.h"

#define TAG "MCP"

McpServer::McpServer() {
}

McpServer::~McpServer() {
    for (auto tool : tools_) {
        delete tool;
    }
    tools_.clear();
}

void McpServer::AddCommonTools() {
    // *Quan trọng* Để tăng tốc độ phản hồi, chúng tôi thêm các công cụ phổ biến vào đầu
    // danh sách công cụ để sử dụng bộ đệm prompt.
    // **Quan trọng** Để cải thiện tốc độ phản hồi, chúng tôi đặt các công cụ thường dùng ở đầu để tận dụng tính năng bộ đệm prompt.

    // Sao lưu danh sách công cụ gốc và khôi phục sau khi thêm các công cụ phổ biến.
    auto original_tools = std::move(tools_);
    auto& board = Board::GetInstance();

    // Không thêm công cụ tùy chỉnh ở đây.
    // Các công cụ tùy chỉnh phải được thêm trong hàm InitializeTools của bo mạch.
    
    // Add alarm tools
    auto alarm = board.GetAlarm();
    if (alarm) {
        AddTool("self.alarm.set",
            "Đặt báo thức. index: số báo thức (0-4); hour: giờ (0-23); minute: phút (0-59)",
            PropertyList({
                Property("index", kPropertyTypeInteger, 0, 0, 4),
                Property("hour", kPropertyTypeInteger, 0, 0, 23),
                Property("minute", kPropertyTypeInteger, 0, 0, 59)
            }),
            [alarm](const PropertyList& properties) -> ReturnValue {
                int index = properties["index"].value<int>();
                int hour = properties["hour"].value<int>();
                int minute = properties["minute"].value<int>();
                alarm->AlarmSet(index, hour, minute);
                return true;
            });

        AddTool("self.alarm.clear",
            "Xóa báo thức. index: số báo thức (0-4)",
            PropertyList({
                Property("index", kPropertyTypeInteger, 0, 0, 4)
            }),
            [alarm](const PropertyList& properties) -> ReturnValue {
                int index = properties["index"].value<int>();
                alarm->AlarmClear(index);
                return true;
            });

        AddTool("self.alarm.clear_all",
            "Xóa tất cả báo thức",
            PropertyList(),
            [alarm](const PropertyList& properties) -> ReturnValue {
                alarm->AlarmClearAll();
                return true;
            });

        AddTool("self.alarm.is_set",
            "Kiểm tra báo thức đã được đặt chưa. index: số báo thức (0-4)",
            PropertyList({
                Property("index", kPropertyTypeInteger, 0, 0, 4)
            }),
            [alarm](const PropertyList& properties) -> ReturnValue {
                int index = properties["index"].value<int>();
                return alarm->AlarmIsSet(index);
            });

        AddTool("self.alarm.get_status",
            "Lấy thông tin về tất cả các báo thức đã đặt",
            PropertyList(),
            [alarm](const PropertyList& properties) -> ReturnValue {
                cJSON* json = cJSON_CreateObject();
                cJSON* alarms = cJSON_CreateArray();
                
                for (int i = 0; i < 5; i++) {
                    if (alarm->AlarmIsSet(i)) {
                        cJSON* alarm_info = cJSON_CreateObject();
                        cJSON_AddNumberToObject(alarm_info, "index", i);
                        // Note: We don't have a direct way to get the time, but we can indicate it's set
                        cJSON_AddBoolToObject(alarm_info, "is_set", true);
                        cJSON_AddItemToArray(alarms, alarm_info);
                    }
                }
                
                cJSON_AddItemToObject(json, "alarms", alarms);
                return json;
            });
    }

    AddTool("self.get_device_status",
        "Cung cấp thông tin thời gian thực của thiết bị, bao gồm trạng thái hiện tại của loa âm thanh, màn hình, pin, mạng, v.v.\n"
        "Sử dụng công cụ này để: \n"
        "1. Trả lời các câu hỏi về điều kiện hiện tại (ví dụ: âm lượng hiện tại của loa âm thanh là gì?)\n"
        "2. Là bước đầu tiên để điều khiển thiết bị (ví dụ: tăng / giảm âm lượng của loa âm thanh, v.v.)",
        PropertyList(),
        [&board](const PropertyList& properties) -> ReturnValue {
            return board.GetDeviceStatusJson();
        });

    AddTool("self.audio_speaker.set_volume", 
        "Đặt âm lượng của loa âm thanh. Nếu không biết âm lượng hiện tại, bạn phải gọi công cụ `self.get_device_status` trước rồi mới gọi công cụ này.",
        PropertyList({
            Property("volume", kPropertyTypeInteger, 0, 100)
        }), 
        [&board](const PropertyList& properties) -> ReturnValue {
            auto codec = board.GetAudioCodec();
            codec->SetOutputVolume(properties["volume"].value<int>());
            return true;
        });
    
    // Thêm công cụ nhạc
    auto music_player = board.GetMusicPlayer();
    if (music_player) {
        AddTool("music_player.search_and_play",
            "Tìm kiếm bài hát và bắt đầu phát. Tìm bài hát theo tên và tự động bắt đầu phát. "
            "Sử dụng công cụ này để phát các bài hát cụ thể do người dùng yêu cầu.",
            PropertyList({
                Property("song_name", kPropertyTypeString)
            }),
            [music_player](const PropertyList& properties) -> ReturnValue {
                auto song_name = properties["song_name"].value<std::string>();
                bool result = music_player->Download(song_name, "");
                if (result) {
                    return "Đã bắt đầu phát: " + song_name;
                } else {
                    return "Không tìm thấy hoặc phát: " + song_name;
                }
            });

        AddTool("music_player.stop",
            "Hoàn toàn dừng phát nhạc. Điều này sẽ dừng bài hát hiện tại và đặt lại vị trí về đầu. "
            "Sử dụng công cụ này khi người dùng muốn dừng nhạc hoàn toàn.",
            PropertyList(),
            [music_player](const PropertyList& properties) -> ReturnValue {
                bool result = music_player->StopStreaming();
                if (result) {
                    return "Đã dừng phát nhạc thành công";
                } else {
                    return "Không thể dừng phát nhạc";
                }
            });

        AddTool("music_player.get_status",
            "Lấy trạng thái hiện tại của trình phát nhạc bao gồm bài hát hiện tại và trạng thái phát. "
            "Sử dụng công cụ này để kiểm tra những gì đang phát hoặc nhận thông tin phát lại chi tiết.",
            PropertyList(),
            [music_player](const PropertyList& properties) -> ReturnValue {
                cJSON* json = cJSON_CreateObject();
                cJSON_AddStringToObject(json, "current_song", music_player->GetDownloadResult().c_str());
                cJSON_AddBoolToObject(json, "is_playing", music_player->IsDownloading());
                cJSON_AddNumberToObject(json, "buffer_size", music_player->GetBufferSize());
                return json;
            });
    }
    
    auto backlight = board.GetBacklight();
    if (backlight) {
        AddTool("self.screen.set_brightness",
            "Đặt độ sáng của màn hình.",
            PropertyList({
                Property("brightness", kPropertyTypeInteger, 0, 100)
            }),
            [backlight](const PropertyList& properties) -> ReturnValue {
                uint8_t brightness = static_cast<uint8_t>(properties["brightness"].value<int>());
                backlight->SetBrightness(brightness, true);
                return true;
            });
    }

#ifdef HAVE_LVGL
    auto display = board.GetDisplay();
    if (display && display->GetTheme() != nullptr) {
        AddTool("self.screen.set_theme",
            "Đặt chủ đề của màn hình. Chủ đề có thể là `light` hoặc `dark`.",
            PropertyList({
                Property("theme", kPropertyTypeString)
            }),
            [display](const PropertyList& properties) -> ReturnValue {
                auto theme_name = properties["theme"].value<std::string>();
                auto& theme_manager = LvglThemeManager::GetInstance();
                auto theme = theme_manager.GetTheme(theme_name);
                if (theme != nullptr) {
                    display->SetTheme(theme);
                    return true;
                }
                return false;
            });
    }

    auto camera = board.GetCamera();
    if (camera) {
        AddTool("self.camera.take_photo",
            "Chụp ảnh và giải thích nó. Sử dụng công cụ này sau khi người dùng yêu cầu xem điều gì đó.\n"
            "Tham số:\n"
            "  `question`: Câu hỏi bạn muốn đặt ra về ảnh.\n"
            "Trả về:\n"
            "  Một đối tượng JSON cung cấp thông tin ảnh.",
            PropertyList({
                Property("question", kPropertyTypeString)
            }),
            [camera](const PropertyList& properties) -> ReturnValue {
                // Giảm mức độ ưu tiên để chụp ảnh
                TaskPriorityReset priority_reset(1);

                if (!camera->Capture()) {
                    throw std::runtime_error("Không thể chụp ảnh");
                }
                auto question = properties["question"].value<std::string>();
                return camera->Explain(question);
            });
    }
#endif

    // Khôi phục danh sách công cụ gốc về cuối danh sách công cụ
    tools_.insert(tools_.end(), original_tools.begin(), original_tools.end());
}

void McpServer::AddUserOnlyTools() {
    // Công cụ hệ thống
    AddUserOnlyTool("self.get_system_info",
        "Lấy thông tin hệ thống",
        PropertyList(),
        [this](const PropertyList& properties) -> ReturnValue {
            auto& board = Board::GetInstance();
            return board.GetSystemInfoJson();
        });

    AddUserOnlyTool("self.reboot", "Khởi động lại hệ thống",
        PropertyList(),
        [this](const PropertyList& properties) -> ReturnValue {
            auto& app = Application::GetInstance();
            app.Schedule([&app]() {
                ESP_LOGW(TAG, "Người dùng yêu cầu khởi động lại");
                vTaskDelay(pdMS_TO_TICKS(1000));

                app.Reboot();
            });
            return true;
        });

    // Nâng cấp firmware
    AddUserOnlyTool("self.upgrade_firmware", "Nâng cấp firmware từ một URL cụ thể. Điều này sẽ tải xuống và cài đặt firmware, sau đó khởi động lại thiết bị.",
        PropertyList({
            Property("url", kPropertyTypeString, "URL của tệp firmware nhị phân để tải xuống và cài đặt")
        }),
        [this](const PropertyList& properties) -> ReturnValue {
            auto url = properties["url"].value<std::string>();
            ESP_LOGI(TAG, "Người dùng yêu cầu nâng cấp firmware từ URL: %s", url.c_str());
            
            auto& app = Application::GetInstance();
            app.Schedule([url, &app]() {
                auto ota = std::make_unique<Ota>();
                
                bool success = app.UpgradeFirmware(*ota, url);
                if (!success) {
                    ESP_LOGE(TAG, "Nâng cấp firmware thất bại");
                }
            });
            
            return true;
        });

    // Điều khiển hiển thị
#ifdef HAVE_LVGL
    auto display = dynamic_cast<LvglDisplay*>(Board::GetInstance().GetDisplay());
    if (display) {
        AddUserOnlyTool("self.screen.get_info", "Thông tin về màn hình, bao gồm chiều rộng, chiều cao, v.v.",
            PropertyList(),
            [display](const PropertyList& properties) -> ReturnValue {
                cJSON *json = cJSON_CreateObject();
                cJSON_AddNumberToObject(json, "width", display->width());
                cJSON_AddNumberToObject(json, "height", display->height());
                if (dynamic_cast<OledDisplay*>(display)) {
                    cJSON_AddBoolToObject(json, "monochrome", true);
                } else {
                    cJSON_AddBoolToObject(json, "monochrome", false);
                }
                return json;
            });

#if CONFIG_LV_USE_SNAPSHOT
        AddUserOnlyTool("self.screen.snapshot", "Chụp ảnh màn hình và tải lên một URL cụ thể",
            PropertyList({
                Property("url", kPropertyTypeString),
                Property("quality", kPropertyTypeInteger, 80, 1, 100)
            }),
            [display](const PropertyList& properties) -> ReturnValue {
                auto url = properties["url"].value<std::string>();
                auto quality = properties["quality"].value<int>();

                std::string jpeg_data;
                if (!display->SnapshotToJpeg(jpeg_data, quality)) {
                    throw std::runtime_error("Không thể chụp ảnh màn hình");
                }

                ESP_LOGI(TAG, "Tải ảnh chụp màn hình %u byte lên %s", jpeg_data.size(), url.c_str());
                
                // Xây dựng yêu cầu multipart/form-data
                std::string boundary = "----ESP32_SCREEN_SNAPSHOT_BOUNDARY";
                
                auto http = Board::GetInstance().GetNetwork()->CreateHttp(3);
                http->SetHeader("Content-Type", "multipart/form-data; boundary=" + boundary);
                if (!http->Open("POST", url)) {
                    throw std::runtime_error("Không thể mở URL: " + url);
                }
                {
                    // Tiêu đề trường tệp
                    std::string file_header;
                    file_header += "--" + boundary + "\r\n";
                    file_header += "Content-Disposition: form-data; name=\"file\"; filename=\"screenshot.jpg\"\r\n";
                    file_header += "Content-Type: image/jpeg\r\n";
                    file_header += "\r\n";
                    http->Write(file_header.c_str(), file_header.size());
                }

                // Dữ liệu JPEG
                http->Write((const char*)jpeg_data.data(), jpeg_data.size());

                {
                    // Chân multipart
                    std::string multipart_footer;
                    multipart_footer += "\r\n--" + boundary + "--\r\n";
                    http->Write(multipart_footer.c_str(), multipart_footer.size());
                }
                http->Write("", 0);

                if (http->GetStatusCode() != 200) {
                    throw std::runtime_error("Mã trạng thái không mong đợi: " + std::to_string(http->GetStatusCode()));
                }
                std::string result = http->ReadAll();
                http->Close();
                ESP_LOGI(TAG, "Kết quả chụp màn hình: %s", result.c_str());
                return true;
            });
        
        AddUserOnlyTool("self.screen.preview_image", "Xem trước hình ảnh trên màn hình",
            PropertyList({
                Property("url", kPropertyTypeString)
            }),
            [display](const PropertyList& properties) -> ReturnValue {
                auto url = properties["url"].value<std::string>();
                auto http = Board::GetInstance().GetNetwork()->CreateHttp(3);

                if (!http->Open("GET", url)) {
                    throw std::runtime_error("Không thể mở URL: " + url);
                }
                int status_code = http->GetStatusCode();
                if (status_code != 200) {
                    throw std::runtime_error("Mã trạng thái không mong đợi: " + std::to_string(status_code));
                }

                size_t content_length = http->GetBodyLength();
                char* data = (char*)heap_caps_malloc(content_length, MALLOC_CAP_8BIT);
                if (data == nullptr) {
                    throw std::runtime_error("Không thể cấp phát bộ nhớ cho hình ảnh: " + url);
                }
                size_t total_read = 0;
                while (total_read < content_length) {
                    int ret = http->Read(data + total_read, content_length - total_read);
                    if (ret < 0) {
                        heap_caps_free(data);
                        throw std::runtime_error("Không thể tải xuống hình ảnh: " + url);
                    }
                    if (ret == 0) {
                        break;
                    }
                    total_read += ret;
                }
                http->Close();

                auto image = std::make_unique<LvglAllocatedImage>(data, content_length);
                display->SetPreviewImage(std::move(image));
                return true;
            });
#endif // CONFIG_LV_USE_SNAPSHOT
    }
#endif // HAVE_LVGL

    // URL tải xuống tài sản
    auto& assets = Assets::GetInstance();
    if (assets.partition_valid()) {
        AddUserOnlyTool("self.assets.set_download_url", "Đặt url tải xuống cho tài sản",
            PropertyList({
                Property("url", kPropertyTypeString)
            }),
            [](const PropertyList& properties) -> ReturnValue {
                auto url = properties["url"].value<std::string>();
                Settings settings("assets", true);
                settings.SetString("download_url", url);
                return true;
            });
    }
}

void McpServer::AddTool(McpTool* tool) {
    // Ngăn chặn việc thêm công cụ trùng lặp
    if (std::find_if(tools_.begin(), tools_.end(), [tool](const McpTool* t) { return t->name() == tool->name(); }) != tools_.end()) {
        ESP_LOGW(TAG, "Công cụ %s đã được thêm", tool->name().c_str());
        return;
    }

    ESP_LOGI(TAG, "Thêm công cụ: %s%s", tool->name().c_str(), tool->user_only() ? " [người dùng]" : "");
    tools_.push_back(tool);
}

void McpServer::AddTool(const std::string& name, const std::string& description, const PropertyList& properties, std::function<ReturnValue(const PropertyList&)> callback) {
    AddTool(new McpTool(name, description, properties, callback));
}

void McpServer::AddUserOnlyTool(const std::string& name, const std::string& description, const PropertyList& properties, std::function<ReturnValue(const PropertyList&)> callback) {
    auto tool = new McpTool(name, description, properties, callback);
    tool->set_user_only(true);
    AddTool(tool);
}

void McpServer::ParseMessage(const std::string& message) {
    cJSON* json = cJSON_Parse(message.c_str());
    if (json == nullptr) {
        ESP_LOGE(TAG, "Không thể phân tích tin nhắn MCP: %s", message.c_str());
        return;
    }
    ParseMessage(json);
    cJSON_Delete(json);
}

void McpServer::ParseCapabilities(const cJSON* capabilities) {
    auto vision = cJSON_GetObjectItem(capabilities, "vision");
    if (cJSON_IsObject(vision)) {
        auto url = cJSON_GetObjectItem(vision, "url");
        auto token = cJSON_GetObjectItem(vision, "token");
        if (cJSON_IsString(url)) {
            auto camera = Board::GetInstance().GetCamera();
            if (camera) {
                std::string url_str = std::string(url->valuestring);
                std::string token_str;
                if (cJSON_IsString(token)) {
                    token_str = std::string(token->valuestring);
                }
                camera->SetExplainUrl(url_str, token_str);
            }
        }
    }
}

void McpServer::ParseMessage(const cJSON* json) {
    // Kiểm tra phiên bản JSONRPC
    auto version = cJSON_GetObjectItem(json, "jsonrpc");
    if (version == nullptr || !cJSON_IsString(version) || strcmp(version->valuestring, "2.0") != 0) {
        ESP_LOGE(TAG, "Phiên bản JSONRPC không hợp lệ: %s", version ? version->valuestring : "null");
        return;
    }
    
    // Kiểm tra phương thức
    auto method = cJSON_GetObjectItem(json, "method");
    if (method == nullptr || !cJSON_IsString(method)) {
        ESP_LOGE(TAG, "Thiếu phương thức");
        return;
    }
    
    auto method_str = std::string(method->valuestring);
    if (method_str.find("notifications") == 0) {
        return;
    }
    
    // Kiểm tra tham số
    auto params = cJSON_GetObjectItem(json, "params");
    if (params != nullptr && !cJSON_IsObject(params)) {
        ESP_LOGE(TAG, "Tham số không hợp lệ cho phương thức: %s", method_str.c_str());
        return;
    }

    auto id = cJSON_GetObjectItem(json, "id");
    if (id == nullptr || !cJSON_IsNumber(id)) {
        ESP_LOGE(TAG, "ID không hợp lệ cho phương thức: %s", method_str.c_str());
        return;
    }
    auto id_int = id->valueint;
    
    if (method_str == "initialize") {
        if (cJSON_IsObject(params)) {
            auto capabilities = cJSON_GetObjectItem(params, "capabilities");
            if (cJSON_IsObject(capabilities)) {
                ParseCapabilities(capabilities);
            }
        }
        auto app_desc = esp_app_get_description();
        std::string message = "{\"protocolVersion\":\"2024-11-05\",\"capabilities\":{\"tools\":{}},\"serverInfo\":{\"name\":\"" BOARD_NAME "\",\"version\":\"";
        message += app_desc->version;
        message += "\"}}";
        ReplyResult(id_int, message);
    } else if (method_str == "tools/list") {
        std::string cursor_str = "";
        bool list_user_only_tools = false;
        if (params != nullptr) {
            auto cursor = cJSON_GetObjectItem(params, "cursor");
            if (cJSON_IsString(cursor)) {
                cursor_str = std::string(cursor->valuestring);
            }
            auto with_user_tools = cJSON_GetObjectItem(params, "withUserTools");
            if (cJSON_IsBool(with_user_tools)) {
                list_user_only_tools = with_user_tools->valueint == 1;
            }
        }
        GetToolsList(id_int, cursor_str, list_user_only_tools);
    } else if (method_str == "tools/call") {
        if (!cJSON_IsObject(params)) {
            ESP_LOGE(TAG, "tools/call: Thiếu tham số");
            ReplyError(id_int, "Thiếu tham số");
            return;
        }
        auto tool_name = cJSON_GetObjectItem(params, "name");
        if (!cJSON_IsString(tool_name)) {
            ESP_LOGE(TAG, "tools/call: Thiếu tên");
            ReplyError(id_int, "Thiếu tên");
            return;
        }
        auto tool_arguments = cJSON_GetObjectItem(params, "arguments");
        if (tool_arguments != nullptr && !cJSON_IsObject(tool_arguments)) {
            ESP_LOGE(TAG, "tools/call: Tham số không hợp lệ");
            ReplyError(id_int, "Tham số không hợp lệ");
            return;
        }
        DoToolCall(id_int, std::string(tool_name->valuestring), tool_arguments);
    } else {
        ESP_LOGE(TAG, "Phương thức chưa được triển khai: %s", method_str.c_str());
        ReplyError(id_int, "Phương thức chưa được triển khai: " + method_str);
    }
}

void McpServer::ReplyResult(int id, const std::string& result) {
    std::string payload = "{\"jsonrpc\":\"2.0\",\"id\":";
    payload += std::to_string(id) + ",\"result\":";
    payload += result;
    payload += "}";
    Application::GetInstance().SendMcpMessage(payload);
}

void McpServer::ReplyError(int id, const std::string& message) {
    std::string payload = "{\"jsonrpc\":\"2.0\",\"id\":";
    payload += std::to_string(id);
    payload += ",\"error\":{\"message\":\"";
    payload += message;
    payload += "\"}}";
    Application::GetInstance().SendMcpMessage(payload);
}

void McpServer::GetToolsList(int id, const std::string& cursor, bool list_user_only_tools) {
    const int max_payload_size = 8000;
    std::string json = "{\"tools\":[";
    
    bool found_cursor = cursor.empty();
    auto it = tools_.begin();
    std::string next_cursor = "";
    
    while (it != tools_.end()) {
        // Nếu chúng ta chưa tìm thấy vị trí bắt đầu, tiếp tục tìm kiếm
        if (!found_cursor) {
            if ((*it)->name() == cursor) {
                found_cursor = true;
            } else {
                ++it;
                continue;
            }
        }

        if (!list_user_only_tools && (*it)->user_only()) {
            ++it;
            continue;
        }
        
        // Thêm công cụ trước khi kiểm tra kích thước
        std::string tool_json = (*it)->to_json() + ",";
        if (json.length() + tool_json.length() + 30 > max_payload_size) {
            // Nếu thêm công cụ này sẽ vượt quá giới hạn kích thước, đặt next_cursor và thoát khỏi vòng lặp
            next_cursor = (*it)->name();
            break;
        }
        
        json += tool_json;
        ++it;
    }
    
    if (json.back() == ',') {
        json.pop_back();
    }
    
    if (json.back() == '[' && !tools_.empty()) {
        // Nếu không thêm được công cụ nào, trả về lỗi
        ESP_LOGE(TAG, "tools/list: Không thể thêm công cụ %s vì giới hạn kích thước payload", next_cursor.c_str());
        ReplyError(id, "Không thể thêm công cụ " + next_cursor + " vì giới hạn kích thước payload");
        return;
    }

    if (next_cursor.empty()) {
        json += "]}";
    } else {
        json += "],\"nextCursor\":\"" + next_cursor + "\"}";
    }
    
    ReplyResult(id, json);
}

void McpServer::DoToolCall(int id, const std::string& tool_name, const cJSON* tool_arguments) {
    auto tool_iter = std::find_if(tools_.begin(), tools_.end(), 
                                 [&tool_name](const McpTool* tool) { 
                                     return tool->name() == tool_name; 
                                 });
    
    if (tool_iter == tools_.end()) {
        ESP_LOGE(TAG, "tools/call: Công cụ không xác định: %s", tool_name.c_str());
        ReplyError(id, "Công cụ không xác định: " + tool_name);
        return;
    }

    PropertyList arguments = (*tool_iter)->properties();
    try {
        for (auto& argument : arguments) {
            bool found = false;
            if (cJSON_IsObject(tool_arguments)) {
                auto value = cJSON_GetObjectItem(tool_arguments, argument.name().c_str());
                if (argument.type() == kPropertyTypeBoolean && cJSON_IsBool(value)) {
                    argument.set_value<bool>(value->valueint == 1);
                    found = true;
                } else if (argument.type() == kPropertyTypeInteger && cJSON_IsNumber(value)) {
                    argument.set_value<int>(value->valueint);
                    found = true;
                } else if (argument.type() == kPropertyTypeString && cJSON_IsString(value)) {
                    argument.set_value<std::string>(value->valuestring);
                    found = true;
                }
            }

            if (!argument.has_default_value() && !found) {
                ESP_LOGE(TAG, "tools/call: Thiếu tham số hợp lệ: %s", argument.name().c_str());
                ReplyError(id, "Thiếu tham số hợp lệ: " + argument.name());
                return;
            }
        }
    } catch (const std::exception& e) {
        ESP_LOGE(TAG, "tools/call: %s", e.what());
        ReplyError(id, e.what());
        return;
    }

    // Sử dụng luồng chính để gọi công cụ
    auto& app = Application::GetInstance();
    app.Schedule([this, id, tool_iter, arguments = std::move(arguments)]() {
        try {
            ReplyResult(id, (*tool_iter)->Call(arguments));
        } catch (const std::exception& e) {
            ESP_LOGE(TAG, "tools/call: %s", e.what());
            ReplyError(id, e.what());
        }
    });
}