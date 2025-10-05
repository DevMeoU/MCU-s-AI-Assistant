#include <time.h>
#include <sys/time.h>
#include "esp_log.h"
#include "esp_sntp.h"
#include "esp_timer.h"
#include "esp_sleep.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "alarm.h"

static const char* TAG = "Alarm";

// Timer handle for alarm checking
static esp_timer_handle_t alarm_timer = nullptr;

// Event queue for alarm triggering
static QueueHandle_t alarm_event_queue = nullptr;

Alarm::Alarm(int seconds_to_light_sleep, int seconds_to_deep_sleep)
    : SleepTimer(seconds_to_light_sleep, seconds_to_deep_sleep) {
    // Initialize alarm times array from NVS storage
    alarm_times_.resize(ALARM_MAX_SETTINGS, 0);
    LoadAlarmsFromNVS();
    
    // Create event queue for alarm events
    if (alarm_event_queue == nullptr) {
        alarm_event_queue = xQueueCreate(10, sizeof(int));
    }
    
    // Set up callback to check alarms every second as required
    this->OnEnterLightSleepMode([this]() {
        if (alarm_timer != nullptr) {
            esp_timer_stop(alarm_timer);
            esp_timer_delete(alarm_timer);
            alarm_timer = nullptr;
        }
        
        const esp_timer_create_args_t timer_args = {
            .callback = [](void* arg) {
                Alarm* alarm = static_cast<Alarm*>(arg);
                // Check alarms every second and set flag when triggered
                time_t now = time(nullptr);
                for (int i = 0; i < ALARM_MAX_SETTINGS; i++) {
                    if (alarm->alarm_times_[i] > 0 && now >= alarm->alarm_times_[i]) {
                        // Set flag and store triggered alarm ID
                        alarm->alarm_triggered_ = true;
                        alarm->triggered_alarm_id_ = i;
                        // Reset alarm time
                        alarm->alarm_times_[i] = 0;
                        alarm->SaveAlarmsToNVS();
                        ESP_LOGI(TAG, "Alarm %d triggered!", i);
                        break; // Only trigger one alarm at a time
                    }
                }
            },
            .arg = this,
            .name = "alarm_check_timer"
        };
        esp_timer_create(&timer_args, &alarm_timer);
        // Check every second as required
        esp_timer_start_periodic(alarm_timer, 1 * 1000 * 1000); // every 1 second
    });
    
    this->OnExitLightSleepMode([this]() {
        // Stop and delete timer when exiting light sleep mode
        if (alarm_timer != nullptr) {
            esp_timer_stop(alarm_timer);
            esp_timer_delete(alarm_timer);
            alarm_timer = nullptr;
        }
    });
}

Alarm::~Alarm() {
    // Cancel all timers when object is destroyed
    if (alarm_timer != nullptr) {
        esp_timer_stop(alarm_timer);
        esp_timer_delete(alarm_timer);
        alarm_timer = nullptr;
    }
    
    // Clean up event queue
    if (alarm_event_queue != nullptr) {
        vQueueDelete(alarm_event_queue);
        alarm_event_queue = nullptr;
    }
    
    SaveAlarmsToNVS();
}

bool Alarm::IsAlarmTriggered() const {
    return alarm_triggered_;
}

void Alarm::ClearAlarmTrigger() {
    alarm_triggered_ = false;
    triggered_alarm_id_ = -1;
}

int Alarm::GetTriggeredAlarmId() const {
    return triggered_alarm_id_;
}

void Alarm::ProcessAlarmTrigger() {
    // This should be called from application task context
    // Process alarm events that were sent from timer callback
    int alarm_id;
    while (xQueueReceive(alarm_event_queue, &alarm_id, 0) == pdTRUE) {
        if (alarm_id >= 0 && alarm_id < ALARM_MAX_SETTINGS) {
            if (alarm_times_[alarm_id] > 0) {
                ESP_LOGI(TAG, "Alarm %d triggered!", alarm_id);
                alarm_times_[alarm_id] = 0; // Reset alarm
                SaveAlarmsToNVS(); // Save reset state
                // Perform alarm action (sound, vibration, etc.) in task context
                if (on_alarm_) {
                    on_alarm_();
                }
            }
        }
    }
}

/*
 * @brief Wait for time synchronization via SNTP
 * @param void
 * @return void
 */
// void Alarm::WaitSyncTime(void) {
//     // Wait for time synchronization
//     int retry = 0;
//     const int retry_count = 10;
//     while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && retry < retry_count) {
//         ESP_LOGI(TAG, "Waiting for time synchronization...");
//         vTaskDelay(2000 / portTICK_PERIOD_MS);
//         retry++;
//     }
//     if (retry == retry_count) {
//         ESP_LOGW(TAG, "Time synchronization failed after %d attempts", retry_count);
//     } else {
//         ESP_LOGI(TAG, "Time synchronization successful");
//     }
// }

void Alarm::AlarmSet(int index, int hour, int minute) {
    if (index < 0 || index >= ALARM_MAX_SETTINGS) {
        ESP_LOGW(TAG, "Invalid alarm index: %d", index);
        return;
    }
    
    // Get current time
    time_t now = time(nullptr);
    struct tm timeinfo = {0};
    localtime_r(&now, &timeinfo);
    
    // Set hour and minute
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = 0;
    
    // Convert back to time_t
    time_t alarm_time = mktime(&timeinfo);
    
    // If alarm time has passed, set for next day
    if (alarm_time <= now) {
        alarm_time += 24 * 3600; // add one day
    }
    
    alarm_times_[index] = alarm_time;
    SaveAlarmsToNVS(); // Save to NVS
    ESP_LOGI(TAG, "Alarm %d set for %s", index, ctime(&alarm_time));
}

void Alarm::AlarmClear(int index) {
    if (index < 0 || index >= ALARM_MAX_SETTINGS) {
        ESP_LOGW(TAG, "Invalid alarm index: %d", index);
        return;
    }
    
    alarm_times_[index] = 0;
    SaveAlarmsToNVS(); // Save to NVS
    ESP_LOGI(TAG, "Alarm %d cleared", index);
}

bool Alarm::AlarmIsSet(int index) const {
    if (index < 0 || index >= ALARM_MAX_SETTINGS) {
        ESP_LOGW(TAG, "Invalid alarm index: %d", index);
        return false;
    }
    
    return alarm_times_[index] > 0;
}

void Alarm::AlarmClearAll() {
    for (int i = 0; i < ALARM_MAX_SETTINGS; i++) {
        alarm_times_[i] = 0;
    }
    SaveAlarmsToNVS(); // Save to NVS
    ESP_LOGI(TAG, "All alarms cleared");
}

void Alarm::Start() {
    // Start the sleep timer functionality
    // SleepTimer::Start();
}

void Alarm::Stop() {
    // Stop the sleep timer functionality
    // SleepTimer::Stop();
}

void Alarm::OnExitLightAlarmMode(std::function<void()> callback) {
    on_exit_light_alarm_mode_ = callback;
}

void Alarm::OnAlarm(std::function<void()> callback) {
    on_alarm_ = callback;
}

void Alarm::LoadAlarmsFromNVS() {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("alarm", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to open NVS for loading alarms: %s", esp_err_to_name(err));
        return;
    }

    size_t required_size;
    err = nvs_get_blob(nvs_handle, "alarm_times", nullptr, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "Failed to read alarm times size from NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return;
    }

    if (required_size == 0) {
        ESP_LOGI(TAG, "No alarm data found in NVS");
        nvs_close(nvs_handle);
        return;
    }

    if (required_size != ALARM_MAX_SETTINGS * sizeof(time_t)) {
        ESP_LOGW(TAG, "Alarm data size mismatch in NVS");
        nvs_close(nvs_handle);
        return;
    }

    err = nvs_get_blob(nvs_handle, "alarm_times", alarm_times_.data(), &required_size);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to read alarm times from NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return;
    }

    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "Loaded alarms from NVS successfully");
}

void Alarm::SaveAlarmsToNVS() {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("alarm", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to open NVS for saving alarms: %s", esp_err_to_name(err));
        return;
    }

    err = nvs_set_blob(nvs_handle, "alarm_times", alarm_times_.data(), ALARM_MAX_SETTINGS * sizeof(time_t));
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to write alarm times to NVS: %s", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return;
    }

    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to commit alarm times to NVS: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "Saved alarms to NVS successfully");
    }

    nvs_close(nvs_handle);
}
