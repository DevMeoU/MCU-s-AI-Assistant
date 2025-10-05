#include "system_establish.h"

#include <esp_log.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <esp_event.h>
#include <lwip/apps/sntp.h> // Sử dụng lwip SNTP cho ESP-IDF v5.x
#include <sys/time.h> // Thêm cho settimeofday

#define TAG "SystemEstablish"

// Khai báo tiền thân cho hàm callback
static void time_sync_notification_cb(struct timeval *tv);

void SystemEstablish::Init() {
    // Khởi tạo event loop mặc định
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Khởi tạo NVS flash cho cấu hình WiFi
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Xóa NVS flash để khắc phục lỗi");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Khởi tạo SNTP với callback mặc định
    InitSNTP(time_sync_notification_cb);

    ESP_LOGI(TAG, "Khởi tạo hệ thống thành công");
}

void SystemEstablish::Deinit() {
    // Dọn dẹp tài nguyên nếu cần
    esp_event_loop_delete_default();
}

/**
 * @brief Khởi tạo SNTP và thiết lập callback đồng bộ thời gian
 * @param time_sync_callback Hàm callback được gọi khi thời gian được đồng bộ
 */
void SystemEstablish::InitSNTP(std::function<void(struct timeval*)> time_sync_callback) {
    // Cấu hình PM
    CpuPowerManager cpuPM(80, 240, true);
    
    cpuPM.init();
    ESP_LOGI(TAG, "Cấu hình hiện tại: %s", cpuPM.getCurrentConfig().c_str());

    cpuPM.setFrequency(80, 240);
    ESP_LOGI(TAG, "ESP khởi động ở chế độ tiết kiệm");

    // Cấu hình chế độ hoạt động cho SNTP
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    
    // Thiết lập máy chủ NTP
    sntp_setservername(0, "pool.ntp.org");
    
    // Lưu callback để sử dụng sau này nếu cần
    // Lưu ý: Một số phiên bản ESP-IDF v5.x có thể không hỗ trợ callback trực tiếp
    if (time_sync_callback) {
        // TODO: Có thể cần đăng ký sự kiện thay vì dùng callback trực tiếp
        // trong các phiên bản ESP-IDF mới hơn
    }
    
    // Khởi tạo SNTP
    sntp_init();
}

/**
 * @brief Hàm callback khi thời gian được đồng bộ qua SNTP
 * @param tv Thời gian hiện tại
 */
static void time_sync_notification_cb(struct timeval *tv) {
    ESP_LOGI(TAG, "Thời gian đã được đồng bộ qua SNTP");
    // Cập nhật thời gian hệ thống - không cần thiết trong ESP-IDF v5.x vì được xử lý tự động
    // Có thể thêm các hành động bổ sung khi thời gian được đồng bộ
}

static const char* PM_TAG = "CpuPowerManager";
/* 
 * @brief
 * @param
 * @param
 * @param
 * @return
 */

CpuPowerManager::CpuPowerManager(int minFreqMhz, int maxFreqMhz, bool lightSleep)
{
    // Cấu hình tần số hoạt động flex
    pm_config.min_freq_mhz = minFreqMhz;
    pm_config.max_freq_mhz = maxFreqMhz; // Giảm khi không tải
    pm_config.light_sleep_enable = lightSleep; // tiết kiểm điện

    initialized = false;
}

CpuCpuPowerManager::~CpuPowerManager() {}

esp_err_t CpuPowerManager::init()
{
    esp_err_t err = esp_pm_configure(&pm_config);
    if(err == ESP_OK)
    {
        initialized = true;
        ESP_LOGI(PM_TAG, "Power management initialized: %d - %d MHz, light sleep: %s",
            pm_config.min_freq_mhz,
            pm_config.max_freq_mhz,
            pm_config.light_sleep_enable ? "ON" : "OFF"
        );
    }
    else
    {
        ESP_LOGE(PM_TAG, "Failed to configure PM: %s", esp_err_to_name(err));
    }
    return err;
}

esp_err_t CpuPowerManager::setFrequency(int minFreqMhz, int maxFreqMhz)
{
    if(!initialized)
    {
        ESP_LOGW(PM_TAG, "PM not initialized yet");
        return ESP_ERR_INVALID_STATE;
    }
    pm_config.min_freq_mhz = minFreqMhz;
    pm_config.max_freq_mhz = maxFreqMhz; // Giảm khi không tải
    return esp_pm_configure(&pm_config);
}

void CpuPowerManager::enableLightSleep(bool enable) {
    pm_config.light_sleep_enable = enable;
    if(initialized)
    {
        esp_pm_configure(&pm_config);
    }
}

std::string CpuPowerManager::getCurrentConfig() const
{
    char buf[64];
    snprintf(buf, sizeof(buf),
            "Min: %d MHz, Max: % MHx, LightSleep: %s",
            pm_config.min_freq_mhz,
            pm_config.max_freq_mhz,
            pm_config.light_sleep_enable ? "ON" : "OFF"
            );
    return std::string(buf);
}