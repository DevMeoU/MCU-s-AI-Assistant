#pragma once

#include <libs/gif/lv_gif.h>

#include "display/lcd_display.h"

// Khai báo GIF biểu cảm Electron Bot - Sử dụng 6 biểu cảm giống như Otto
LV_IMAGE_DECLARE(staticstate);  // Trạng thái tĩnh/biểu cảm trung tính
LV_IMAGE_DECLARE(sad);          // Buồn
LV_IMAGE_DECLARE(happy);        // Vui vẻ
LV_IMAGE_DECLARE(scare);        // Giật mình/ngạc nhiên
LV_IMAGE_DECLARE(buxue);        // Không học/lúng túng
LV_IMAGE_DECLARE(anger);        // Tức giận

/**
 * @brief Lớp hiển thị biểu cảm GIF Electron Bot
 * Kế thừa từ LcdDisplay, thêm hỗ trợ biểu cảm GIF
 */
class ElectronEmojiDisplay : public SpiLcdDisplay {
public:
    /**
     * @brief Hàm khởi tạo, tham số giống với SpiLcdDisplay
     */
    ElectronEmojiDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel,
                         int width, int height, int offset_x, int offset_y, bool mirror_x,
                         bool mirror_y, bool swap_xy);

    virtual ~ElectronEmojiDisplay() = default;

    // Ghi đè phương thức thiết lập biểu cảm
    virtual void SetEmotion(const char* emotion) override;

    // Ghi đè phương thức thiết lập tin nhắn trò chuyện
    virtual void SetChatMessage(const char* role, const char* content) override;

private:
    void SetupGifContainer();

    lv_obj_t* emotion_gif_;  ///< Thành phần biểu cảm GIF

    // Ánh xạ biểu cảm
    struct EmotionMap {
        const char* name;
        const lv_image_dsc_t* gif;
    };

    static const EmotionMap emotion_maps_[];
};