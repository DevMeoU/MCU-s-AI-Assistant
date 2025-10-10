#ifndef BOARD_H
#define BOARD_H

#include <http.h>
#include <web_socket.h>
#include <mqtt.h>
#include <udp.h>
#include <string>
#include <network_interface.h>

#include "led/led.h"
#include "backlight.h"
#include "camera.h"
#include "assets.h"
#include "music.h"

// Bao gồm esp_heap_caps.h để sử dụng các hàm PSRAM và esp_ptr_external_ram
#include <esp_heap_caps.h>

void* create_board();

class AudioCodec;
class Display;
class Board {
private:
    Board(const Board&) = delete; // Vô hiệu hóa hàm tạo sao chép
    Board& operator=(const Board&) = delete; // Vô hiệu hóa phép gán

protected:
    Board();
    std::string GenerateUuid();

    // Định danh duy nhất của thiết bị được tạo bởi phần mềm
    std::string uuid_;

    Music* _music_player = nullptr;
    
    // Phương thức dọn dẹp tài nguyên
    void Cleanup();

public:
    static Board& GetInstance() {
        static Board* instance = static_cast<Board*>(create_board());
        return *instance;
    }
    
    virtual ~Board() = default;
    virtual std::string GetBoardType() = 0;
    virtual std::string GetUuid() { return uuid_; }
    virtual Backlight* GetBacklight() { return nullptr; }
    virtual Led* GetLed();
    virtual AudioCodec* GetAudioCodec() = 0;
    virtual bool GetTemperature(float& esp32temp);
    virtual Display* GetDisplay();
    virtual Camera* GetCamera();
    virtual Music* GetMusicPlayer();
    virtual NetworkInterface* GetNetwork() = 0;
    virtual void StartNetwork() = 0;
    virtual const char* GetNetworkStateIcon() = 0;
    virtual bool GetBatteryLevel(int &level, bool& charging, bool& discharging);
    virtual std::string GetSystemInfoJson();
    virtual void SetPowerSaveMode(bool enabled) = 0;
    virtual std::string GetBoardJson() = 0;
    virtual std::string GetDeviceStatusJson() = 0;
};

#define DECLARE_BOARD(BOARD_CLASS_NAME) \
void* create_board() { \
    return new BOARD_CLASS_NAME(); \
}

#endif // BOARD_H