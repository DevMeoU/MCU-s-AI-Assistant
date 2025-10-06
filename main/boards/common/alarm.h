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
    virtual ~Alarm(); // Remove = delete and allow proper destruction

    std::function<void()> on_alarm_;        /* Callback when alarm time is reached */

    virtual void AlarmSet(int index, int hour, int minute);
    virtual void AlarmClear(int index);
    virtual bool AlarmIsSet(int index) const;
    virtual void AlarmClearAll();
    
    // Override or inherit sleep timer functionality
    virtual void Start();
    virtual void Stop();

    virtual void OnExitLightAlarmMode(std::function<void()> callback);        /* Callback when exiting light alarm mode */
    virtual void OnAlarm(std::function<void()> callback);                    /* Callback when alarm time is reached */
    
    // Method to process alarm trigger in application task context
    virtual void ProcessAlarmTrigger();             /* Process alarm trigger - should be called from application task */
    
    // Methods for MCP server integration
    virtual bool IsAlarmTriggered() const;          /* Check if alarm is triggered */
    virtual void ClearAlarmTrigger();               /* Clear alarm trigger flag */
    virtual int GetTriggeredAlarmId() const;        /* Get the ID of triggered alarm */
    
    // NVS storage methods
    virtual void LoadAlarmsFromNVS();               /* Load alarms from NVS storage */
    virtual void SaveAlarmsToNVS();                 /* Save alarms to NVS storage */
    
    // Singleton instance access
    static Alarm& GetInstance() {
        static Alarm instance;
        return instance;
    }
};

class NoAlarm : public Alarm {
public:
    NoAlarm() : Alarm() {}
    
    // Override methods to provide no-op functionality
    void AlarmSet(int index, int hour, int minute) override {}
    void AlarmClear(int index) override {}
    bool AlarmIsSet(int index) const override { return false; }
    void AlarmClearAll() override {}
    void Start() override {}
    void Stop() override {}
    void OnExitLightAlarmMode(std::function<void()> callback) override {}
    void OnAlarm(std::function<void()> callback) override {}
    bool IsAlarmTriggered() const override { return false; }
    void ClearAlarmTrigger() override {}
    int GetTriggeredAlarmId() const override { return -1; }
    void LoadAlarmsFromNVS() override {}
    void SaveAlarmsToNVS() override {}
    void ProcessAlarmTrigger() override {}
    
    // Singleton instance access
    static NoAlarm& GetInstance() {
        static NoAlarm instance;
        return instance;
    }
};