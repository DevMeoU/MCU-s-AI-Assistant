#ifndef CORE_MANAGEMENT_H
#define CORE_MANAGEMENT_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Định nghĩa số core tối đa
#define CORE_MANAGEMENT_MAX_CORES 2

// Định nghĩa các core
#define CORE_0 0
#define CORE_1 1

// Định nghĩa các loại tác vụ
typedef enum {
    CORE_TASK_TYPE_WIFI,

    CORE_TASK_TYPE_NETWORK,
    CORE_TASK_TYPE_SYSTEM,
    CORE_TASK_TYPE_AUDIO_INPUT,
    CORE_TASK_TYPE_AUDIO_OUTPUT,
    CORE_TASK_TYPE_AUDIO_CODEC,
    CORE_TASK_TYPE_DISPLAY,
    CORE_TASK_TYPE_ALARM,
    CORE_TASK_TYPE_PROTOCOL,
    CORE_TASK_TYPE_MCP,
    CORE_TASK_TYPE_OTA,
    CORE_TASK_TYPE_UI,
    CORE_TASK_TYPE_AUDIO_PROCESSOR,      // New: Audio processor task
    CORE_TASK_TYPE_WAKE_WORD_DETECTION,  // New: Wake word detection task
    CORE_TASK_TYPE_WAKE_WORD_ENCODING,   // New: Wake word encoding task
    CORE_TASK_TYPE_BOARD_ACTION,         // New: Board-specific action task
    CORE_TASK_TYPE_BATTERY_MONITORING,   // New: Battery monitoring task
    CORE_TASK_TYPE_TOUCHPAD,             // New: Touchpad handling task
    CORE_TASK_TYPE_MAX
} core_task_type_t;

// Cấu trúc thông tin core
typedef struct {
    uint32_t task_count;
    uint64_t total_stack_size;  // Sử dụng uint64_t để tránh tràn
    bool is_initialized;
} core_info_t;

// Hàm khởi tạo quản lý core
void core_management_init(void);

// Hàm lấy core được chỉ định cho từng loại tác vụ
BaseType_t core_management_get_task_core(core_task_type_t task_type);

// Hàm lấy priority được chỉ định cho từng loại tác vụ
UBaseType_t core_management_get_task_priority(core_task_type_t task_type);

// Hàm lấy stack size được chỉ định cho từng loại tác vụ
uint32_t core_management_get_task_stack_size(core_task_type_t task_type);

// Hàm đăng ký task với hệ thống quản lý core
void core_management_register_task(core_task_type_t task_type, uint32_t stack_size);

// Hàm lấy thông tin core
core_info_t* core_management_get_core_info(BaseType_t core_id);

// Hàm kiểm tra xem tác vụ có thể chạy trên core được chỉ định không
bool core_management_is_task_allowed_on_core(core_task_type_t task_type, BaseType_t core_id);

#ifdef __cplusplus
}
#endif

#endif // CORE_MANAGEMENT_H