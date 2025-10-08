#pragma once

// Định nghĩa stack size cho từng task (bytes)
// Giảm stack size để tiết kiệm RAM
#define MAIN_EVENT_LOOP_STACK_SIZE      (8 * 1024)
#define AUDIO_INPUT_STACK_SIZE          (6 * 1024)
#define AUDIO_OUTPUT_STACK_SIZE         (4 * 1024)
#define OPUS_CODEC_STACK_SIZE           (24 * 1024)  // Giảm từ 26KB xuống 24KB
#define PROTOCOL_HANDLER_STACK_SIZE     (10 * 1024)  // Giảm từ 12KB xuống 10KB
#define DISPLAY_UPDATE_STACK_SIZE       (4 * 1024)
#define ALARM_CHECKER_STACK_SIZE        (2 * 1024)
#define CLOCK_TIMER_STACK_SIZE          (2 * 1024)
#define WIFI_TASK_STACK_SIZE            (4 * 1024)
#define NETWORK_TASK_STACK_SIZE         (6 * 1024)
#define SYSTEM_TASK_STACK_SIZE          (4 * 1024)
#define MCP_TASK_STACK_SIZE             (8 * 1024)
#define OTA_TASK_STACK_SIZE             (4 * 1024)
#define UI_TASK_STACK_SIZE              (4 * 1024)
#define AUDIO_PROCESSOR_STACK_SIZE      (4 * 1024)
#define WAKE_WORD_DETECTION_STACK_SIZE  (4 * 1024)
#define WAKE_WORD_ENCODING_STACK_SIZE   (3 * 1024 * 3) // 9KB thay vì 12KB

#define BOARD_ACTION_STACK_SIZE         (4 * 1024)
#define BATTERY_MONITORING_STACK_SIZE   (1024)  // Giảm từ 2KB xuống 1KB
#define TOUCHPAD_STACK_SIZE             (4 * 1024)

// Định nghĩa priority cho từng task (0-25)
#define MAIN_EVENT_LOOP_PRIORITY        3
#define AUDIO_INPUT_PRIORITY            4  // Priority cao thứ hai
#define AUDIO_OUTPUT_PRIORITY           8
#define OPUS_CODEC_PRIORITY             12
#define PROTOCOL_HANDLER_PRIORITY       5
#define DISPLAY_UPDATE_PRIORITY         1
#define ALARM_CHECKER_PRIORITY          1
#define CLOCK_TIMER_PRIORITY            1
#define WIFI_TASK_PRIORITY              10
#define NETWORK_TASK_PRIORITY           5
#define SYSTEM_TASK_PRIORITY            3
#define MCP_TASK_PRIORITY               5
#define OTA_TASK_PRIORITY               1
#define UI_TASK_PRIORITY                1
#define AUDIO_PROCESSOR_PRIORITY        2  // Priority cao nhất
#define WAKE_WORD_DETECTION_PRIORITY    6  // Priority 6
#define WAKE_WORD_ENCODING_PRIORITY     13 // Priority 13
#define BOARD_ACTION_PRIORITY           (configMAX_PRIORITIES - 1)
#define BATTERY_MONITORING_PRIORITY     10
#define TOUCHPAD_PRIORITY               5

// Định nghĩa core cho từng task (0 hoặc 1)
#define MAIN_EVENT_LOOP_CORE            0
#define AUDIO_INPUT_CORE                1
#define AUDIO_OUTPUT_CORE               1
#define OPUS_CODEC_CORE                 0
#define PROTOCOL_HANDLER_CORE           0
#define DISPLAY_UPDATE_CORE             1
#define ALARM_CHECKER_CORE              0
#define CLOCK_TIMER_CORE                0
#define WIFI_TASK_CORE                  0
#define NETWORK_TASK_CORE               0
#define SYSTEM_TASK_CORE                0
#define MCP_TASK_CORE                   0
#define OTA_TASK_CORE                   0
#define UI_TASK_CORE                    1
#define AUDIO_PROCESSOR_CORE            1
#define WAKE_WORD_DETECTION_CORE        1
#define WAKE_WORD_ENCODING_CORE         0  // Chạy trên Core 0
#define BOARD_ACTION_CORE               0
#define BATTERY_MONITORING_CORE         0
#define TOUCHPAD_CORE                   0