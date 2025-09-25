# Custom Development Board Guide

This guide describes how to customize a new development board initialization program for the Xiaozhi AI voice chatbot project. Xiaozhi AI supports more than 70 types of ESP32 series development boards, and the initialization code for each development board is placed in its corresponding directory.

## Important Notes

> **Warning**: For custom development boards, when the IO configuration is different from the original development board, do not directly overwrite the configuration of the original development board to compile the firmware. You must create a new development board type, or distinguish it by configuring different names and sdkconfig macros in the `builds` configuration in the `config.json` file. Use `python scripts/release.py [development_board_directory_name]` to compile and package the firmware.
>
> If you directly overwrite the original configuration, your custom firmware may be overwritten by the standard firmware of the original development board during future OTA upgrades, causing your device to malfunction. Each development board has a unique identifier and corresponding firmware upgrade channel, so maintaining the uniqueness of the development board identifier is very important.

## Directory Structure

The directory structure of each development board usually includes the following files:

- `xxx_board.cc` - The main board-level initialization code, which implements board-related initialization and functions.
- `config.h` - Board-level configuration file, defining hardware pin mappings and other configuration items.
- `config.json` - Compilation configuration, specifying the target chip and special compilation options.
- `README.md` - Documentation related to the development board.

## Steps to Customize a Development Board

### 1. Create a New Development Board Directory

First, create a new directory under `boards/`, for example, `my-custom-board/`:

```bash
mkdir main/boards/my-custom-board
```

### 2. Create Configuration Files

#### config.h

Define all hardware configurations in `config.h`, including:

- Audio sampling rate and I2S pin configuration.
- Audio codec chip address and I2C pin configuration.
- Button and LED pin configuration.
- Display parameters and pin configuration.

Reference example (from lichuang-c3-dev):

```c
#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

// Audio configuration
#define AUDIO_INPUT_SAMPLE_RATE  24000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

#define AUDIO_I2S_GPIO_MCLK GPIO_NUM_10
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_12
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_8
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_7
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_11

#define AUDIO_CODEC_PA_PIN       GPIO_NUM_13
#define AUDIO_CODEC_I2C_SDA_PIN  GPIO_NUM_0
#define AUDIO_CODEC_I2C_SCL_PIN  GPIO_NUM_1
#define AUDIO_CODEC_ES8311_ADDR  ES8311_CODEC_DEFAULT_ADDR

// Button configuration
#define BOOT_BUTTON_GPIO        GPIO_NUM_9

// Display configuration
#define DISPLAY_SPI_SCK_PIN     GPIO_NUM_3
#define DISPLAY_SPI_MOSI_PIN    GPIO_NUM_5
#define DISPLAY_DC_PIN          GPIO_NUM_6
#define DISPLAY_SPI_CS_PIN      GPIO_NUM_4

#define DISPLAY_WIDTH   320
#define DISPLAY_HEIGHT  240
#define DISPLAY_MIRROR_X true
#define DISPLAY_MIRROR_Y false
#define DISPLAY_SWAP_XY true

#define DISPLAY_OFFSET_X  0
#define DISPLAY_OFFSET_Y  0

#define DISPLAY_BACKLIGHT_PIN GPIO_NUM_2
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT true

#endif // _BOARD_CONFIG_H_
```

#### config.json

Define compilation configurations in `config.json`:

```json
{
    "target": "esp32s3",  // Target chip model: esp32, esp32s3, esp32c3, etc.
    "builds": [
        {
            "name": "my-custom-board",  // Development board name
            "sdkconfig_append": [
                // Additional required compilation configurations
                "CONFIG_ESPTOOLPY_FLASHSIZE_8MB=y",
                "CONFIG_PARTITION_TABLE_CUSTOM_FILENAME=\"partitions/v2/8m.csv\""
            ]
        }
    ]
}
```

### 3. Write Board-Level Initialization Code

Create a `my_custom_board.cc` file to implement all initialization logic for the development board.

A basic development board class definition includes the following parts:

1. **Class Definition**: Inherits from `WifiBoard` or `Ml307Board`.
2. **Initialization Function**: Includes initialization of components such as I2C, display, buttons, IoT, etc.
3. **Virtual Function Overrides**: Such as `GetAudioCodec()`, `GetDisplay()`, `GetBacklight()`, etc.
4. **Register Development Board**: Use the `DECLARE_BOARD` macro to register the development board.

```cpp
#include "wifi_board.h"
#include "codecs/es8311_audio_codec.h"
#include "display/lcd_display.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "mcp_server.h"

#include <esp_log.h>
#include <driver/i2c_master.h>
#include <driver/spi_common.h>

#define TAG "MyCustomBoard"

class MyCustomBoard : public WifiBoard {
private:
    i2c_master_bus_handle_t codec_i2c_bus_;
    Button boot_button_;
    LcdDisplay* display_;

    // I2C initialization
    void InitializeI2c() {
        i2c_master_bus_config_t i2c_bus_cfg = {
            .i2c_port = I2C_NUM_0,
            .sda_io_num = AUDIO_CODEC_I2C_SDA_PIN,
            .scl_io_num = AUDIO_CODEC_I2C_SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {
                .enable_internal_pullup = 1,
            },
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &codec_i2c_bus_));
    }

    // SPI initialization (for display)
    void InitializeSpi() {
        spi_bus_config_t buscfg = {};
        buscfg.mosi_io_num = DISPLAY_SPI_MOSI_PIN;
        buscfg.miso_io_num = GPIO_NUM_NC;
        buscfg.sclk_io_num = DISPLAY_SPI_SCK_PIN;
        buscfg.quadwp_io_num = GPIO_NUM_NC;
        buscfg.quadhd_io_num = GPIO_NUM_NC;
        buscfg.max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t);
        ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    }

    // Button initialization
    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting && !WifiStation::GetInstance().IsConnected()) {
                ResetWifiConfiguration();
            }
            app.ToggleChatState();
        });
    }

    // Display initialization (e.g., ST7789)
    void InitializeDisplay() {
        esp_lcd_panel_io_handle_t panel_io = nullptr;
        esp_lcd_panel_handle_t panel = nullptr;
        
        esp_lcd_panel_io_spi_config_t io_config = {};
        io_config.cs_gpio_num = DISPLAY_SPI_CS_PIN;
        io_config.dc_gpio_num = DISPLAY_DC_PIN;
        io_config.spi_mode = 2;
        io_config.pclk_hz = 80 * 1000 * 1000;
        io_config.trans_queue_depth = 10;
        io_config.lcd_cmd_bits = 8;
        io_config.lcd_param_bits = 8;
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(SPI2_HOST, &io_config, &panel_io));

        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = GPIO_NUM_NC;
        panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB;
        panel_config.bits_per_pixel = 16;
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(panel_io, &panel_config, &panel));
        
        esp_lcd_panel_reset(panel);
        esp_lcd_panel_init(panel);
        esp_lcd_panel_invert_color(panel, true);
        esp_lcd_panel_swap_xy(panel, DISPLAY_SWAP_XY);
        esp_lcd_panel_mirror(panel, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);
        
        // Create display object
        display_ = new SpiLcdDisplay(panel_io, panel,
                                    DISPLAY_WIDTH, DISPLAY_HEIGHT, 
                                    DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y, 
                                    DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y, DISPLAY_SWAP_XY);
    }

    // MCP Tools initialization
    void InitializeTools() {
        // Refer to MCP documentation
    }

public:
    // Constructor
    MyCustomBoard() : boot_button_(BOOT_BUTTON_GPIO) {
        InitializeI2c();
        InitializeSpi();
        InitializeDisplay();
        InitializeButtons();
        InitializeTools();
        GetBacklight()->SetBrightness(100);
    }

    // Get audio codec
    virtual AudioCodec* GetAudioCodec() override {
        static Es8311AudioCodec audio_codec(
            codec_i2c_bus_, 
            I2C_NUM_0, 
            AUDIO_INPUT_SAMPLE_RATE, 
            AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_MCLK, 
            AUDIO_I2S_GPIO_BCLK, 
            AUDIO_I2S_GPIO_WS, 
            AUDIO_I2S_GPIO_DOUT, 
            AUDIO_I2S_GPIO_DIN,
            AUDIO_CODEC_PA_PIN, 
            AUDIO_CODEC_ES8311_ADDR);
        return &audio_codec;
    }

    // Get display
    virtual Display* GetDisplay() override {
        return display_;
    }
    
    // Get backlight control
    virtual Backlight* GetBacklight() override {
        static PwmBacklight backlight(DISPLAY_BACKLIGHT_PIN, DISPLAY_BACKLIGHT_OUTPUT_INVERT);
        return &backlight;
    }
};

// Register development board
DECLARE_BOARD(MyCustomBoard);
```

### 4. Create README.md

In README.md, explain the features, hardware requirements, compilation, and flashing steps for the development board:


## Common Development Board Components

### 1. Display

The project supports various display drivers, including:
- ST7789 (SPI)
- ILI9341 (SPI)
- SH8601 (QSPI)
- etc...

### 2. Audio Codec

Supported codecs include:
- ES8311 (Commonly used)
- ES7210 (Microphone array)
- AW88298 (Power amplifier)
- etc...

### 3. Power Management

Some development boards use power management chips:
- AXP2101
- Other available PMICs

### 4. MCP Device Control

Various MCP tools can be added for AI to use:
- Speaker (Speaker control)
- Screen (Screen brightness adjustment)
- Battery (Battery level reading)
- Light (Light control)
- etc...

## Development Board Class Inheritance

- `Board` - Base board-level class
  - `WifiBoard` - Development board with Wi-Fi connection
  - `Ml307Board` - Development board using 4G module
  - `DualNetworkBoard` - Development board supporting Wi-Fi and 4G network switching

## Development Tips

1. **Refer to similar development boards**: If your new development board is similar to an existing one, you can refer to the existing implementation.
2. **Step-by-step debugging**: First implement basic functions (e.g., display), then add more complex functions (e.g., audio).
3. **Pin mapping**: Ensure all pin mappings are correctly configured in config.h.
4. **Check hardware compatibility**: Confirm compatibility of all chips and drivers.

## Possible Issues

1. **Display abnormal**: Check SPI configuration, mirror settings, and color inversion settings.
2. **No audio output**: Check I2S configuration, PA enable pin, and codec address.
3. **Unable to connect to network**: Check Wi-Fi credentials and network configuration.
4. **Unable to communicate with server**: Check MQTT or WebSocket configuration.

## References

- ESP-IDF Documentation: https://docs.espressif.com/projects/esp-idf/
- LVGL Documentation: https://docs.lvgl.io/
- ESP-SR Documentation: https://github.com/espressif/esp-sr 

# Hướng dẫn bảng phát triển tùy chỉnh

Hướng dẫn này mô tả cách tùy chỉnh chương trình khởi tạo bảng phát triển mới cho dự án chatbot giọng nói AI Xiaozhi. Xiaozhi AI hỗ trợ hơn 70 loại bảng phát triển dòng ESP32, và mã khởi tạo cho mỗi bảng phát triển được đặt trong thư mục tương ứng của nó.

## Lưu ý quan trọng

> **Cảnh báo**: Đối với các bảng phát triển tùy chỉnh, khi cấu hình IO khác với bảng phát triển gốc, không được trực tiếp ghi đè cấu hình của bảng phát triển gốc để biên dịch firmware. Bạn phải tạo một loại bảng phát triển mới, hoặc phân biệt nó bằng cách cấu hình các tên và macro sdkconfig khác nhau trong cấu hình `builds` trong tệp `config.json`. Sử dụng `python scripts/release.py [tên_thư_mục_bảng_phát_triển]` để biên dịch và đóng gói firmware.
>
> Nếu bạn trực tiếp ghi đè cấu hình gốc, firmware tùy chỉnh của bạn có thể bị ghi đè bởi firmware tiêu chuẩn của bảng phát triển gốc trong quá trình nâng cấp OTA trong tương lai, khiến thiết bị của bạn không hoạt động bình thường. Mỗi bảng phát triển có một định danh duy nhất và kênh nâng cấp firmware tương ứng, việc duy trì tính duy nhất của định danh bảng phát triển là rất quan trọng.

## Cấu trúc thư mục

Cấu trúc thư mục của mỗi bảng phát triển thường bao gồm các tệp sau:

- `xxx_board.cc` - Mã khởi tạo cấp bảng chính, thực hiện khởi tạo và các chức năng liên quan đến bảng.
- `config.h` - Tệp cấu hình cấp bảng, định nghĩa ánh xạ chân phần cứng và các mục cấu hình khác.
- `config.json` - Cấu hình biên dịch, chỉ định chip mục tiêu và các tùy chọn biên dịch đặc biệt.
- `README.md` - Tài liệu liên quan đến bảng phát triển.

## Các bước để tùy chỉnh bảng phát triển

### 1. Tạo thư mục bảng phát triển mới

Đầu tiên, tạo một thư mục mới trong `boards/`, ví dụ: `my-custom-board/`:

```bash
mkdir main/boards/my-custom-board
```

### 2. Tạo tệp cấu hình

#### config.h

Định nghĩa tất cả các cấu hình phần cứng trong `config.h`, bao gồm:

- Tốc độ lấy mẫu âm thanh và cấu hình chân I2S.
- Địa chỉ chip codec âm thanh và cấu hình chân I2C.
- Cấu hình chân nút và đèn LED.
- Tham số hiển thị và cấu hình chân.

Ví dụ tham khảo (từ lichuang-c3-dev):

```c
#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

// Cấu hình âm thanh
#define AUDIO_INPUT_SAMPLE_RATE  24000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

#define AUDIO_I2S_GPIO_MCLK GPIO_NUM_10
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_12
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_8
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_7
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_11

#define AUDIO_CODEC_PA_PIN       GPIO_NUM_13
#define AUDIO_CODEC_I2C_SDA_PIN  GPIO_NUM_0
#define AUDIO_CODEC_I2C_SCL_PIN  GPIO_NUM_1
#define AUDIO_CODEC_ES8311_ADDR  ES8311_CODEC_DEFAULT_ADDR

// Cấu hình nút
#define BOOT_BUTTON_GPIO        GPIO_NUM_9

// Cấu hình hiển thị
#define DISPLAY_SPI_SCK_PIN     GPIO_NUM_3
#define DISPLAY_SPI_MOSI_PIN    GPIO_NUM_5
#define DISPLAY_DC_PIN          GPIO_NUM_6
#define DISPLAY_SPI_CS_PIN      GPIO_NUM_4

#define DISPLAY_WIDTH   320
#define DISPLAY_HEIGHT  240
#define DISPLAY_MIRROR_X true
#define DISPLAY_MIRROR_Y false
#define DISPLAY_SWAP_XY true

#define DISPLAY_OFFSET_X  0
#define DISPLAY_OFFSET_Y  0

#define DISPLAY_BACKLIGHT_PIN GPIO_NUM_2
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT true

#endif // _BOARD_CONFIG_H_
```

#### config.json

Định nghĩa cấu hình biên dịch trong `config.json`:

```json
{
    "target": "esp32s3",  // Mô hình chip mục tiêu: esp32, esp32s3, esp32c3, v.v.
    "builds": [
        {
            "name": "my-custom-board",  // Tên bảng phát triển
            "sdkconfig_append": [
                // Cấu hình biên dịch bổ sung cần thiết
                "CONFIG_ESPTOOLPY_FLASHSIZE_8MB=y",
                "CONFIG_PARTITION_TABLE_CUSTOM_FILENAME=\"partitions/v2/8m.csv\""
            ]
        }
    ]
}
```

### 3. Viết mã khởi tạo cấp bảng

Tạo tệp `my_custom_board.cc` để triển khai tất cả logic khởi tạo cho bảng phát triển.

Định nghĩa lớp bảng phát triển cơ bản bao gồm các phần sau:

1. **Định nghĩa lớp**: Kế thừa từ `WifiBoard` hoặc `Ml307Board`.
2. **Hàm khởi tạo**: Bao gồm khởi tạo các thành phần như I2C, màn hình, nút, IoT, v.v.
3. **Ghi đè hàm ảo**: Chẳng hạn như `GetAudioCodec()`, `GetDisplay()`, `GetBacklight()`, v.v.
4. **Đăng ký bảng phát triển**: Sử dụng macro `DECLARE_BOARD` để đăng ký bảng phát triển.

```cpp
#include "wifi_board.h"
#include "codecs/es8311_audio_codec.h"
#include "display/lcd_display.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "mcp_server.h"

#include <esp_log.h>
#include <driver/i2c_master.h>
#include <driver/spi_common.h>

#define TAG "MyCustomBoard"

class MyCustomBoard : public WifiBoard {
private:
    i2c_master_bus_handle_t codec_i2c_bus_;
    Button boot_button_;
    LcdDisplay* display_;

    // Khởi tạo I2C
    void InitializeI2c() {
        i2c_master_bus_config_t i2c_bus_cfg = {
            .i2c_port = I2C_NUM_0,
            .sda_io_num = AUDIO_CODEC_I2C_SDA_PIN,
            .scl_io_num = AUDIO_CODEC_I2C_SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {
                .enable_internal_pullup = 1,
            },
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &codec_i2c_bus_));
    }

    // Khởi tạo SPI (cho màn hình)
    void InitializeSpi() {
        spi_bus_config_t buscfg = {};
        buscfg.mosi_io_num = DISPLAY_SPI_MOSI_PIN;
        buscfg.miso_io_num = GPIO_NUM_NC;
        buscfg.sclk_io_num = DISPLAY_SPI_SCK_PIN;
        buscfg.quadwp_io_num = GPIO_NUM_NC;
        buscfg.quadhd_io_num = GPIO_NUM_NC;
        buscfg.max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t);
        ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    }

    // Khởi tạo nút
    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting && !WifiStation::GetInstance().IsConnected()) {
                ResetWifiConfiguration();
            }
            app.ToggleChatState();
        });
    }

    // Khởi tạo màn hình (ví dụ: ST7789)
    void InitializeDisplay() {
        esp_lcd_panel_io_handle_t panel_io = nullptr;
        esp_lcd_panel_handle_t panel = nullptr;
        
        esp_lcd_panel_io_spi_config_t io_config = {};
        io_config.cs_gpio_num = DISPLAY_SPI_CS_PIN;
        io_config.dc_gpio_num = DISPLAY_DC_PIN;
        io_config.spi_mode = 2;
        io_config.pclk_hz = 80 * 1000 * 1000;
        io_config.trans_queue_depth = 10;
        io_config.lcd_cmd_bits = 8;
        io_config.lcd_param_bits = 8;
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(SPI2_HOST, &io_config, &panel_io));

        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = GPIO_NUM_NC;
        panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB;
        panel_config.bits_per_pixel = 16;
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(panel_io, &panel_config, &panel));
        
        esp_lcd_panel_reset(panel);
        esp_lcd_panel_init(panel);
        esp_lcd_panel_invert_color(panel, true);
        esp_lcd_panel_swap_xy(panel, DISPLAY_SWAP_XY);
        esp_lcd_panel_mirror(panel, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);
        
        // Tạo đối tượng hiển thị
        display_ = new SpiLcdDisplay(panel_io, panel,
                                    DISPLAY_WIDTH, DISPLAY_HEIGHT, 
                                    DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y, 
                                    DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y, DISPLAY_SWAP_XY);
    }

    // Khởi tạo công cụ MCP
    void InitializeTools() {
        // Tham khảo tài liệu MCP
    }

public:
    // Hàm tạo
    MyCustomBoard() : boot_button_(BOOT_BUTTON_GPIO) {
        InitializeI2c();
        InitializeSpi();
        InitializeDisplay();
        InitializeButtons();
        InitializeTools();
        GetBacklight()->SetBrightness(100);
    }

    // Lấy codec âm thanh
    virtual AudioCodec* GetAudioCodec() override {
        static Es8311AudioCodec audio_codec(
            codec_i2c_bus_, 
            I2C_NUM_0, 
            AUDIO_INPUT_SAMPLE_RATE, 
            AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_MCLK, 
            AUDIO_I2S_GPIO_BCLK, 
            AUDIO_I2S_GPIO_WS, 
            AUDIO_I2S_GPIO_DOUT, 
            AUDIO_I2S_GPIO_DIN,
            AUDIO_CODEC_PA_PIN, 
            AUDIO_CODEC_ES8311_ADDR);
        return &audio_codec;
    }

    // Lấy màn hình
    virtual Display* GetDisplay() override {
        return display_;
    }
    
    // Lấy điều khiển đèn nền
    virtual Backlight* GetBacklight() override {
        static PwmBacklight backlight(DISPLAY_BACKLIGHT_PIN, DISPLAY_BACKLIGHT_OUTPUT_INVERT);
        return &backlight;
    }
};

// Đăng ký bảng phát triển
DECLARE_BOARD(MyCustomBoard);
```

### 4. Tạo README.md

Trong README.md, giải thích các tính năng, yêu cầu phần cứng, các bước biên dịch và nạp firmware cho bảng phát triển:


## Các thành phần bảng phát triển phổ biến

### 1. Màn hình hiển thị

Dự án hỗ trợ nhiều trình điều khiển màn hình khác nhau, bao gồm:
- ST7789 (SPI)
- ILI9341 (SPI)
- SH8601 (QSPI)
- v.v...

### 2. Bộ giải mã âm thanh

Các bộ giải mã được hỗ trợ bao gồm:
- ES8311 (Thường dùng)
- ES7210 (Mảng micro)
- AW88298 (Bộ khuếch đại công suất)
- v.v...

### 3. Quản lý nguồn

Một số bảng phát triển sử dụng chip quản lý nguồn:
- AXP2101
- Các PMIC khác có sẵn

### 4. Điều khiển thiết bị MCP

Có thể thêm nhiều công cụ MCP khác nhau để AI có thể sử dụng:
- Speaker (Điều khiển loa)
- Screen (Điều chỉnh độ sáng màn hình)
- Battery (Đọc mức pin)
- Light (Điều khiển đèn)
- v.v...

## Quan hệ kế thừa lớp bảng phát triển

- `Board` - Lớp cấp bảng cơ sở
  - `WifiBoard` - Bảng phát triển có kết nối Wi-Fi
  - `Ml307Board` - Bảng phát triển sử dụng mô-đun 4G
  - `DualNetworkBoard` - Bảng phát triển hỗ trợ chuyển đổi mạng Wi-Fi và 4G

## Mẹo phát triển

1. **Tham khảo các bảng phát triển tương tự**: Nếu bảng phát triển mới của bạn tương tự với một bảng hiện có, bạn có thể tham khảo cách triển khai hiện có.
2. **Gỡ lỗi từng bước**: Đầu tiên triển khai các chức năng cơ bản (ví dụ: hiển thị), sau đó thêm các chức năng phức tạp hơn (ví dụ: âm thanh).
3. **Ánh xạ chân**: Đảm bảo tất cả các ánh xạ chân được cấu hình chính xác trong config.h.
4. **Kiểm tra khả năng tương thích phần cứng**: Xác nhận khả năng tương thích của tất cả các chip và trình điều khiển.

## Các vấn đề có thể gặp phải

1. **Màn hình hiển thị bất thường**: Kiểm tra cấu hình SPI, cài đặt gương và cài đặt đảo ngược màu.
2. **Không có đầu ra âm thanh**: Kiểm tra cấu hình I2S, chân kích hoạt PA và địa chỉ codec.
3. **Không thể kết nối mạng**: Kiểm tra thông tin đăng nhập Wi-Fi và cấu hình mạng.
4. **Không thể giao tiếp với máy chủ**: Kiểm tra cấu hình MQTT hoặc WebSocket.

## Tài liệu tham khảo

- Tài liệu ESP-IDF: https://docs.espressif.com/projects/esp-idf/
- Tài liệu LVGL: https://docs.lvgl.io/
- Tài liệu ESP-SR: https://github.com/espressif/esp-sr