#include "board.h"
#include <esp_log.h>
#include <esp_heap_caps.h>
#include <esp_random.h>
#include <esp_chip_info.h>
#include <esp_app_desc.h>
#include <esp_partition.h>
#include <esp_ota_ops.h>
#include "system_info.h"
#include "assets/lang_config.h"
#include "display.h"
#include "oled_display.h"
#include "esp32_music.h"

static const char* TAG = "Board";

Board::Board() : uuid_(GenerateUuid()) {
    // Kiểm tra xem PSRAM có được bật không
    if (SystemInfo::GetPsramSize() > 0) {
        ESP_LOGI(TAG, "PSRAM detected, allocating music player in PSRAM");
        // Phân bổ music player trong PSRAM
        _music_player = (Music*)heap_caps_malloc(sizeof(Esp32Music), MALLOC_CAP_SPIRAM);
        if (_music_player) {
            // Gọi hàm tạo trực tiếp trên bộ nhớ đã phân bổ
            new(_music_player) Esp32Music();
        } else {
            ESP_LOGE(TAG, "Failed to allocate music player in PSRAM, falling back to normal allocation");
            _music_player = new Esp32Music();
        }
    } else {
        ESP_LOGI(TAG, "No PSRAM detected, using normal allocation for music player");
        _music_player = new Esp32Music();
    }
}

void Board::Cleanup() {
    if (_music_player) {
        // Kiểm tra xem đối tượng có được phân bổ trong PSRAM không
        if (esp_ptr_external_ram(_music_player)) {
            // Gọi destructor thủ công cho đối tượng trong PSRAM
            _music_player->~Music();
            // Giải phóng bộ nhớ PSRAM
            heap_caps_free(_music_player);
            ESP_LOGI(TAG, "Music player in PSRAM destroyed");
        } else {
            // Xóa đối tượng được phân bổ thông thường
            delete _music_player;
            ESP_LOGI(TAG, "Music player destroyed");
        }
        _music_player = nullptr;
    }
}

std::string Board::GenerateUuid() {
    // UUID v4 cần 16 byte dữ liệu ngẫu nhiên
    uint8_t uuid[16];
    
    // Sử dụng bộ phát sinh số ngẫu nhiên phần cứng của ESP32
    esp_fill_random(uuid, sizeof(uuid));
    
    // Đặt phiên bản (phiên bản 4) và bit biến thể
    uuid[6] = (uuid[6] & 0x0F) | 0x40;    // Phiên bản 4
    uuid[8] = (uuid[8] & 0x3F) | 0x80;    // Biến thể 1
    
    // Chuyển đổi byte thành định dạng chuỗi UUID tiêu chuẩn
    char uuid_str[37];
    snprintf(uuid_str, sizeof(uuid_str),
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        uuid[0], uuid[1], uuid[2], uuid[3],
        uuid[4], uuid[5], uuid[6], uuid[7],
        uuid[8], uuid[9], uuid[10], uuid[11],
        uuid[12], uuid[13], uuid[14], uuid[15]);
    
    return std::string(uuid_str);
}

bool Board::GetBatteryLevel(int &level, bool& charging, bool& discharging) {
    return false;
}

bool Board::GetTemperature(float& esp32temp){
    return false;
}

Display* Board::GetDisplay() {
    static NoDisplay display;
    return &display;
}

Camera* Board::GetCamera() {
    return nullptr;
}

Music* Board::GetMusicPlayer() {
    return _music_player;
}

Led* Board::GetLed() {
    static NoLed led;
    return &led;
}

std::string Board::GetSystemInfoJson() {
    /* 
        {
            "version": 2,
            "flash_size": 4194304,
            "psram_size": 0,
            "minimum_free_heap_size": 123456,
            "mac_address": "00:00:00:00:00:00",
            "uuid": "00000000-0000-0000-0000-000000000000",
            "chip_model_name": "esp32s3",
            "chip_info": {
                "model": 1,
                "cores": 2,
                "revision": 0,
                "features": 0
            },
            "application": {
                "name": "my-app",
                "version": "1.0.0",
                "compile_time": "2021-01-01T00:00:00Z"
                "idf_version": "4.2-dev"
                "elf_sha256": ""
            },
            "partition_table": [
                "app": {
                    "label": "app",
                    "type": 1,
                    "subtype": 2,
                    "address": 0x10000,
                    "size": 0x100000
                }
            ],
            "ota": {
                "label": "ota_0"
            },
            "board": {
                ...
            }
        }
    */
    std::string json = R"({"version":2,"language":")" + std::string(Lang::CODE) + R"(",)";
    json += R"("flash_size":)" + std::to_string(SystemInfo::GetFlashSize()) + R"(,)";
    json += R"("minimum_free_heap_size":")" + std::to_string(SystemInfo::GetMinimumFreeHeapSize()) + R"(",)";
    json += R"("mac_address":")" + SystemInfo::GetMacAddress() + R"(",)";
    json += R"("uuid":")" + uuid_ + R"(",)";
    json += R"("chip_model_name":")" + SystemInfo::GetChipModelName() + R"(",)";

    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    json += R"("chip_info":{)";
    json += R"("model":)" + std::to_string(chip_info.model) + R"(,)";
    json += R"("cores":)" + std::to_string(chip_info.cores) + R"(,)";
    json += R"("revision":)" + std::to_string(chip_info.revision) + R"(,)";
    json += R"("features":)" + std::to_string(chip_info.features) + R"(},)";

    auto app_desc = esp_app_get_description();
    json += R"("application":{)";
    json += R"("name":")" + std::string(app_desc->project_name) + R"(",)";
    json += R"("version":")" + std::string(app_desc->version) + R"(",)";
    json += R"("compile_time":")" + std::string(app_desc->date) + R"(T)" + std::string(app_desc->time) + R"(Z",)";
    json += R"("idf_version":")" + std::string(app_desc->idf_ver) + R"(",)";
    char sha256_str[65];
    for (int i = 0; i < 32; i++) {
        snprintf(sha256_str + i * 2, sizeof(sha256_str) - i * 2, "%02x", app_desc->app_elf_sha256[i]);
    }
    json += R"("elf_sha256":")" + std::string(sha256_str) + R"(")";
    json += R"(},)";

    json += R"("partition_table": [)";
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);
    while (it) {
        const esp_partition_t *partition = esp_partition_get(it);
        json += R"({)";
        json += R"("label":")" + std::string(partition->label) + R"(",)";
        json += R"("type":)" + std::to_string(partition->type) + R"(,)";
        json += R"("subtype":)" + std::to_string(partition->subtype) + R"(,)";
        json += R"("address":)" + std::to_string(partition->address) + R"(,)";
        json += R"("size":)" + std::to_string(partition->size) + R"(},)";;
        it = esp_partition_next(it);
    }
    // Xóa dấu phẩy cuối cùng
    json.pop_back(); 
    json += R"(],)";

    json += R"("ota":{)";
    auto ota_partition = esp_ota_get_running_partition();
    json += R"("label":")" + std::string(ota_partition->label) + R"(")";
    json += R"(},)";

    // Thêm thông tin hiển thị
    auto display = GetDisplay();
    if (display) {
        json += R"("display":{)";
        if (dynamic_cast<OledDisplay*>(display)) {
            json += R"("monochrome":)" + std::string("true") + R"(,)";
        } else {
            json += R"("monochrome":)" + std::string("false") + R"(,)";
        }
        json += R"("width":)" + std::to_string(display->width()) + R"(,)";
        json += R"("height":)" + std::to_string(display->height()) + R"(,)";
        // Xóa dấu phẩy cuối cùng
        json.pop_back(); 
    }
    json += R"(},)";

    json += R"("board":)" + GetBoardJson();

    // Đóng đối tượng JSON
    json += R"(})";
    return json;
}
