#include "core_management.h"
#include "tasks_config.h"
#include <esp_log.h>
#include <string.h>

static const char* TAG = "CoreManagement";

// Mutex để bảo vệ core_info
static SemaphoreHandle_t core_info_mutex = NULL;

// Thông tin core
static core_info_t core_info[CORE_MANAGEMENT_MAX_CORES] = {0};

// Bảng ánh xạ task type sang core
static const BaseType_t task_core_mapping[CORE_TASK_TYPE_MAX] = {
    [CORE_TASK_TYPE_WIFI] = WIFI_TASK_CORE,              // Wi-Fi task trên core 0
    [CORE_TASK_TYPE_NETWORK] = NETWORK_TASK_CORE,        // Network task trên core 0
    [CORE_TASK_TYPE_SYSTEM] = SYSTEM_TASK_CORE,          // System task trên core 0
    [CORE_TASK_TYPE_AUDIO_INPUT] = AUDIO_INPUT_CORE,     // Audio input task trên core 1
    [CORE_TASK_TYPE_AUDIO_OUTPUT] = AUDIO_OUTPUT_CORE,   // Audio output task trên core 1
    [CORE_TASK_TYPE_AUDIO_CODEC] = OPUS_CODEC_CORE,      // Audio codec task trên core 0
    [CORE_TASK_TYPE_DISPLAY] = DISPLAY_UPDATE_CORE,      // Display task trên core 1
    [CORE_TASK_TYPE_ALARM] = ALARM_CHECKER_CORE,         // Alarm task trên core 0
    [CORE_TASK_TYPE_PROTOCOL] = PROTOCOL_HANDLER_CORE,   // Protocol task trên core 0
    [CORE_TASK_TYPE_MCP] = PROTOCOL_HANDLER_CORE,        // MCP task trên core 0
    [CORE_TASK_TYPE_OTA] = SYSTEM_TASK_CORE,             // OTA task trên core 0
    [CORE_TASK_TYPE_UI] = DISPLAY_UPDATE_CORE,           // UI task trên core 1
    [CORE_TASK_TYPE_AUDIO_PROCESSOR] = AUDIO_PROCESSOR_CORE, // Audio processor task trên core 1
    [CORE_TASK_TYPE_WAKE_WORD_DETECTION] = WAKE_WORD_DETECTION_CORE, // Wake word detection task trên core 1
    [CORE_TASK_TYPE_WAKE_WORD_ENCODING] = WAKE_WORD_ENCODING_CORE,  // Wake word encoding task trên core 1
    [CORE_TASK_TYPE_BOARD_ACTION] = SYSTEM_TASK_CORE,        // Board action task trên core 0
    [CORE_TASK_TYPE_BATTERY_MONITORING] = SYSTEM_TASK_CORE,  // Battery monitoring task trên core 0
    [CORE_TASK_TYPE_TOUCHPAD] = SYSTEM_TASK_CORE             // Touchpad handling task trên core 0
};

// Bảng ánh xạ task type sang priority
static const UBaseType_t task_priority_mapping[CORE_TASK_TYPE_MAX] = {
    [CORE_TASK_TYPE_WIFI] = WIFI_TASK_PRIORITY,                          // Wi-Fi có priority cao
    [CORE_TASK_TYPE_NETWORK] = PROTOCOL_HANDLER_PRIORITY, // Network task
    [CORE_TASK_TYPE_SYSTEM] = MAIN_EVENT_LOOP_PRIORITY,  // System task
    [CORE_TASK_TYPE_AUDIO_INPUT] = AUDIO_INPUT_PRIORITY, // Audio input task (real-time)
    [CORE_TASK_TYPE_AUDIO_OUTPUT] = AUDIO_OUTPUT_PRIORITY, // Audio output task
    [CORE_TASK_TYPE_AUDIO_CODEC] = OPUS_CODEC_PRIORITY,  // Audio codec task
    [CORE_TASK_TYPE_DISPLAY] = DISPLAY_UPDATE_PRIORITY,   // Display task
    [CORE_TASK_TYPE_ALARM] = ALARM_CHECKER_PRIORITY,     // Alarm task
    [CORE_TASK_TYPE_PROTOCOL] = PROTOCOL_HANDLER_PRIORITY, // Protocol task
    [CORE_TASK_TYPE_MCP] = PROTOCOL_HANDLER_PRIORITY,    // MCP task
    [CORE_TASK_TYPE_OTA] = OTA_TASK_PRIORITY,                            // OTA task (low priority)
    [CORE_TASK_TYPE_UI] = DISPLAY_UPDATE_PRIORITY,       // UI task
    [CORE_TASK_TYPE_AUDIO_PROCESSOR] = AUDIO_PROCESSOR_PRIORITY, // Audio processor task (real-time)
    [CORE_TASK_TYPE_WAKE_WORD_DETECTION] = WAKE_WORD_DETECTION_PRIORITY, // Wake word detection task (real-time)
    [CORE_TASK_TYPE_WAKE_WORD_ENCODING] = WAKE_WORD_ENCODING_PRIORITY, // Wake word encoding task
    [CORE_TASK_TYPE_BOARD_ACTION] = BOARD_ACTION_PRIORITY, // Board action task (high priority)
    [CORE_TASK_TYPE_BATTERY_MONITORING] = BATTERY_MONITORING_PRIORITY,            // Battery monitoring task (high priority)
    [CORE_TASK_TYPE_TOUCHPAD] = TOUCHPAD_PRIORITY                        // Touchpad handling task
};

// Bảng ánh xạ task type sang stack size
static const uint32_t task_stack_size_mapping[CORE_TASK_TYPE_MAX] = {
    [CORE_TASK_TYPE_WIFI] = WIFI_TASK_STACK_SIZE,                        // Wi-Fi stack size
    [CORE_TASK_TYPE_NETWORK] = PROTOCOL_HANDLER_STACK_SIZE, // Network task
    [CORE_TASK_TYPE_SYSTEM] = MAIN_EVENT_LOOP_STACK_SIZE, // System task
    [CORE_TASK_TYPE_AUDIO_INPUT] = AUDIO_INPUT_STACK_SIZE, // Audio input task
    [CORE_TASK_TYPE_AUDIO_OUTPUT] = AUDIO_OUTPUT_STACK_SIZE, // Audio output task
    [CORE_TASK_TYPE_AUDIO_CODEC] = OPUS_CODEC_STACK_SIZE, // Audio codec task
    [CORE_TASK_TYPE_DISPLAY] = DISPLAY_UPDATE_STACK_SIZE, // Display task
    [CORE_TASK_TYPE_ALARM] = ALARM_CHECKER_STACK_SIZE,   // Alarm task
    [CORE_TASK_TYPE_PROTOCOL] = PROTOCOL_HANDLER_STACK_SIZE, // Protocol task
    [CORE_TASK_TYPE_MCP] = PROTOCOL_HANDLER_STACK_SIZE,  // MCP task
    [CORE_TASK_TYPE_OTA] = OTA_TASK_STACK_SIZE,                         // OTA task
    [CORE_TASK_TYPE_UI] = DISPLAY_UPDATE_STACK_SIZE,     // UI task
    [CORE_TASK_TYPE_AUDIO_PROCESSOR] = AUDIO_PROCESSOR_STACK_SIZE,             // Audio processor task
    [CORE_TASK_TYPE_WAKE_WORD_DETECTION] = WAKE_WORD_DETECTION_STACK_SIZE,         // Wake word detection task
    [CORE_TASK_TYPE_WAKE_WORD_ENCODING] = WAKE_WORD_ENCODING_STACK_SIZE,      // Wake word encoding task
    [CORE_TASK_TYPE_BOARD_ACTION] = BOARD_ACTION_STACK_SIZE,            // Board action task
    [CORE_TASK_TYPE_BATTERY_MONITORING] = BATTERY_MONITORING_STACK_SIZE,          // Battery monitoring task
    [CORE_TASK_TYPE_TOUCHPAD] = TOUCHPAD_STACK_SIZE                     // Touchpad handling task
};

void core_management_init(void) {
    // Tạo mutex
    core_info_mutex = xSemaphoreCreateMutex();
    if (core_info_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create core_info_mutex");
        return;
    }
    
    // Khởi tạo thông tin core
    memset(core_info, 0, sizeof(core_info));
    for (int i = 0; i < CORE_MANAGEMENT_MAX_CORES; i++) {
        core_info[i].is_initialized = true;
    }
    
    ESP_LOGI(TAG, "Core management initialized");
    ESP_LOGI(TAG, "Core 0: Wi-Fi, Network, System, Alarm, Protocol tasks");
    ESP_LOGI(TAG, "Core 1: Audio, Display, UI tasks");
}

BaseType_t core_management_get_task_core(core_task_type_t task_type) {
    if (task_type >= CORE_TASK_TYPE_MAX) {
        ESP_LOGW(TAG, "Invalid task type: %d", task_type);
        return CORE_0; // Mặc định về core 0
    }
    
    return task_core_mapping[task_type];
}

UBaseType_t core_management_get_task_priority(core_task_type_t task_type) {
    if (task_type >= CORE_TASK_TYPE_MAX) {
        ESP_LOGW(TAG, "Invalid task type: %d", task_type);
        return 1; // Mặc định priority thấp
    }
    
    return task_priority_mapping[task_type];
}

uint32_t core_management_get_task_stack_size(core_task_type_t task_type) {
    if (task_type >= CORE_TASK_TYPE_MAX) {
        ESP_LOGW(TAG, "Invalid task type: %d", task_type);
        return 2048; // Mặc định stack size
    }
    
    return task_stack_size_mapping[task_type];
}

void core_management_register_task(core_task_type_t task_type, uint32_t stack_size) {
    BaseType_t core_id = core_management_get_task_core(task_type);
    
    // Kiểm tra core_id hợp lệ
    if (core_id < 0 || core_id >= CORE_MANAGEMENT_MAX_CORES) {
        ESP_LOGW(TAG, "Invalid core ID: %d for task type: %d", core_id, task_type);
        return;
    }
    
    // Sử dụng mutex để bảo vệ khi cập nhật core_info
    if (xSemaphoreTake(core_info_mutex, portMAX_DELAY) == pdTRUE) {
        core_info[core_id].task_count++;
        
        // Kiểm tra tràn trước khi cộng
        if (core_info[core_id].total_stack_size + stack_size < core_info[core_id].total_stack_size) {
            ESP_LOGE(TAG, "Stack size overflow detected for core %d", core_id);
            core_info[core_id].task_count--; // Hoàn tác tăng task_count
        } else {
            core_info[core_id].total_stack_size += stack_size;
        }
        
        xSemaphoreGive(core_info_mutex);
        
        ESP_LOGI(TAG, "Task type %d registered on core %d, stack size: %u", 
                 task_type, core_id, stack_size);
    } else {
        ESP_LOGE(TAG, "Failed to take core_info_mutex for task type: %d", task_type);
    }
}

core_info_t* core_management_get_core_info(BaseType_t core_id) {
    // Kiểm tra core_id hợp lệ
    if (core_id < 0 || core_id >= CORE_MANAGEMENT_MAX_CORES) {
        ESP_LOGW(TAG, "Invalid core ID: %d", core_id);
        return NULL;
    }
    
    return &core_info[core_id];
}

bool core_management_is_task_allowed_on_core(core_task_type_t task_type, BaseType_t core_id) {
    if (task_type >= CORE_TASK_TYPE_MAX) {
        ESP_LOGW(TAG, "Invalid task type: %d", task_type);
        return false;
    }
    
    if (core_id < 0 || core_id >= CORE_MANAGEMENT_MAX_CORES) {
        ESP_LOGW(TAG, "Invalid core ID: %d", core_id);
        return false;
    }
    
    return (task_core_mapping[task_type] == core_id);
}