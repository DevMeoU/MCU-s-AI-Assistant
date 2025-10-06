#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <esp_heap_caps.h>
#include <esp_log.h>

class MemoryManager {
    public:
        static void* allocateInternal(size_t size) {
            void* ptr = heap_caps_malloc(size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            if (ptr == nullptr) {
                ESP_LOGE(TAG, "Failed to allocate memory of size %zu with caps 0x%08" PRIx32 " in Internal RAM", (unsigned)size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            }
            return ptr;
        }

        static void* allocatePsram(size_t size) {
            void* ptr = heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
            if (ptr == nullptr) {
                ESP_LOGE(TAG, "Failed to allocate memory of size %zu with caps 0x%08" PRIx32 " in PSRAM", (unsigned)size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
            }
            return ptr;
        }

        /* MALLOC_CAP_RTC | MALLOC_CAP_8BIT is not expire, so it can be used for long-term storage */
        static void* allocateRtc() {
            return rtcMemory;
        }

        static void freeMemory(void* ptr) {
            if(ptr) heap_caps_free(ptr);
        }

        struct MemoryInfo
        {
            size_t freeBytes;
            size_t minFreeBytes;
            size_t largestFreeBlock;
        };

        static MemoryInfo getInternalInfo() {
            MemoryInfo info;
            info.freeBytes = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            info.minFreeBytes = heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            info.largestFreeBlock = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            return info;
        }

        static MemoryInfo getPsramInfo() {
            MemoryInfo info;
            info.freeBytes = heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
            info.minFreeBytes = heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
            info.largestFreeBlock = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
            return info;
        }

        static MemoryInfo getRtcInfo() {
            MemoryInfo info;
            info.freeBytes = sizeof(rtcMemory);
            info.minFreeBytes = sizeof(rtcMemory);
            info.largestFreeBlock = sizeof(rtcMemory);
            return info;
        }

        static void logMemoryInfo() {
            MemoryInfo internalInfo = getInternalInfo();
            MemoryInfo psramInfo = getPsramInfo();
            MemoryInfo rtcInfo = getRtcInfo();

            ESP_LOGI(TAG, "_________________ MEMORY STATUS _________________");
            ESP_LOGI(TAG, "Internal RAM: Free Bytes: %zu, Min Free Bytes: %zu, Largest Free Block: %zu",
                     internalInfo.freeBytes, internalInfo.minFreeBytes, internalInfo.largestFreeBlock);
            ESP_LOGI(TAG, "PSRAM: Free Bytes: %zu, Min Free Bytes: %zu, Largest Free Block: %zu",
                     psramInfo.freeBytes, psramInfo.minFreeBytes, psramInfo.largestFreeBlock);
            ESP_LOGI(TAG, "RTC RAM: Free Bytes: %zu, Min Free Bytes: %zu, Largest Free Block: %zu",
                     rtcInfo.freeBytes, rtcInfo.minFreeBytes, rtcInfo.largestFreeBlock);
        }
    private:
        static constexpr const char* TAG = "MemoryManager";
        static uint8_t rtcMemory[1024];
};