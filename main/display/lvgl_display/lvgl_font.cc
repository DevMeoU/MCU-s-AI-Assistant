#include "lvgl_font.h"
#include <cbin_font.h>
#include <esp_log.h>

static const char* TAG = "LvglCBinFont";

LvglCBinFont::LvglCBinFont(void* data) : font_(nullptr) {
    if (data == nullptr) {
        ESP_LOGE(TAG, "Invalid font data pointer (null)");
        return;
    }
    
    // Add a check for the magic bytes to ensure this is valid font data
    uint8_t* font_data = static_cast<uint8_t*>(data);
    if (font_data[0] != 'Z' || font_data[1] != 'Z') {
        ESP_LOGE(TAG, "Invalid font data magic bytes: 0x%02x 0x%02x", font_data[0], font_data[1]);
        return;
    }
    
    font_ = cbin_font_create(font_data);
    if (font_ == nullptr) {
        ESP_LOGE(TAG, "Failed to create font from data");
    }
}

LvglCBinFont::~LvglCBinFont() {
    if (font_ != nullptr) {
        cbin_font_delete(font_);
        font_ = nullptr;
    }
}