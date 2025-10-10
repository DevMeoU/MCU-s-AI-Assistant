#pragma once

// ============================================
// CORE DEFINITION - ESP32-S3 OPTIMIZED
// ============================================
#define CORE_AUDIO       1  // Real-time audio processing
#define CORE_NETWORK     0  // Network, system, protocols

// ============================================
// AUDIO TASKS - CORE 1 (Real-time Critical)
// ============================================

// Audio Input - Highest priority, internal RAM for low latency
#define AUDIO_INPUT_CORE                CORE_AUDIO
#define AUDIO_INPUT_PRIORITY            24  // configMAX_PRIORITIES - 1
#define AUDIO_INPUT_STACK_SIZE          (8 * 1024)   // Internal RAM

// Audio Output - High priority, internal RAM  
#define AUDIO_OUTPUT_CORE               CORE_AUDIO
#define AUDIO_OUTPUT_PRIORITY           20
#define AUDIO_OUTPUT_STACK_SIZE         (6 * 1024)   // Internal RAM

// Wake Word Detection - High priority, internal RAM
#define WAKE_WORD_DETECTION_CORE        CORE_AUDIO
#define WAKE_WORD_DETECTION_PRIORITY    22
#define WAKE_WORD_DETECTION_STACK_SIZE  (6 * 1024)   // Internal RAM

// Opus Codec - Large stack, can use PSRAM for buffers
#define OPUS_CODEC_CORE                 CORE_AUDIO    // All audio on same core
#define OPUS_CODEC_PRIORITY             12
#define OPUS_CODEC_STACK_SIZE           (40 * 1024)  // PSRAM preferred

// Wake Word Encoding - Medium priority
#define WAKE_WORD_ENCODING_CORE         CORE_AUDIO
#define WAKE_WORD_ENCODING_PRIORITY     10
#define WAKE_WORD_ENCODING_STACK_SIZE   (12 * 1024)  // PSRAM

// Audio Processor - Background processing
#define AUDIO_PROCESSOR_CORE            CORE_AUDIO
#define AUDIO_PROCESSOR_PRIORITY        6
#define AUDIO_PROCESSOR_STACK_SIZE      (4 * 1024)   // Internal RAM

// ============================================
// NETWORK TASKS - CORE 0 (Stability Focused)
// ============================================

// WiFi Task - High priority for connectivity
#define WIFI_TASK_CORE                  CORE_NETWORK
#define WIFI_TASK_PRIORITY              18
#define WIFI_TASK_STACK_SIZE            (4 * 1024)   // Internal RAM

// Protocol Handler (MQTT) - Medium priority
#define PROTOCOL_HANDLER_CORE           CORE_NETWORK
#define PROTOCOL_HANDLER_PRIORITY       10
#define PROTOCOL_HANDLER_STACK_SIZE     (8 * 1024)   // Internal RAM

// Network Task - Medium priority
#define NETWORK_TASK_CORE               CORE_NETWORK
#define NETWORK_TASK_PRIORITY           12
#define NETWORK_TASK_STACK_SIZE         (6 * 1024)   // Internal RAM

// MCP Task - Same as protocol
#define MCP_TASK_CORE                   CORE_NETWORK
#define MCP_TASK_PRIORITY               10
#define MCP_TASK_STACK_SIZE             (8 * 1024)   // Internal RAM

// ============================================
// SYSTEM TASKS - CORE 0 (Background/Low Priority)
// ============================================

// Main Event Loop - Medium priority
#define MAIN_LOOP_CORE                  CORE_NETWORK
#define MAIN_LOOP_PRIORITY              8
#define MAIN_LOOP_STACK_SIZE            (6 * 1024)   // Internal RAM

#define MAIN_EVENT_LOOP_STACK_SIZE      MAIN_LOOP_STACK_SIZE
#define MAIN_EVENT_LOOP_PRIORITY        MAIN_LOOP_PRIORITY
#define MAIN_EVENT_LOOP_CORE            MAIN_LOOP_CORE

// System Task - Low priority
#define SYSTEM_TASK_CORE                CORE_NETWORK
#define SYSTEM_TASK_PRIORITY            5
#define SYSTEM_TASK_STACK_SIZE          (4 * 1024)   // Internal RAM

// OTA Task - Low priority, larger stack for updates
#define OTA_TASK_CORE                   CORE_NETWORK
#define OTA_TASK_PRIORITY               3
#define OTA_TASK_STACK_SIZE             (12 * 1024)  // PSRAM during updates

// Battery Monitoring - Medium priority (safety)
#define BATTERY_MONITORING_CORE         CORE_NETWORK
#define BATTERY_MONITORING_PRIORITY     14
#define BATTERY_MONITORING_STACK_SIZE   (1 * 1024)   // Internal RAM

// ============================================
// UI/DISPLAY TASKS - CORE 1 (Lower Priority)
// ============================================

// Display Update - Low priority on audio core
#define DISPLAY_UPDATE_CORE             CORE_AUDIO
#define DISPLAY_UPDATE_PRIORITY         4
#define DISPLAY_UPDATE_STACK_SIZE       (4 * 1024)   // PSRAM for framebuffer

// UI Task - Low priority
#define UI_TASK_CORE                    CORE_AUDIO
#define UI_TASK_PRIORITY                4
#define UI_TASK_STACK_SIZE              (4 * 1024)   // PSRAM

// ============================================
// MISCELLANEOUS TASKS
// ============================================

// Board Action - Highest priority for critical actions
#define BOARD_ACTION_CORE               CORE_NETWORK
#define BOARD_ACTION_PRIORITY           24  // configMAX_PRIORITIES - 1
#define BOARD_ACTION_STACK_SIZE         (2 * 1024)   // Internal RAM

// Touchpad - Medium priority
#define TOUCHPAD_CORE                   CORE_NETWORK
#define TOUCHPAD_PRIORITY               8
#define TOUCHPAD_STACK_SIZE             (3 * 1024)   // Internal RAM

// Alarm Checker - Low priority
#define ALARM_CHECKER_CORE              CORE_NETWORK
#define ALARM_CHECKER_PRIORITY          3
#define ALARM_CHECKER_STACK_SIZE        (2 * 1024)   // Internal RAM

// Clock Timer - Low priority
#define CLOCK_TIMER_CORE                CORE_NETWORK
#define CLOCK_TIMER_PRIORITY            3
#define CLOCK_TIMER_STACK_SIZE          (2 * 1024)   // Internal RAM