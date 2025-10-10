#ifndef PSRAM_ALLOCATOR_H
#define PSRAM_ALLOCATOR_H

#include "memory_management.h"
#include <cstddef>
#include <new>

// Custom allocator for PSRAM
template<typename T>
class PsramAllocator {
public:
    typedef T value_type;
    
    PsramAllocator() = default;
    template<class U> constexpr PsramAllocator(const PsramAllocator<U>&) noexcept {}
    
    T* allocate(std::size_t n) {
        void* ptr = MemoryManager::allocatePsram(n * sizeof(T));
        if (!ptr) {
            throw std::bad_alloc();
        }
        return static_cast<T*>(ptr);
    }
    
    void deallocate(T* p, std::size_t n) noexcept {
        MemoryManager::freeMemory(p);
    }
};

template<typename T, typename U>
bool operator==(const PsramAllocator<T>&, const PsramAllocator<U>&) { return true; }

template<typename T, typename U>
bool operator!=(const PsramAllocator<T>&, const PsramAllocator<U>&) { return false; }

#endif // PSRAM_ALLOCATOR_H