#ifndef _SYSTEM_INFO_H_
#define _SYSTEM_INFO_H_

#include <string>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>

/**
 * @brief Cấu trúc lưu thông tin bộ nhớ
 */
struct MemoryStats {
    size_t sramFree;
    size_t sramTotal;
    size_t rtcRamFree;
    size_t rtcRamTotal;
    size_t spiRamFree;
    size_t spiRamTotal;
};

/**
 * @brief Lớp cung cấp thông tin hệ thống (flash, heap, chip, task, memory, v.v.)
 */
class SystemInfo {
public:
    // Flash / Heap
    static size_t GetFlashSize();
    static size_t GetMinimumFreeHeapSize();
    static size_t GetFreeHeapSize();

    // PSRAM
    static size_t GetSPIRAMSize();
    static MemoryStats GetMemoryStats();
    static void PrintMemoryStats();

    // Chip / Device
    static std::string GetMacAddress();
    static std::string GetChipModelName();
    static std::string GetUserAgent();
    static size_t GetFreqMHz();

    // Task / Debug
    static esp_err_t PrintTaskCpuUsage(TickType_t xTicksToWait);
    static void PrintTaskList();
    static void PrintHeapStats();
};

#endif // _SYSTEM_INFO_H_
