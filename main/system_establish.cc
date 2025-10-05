#include "system_establish.h"

#include <esp_log.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <esp_event.h>
#include <lwip/apps/sntp.h> // Added missing include for sntp functions

#define TAG "SystemEstablish"

void SystemEstablish::Init() {
    // Initialize the default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Initialize NVS flash for WiFi configuration
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Erasing NVS flash to fix corruption");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize SNTP with a default callback
    SystemEstablish::InitSNTP(time_sync_notification_cb);

    ESP_LOGI(TAG, "System initialized successfully");
}

void SystemEstablish::Deinit() {
    // Cleanup resources if needed
    esp_event_loop_delete_default();
}

/*
 * @brief Initialize SNTP and set time synchronization callback
 * @param time_sync_callback Callback to be called when time is synchronized
 * @return void
 */
void SystemEstablish::InitSNTP(std::function<void(struct timeval*)> time_sync_callback) {
    // Initialize SNTP
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
    
    // Set the time synchronization callback
    // Note: The original code had a logic error here. We should use sntp_set_time_sync_notification_cb
    // instead of assigning to the parameter.
    if (time_sync_callback) {
        sntp_set_time_sync_notification_cb(time_sync_callback);
    }
}

/* ******************************************************************
 * private method implementations
 * ******************************************************************/

/* 
 * @brief Callback when time is synchronized via SNTP
 * @param tv Current time
 * @return void
 */
void time_sync_notification_cb(struct timeval *tv) {
    ESP_LOGI(TAG, "Time has been synchronized via SNTP");
    // Update system time
    settimeofday(tv, nullptr);
    // Additional actions can be added here when time is synchronized
}
