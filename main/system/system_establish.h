#ifndef _SYSTEM_ESTABLISH_H_
#define _SYSTEM_ESTABLISH_H_

#include <functional>
#include <string>
#include "esp_pm.h"

/**
 * @brief Lớp quản lý khởi tạo và dọn dẹp hệ thống
 */
class SystemEstablish {
public:
    /**
     * @brief Khởi tạo hệ thống
     * Thiết lập event loop, NVS flash và các dịch vụ cơ bản
     */
    static void Init();

    /**
     * @brief Dọn dẹp tài nguyên hệ thống
     */
    static void Deinit();

    /**
     * @brief Khởi tạo đồng bộ thời gian qua SNTP
     * @param time_sync_callback Hàm callback được gọi khi thời gian được đồng bộ
     */
    static void InitSNTP(std::function<void(struct timeval*)> time_sync_callback);
};

class CpuPowerManager {
public:
    CpuPowerManager(int minFreqMhz = 80, int maxFreqMhz = 240, bool lightSleep = true);
    ~CpuPowerManager();

    // Khởi tạo cấu hình PM
    esp_err_t init();

    // Thay đổi tần số hoạt động khi cần
    esp_err_t setFrequency(int minFreqMhz, int maxFreqMhz);

    // Bật/tắt chế độ light sleep
    void enableLightSleep(bool enable);

    // Lấy thông tin cấu hình hiện tại
    std::string getCurrentConfig() const;

    // Không cho phép copy
    CpuPowerManager(const CpuPowerManager&) = delete;
    CpuPowerManager& operator=(const CpuPowerManager&) = delete;

    // Cho phép move nếu cần
    CpuPowerManager(CpuPowerManager&&) = default;
    CpuPowerManager& operator=(CpuPowerManager&&) = default;
private:
// #ifdef ESP_32S3
    esp_pm_config_t pm_config;
    bool initialized;
// #endif
};

#endif // _SYSTEM_ESTABLISH_H_