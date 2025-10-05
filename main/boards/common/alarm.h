#pragma once

#include <functional>
#include <vector>
#include <ctime>

#include <esp_timer.h>
#include <esp_pm.h>
#include <freertos/queue.h>

#include "sleep_timer.h"

#define ALARM_CHECK_INTERVAL_MS 1000        /* Check alarm every second */
#define ALARM_DEFAULT_TIMEZONE_OFFSET 0     /* Default timezone (UTC) */
#define ALARM_MAX_TIMEZONE_OFFSET 12        /* Maximum timezone +12 */
#define ALARM_MIN_TIMEZONE_OFFSET -12       /* Minimum timezone -12 */
#define ALARM_MAX_SETTINGS 5                /* Maximum number of alarms */

/* Inherit from SleepTimer */
class SleepTimer; // Forward declaration
class Alarm : public SleepTimer {
private:
    time_t timestamp_ = 0;                  /* Current time */
    int alarm_check_settings_ = 0;          /* Number of alarms set */
    std::vector<time_t> alarm_times_{ALARM_MAX_SETTINGS, 0}; /* Array of alarm times */
    
    esp_timer_handle_t check_timer_ = nullptr;  /* Timer for periodic alarm checking */
    volatile bool alarm_triggered_ = false;     /* Flag to indicate if alarm should be triggered */
    int triggered_alarm_id_ = -1;               /* ID of the triggered alarm */
    
    void CheckAlarms();                     /* Internal method to check if alarms should trigger */
    
    std::function<void()> on_exit_light_alarm_mode_;        /* Callback when exiting light alarm mode */
    
    static void TimerCallback(void* arg);   /* Static timer callback */
    
public:
    Alarm(int seconds_to_light_sleep = 20, int seconds_to_deep_sleep = -1);
    ~Alarm(); // Remove = delete and allow proper destruction

    std::function<void()> on_alarm_;        /* Callback when alarm time is reached */

    void AlarmSet(int index, int hour, int minute);
    void AlarmClear(int index);
    bool AlarmIsSet(int index) const;
    void AlarmClearAll();
    
    // Override or inherit sleep timer functionality
    void Start();
    void Stop();

    void OnExitLightAlarmMode(std::function<void()> callback);        /* Callback when exiting light alarm mode */
    void OnAlarm(std::function<void()> callback);                    /* Callback when alarm time is reached */
    
    // Method to process alarm trigger in application task context
    void ProcessAlarmTrigger();             /* Process alarm trigger - should be called from application task */
    
    // Methods for MCP server integration
    bool IsAlarmTriggered() const;          /* Check if alarm is triggered */
    void ClearAlarmTrigger();               /* Clear alarm trigger flag */
    int GetTriggeredAlarmId() const;        /* Get the ID of triggered alarm */
    
    // NVS storage methods
    void LoadAlarmsFromNVS();               /* Load alarms from NVS storage */
    void SaveAlarmsToNVS();                 /* Save alarms to NVS storage */
};