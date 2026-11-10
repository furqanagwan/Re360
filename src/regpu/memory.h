#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "regpu/regpu.h"

// The guest's 4 GB address space (Host::base): guest address A lives at g_base + A.
inline uint8_t* g_base = nullptr;

template <typename T = void>
inline T* GuestPtr(uint32_t address)
{
    return reinterpret_cast<T*>(g_base + address);
}

inline uint32_t GuestAddr(const void* host)
{
    return uint32_t(static_cast<const uint8_t*>(host) - g_base);
}

inline uint32_t LoadBE32(uint32_t address) { return __builtin_bswap32(*GuestPtr<uint32_t>(address)); }
inline void StoreBE32(uint32_t address, uint32_t v) { *GuestPtr<uint32_t>(address) = __builtin_bswap32(v); }
inline uint64_t LoadBE64(uint32_t address) { return __builtin_bswap64(*GuestPtr<uint64_t>(address)); }
inline void StoreBE64(uint32_t address, uint64_t v) { *GuestPtr<uint64_t>(address) = __builtin_bswap64(v); }

// Guest memory from the game's allocator (Host::alloc).
enum class Heap
{
    Virtual,
    Physical, // GPU-visible
};

inline uint32_t RegpuGuestAlloc(Heap heap, uint32_t size, uint32_t alignment = 16)
{
    return regpu::GetHost().alloc(heap == Heap::Physical, size, alignment);
}

// File-mapping handle of the 512 MB physical memory section (Host::physicalSection), or null.
inline void* RegpuPhysicalSection() { return regpu::GetHost().physicalSection; }
