#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <memory>
#include <new>
#include <esp_heap_caps.h>
#include <esp_memory_utils.h>
#include <esp_log.h>

// Macro để overload operator new/delete cho class cần ép vào PSRAM
#define DECLARE_PSRAM_NEW_DELETE(ClassName) \
    static void* operator new(std::size_t sz) { \
        void* p = MemoryManager::allocatePsram(sz); \
        if (!p) throw std::bad_alloc(); \
        return p; \
    } \
    static void* operator new(std::size_t sz, const std::nothrow_t&) noexcept { \
        return MemoryManager::allocatePsram(sz); \
    } \
    static void* operator new(std::size_t sz, void* ptr) noexcept { \
        return ptr; \
    } \
    static void operator delete(void* p) noexcept { \
        MemoryManager::freeMemory(p); \
    }

// Macro để tạo factory method cho class cần ép vào PSRAM
#define DECLARE_PSRAM_FACTORY(ClassName) \
    struct Deleter { \
        void operator()(ClassName* obj) { \
            if (obj) { \
                if (esp_ptr_external_ram(obj)) { \
                    obj->~ClassName(); \
                    MemoryManager::freeMemory(obj); \
                } else { \
                    delete obj; \
                } \
            } \
        } \
    }; \
    static std::unique_ptr<ClassName, Deleter> CreateInPsram() { \
        void* memory = MemoryManager::allocatePsram(sizeof(ClassName)); \
        if (memory) { \
            ClassName* obj = new(memory) ClassName(); \
            return std::unique_ptr<ClassName, Deleter>(obj); \
        } else { \
            return std::unique_ptr<ClassName, Deleter>(new ClassName()); \
        } \
    }

class MemoryManager {
    public:
        static void* allocateInternal(size_t size) {
            // Kiểm tra kích thước yêu cầu
            if (size == 0) {
                ESP_LOGW(TAG, "Attempt to allocate 0 bytes in Internal RAM");
                return nullptr;
            }
            
            void* ptr = heap_caps_malloc(size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            if (ptr == nullptr) {
                ESP_LOGE(TAG, "Failed to allocate memory of size %zu with caps 0x%08" PRIx32 " in Internal RAM", (unsigned)size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
                
                // Log thông tin bộ nhớ hiện tại để debug
                size_t free_bytes = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
                size_t largest_block = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);
                ESP_LOGE(TAG, "Current Internal RAM - Free: %u bytes, Largest block: %u bytes", (unsigned)free_bytes, (unsigned)largest_block);
            }
            return ptr;
        }

        static void* allocatePsram(size_t size) {
            // Kiểm tra kích thước yêu cầu
            if (size == 0) {
                ESP_LOGW(TAG, "Attempt to allocate 0 bytes in PSRAM");
                return nullptr;
            }
            
            void* ptr = heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
            if (ptr == nullptr) {
                ESP_LOGE(TAG, "Failed to allocate memory of size %zu with caps 0x%08" PRIx32 " in PSRAM", (unsigned)size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
                
                // Log thông tin bộ nhớ hiện tại để debug
                size_t free_bytes = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
                size_t largest_block = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM);
                ESP_LOGE(TAG, "Current PSRAM - Free: %u bytes, Largest block: %u bytes", (unsigned)free_bytes, (unsigned)largest_block);
            }
            return ptr;
        }

        /* MALLOC_CAP_RTC | MALLOC_CAP_8BIT is not expire, so it can be used for long-term storage */
        static void* allocateRtc() {
            return rtcMemory;
        }

        static void freeMemory(void* ptr) {
            if (ptr == nullptr) return;
            
            // Kiểm tra nếu con trỏ thuộc RTC fast/slow memory
            #if CONFIG_IDF_TARGET_ESP32
                // RTC memory thường không quản lý bằng heap_caps_malloc
                // Nếu bạn dùng RTC_DATA_ATTR thì không cần free
                // Nếu bạn có hàm rtc_free() riêng thì gọi ở đây
                // rtc_free(ptr);
                if (esp_ptr_in_rtc_fast(ptr) || esp_ptr_in_rtc_slow(ptr)) {
                    // RTC memory
                    ESP_LOGW(TAG, "RTC memory is statically allocated, not freed");
                    return;
                }
            #else
                if (esp_ptr_in_rtc_slow(ptr)) {
                    // RTC memory (chỉ slow)
                    ESP_LOGW(TAG, "RTC memory is statically allocated, not freed");
                    return;
                }
            #endif

            // Kiểm tra nếu con trỏ thuộc internal RAM
            if (esp_ptr_internal(ptr)) {
                ESP_LOGD(TAG, "Freeing memory in Internal RAM: %p", ptr);
                heap_caps_free(ptr);
                return;
            }

            // Kiểm tra nếu con trỏ thuộc PSRAM (external RAM)
            if (esp_ptr_external_ram(ptr)) {
                ESP_LOGD(TAG, "Freeing memory in PSRAM: %p", ptr);
                heap_caps_free(ptr);
                return;
            }

            // Nếu không thuộc heap nào
            ESP_LOGW("MemoryManager", "Pointer %p is outside known heap areas", ptr);
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
            ESP_LOGI(TAG, "Internal RAM: Free Bytes: %u, Min Free Bytes: %u, Largest Free Block: %u",
                     (unsigned)internalInfo.freeBytes, (unsigned)internalInfo.minFreeBytes, (unsigned)internalInfo.largestFreeBlock);
            ESP_LOGI(TAG, "PSRAM: Free Bytes: %u, Min Free Bytes: %u, Largest Free Block: %u",
                     (unsigned)psramInfo.freeBytes, (unsigned)psramInfo.minFreeBytes, (unsigned)psramInfo.largestFreeBlock);
            ESP_LOGI(TAG, "RTC RAM: Free Bytes: %u, Min Free Bytes: %u, Largest Free Block: %u",
                     (unsigned)rtcInfo.freeBytes, (unsigned)rtcInfo.minFreeBytes, (unsigned)rtcInfo.largestFreeBlock);
        }
    private:
        static constexpr const char* TAG = "MemoryManager";
        RTC_DATA_ATTR static uint8_t rtcMemory[1024];
};