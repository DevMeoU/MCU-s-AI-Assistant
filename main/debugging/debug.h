#pragma once

#ifdef _DEBUG
#define DEBUG_ONLY(x) x
#else
#define DEBUG_ONLY(x)
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

void print_task_list() {
    TaskStatus_t *task_list;
    char *task_list_buffer;
    uint32_t task_count;

    task_list_buffer = (char *)malloc(1024);
    if (task_list_buffer == NULL) {
        ESP_LOGE("Task", "Failed to allocate memory for task list buffer");
        return;
    }

    vTaskList(task_list_buffer);
    ESP_LOGW("Task", "Task name      State      Prio    Stack    Num");
    ESP_LOGW("Task", "%s", task_list_buffer);

    task_count = uxTaskGetNumberOfTasks();
    task_list = (TaskStatus_t *)malloc(sizeof(TaskStatus_t) * task_count);
    if (task_list == NULL) {
        ESP_LOGE("Task", "Failed to allocate memory for task list");
        free(task_list_buffer);
        return;
    }

    task_count = uxTaskGetSystemState(task_list, task_count, NULL);
    for (uint32_t i = 0; i < task_count; i++) {
        TaskStatus_t task = task_list[i];
        ESP_LOGW("Task", "Task %s (%d) prio %d state %d", 
                 task.pcTaskName, task.xTaskNumber, task.uxCurrentPriority, task.eCurrentState);
    }

    free(task_list);
    free(task_list_buffer);
}
