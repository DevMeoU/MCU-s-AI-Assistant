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

    // Launch the application
    auto& app = Application::GetInstance();
    ESP_LOGI(TAG, "Application started");
    app.Start();
}
