#ifndef DUAL_NETWORK_BOARD_H
#define DUAL_NETWORK_BOARD_H

#include "board.h"
#include "wifi_board.h"
#include "ml307_board.h"
#include <memory>

//enum NetworkType
enum class NetworkType {
    WIFI,
    ML307
};

// Lớp bo mạch mạng kép, có thể chuyển đổi giữa WiFi và ML307
class DualNetworkBoard : public Board {
private:
    // Sử dụng con trỏ lớp cơ sở để lưu trữ bo mạch hoạt động hiện tại
    std::unique_ptr<Board> current_board_;
    NetworkType network_type_ = NetworkType::ML307;  // Default to ML307

    // Cấu hình chân ML307
    gpio_num_t ml307_tx_pin_;
    gpio_num_t ml307_rx_pin_;
    gpio_num_t ml307_dtr_pin_;
    
    // Tải loại mạng từ Cài đặt
    NetworkType LoadNetworkTypeFromSettings(int32_t default_net_type);
    
    // Lưu loại mạng vào Cài đặt
    void SaveNetworkTypeToSettings(NetworkType type);

    // Khởi tạo bo mạch tương ứng với loại mạng hiện tại
    void InitializeCurrentBoard();
 
public:
    DualNetworkBoard(gpio_num_t ml307_tx_pin, gpio_num_t ml307_rx_pin, gpio_num_t ml307_dtr_pin = GPIO_NUM_NC, int32_t default_net_type = 1);
    virtual ~DualNetworkBoard() = default;
 
    // Chuyển đổi loại mạng
    void SwitchNetworkType();
    
    // Lấy loại mạng hiện tại
    NetworkType GetNetworkType() const { return network_type_; }
    
    // Lấy tham chiếu bo mạch hoạt động hiện tại
    Board& GetCurrentBoard() const { return *current_board_; }
    
    // Ghi đè giao diện Board
    virtual std::string GetBoardType() override;
    virtual void StartNetwork() override;
    virtual NetworkInterface* GetNetwork() override;
    virtual const char* GetNetworkStateIcon() override;
    virtual void SetPowerSaveMode(bool enabled) override;
    virtual std::string GetBoardJson() override;
    virtual std::string GetDeviceStatusJson() override;
};

#endif // DUAL_NETWORK_BOARD_H