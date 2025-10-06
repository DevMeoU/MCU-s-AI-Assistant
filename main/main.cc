#include <esp_log.h>
#include <esp_err.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include <esp_event.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "application.h"
#include "system_establish.h"
#include "system_info.h"
#include "memory_management.h"

#define TAG "app_main"

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting MCU's AI Assistant...");
    /* System Init */
    SystemEstablish::Init();

    // Log memory info
    MemoryManager::logMemoryInfo();

    // Allocate internal memory
    ESP_LOGI(TAG, "Allocating internal memory...");
    void* internalPtr = MemoryManager::allocateInternal(1024);
    if (internalPtr == nullptr) {
        ESP_LOGE(TAG, "Failed to allocate internal memory");
    } else {
        ESP_LOGI(TAG, "Successfully allocated internal memory at %p", internalPtr);
    }

    // Allocate PSRAM memory
    ESP_LOGI(TAG, "Allocating PSRAM memory...");
    void* psramPtr = MemoryManager::allocatePsram(1024 * 100);
    if (psramPtr == nullptr) {
        ESP_LOGE(TAG, "Failed to allocate PSRAM memory");
    } else {
        ESP_LOGI(TAG, "Successfully allocated PSRAM memory at %p", psramPtr);
    }

    // Allocate RTC memory
    ESP_LOGI(TAG, "Allocating RTC memory...");
    void* rtcPtr = MemoryManager::allocateRtc();
    if (rtcPtr == nullptr) {
        ESP_LOGE(TAG, "Failed to allocate RTC memory");
    } else {
        ESP_LOGI(TAG, "Successfully allocated RTC memory at %p", rtcPtr);
    }

    // Log memory info after allocation
    ESP_LOGI(TAG, "Memory info after allocation:");
    MemoryManager::logMemoryInfo();
    
    // Free memory
    ESP_LOGI(TAG, "Freeing memory...");
    MemoryManager::freeMemory(internalPtr);
    MemoryManager::freeMemory(psramPtr);
    MemoryManager::freeMemory(rtcPtr);

    // Log memory info after freeing
    ESP_LOGI(TAG, "Memory info after freeing:");
    MemoryManager::logMemoryInfo();

    // Launch the application
    auto& app = Application::GetInstance();
    ESP_LOGI(TAG, "Application started");
    // app.Start();
}
