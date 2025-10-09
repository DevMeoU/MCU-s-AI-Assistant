#pragma once

// Định nghĩa core
#define CORE_AUDIO       1
#define CORE_NETWORK     0

// Wake word
#define WAKE_WORD_DETECTION_CORE        CORE_AUDIO
#define WAKE_WORD_DETECTION_PRIORITY    6
#define WAKE_WORD_DETECTION_STACK_SIZE  (4 * 1024)

// Wake word encoding (Opus)
#define WAKE_WORD_ENCODING_CORE         CORE_AUDIO   // chuyển từ CORE_NETWORK -> CORE_AUDIO
#define WAKE_WORD_ENCODING_PRIORITY     5
#define WAKE_WORD_ENCODING_STACK_SIZE   (9 * 1024)

// Audio I/O
#define AUDIO_INPUT_CORE                CORE_AUDIO
#define AUDIO_INPUT_PRIORITY            5
#define AUDIO_INPUT_STACK_SIZE          (6 * 1024)

#define AUDIO_OUTPUT_CORE               CORE_AUDIO
#define AUDIO_OUTPUT_PRIORITY           5
#define AUDIO_OUTPUT_STACK_SIZE         (4 * 1024)

// Opus codec heavy processing (if separate task)
#define OPUS_CODEC_CORE                 CORE_NETWORK // nếu codec dùng HW offloading hoặc không ảnh hưởng WiFi; có thể CORE_AUDIO nếu cần
#define OPUS_CODEC_PRIORITY             12
#define OPUS_CODEC_STACK_SIZE           (28 * 1024)

// Protocol/MQTT
#define PROTOCOL_HANDLER_CORE           CORE_NETWORK
#define PROTOCOL_HANDLER_PRIORITY       5
#define PROTOCOL_HANDLER_STACK_SIZE     (12 * 1024)

// Main loop
#define MAIN_LOOP_CORE                  CORE_NETWORK
#define MAIN_LOOP_PRIORITY              3
#define MAIN_LOOP_STACK_SIZE            (8 * 1024)

// Các define cũ được giữ lại để tương thích ngược
#define MAIN_EVENT_LOOP_STACK_SIZE      MAIN_LOOP_STACK_SIZE
#define MAIN_EVENT_LOOP_PRIORITY        MAIN_LOOP_PRIORITY
#define MAIN_EVENT_LOOP_CORE            MAIN_LOOP_CORE

#define AUDIO_PROCESSOR_STACK_SIZE      (4 * 1024)
#define AUDIO_PROCESSOR_PRIORITY        2
#define AUDIO_PROCESSOR_CORE            CORE_AUDIO

#define DISPLAY_UPDATE_STACK_SIZE       (4 * 1024)
#define DISPLAY_UPDATE_PRIORITY         1
#define DISPLAY_UPDATE_CORE             CORE_AUDIO

#define ALARM_CHECKER_STACK_SIZE        (2 * 1024)
#define ALARM_CHECKER_PRIORITY          1
#define ALARM_CHECKER_CORE              CORE_NETWORK

#define MCP_TASK_STACK_SIZE             PROTOCOL_HANDLER_STACK_SIZE
#define MCP_TASK_PRIORITY               PROTOCOL_HANDLER_PRIORITY
#define MCP_TASK_CORE                   PROTOCOL_HANDLER_CORE

#define OTA_TASK_STACK_SIZE             (4 * 1024)
#define OTA_TASK_PRIORITY               1
#define OTA_TASK_CORE                   CORE_NETWORK

#define UI_TASK_STACK_SIZE              DISPLAY_UPDATE_STACK_SIZE
#define UI_TASK_PRIORITY                DISPLAY_UPDATE_PRIORITY
#define UI_TASK_CORE                    DISPLAY_UPDATE_CORE

#define BOARD_ACTION_STACK_SIZE         (4 * 1024)
#define BOARD_ACTION_PRIORITY           (configMAX_PRIORITIES - 1)
#define BOARD_ACTION_CORE               CORE_NETWORK

#define BATTERY_MONITORING_STACK_SIZE   (1024)
#define BATTERY_MONITORING_PRIORITY     10
#define BATTERY_MONITORING_CORE         CORE_NETWORK

#define TOUCHPAD_STACK_SIZE             (4 * 1024)
#define TOUCHPAD_PRIORITY               5
#define TOUCHPAD_CORE                   CORE_NETWORK

#define WIFI_TASK_STACK_SIZE            (4 * 1024)
#define WIFI_TASK_PRIORITY              10
#define WIFI_TASK_CORE                  CORE_NETWORK

#define NETWORK_TASK_STACK_SIZE         (6 * 1024)
#define NETWORK_TASK_PRIORITY           5
#define NETWORK_TASK_CORE               CORE_NETWORK

#define SYSTEM_TASK_STACK_SIZE          (4 * 1024)
#define SYSTEM_TASK_PRIORITY            3
#define SYSTEM_TASK_CORE                CORE_NETWORK

#define CLOCK_TIMER_STACK_SIZE          (2 * 1024)
#define CLOCK_TIMER_PRIORITY            1
#define CLOCK_TIMER_CORE                CORE_NETWORK