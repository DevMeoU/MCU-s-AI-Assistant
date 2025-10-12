#pragma once

#ifdef _DEBUG
#define DEBUG_ONLY(x) x
#else
#define DEBUG_ONLY(x)
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <vector>

void print_task_list() {
    constexpr size_t BUFFER_SIZE = 1024;
    char task_list_buffer[BUFFER_SIZE];

    // In danh sách task dạng bảng
    vTaskList(task_list_buffer);
    ESP_LOGW("Task", "Task name      State   Prio   Stack   Num");
    ESP_LOGW("Task", "%s", task_list_buffer);

    // Lấy số lượng task hiện tại
    UBaseType_t task_count = uxTaskGetNumberOfTasks();
    std::vector<TaskStatus_t> task_list(task_count);

    // Lấy trạng thái hệ thống
    task_count = uxTaskGetSystemState(task_list.data(), task_count, nullptr);

    for (UBaseType_t i = 0; i < task_count; i++) {
        const TaskStatus_t& task = task_list[i];
        ESP_LOGW("Task", "Task %-12s (ID=%d) prio=%d state=%d stack=%u",
                 task.pcTaskName,
                 task.xTaskNumber,
                 task.uxCurrentPriority,
                 task.eCurrentState,
                 (unsigned)task.usStackHighWaterMark);
    }
}
