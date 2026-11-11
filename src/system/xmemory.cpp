/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2020 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 *
 * @modified    Tom Clay, 2026 - Adapted for ReXGlue runtime
 */

#include <algorithm>
#include <cstring>
#include <utility>

#include <fmt/format.h>

#include <rex/assert.h>
#include <rex/chrono/clock.h>
#include <rex/cvar.h>
#include <rex/logging.h>
#include <rex/math.h>
#include <rex/stream.h>
#include <rex/system/function_dispatcher.h>
#include <rex/system/mmio_handler.h>
#include <rex/system/xmemory.h>
#include <rex/thread.h>

#include <rex/system/xtypes.h>

#include <windows.h>

REXCVAR_DEFINE_BOOL(protect_zero, true, "Memory", "Protect the zero page from reads and writes")
    .lifecycle(rex::cvar::Lifecycle::kRequiresRestart);

REXCVAR_DEFINE_BOOL(protect_on_release, false, "Memory",
                    "Protect released memory to prevent accesses");

REXCVAR_DEFINE_BOOL(scribble_heap, false, "Memory", "Scribble 0xCD into all allocated heap memory");

namespace rex::memory {

uint32_t get_page_count(uint32_t value, uint32_t page_size, uint32_t page_size_shift) {
  return rex::round_up(value, page_size) >> page_size_shift;
}

static memory::Memory* active_memory_ = nullptr;

void CrashDump() {
  static std::atomic<int> in_crash_dump(0);
  if (in_crash_dump.fetch_add(1)) {
    return;
  }
  active_memory_->DumpMap();
  --in_crash_dump;
}

Memory::Memory() {
  system_page_size_ = uint32_t(rex::memory::page_size());
  system_allocation_granularity_ = uint32_t(rex::memory::allocation_granularity());
  assert_zero(active_memory_);
  active_memory_ = this;
}

Memory::~Memory() {
  assert_true(active_memory_ == this);
  active_memory_ = nullptr;

  mmio_handler_.reset();

  for (auto invalidation_callback : physical_memory_invalidation_callbacks_) {
    delete invalidation_callback;
  }

  heaps_.v00000000.Dispose();
  heaps_.v40000000.Dispose();
  heaps_.v80000000.Dispose();
  heaps_.v90000000.Dispose();
  heaps_.vA0000000.Dispose();
  heaps_.vC0000000.Dispose();
  heaps_.vE0000000.Dispose();
  heaps_.physical.Dispose();

  if (mapping_ != rex::memory::kFileMappingHandleInvalid) {
    UnmapViews();
    rex::memory::CloseFileMappingHandle(mapping_, file_name_);
    mapping_base_ = nullptr;
    mapping_ = rex::memory::kFileMappingHandleInvalid;
  }

  virtual_membase_ = nullptr;
  physical_membase_ = nullptr;
}

bool Memory::Initialize() {
  file_name_ = fmt::format("xenia_memory_{}", chrono::Clock::QueryHostTickCount());

  const size_t mapping_size =
      rex::round_up(static_cast<size_t>(0x120000000ull) + system_allocation_granularity_,
                    static_cast<size_t>(system_allocation_granularity_));
  mapping_ = rex::memory::CreateFileMappingHandle(file_name_, mapping_size,
                                                  rex::memory::PageAccess::kReadWrite, false);
  if (mapping_ == rex::memory::kFileMappingHandleInvalid) {
    REXSYS_ERROR("Unable to reserve the 4gb guest address space.");
    assert_always();
    return false;
  }

  mapping_base_ = 0;
  for (size_t n = 32; n < 64; n++) {
    auto mapping_base = reinterpret_cast<uint8_t*>(1ull << n);
    if (!MapViews(mapping_base)) {
      mapping_base_ = mapping_base;
      break;
    }
  }
  if (!mapping_base_) {
    REXSYS_ERROR("Unable to find a continuous block in the 64bit address space.");
    assert_always();
    return false;
  }
  virtual_membase_ = mapping_base_;
  physical_membase_ = mapping_base_ + 0x100000000ull;

  REXSYS_INFO("Guest memory arena mapped: virtual base 0x{:016X}, physical base 0x{:016X}",
              reinterpret_cast<uintptr_t>(virtual_membase_),
              reinterpret_cast<uintptr_t>(physical_membase_));

  heaps_.v00000000.Initialize(this, virtual_membase_, memory::HeapType::kGuestVirtual, 0x00000000,
                              0x40000000, 4096);
  heaps_.v40000000.Initialize(this, virtual_membase_, memory::HeapType::kGuestVirtual, 0x40000000,
                              0x40000000 - 0x01000000, 64 * 1024);
  heaps_.v80000000.Initialize(this, virtual_membase_, memory::HeapType::kGuestXex, 0x80000000,
                              0x10000000, 64 * 1024);
  heaps_.v90000000.Initialize(this, virtual_membase_, memory::HeapType::kGuestXex, 0x90000000,
                              0x10000000, 4096);

  heaps_.physical.Initialize(this, physical_membase_, memory::HeapType::kGuestPhysical, 0x00000000,
                             0x20000000, 4096);
  heaps_.vA0000000.Initialize(this, virtual_membase_, memory::HeapType::kGuestPhysical, 0xA0000000,
                              0x20000000, 64 * 1024, &heaps_.physical);
  heaps_.vC0000000.Initialize(this, virtual_membase_, memory::HeapType::kGuestPhysical, 0xC0000000,
                              0x20000000, 16 * 1024 * 1024, &heaps_.physical);
  heaps_.vE0000000.Initialize(this, virtual_membase_, memory::HeapType::kGuestPhysical, 0xE0000000,
                              0x1FD00000, 4096, &heaps_.physical);

  heaps_.v00000000.AllocFixed(0x00000000, 0x10000, 0x10000,
                              memory::kMemoryAllocationReserve | memory::kMemoryAllocationCommit,
                              !REXCVAR_GET(protect_zero)
                                  ? memory::kMemoryProtectRead | memory::kMemoryProtectWrite
                                  : memory::kMemoryProtectNoAccess);
  heaps_.physical.AllocFixed(0x1FFF0000, 0x10000, 0x10000, memory::kMemoryAllocationReserve,
                             memory::kMemoryProtectNoAccess);

  heaps_.vC0000000.AllocFixed(0xC0000000, 0x01000000, 32,
                              memory::kMemoryAllocationReserve | memory::kMemoryAllocationCommit,
                              memory::kMemoryProtectRead | memory::kMemoryProtectWrite);

  rex::memory::AllocFixed(heaps_.physical.TranslateRelative(0), heaps_.physical.heap_size(),
                          rex::memory::AllocationType::kCommit,
                          rex::memory::PageAccess::kReadWrite);

  mmio_handler_ = runtime::MMIOHandler::Install(
      virtual_membase_, physical_membase_, physical_membase_ + 0x1FFFFFFF, HostToGuestVirtualThunk,
      this, AccessViolationCallbackThunk, this);
  if (!mmio_handler_) {
    REXSYS_ERROR("Unable to install MMIO handlers");
    assert_always();
    return false;
  }
  REXSYS_DEBUG("Installed MMIO handler for physical address translation");

  uint32_t unk_phys_alloc;
  heaps_.vA0000000.Alloc(0x340000, 64 * 1024, memory::kMemoryAllocationReserve,
                         memory::kMemoryProtectNoAccess, true, &unk_phys_alloc);

  uint32_t unknown_xex_range;
  heaps_.v80000000.Alloc(0x40000, 4 * 1024, memory::kMemoryAllocationCommit,
                         memory::kMemoryProtectRead | memory::kMemoryProtectWrite, false,
                         &unknown_xex_range);

  uint32_t value_to_write = rex::byte_swap(uint32_t(0x2a6e3f38));
  std::memcpy(TranslateVirtual(0x80000000 + 0x1C), &value_to_write, sizeof(uint32_t));

  return true;
}

static const struct {
  uint64_t virtual_address_start;
  uint64_t virtual_address_end;
  uint64_t target_address;
} map_info[] = {

    {
        0x00000000,
        0x3FFFFFFF,
        0x0000000000000000ull,
    },

    {
        0x40000000,
        0x7EFFFFFF,
        0x0000000040000000ull,
    },

    {
        0x7F000000,
        0x7FFFFFFF,
        0x0000000100000000ull,
    },

    {
        0x80000000,
        0x8FFFFFFF,
        0x0000000080000000ull,
    },

    {
        0x90000000,
        0x9FFFFFFF,
        0x0000000080000000ull,
    },

    {
        0xA0000000,
        0xBFFFFFFF,
        0x0000000100000000ull,
    },

    {
        0xC0000000,
        0xDFFFFFFF,
        0x0000000100000000ull,
    },

    {
        0xE0000000,
        0xFFFFFFFF,
        0x0000000100001000ull,
    },

    {
        0x100000000,
        0x11FFFFFFF,
        0x0000000100000000ull,
    },
};

int Memory::MapViews(uint8_t* mapping_base) {
  assert_true(rex::countof(map_info) == rex::countof(views_.all_views));

  uint64_t granularity_mask = ~uint64_t(system_allocation_granularity_ - 1);
  for (size_t n = 0; n < rex::countof(map_info); n++) {
    views_.all_views[n] = reinterpret_cast<uint8_t*>(rex::memory::MapFileView(
        mapping_, mapping_base + map_info[n].virtual_address_start,
        map_info[n].virtual_address_end - map_info[n].virtual_address_start + 1,
        rex::memory::PageAccess::kReadWrite, map_info[n].target_address & granularity_mask));
    if (!views_.all_views[n]) {
      UnmapViews();
      return 1;
    }
  }
  return 0;
}

void Memory::UnmapViews() {
  for (size_t n = 0; n < rex::countof(views_.all_views); n++) {
    if (views_.all_views[n]) {
      size_t length = map_info[n].virtual_address_end - map_info[n].virtual_address_start + 1;
      rex::memory::UnmapFileView(mapping_, views_.all_views[n], length);
    }
  }
}

void Memory::Reset() {
  heaps_.v00000000.Reset();
  heaps_.v40000000.Reset();
  heaps_.v80000000.Reset();
  heaps_.v90000000.Reset();
  heaps_.physical.Reset();
}

const BaseHeap* Memory::LookupHeap(uint32_t address) const {
  if (address < 0x40000000) {
    return &heaps_.v00000000;
  } else if (address < 0x7F000000) {
    return &heaps_.v40000000;
  } else if (address < 0x80000000) {
    return nullptr;
  } else if (address < 0x90000000) {
    return &heaps_.v80000000;
  } else if (address < 0xA0000000) {
    return &heaps_.v90000000;
  } else if (address < 0xC0000000) {
    return &heaps_.vA0000000;
  } else if (address < 0xE0000000) {
    return &heaps_.vC0000000;
  } else if (address < 0xFFD00000) {
    return &heaps_.vE0000000;
  } else {
    return nullptr;
  }
}

BaseHeap* Memory::LookupHeapByType(bool physical, uint32_t page_size) {
  if (physical) {
    if (page_size <= 4096) {
      return &heaps_.vE0000000;
    } else if (page_size <= 64 * 1024) {
      return &heaps_.vA0000000;
    } else {
      return &heaps_.vC0000000;
    }
  } else {
    if (page_size <= 4096) {
      return &heaps_.v00000000;
    } else {
      return &heaps_.v40000000;
    }
  }
}

VirtualHeap* Memory::GetPhysicalHeap() {
  return &heaps_.physical;
}

uint32_t Memory::HostToGuestVirtual(const void* host_address) const {
  size_t virtual_address =
      reinterpret_cast<size_t>(host_address) - reinterpret_cast<size_t>(virtual_membase_);
  uint32_t vE0000000_host_offset = heaps_.vE0000000.host_address_offset();
  size_t vE0000000_host_base = size_t(heaps_.vE0000000.heap_base()) + vE0000000_host_offset;
  if (virtual_address >= vE0000000_host_base &&
      virtual_address <= (vE0000000_host_base + (heaps_.vE0000000.heap_size() - 1))) {
    virtual_address -= vE0000000_host_offset;
  }
  return uint32_t(virtual_address);
}

uint32_t Memory::HostToGuestVirtualThunk(const void* context, const void* host_address) {
  return reinterpret_cast<const memory::Memory*>(context)->HostToGuestVirtual(host_address);
}

uint32_t Memory::GetPhysicalAddress(uint32_t address) const {
  const BaseHeap* heap = LookupHeap(address);
  if (!heap || heap->heap_type() != memory::HeapType::kGuestPhysical) {
    return UINT32_MAX;
  }
  return static_cast<const PhysicalHeap*>(heap)->GetPhysicalAddress(address);
}

void Memory::Zero(uint32_t address, uint32_t size) {
  std::memset(TranslateVirtual(address), 0, size);
}

void Memory::Fill(uint32_t address, uint32_t size, uint8_t value) {
  std::memset(TranslateVirtual(address), value, size);
}

void Memory::Copy(uint32_t dest, uint32_t src, uint32_t size) {
  uint8_t* pdest = TranslateVirtual(dest);
  const uint8_t* psrc = TranslateVirtual(src);
  std::memcpy(pdest, psrc, size);
}

uint32_t Memory::SearchAligned(uint32_t start, uint32_t end, const uint32_t* values,
                               size_t value_count) {
  assert_true(start <= end);
  auto p = TranslateVirtual<const uint32_t*>(start);
  auto pe = TranslateVirtual<const uint32_t*>(end);
  while (p != pe) {
    if (*p == values[0]) {
      const uint32_t* pc = p + 1;
      size_t matched = 1;
      for (size_t n = 1; n < value_count; n++, pc++) {
        if (*pc != values[n]) {
          break;
        }
        matched++;
      }
      if (matched == value_count) {
        return HostToGuestVirtual(p);
      }
    }
    p++;
  }
  return 0;
}

bool Memory::AddVirtualMappedRange(uint32_t virtual_address, uint32_t mask, uint32_t size,
                                   void* context, runtime::MMIOReadCallback read_callback,
                                   runtime::MMIOWriteCallback write_callback) {
  const size_t host_page_size = rex::memory::page_size();
  uint8_t* const range_start = TranslateVirtual(virtual_address);
  uint8_t* const aligned_start = reinterpret_cast<uint8_t*>(
      reinterpret_cast<uintptr_t>(range_start) & ~(uintptr_t(host_page_size) - 1));
  const size_t aligned_length =
      rex::round_up(size_t(range_start - aligned_start) + size, host_page_size);
  if (!rex::memory::AllocFixed(aligned_start, aligned_length, rex::memory::AllocationType::kCommit,
                               rex::memory::PageAccess::kNoAccess)) {
    REXSYS_ERROR("Unable to map range; commit/protect failed");
    return false;
  }

  return mmio_handler_->RegisterRange(virtual_address, mask, size, context, read_callback,
                                      write_callback);
}

runtime::MMIORange* Memory::LookupVirtualMappedRange(uint32_t virtual_address) {
  return mmio_handler_->LookupRange(virtual_address);
}

bool Memory::AccessViolationCallback(std::unique_lock<std::recursive_mutex> global_lock_locked_once,
                                     void* host_address, bool is_write) {
  if (reinterpret_cast<size_t>(host_address) < reinterpret_cast<size_t>(virtual_membase_) ||
      reinterpret_cast<size_t>(host_address) >= reinterpret_cast<size_t>(physical_membase_)) {
    return false;
  }
  uint32_t virtual_address = HostToGuestVirtual(host_address);
  BaseHeap* heap = LookupHeap(virtual_address);
  if (!heap || heap->heap_type() != memory::HeapType::kGuestPhysical) {
    REXSYS_ERROR(
        "Unhandled guest access violation: {} of guest 0x{:08X} (host 0x{:016X}) on thread 0x{:X}",
        is_write ? "write" : "read", virtual_address, reinterpret_cast<uintptr_t>(host_address),
        rex::thread::current_thread_id());
    return false;
  }

  auto physical_heap = static_cast<PhysicalHeap*>(heap);
  if (physical_heap->TriggerCallbacks(std::move(global_lock_locked_once), virtual_address, 1,
                                      is_write, false)) {
    return true;
  }

  if (is_write) {
    constexpr uint32_t kWriteProtectMask =
        memory::kMemoryProtectWrite | memory::kMemoryProtectWriteCombine;
    uint32_t guest_protect = 0;
    bool allow_write = false;
    if (heap->QueryProtect(virtual_address, &guest_protect) &&
        (guest_protect & kWriteProtectMask)) {
      allow_write = true;
    } else {
      HeapAllocationInfo guest_info{};
      if (heap->QueryRegionInfo(virtual_address, &guest_info) &&
          (guest_info.state & memory::kMemoryAllocationCommit) &&
          (guest_info.allocation_protect & kWriteProtectMask)) {
        guest_protect = guest_info.protect;
        allow_write = true;
      } else {
        uint32_t physical_address = GetPhysicalAddress(virtual_address);
        if (physical_address != UINT32_MAX) {
          uint32_t physical_protect = 0;
          if (heaps_.physical.QueryProtect(physical_address, &physical_protect) &&
              (physical_protect & kWriteProtectMask)) {
            guest_protect = physical_protect;
            allow_write = true;
          } else {
            HeapAllocationInfo physical_info{};
            if (heaps_.physical.QueryRegionInfo(physical_address, &physical_info) &&
                (physical_info.state & memory::kMemoryAllocationCommit) &&
                (physical_info.allocation_protect & kWriteProtectMask)) {
              guest_protect = physical_info.protect;
              allow_write = true;
            }
          }
        }
      }
    }
    if (allow_write) {
      size_t host_page_size = rex::memory::page_size();
      uintptr_t page_base =
          reinterpret_cast<uintptr_t>(host_address) & ~(uintptr_t(host_page_size - 1));
      if (rex::memory::Protect(reinterpret_cast<void*>(page_base), host_page_size,
                               rex::memory::PageAccess::kReadWrite, nullptr)) {
        REXSYS_WARN(
            "Recovered stale physical page protection for guest {:08X} (host {:016X}, "
            "guest_protect {:08X})",
            virtual_address, static_cast<uint64_t>(page_base), guest_protect);
        return true;
      }
    }

    uint32_t current_guest_protect = 0;
    heap->QueryProtect(virtual_address, &current_guest_protect);
    uint32_t physical_address = GetPhysicalAddress(virtual_address);
    uint32_t physical_protect = 0;
    if (physical_address != UINT32_MAX) {
      heaps_.physical.QueryProtect(physical_address, &physical_protect);
    }
    REXSYS_ERROR(
        "Unhandled guest physical write fault: guest={:08X} host={:016X} phys={:08X} "
        "guest_protect={:08X} physical_protect={:08X}",
        virtual_address, static_cast<uint64_t>(reinterpret_cast<uintptr_t>(host_address)),
        physical_address, current_guest_protect, physical_protect);
  }

  return false;
}

bool Memory::AccessViolationCallbackThunk(
    std::unique_lock<std::recursive_mutex> global_lock_locked_once, void* context,
    void* host_address, bool is_write) {
  return reinterpret_cast<memory::Memory*>(context)->AccessViolationCallback(
      std::move(global_lock_locked_once), host_address, is_write);
}

bool Memory::TriggerPhysicalMemoryCallbacks(
    std::unique_lock<std::recursive_mutex> global_lock_locked_once, uint32_t virtual_address,
    uint32_t length, bool is_write, bool unwatch_exact_range, bool unprotect) {
  BaseHeap* heap = LookupHeap(virtual_address);
  if (!heap) {
    return false;
  }
  if (heap->heap_type() == memory::HeapType::kGuestPhysical) {
    auto physical_heap = static_cast<PhysicalHeap*>(heap);
    return physical_heap->TriggerCallbacks(std::move(global_lock_locked_once), virtual_address,
                                           length, is_write, unwatch_exact_range, unprotect);
  }
  return false;
}

void* Memory::RegisterPhysicalMemoryInvalidationCallback(
    PhysicalMemoryInvalidationCallback callback, void* callback_context) {
  auto entry = new std::pair<PhysicalMemoryInvalidationCallback, void*>(callback, callback_context);
  auto lock = global_critical_region_.Acquire();
  physical_memory_invalidation_callbacks_.push_back(entry);
  return entry;
}

void Memory::UnregisterPhysicalMemoryInvalidationCallback(void* callback_handle) {
  auto entry =
      reinterpret_cast<std::pair<PhysicalMemoryInvalidationCallback, void*>*>(callback_handle);
  {
    auto lock = global_critical_region_.Acquire();
    auto it = std::find(physical_memory_invalidation_callbacks_.begin(),
                        physical_memory_invalidation_callbacks_.end(), entry);
    assert_true(it != physical_memory_invalidation_callbacks_.end());
    if (it != physical_memory_invalidation_callbacks_.end()) {
      physical_memory_invalidation_callbacks_.erase(it);
    }
  }
  delete entry;
}

void Memory::EnablePhysicalMemoryAccessCallbacks(uint32_t physical_address, uint32_t length,
                                                 bool enable_invalidation_notifications,
                                                 bool enable_data_providers) {
  heaps_.vA0000000.EnableAccessCallbacks(physical_address, length,
                                         enable_invalidation_notifications, enable_data_providers);
  heaps_.vC0000000.EnableAccessCallbacks(physical_address, length,
                                         enable_invalidation_notifications, enable_data_providers);
  heaps_.vE0000000.EnableAccessCallbacks(physical_address, length,
                                         enable_invalidation_notifications, enable_data_providers);
}

uint32_t Memory::SystemHeapAlloc(uint32_t size, uint32_t alignment, uint32_t system_heap_flags) {
  bool is_physical = !!(system_heap_flags & memory::kSystemHeapPhysical);
  auto heap = LookupHeapByType(is_physical, 4096);
  uint32_t address;
  if (!heap->AllocSystemHeap(
          size, alignment, memory::kMemoryAllocationReserve | memory::kMemoryAllocationCommit,
          memory::kMemoryProtectRead | memory::kMemoryProtectWrite, false, &address)) {
    return 0;
  }
  Zero(address, size);
  return address;
}

void Memory::SystemHeapFree(uint32_t address, uint32_t* out_region_size) {
  if (!address) {
    return;
  }

  auto heap = LookupHeap(address);
  heap->Release(address, out_region_size);
}

void Memory::GetHeapsPageStatsSummary(const BaseHeap* const* provided_heaps, size_t heaps_count,
                                      uint32_t& unreserved_pages, uint32_t& reserved_pages,
                                      uint32_t& used_pages, uint32_t& reserved_bytes) {
  for (size_t i = 0; i < heaps_count; i++) {
    const BaseHeap* heap = provided_heaps[i];
    uint32_t heap_unreserved = heap->unreserved_page_count();
    uint32_t heap_reserved = heap->reserved_page_count();

    unreserved_pages += heap_unreserved;
    reserved_pages += heap_reserved;
    used_pages += ((heap->total_page_count() - heap_unreserved) * heap->page_size()) / 4096;
    reserved_bytes += heap_reserved * heap->page_size();
  }
}

void Memory::DumpMap() {
  REXSYS_ERROR("==================================================================");
  REXSYS_ERROR("Memory Dump");
  REXSYS_ERROR("==================================================================");
  REXSYS_ERROR("               System Page Size: {0} ({0:08X})", system_page_size_);
  REXSYS_ERROR("  System Allocation Granularity: {0} ({0:08X})", system_allocation_granularity_);
  REXSYS_ERROR("                Virtual Membase: {}", fmt::ptr(virtual_membase_));
  REXSYS_ERROR("               Physical Membase: {}", fmt::ptr(physical_membase_));
  REXSYS_ERROR("");
  REXSYS_ERROR("------------------------------------------------------------------");
  REXSYS_ERROR("Virtual Heaps");
  REXSYS_ERROR("------------------------------------------------------------------");
  REXSYS_ERROR("");
  heaps_.v00000000.DumpMap();
  heaps_.v40000000.DumpMap();
  heaps_.v80000000.DumpMap();
  heaps_.v90000000.DumpMap();
  REXSYS_ERROR("");
  REXSYS_ERROR("------------------------------------------------------------------");
  REXSYS_ERROR("Physical Heaps");
  REXSYS_ERROR("------------------------------------------------------------------");
  REXSYS_ERROR("");
  heaps_.physical.DumpMap();
  heaps_.vA0000000.DumpMap();
  heaps_.vC0000000.DumpMap();
  heaps_.vE0000000.DumpMap();
  REXSYS_ERROR("");
}

bool Memory::Save(stream::ByteStream* stream) {
  REXSYS_DEBUG("Serializing memory...");
  heaps_.v00000000.Save(stream);
  heaps_.v40000000.Save(stream);
  heaps_.v80000000.Save(stream);
  heaps_.v90000000.Save(stream);
  heaps_.physical.Save(stream);

  return true;
}

bool Memory::Restore(stream::ByteStream* stream) {
  REXSYS_DEBUG("Restoring memory...");
  heaps_.v00000000.Restore(stream);
  heaps_.v40000000.Restore(stream);
  heaps_.v80000000.Restore(stream);
  heaps_.v90000000.Restore(stream);
  heaps_.physical.Restore(stream);

  return true;
}

bool Memory::InitializeFunctionTable(uint32_t code_base, uint32_t code_size, uint32_t table_base) {
  constexpr uint32_t kThunkReserveSize = runtime::FunctionDispatcher::kThunkReserveSize;
  uint32_t table_size = (code_size + kThunkReserveSize) * 2;

  REXSYS_DEBUG(
      "Initializing function table at {:08X}, size {:08X} for code {:08X}-{:08X} "
      "(+{:08X} thunk reserve)",
      table_base, table_size, code_base, code_base + code_size, kThunkReserveSize);

  if (!heaps_.v80000000.AllocFixed(
          table_base, table_size, 0x10000,
          memory::kMemoryAllocationReserve | memory::kMemoryAllocationCommit,
          memory::kMemoryProtectRead | memory::kMemoryProtectWrite)) {
    REXSYS_ERROR("Failed to allocate function table at {:08X}", table_base);
    return false;
  }

  Zero(table_base, table_size);

  std::lock_guard lock(function_tables_mutex_);
  function_tables_.push_back({
      .table_base = table_base,
      .code_base = code_base,
      .code_size = code_size,
      .thunk_reserve = kThunkReserveSize,
  });

  return true;
}

bool Memory::DestroyFunctionTable(uint32_t code_base) {
  uint32_t table_base = 0;
  uint32_t table_size = 0;
  uint32_t logged_code_base = 0;
  uint32_t logged_code_size = 0;
  {
    std::lock_guard lock(function_tables_mutex_);
    auto it = std::find_if(
        function_tables_.begin(), function_tables_.end(),
        [code_base](const FunctionTableEntry& entry) { return entry.code_base == code_base; });
    if (it == function_tables_.end()) {
      return false;
    }
    table_base = it->table_base;
    table_size = (it->code_size + it->thunk_reserve) * 2;
    logged_code_base = it->code_base;
    logged_code_size = it->code_size;
    function_tables_.erase(it);
  }

  REXSYS_DEBUG("Destroying function table at {:08X}, size {:08X} for code {:08X}-{:08X}",
               table_base, table_size, logged_code_base, logged_code_base + logged_code_size);

  heaps_.v80000000.Release(table_base);
  return true;
}

bool Memory::SetFunction(uint32_t guest_address, PPCFunc* host_function) {
  uint32_t table_address = 0;
  {
    std::lock_guard lock(function_tables_mutex_);
    for (const auto& entry : function_tables_) {
      uint32_t range_end = entry.code_base + entry.code_size + entry.thunk_reserve;
      if (guest_address >= entry.code_base && guest_address < range_end) {
        uint64_t offset = (uint64_t(guest_address) - entry.code_base) * 2;
        table_address = entry.table_base + uint32_t(offset);
        break;
      }
    }
  }
  if (!table_address) {
    return false;
  }
  auto* slot = TranslateVirtual<PPCFunc**>(table_address);
  *slot = host_function;
  return true;
}

bool Memory::HasAnyFunctionTable() const {
  std::lock_guard lock(function_tables_mutex_);
  return !function_tables_.empty();
}

rex::memory::PageAccess ToPageAccess(uint32_t protect) {
  bool is_writable =
      (protect & memory::kMemoryProtectWrite) || (protect & memory::kMemoryProtectWriteCombine);

  if ((protect & memory::kMemoryProtectRead) && !is_writable) {
    return rex::memory::PageAccess::kReadOnly;
  } else if ((protect & memory::kMemoryProtectRead) && is_writable) {
    return rex::memory::PageAccess::kReadWrite;
  } else {
    return rex::memory::PageAccess::kNoAccess;
  }
}

uint32_t FromPageAccess(rex::memory::PageAccess protect) {
  switch (protect) {
    case memory::PageAccess::kNoAccess:
      return memory::kMemoryProtectNoAccess;
    case memory::PageAccess::kReadOnly:
      return memory::kMemoryProtectRead;
    case memory::PageAccess::kReadWrite:
      return memory::kMemoryProtectRead | memory::kMemoryProtectWrite;
    case memory::PageAccess::kExecuteReadOnly:

      assert_always();
      return memory::kMemoryProtectRead;
    case memory::PageAccess::kExecuteReadWrite:

      assert_always();
      return memory::kMemoryProtectRead | memory::kMemoryProtectWrite;
  }

  return memory::kMemoryProtectNoAccess;
}

BaseHeap::BaseHeap() : membase_(nullptr), heap_base_(0), heap_size_(0), page_size_(0) {}

BaseHeap::~BaseHeap() = default;

std::unique_lock<std::recursive_mutex> BaseHeap::AcquireHostPageReconcileLock() const {
  if (page_size_ >= memory_->system_page_size_) {
    return {};
  }
  return rex::thread::global_critical_region::AcquireDirect();
}

bool BaseHeap::SyncHostPageAccess(uint32_t start_page_number, uint32_t end_page_number) {
  const uint32_t host_page_size = memory_->system_page_size_;
  if (page_size_ >= host_page_size) {
    return true;
  }

  const auto get_host_page_access = [&](uint32_t host_page_number) {
    const uint64_t host_page_start = uint64_t(host_page_number) * host_page_size;
    const uint64_t host_page_end = host_page_start + host_page_size;
    const uint64_t heap_host_start = host_address_offset_;
    const uint64_t heap_host_end = heap_host_start + heap_size_;
    const uint64_t overlap_start = std::max(host_page_start, heap_host_start);
    const uint64_t overlap_end = std::min(host_page_end, heap_host_end);
    if (overlap_start >= overlap_end) {
      return rex::memory::PageAccess::kNoAccess;
    }

    const uint32_t guest_page_start = uint32_t(overlap_start - heap_host_start) >> page_size_shift_;
    const uint32_t guest_page_end = uint32_t(overlap_end - heap_host_start - 1) >> page_size_shift_;

    bool has_read = false;
    bool has_write = false;
    bool has_committed = false;
    for (uint32_t page_number = guest_page_start; page_number <= guest_page_end; ++page_number) {
      const auto& page_entry = page_table_[page_number];
      if (!(page_entry.state & memory::kMemoryAllocationCommit)) {
        continue;
      }
      has_committed = true;
      const rex::memory::PageAccess page_access = ToPageAccess(page_entry.current_protect);
      if (page_access == rex::memory::PageAccess::kReadWrite) {
        has_read = true;
        has_write = true;
        break;
      }
      if (page_access == rex::memory::PageAccess::kReadOnly) {
        has_read = true;
      }
    }

    rex::memory::PageAccess access;

    if (!has_committed && heap_type_ == memory::HeapType::kGuestPhysical &&
        !REXCVAR_GET(protect_on_release)) {
      access = rex::memory::PageAccess::kReadWrite;
    } else if (!has_read) {
      access = rex::memory::PageAccess::kNoAccess;
    } else {
      access = has_write ? rex::memory::PageAccess::kReadWrite : rex::memory::PageAccess::kReadOnly;
    }

    if (access == rex::memory::PageAccess::kReadWrite && IsHostPageWriteWatched(host_page_number)) {
      access = rex::memory::PageAccess::kReadOnly;
    }
    return access;
  };

  const uint64_t guest_byte_start = uint64_t(start_page_number) << page_size_shift_;
  const uint64_t guest_byte_end = ((uint64_t(end_page_number) + 1) << page_size_shift_) - 1;
  const uint32_t host_page_first =
      uint32_t((guest_byte_start + host_address_offset_) / host_page_size);
  const uint32_t host_page_last =
      uint32_t((guest_byte_end + host_address_offset_) / host_page_size);

  uint8_t* const heap_host_base = membase_ + heap_base_;
  uint32_t run_first_page = host_page_first;
  rex::memory::PageAccess run_access = get_host_page_access(host_page_first);
  const auto flush_run = [&](uint32_t run_last_page) {
    const size_t run_length = (size_t(run_last_page - run_first_page) + 1) * host_page_size;
    uint8_t* const run_pointer = heap_host_base + size_t(run_first_page) * host_page_size;
    if (!rex::memory::Protect(run_pointer, run_length, run_access, nullptr)) {
      REXSYS_ERROR("BaseHeap::SyncHostPageAccess failed for heap {:08X} host pages {}-{}",
                   heap_base_, run_first_page, run_last_page);
      return false;
    }
    return true;
  };

  for (uint32_t host_page = host_page_first + 1; host_page <= host_page_last; ++host_page) {
    const rex::memory::PageAccess access = get_host_page_access(host_page);
    if (access == run_access) {
      continue;
    }
    if (!flush_run(host_page - 1)) {
      return false;
    }
    run_first_page = host_page;
    run_access = access;
  }
  return flush_run(host_page_last);
}

void BaseHeap::Initialize(memory::Memory* memory, uint8_t* membase, HeapType heap_type,
                          uint32_t heap_base, uint32_t heap_size, uint32_t page_size,
                          uint32_t host_address_offset) {
  memory_ = memory;
  membase_ = membase;
  heap_type_ = heap_type;
  heap_base_ = heap_base;
  heap_size_ = heap_size;
  page_size_ = page_size;
  assert_true(rex::is_pow2(page_size_));
  page_size_shift_ = rex::log2_floor(page_size_);
  host_address_offset_ = host_address_offset;
  page_table_.resize(heap_size / page_size);
  unreserved_page_count_ = uint32_t(page_table_.size());
}

void BaseHeap::Dispose() {
  for (uint32_t page_number = 0; page_number < page_table_.size(); ++page_number) {
    auto& page_entry = page_table_[page_number];
    if (page_entry.state) {
      rex::memory::DeallocFixed(TranslateRelative(page_number << page_size_shift_), 0,
                                rex::memory::DeallocationType::kRelease);
      page_number += page_entry.region_page_count;
    }
  }
}

void BaseHeap::DumpMap() {
  std::lock_guard<std::recursive_mutex> heap_lock(heap_mutex_);
  REXSYS_ERROR("------------------------------------------------------------------");
  REXSYS_ERROR("Heap: {:08X}-{:08X}", heap_base_, heap_base_ + (heap_size_ - 1));
  REXSYS_ERROR("------------------------------------------------------------------");
  REXSYS_ERROR("            Heap Base: {:08X}", heap_base_);
  REXSYS_ERROR("            Heap Size: {0} ({0:08X})", heap_size_);
  REXSYS_ERROR("            Page Size: {0} ({0:08X})", page_size_);
  REXSYS_ERROR("           Page Count: {}", page_table_.size());
  REXSYS_ERROR("  Host Address Offset: {0} ({0:08X})", host_address_offset_);
  bool is_empty_span = false;
  uint32_t empty_span_start = 0;
  for (uint32_t i = 0; i < uint32_t(page_table_.size()); ++i) {
    auto& page = page_table_[i];
    if (!page.state) {
      if (!is_empty_span) {
        is_empty_span = true;
        empty_span_start = i;
      }
      continue;
    }
    if (is_empty_span) {
      REXSYS_ERROR("  {:08X}-{:08X} {:6d}p {:10d}b unreserved",
                   heap_base_ + (empty_span_start << page_size_shift_),
                   heap_base_ + (i << page_size_shift_), i - empty_span_start,
                   (i - empty_span_start) << page_size_shift_);
      is_empty_span = false;
    }
    const char* state_name = "   ";
    if (page.state & memory::kMemoryAllocationCommit) {
      state_name = "COM";
    } else if (page.state & memory::kMemoryAllocationReserve) {
      state_name = "RES";
    }
    char access_r = (page.current_protect & memory::kMemoryProtectRead) ? 'R' : ' ';
    char access_w = (page.current_protect & memory::kMemoryProtectWrite) ? 'W' : ' ';
    uint32_t region_pages = page.region_page_count;
    REXSYS_ERROR("  {:08X}-{:08X} {:6d}p {:10d}b {} {}{}", heap_base_ + (i << page_size_shift_),
                 heap_base_ + ((i + region_pages) << page_size_shift_), region_pages,
                 region_pages << page_size_shift_, state_name, access_r, access_w);
    i += region_pages - 1;
  }
  if (is_empty_span) {
    REXSYS_ERROR("  {:08X}-{:08X} - {} unreserved pages)",
                 heap_base_ + (empty_span_start << page_size_shift_), heap_base_ + (heap_size_ - 1),
                 page_table_.size() - empty_span_start);
  }
}

uint32_t BaseHeap::GetTotalPageCount() {
  return uint32_t(page_table_.size());
}

uint32_t BaseHeap::GetUnreservedPageCount() {
  std::lock_guard<std::recursive_mutex> heap_lock(heap_mutex_);
  uint32_t count = 0;
  bool is_empty_span = false;
  uint32_t empty_span_start = 0;
  uint32_t size = uint32_t(page_table_.size());
  for (uint32_t i = 0; i < size; ++i) {
    auto& page = page_table_[i];
    if (!page.state) {
      if (!is_empty_span) {
        is_empty_span = true;
        empty_span_start = i;
      }
      continue;
    }
    if (is_empty_span) {
      is_empty_span = false;
      count += i - empty_span_start;
    }
    i += page.region_page_count - 1;
  }
  if (is_empty_span) {
    count += size - empty_span_start;
  }
  return count;
}

bool BaseHeap::Save(stream::ByteStream* stream) {
  REXSYS_DEBUG("Heap {:08X}-{:08X}", heap_base_, heap_base_ + (heap_size_ - 1));

  for (size_t i = 0; i < page_table_.size(); i++) {
    auto& page = page_table_[i];
    stream->Write(page.qword);
    if (!page.state) {
      continue;
    }

    if (page.state & memory::kMemoryAllocationCommit) {
      void* addr = TranslateRelative(i << page_size_shift_);

      memory::PageAccess old_access;
      memory::Protect(addr, page_size_, memory::PageAccess::kReadWrite, &old_access);

      stream->Write(addr, page_size_);

      memory::Protect(addr, page_size_, old_access, nullptr);
    }
  }

  return true;
}

bool BaseHeap::Restore(stream::ByteStream* stream) {
  REXSYS_DEBUG("Heap {:08X}-{:08X}", heap_base_, heap_base_ + (heap_size_ - 1));

  const uint32_t host_page_size = memory_->system_page_size_;
  const bool reconcile_host_pages = page_size_ < host_page_size;
  uint32_t writable_host_page = UINT32_MAX;

  auto global_lock = AcquireHostPageReconcileLock();

  for (size_t i = 0; i < page_table_.size(); i++) {
    auto& page = page_table_[i];
    page.qword = stream->Read<uint64_t>();
    if (!page.state) {
      continue;
    }

    memory::PageAccess page_access = memory::PageAccess::kNoAccess;
    if ((page.current_protect & memory::kMemoryProtectRead) &&
        (page.current_protect & memory::kMemoryProtectWrite)) {
      page_access = memory::PageAccess::kReadWrite;
    } else if (page.current_protect & memory::kMemoryProtectRead) {
      page_access = memory::PageAccess::kReadOnly;
    }

    if (!(page.state & memory::kMemoryAllocationCommit)) {
      continue;
    }

    void* addr = TranslateRelative(i << page_size_shift_);
    if (!reconcile_host_pages) {
      rex::memory::AllocFixed(addr, page_size_, memory::AllocationType::kCommit,
                              memory::PageAccess::kReadWrite);

      rex::memory::Protect(addr, page_size_, memory::PageAccess::kReadWrite, nullptr);
      stream->Read(addr, page_size_);
      rex::memory::Protect(addr, page_size_, page_access, nullptr);
      continue;
    }

    const uint32_t host_page =
        uint32_t(((uint64_t(i) << page_size_shift_) + host_address_offset_) / host_page_size);
    if (host_page != writable_host_page) {
      uint8_t* host_page_pointer = membase_ + heap_base_ + size_t(host_page) * host_page_size;
      if (!rex::memory::AllocFixed(host_page_pointer, host_page_size,
                                   memory::AllocationType::kCommit,
                                   memory::PageAccess::kReadWrite)) {
        REXSYS_ERROR("BaseHeap::Restore failed to make host page {} writable", host_page);
        return false;
      }
      writable_host_page = host_page;
    }
    stream->Read(addr, page_size_);
  }

  if (reconcile_host_pages && !page_table_.empty()) {
    if (!SyncHostPageAccess(0, uint32_t(page_table_.size()) - 1)) {
      return false;
    }
  }

  return true;
}

void BaseHeap::Reset() {
  std::memset(page_table_.data(), 0, sizeof(PageEntry) * page_table_.size());
}

bool BaseHeap::Alloc(uint32_t size, uint32_t alignment, uint32_t allocation_type, uint32_t protect,
                     bool top_down, uint32_t* out_address) {
  *out_address = 0;
  size = rex::round_up(size, page_size_);
  alignment = rex::round_up(alignment, page_size_);

  uint32_t heap_virtual_guest_offset = 0;
  if (heap_type_ == memory::HeapType::kGuestVirtual) {
    heap_virtual_guest_offset = 0x10000000;
    if (page_size_ == 0x10000) {
      heap_virtual_guest_offset = 0x0F000000;
    }
  }

  uint32_t low_address = heap_base_;
  uint32_t high_address = heap_base_ + (heap_size_ - 1) - heap_virtual_guest_offset;
  return AllocRange(low_address, high_address, size, alignment, allocation_type, protect, top_down,
                    out_address);
}

bool BaseHeap::AllocSystemHeap(uint32_t size, uint32_t alignment, uint32_t allocation_type,
                               uint32_t protect, bool top_down, uint32_t* out_address) {
  *out_address = 0;
  size = rex::round_up(size, page_size_);
  alignment = rex::round_up(alignment, page_size_);

  uint32_t low_address = heap_base_;
  if (heap_type_ == memory::HeapType::kGuestVirtual) {
    low_address = heap_base_ + heap_size_ - 0x10000000;
  }
  uint32_t high_address = heap_base_ + (heap_size_ - 1);
  return AllocRange(low_address, high_address, size, alignment, allocation_type, protect, top_down,
                    out_address);
}

bool BaseHeap::AllocFixed(uint32_t base_address, uint32_t size, uint32_t alignment,
                          uint32_t allocation_type, uint32_t protect) {
  alignment = rex::round_up(alignment, page_size_);
  if (base_address % alignment != 0) {
    if (base_address % page_size_ != 0) {
      REXSYS_ERROR(
          "BaseHeap::AllocFixed invalid base alignment: base={:08X} page_size={:08X} "
          "requested_alignment={:08X} heap={:08X}-{:08X}",
          base_address, page_size_, alignment, heap_base_, heap_base_ + (heap_size_ - 1));
      return false;
    }

    REXSYS_WARN(
        "BaseHeap::AllocFixed clamping alignment from {:08X} to page size {:08X} for "
        "base={:08X}",
        alignment, page_size_, base_address);
    alignment = page_size_;
  }
  size = rex::align(size, alignment);
  uint32_t page_count = get_page_count(size, page_size_, page_size_shift_);
  uint32_t start_page_number = (base_address - heap_base_) >> page_size_shift_;
  uint32_t end_page_number = start_page_number + page_count - 1;
  if (start_page_number >= page_table_.size() || end_page_number > page_table_.size()) {
    REXSYS_ERROR("BaseHeap::AllocFixed passed out of range address range");
    return false;
  }

  auto global_lock = AcquireHostPageReconcileLock();
  std::lock_guard<std::recursive_mutex> heap_lock(heap_mutex_);

  for (uint32_t page_number = start_page_number; page_number <= end_page_number; ++page_number) {
    uint32_t state = page_table_[page_number].state;
    if ((allocation_type == memory::kMemoryAllocationReserve) && state) {
      REXSYS_ERROR(
          "BaseHeap::AllocFixed attempting to reserve an already reserved "
          "range");
      return false;
    }
    if ((allocation_type == memory::kMemoryAllocationCommit) &&
        !(state & memory::kMemoryAllocationReserve)) {
      REXSYS_WARN("BaseHeap::AllocFixed attempting commit on unreserved page");
      allocation_type |= memory::kMemoryAllocationReserve;
      break;
    }
  }

  if (allocation_type == memory::kMemoryAllocationReserve) {
  } else if (page_size_ >= memory_->system_page_size_) {
    auto alloc_type = (allocation_type & memory::kMemoryAllocationCommit)
                          ? rex::memory::AllocationType::kCommit
                          : rex::memory::AllocationType::kReserve;
    void* result =
        rex::memory::AllocFixed(TranslateRelative(start_page_number << page_size_shift_),
                                page_count << page_size_shift_, alloc_type, ToPageAccess(protect));
    if (!result) {
      const DWORD error = GetLastError();
      MEMORY_BASIC_INFORMATION info{};
      VirtualQuery(TranslateRelative(start_page_number << page_size_shift_), &info, sizeof(info));
      REXSYS_ERROR(
          "BaseHeap::AllocFixed failed to alloc range from host: guest {:08X}, {:X} bytes, {} "
          "protect {:X}, Windows error {}; the host pages: state {:X} protect {:X} type {:X} "
          "region {:X} bytes",
          heap_base_ + (start_page_number << page_size_shift_), page_count << page_size_shift_,
          alloc_type == rex::memory::AllocationType::kCommit ? "commit" : "reserve", protect, error,
          info.State, info.Protect, info.Type, info.RegionSize);
      return false;
    }
  }

  for (uint32_t page_number = start_page_number; page_number <= end_page_number; ++page_number) {
    auto& page_entry = page_table_[page_number];
    if (allocation_type & memory::kMemoryAllocationReserve) {
      if (!page_entry.state) {
        unreserved_page_count_--;
      }

      page_entry.base_address = start_page_number;
      page_entry.region_page_count = page_count;
    }
    page_entry.allocation_protect = protect;
    page_entry.current_protect = protect;
    page_entry.state = memory::kMemoryAllocationReserve | allocation_type;
  }

  if (!SyncHostPageAccess(start_page_number, end_page_number)) {
    return false;
  }

  if (allocation_type != memory::kMemoryAllocationReserve && REXCVAR_GET(scribble_heap) &&
      (protect & memory::kMemoryProtectWrite)) {
    std::memset(TranslateRelative(start_page_number << page_size_shift_), 0xCD,
                page_count << page_size_shift_);
  }

  return true;
}

bool BaseHeap::AllocRange(uint32_t low_address, uint32_t high_address, uint32_t size,
                          uint32_t alignment, uint32_t allocation_type, uint32_t protect,
                          bool top_down, uint32_t* out_address) {
  *out_address = 0;

  alignment = rex::round_up(alignment, page_size_);
  uint32_t page_count = get_page_count(size, page_size_, page_size_shift_);
  low_address = std::max(heap_base_, rex::align(low_address, alignment));

  high_address = std::min(heap_base_ + (heap_size_ - 1), high_address);
  if (high_address < low_address) {
    REXSYS_ERROR("BaseHeap::Alloc invalid requested range");
    return false;
  }

  uint32_t high_page_end = ((high_address - heap_base_) >> page_size_shift_) + 1;
  if (((high_address - heap_base_) & (page_size_ - 1)) != page_size_ - 1) {
    --high_page_end;
  }
  if (!high_page_end) {
    REXSYS_ERROR("BaseHeap::Alloc requested range is smaller than a page");
    return false;
  }
  uint32_t low_page_number = (low_address - heap_base_) >> page_size_shift_;
  uint32_t high_page_number = high_page_end - 1;
  low_page_number = std::min(uint32_t(page_table_.size()) - 1, low_page_number);
  high_page_number = std::min(uint32_t(page_table_.size()) - 1, high_page_number);
  if (high_page_number < low_page_number) {
    REXSYS_ERROR("BaseHeap::Alloc invalid requested page range");
    return false;
  }

  uint32_t available_page_count = high_page_number - low_page_number + 1;
  if (!page_count || page_count > available_page_count) {
    REXSYS_ERROR("BaseHeap::Alloc page count too big for requested range");
    return false;
  }

  auto global_lock = AcquireHostPageReconcileLock();
  std::lock_guard<std::recursive_mutex> heap_lock(heap_mutex_);

  uint32_t start_page_number = UINT_MAX;
  uint32_t end_page_number = UINT_MAX;
  uint32_t page_scan_stride = alignment >> page_size_shift_;
  uint32_t max_base_page_number = high_page_number + 1 - page_count;
  if (top_down) {
    max_base_page_number -= max_base_page_number % page_scan_stride;
    for (int64_t base_page_number = max_base_page_number; base_page_number >= low_page_number;
         base_page_number -= page_scan_stride) {
      if (page_table_[base_page_number].state != 0) {
        continue;
      }

      start_page_number = uint32_t(base_page_number);
      end_page_number = uint32_t(base_page_number) + page_count - 1;
      assert_true(end_page_number < page_table_.size());
      bool any_taken = false;
      for (uint32_t page_number = uint32_t(base_page_number);
           !any_taken && page_number <= end_page_number; ++page_number) {
        bool is_free = page_table_[page_number].state == 0;
        if (!is_free) {
          any_taken = true;
          if (page_count > page_number) {
            base_page_number = -1;
          } else {
            base_page_number = page_number - page_count;
            base_page_number -= base_page_number % page_scan_stride;
            base_page_number += page_scan_stride;
          }
          break;
        }
      }
      if (!any_taken) {
        break;
      }

      start_page_number = end_page_number = UINT_MAX;
    }
  } else {
    for (uint32_t base_page_number = low_page_number; base_page_number <= max_base_page_number;
         base_page_number += page_scan_stride) {
      if (page_table_[base_page_number].state != 0) {
        continue;
      }

      start_page_number = base_page_number;
      end_page_number = base_page_number + page_count - 1;
      bool any_taken = false;
      for (uint32_t page_number = base_page_number; !any_taken && page_number <= end_page_number;
           ++page_number) {
        bool is_free = page_table_[page_number].state == 0;
        if (!is_free) {
          any_taken = true;
          base_page_number = rex::round_up(page_number + 1, page_scan_stride);
          base_page_number -= page_scan_stride;
          break;
        }
      }
      if (!any_taken) {
        break;
      }

      start_page_number = end_page_number = UINT_MAX;
    }
  }
  if (start_page_number == UINT_MAX || end_page_number == UINT_MAX) {
    REXSYS_ERROR("BaseHeap::Alloc failed to find contiguous range");
    assert_always("Heap exhausted!");
    return false;
  }

  if (allocation_type == memory::kMemoryAllocationReserve) {
  } else if (page_size_ >= memory_->system_page_size_) {
    auto alloc_type = (allocation_type & memory::kMemoryAllocationCommit)
                          ? rex::memory::AllocationType::kCommit
                          : rex::memory::AllocationType::kReserve;
    void* result =
        rex::memory::AllocFixed(TranslateRelative(start_page_number << page_size_shift_),
                                page_count << page_size_shift_, alloc_type, ToPageAccess(protect));
    if (!result) {
      REXSYS_ERROR("BaseHeap::Alloc failed to alloc range from host");
      return false;
    }
  }

  for (uint32_t page_number = start_page_number; page_number <= end_page_number; ++page_number) {
    auto& page_entry = page_table_[page_number];
    if (!page_entry.state) {
      unreserved_page_count_--;
    }
    page_entry.base_address = start_page_number;
    page_entry.region_page_count = page_count;
    page_entry.allocation_protect = protect;
    page_entry.current_protect = protect;
    page_entry.state = memory::kMemoryAllocationReserve | allocation_type;
  }

  if (!SyncHostPageAccess(start_page_number, end_page_number)) {
    return false;
  }

  if (allocation_type != memory::kMemoryAllocationReserve && REXCVAR_GET(scribble_heap) &&
      (protect & memory::kMemoryProtectWrite)) {
    std::memset(TranslateRelative(start_page_number << page_size_shift_), 0xCD,
                page_count << page_size_shift_);
  }

  *out_address = heap_base_ + (start_page_number << page_size_shift_);
  return true;
}

bool BaseHeap::Decommit(uint32_t address, uint32_t size) {
  uint32_t page_count = get_page_count(size, page_size_, page_size_shift_);
  uint32_t start_page_number = (address - heap_base_) >> page_size_shift_;
  uint32_t end_page_number = start_page_number + page_count - 1;
  start_page_number = std::min(uint32_t(page_table_.size()) - 1, start_page_number);
  end_page_number = std::min(uint32_t(page_table_.size()) - 1, end_page_number);

  auto global_lock = AcquireHostPageReconcileLock();
  std::lock_guard<std::recursive_mutex> heap_lock(heap_mutex_);

  for (uint32_t page_number = start_page_number; page_number <= end_page_number; ++page_number) {
    auto& page_entry = page_table_[page_number];
    page_entry.state &= ~memory::kMemoryAllocationCommit;
  }

  if (!SyncHostPageAccess(start_page_number, end_page_number)) {
    return false;
  }

  return true;
}

bool BaseHeap::Release(uint32_t base_address, uint32_t* out_region_size) {
  auto global_lock = AcquireHostPageReconcileLock();
  std::lock_guard<std::recursive_mutex> heap_lock(heap_mutex_);

  uint32_t base_page_number = (base_address - heap_base_) >> page_size_shift_;
  auto base_page_entry = page_table_[base_page_number];
  if (base_page_entry.base_address != base_page_number) {
    REXSYS_ERROR("BaseHeap::Release failed because address is not a region start");
    return false;
  }

  if (heap_base_ == 0x00000000 && base_page_number == 0) {
    REXSYS_ERROR("BaseHeap::Release: Attempt to free 0!");
    return false;
  }

  if (out_region_size) {
    *out_region_size = (base_page_entry.region_page_count << page_size_shift_);
  }

  if (page_size_ == rex::memory::page_size() ||
      ((base_page_entry.region_page_count << page_size_shift_) % rex::memory::page_size() == 0 &&
       ((base_page_number << page_size_shift_) % rex::memory::page_size() == 0))) {
    if (REXCVAR_GET(protect_on_release)) {
      if (!rex::memory::Protect(TranslateRelative(base_page_number << page_size_shift_),
                                base_page_entry.region_page_count << page_size_shift_,
                                rex::memory::PageAccess::kNoAccess, nullptr)) {
        REXSYS_WARN("BaseHeap::Release failed due to host VirtualProtect failure");
      }
    }
  }

  uint32_t end_page_number = base_page_number + base_page_entry.region_page_count - 1;
  for (uint32_t page_number = base_page_number; page_number <= end_page_number; ++page_number) {
    auto& page_entry = page_table_[page_number];
    page_entry.qword = 0;
    unreserved_page_count_++;
  }

  if (!SyncHostPageAccess(base_page_number, end_page_number)) {
    return false;
  }

  return true;
}

bool BaseHeap::Protect(uint32_t address, uint32_t size, uint32_t protect, uint32_t* old_protect) {
  if (!size) {
    REXSYS_ERROR("BaseHeap::Protect failed due to zero size");
    return false;
  }

  uint32_t start_page_number = (address - heap_base_) >> page_size_shift_;
  if (start_page_number >= page_table_.size()) {
    REXSYS_ERROR("BaseHeap::Protect failed due to out-of-bounds base address {:08X}", address);
    return false;
  }
  uint32_t end_page_number =
      uint32_t((uint64_t(address) + size - 1 - heap_base_) >> page_size_shift_);
  if (end_page_number >= page_table_.size()) {
    REXSYS_ERROR(
        "BaseHeap::Protect failed due to out-of-bounds range ({:08X} bytes "
        "from {:08x})",
        size, address);
    return false;
  }

  auto global_lock = AcquireHostPageReconcileLock();
  std::lock_guard<std::recursive_mutex> heap_lock(heap_mutex_);

  uint32_t first_base_address = UINT_MAX;
  for (uint32_t page_number = start_page_number; page_number <= end_page_number; ++page_number) {
    auto page_entry = page_table_[page_number];
    if (first_base_address == UINT_MAX) {
      first_base_address = page_entry.base_address;
    } else if (first_base_address != page_entry.base_address) {
      REXSYS_ERROR("BaseHeap::Protect failed due to request spanning regions");
      return false;
    }
    if (!(page_entry.state & memory::kMemoryAllocationCommit)) {
      REXSYS_ERROR("BaseHeap::Protect failed due to uncommitted page");
      return false;
    }
  }

  if (page_size_ >= memory_->system_page_size_) {
    const uint32_t page_count = end_page_number - start_page_number + 1;
    memory::PageAccess old_protect_access;
    if (!rex::memory::Protect(TranslateRelative(start_page_number << page_size_shift_),
                              page_count << page_size_shift_, ToPageAccess(protect),
                              old_protect ? &old_protect_access : nullptr)) {
      REXSYS_ERROR("BaseHeap::Protect failed due to host VirtualProtect failure");
      return false;
    }

    if (old_protect) {
      *old_protect = FromPageAccess(old_protect_access);
    }
  } else {
    if (old_protect) {
      *old_protect = page_table_[start_page_number].current_protect;
    }
  }

  for (uint32_t page_number = start_page_number; page_number <= end_page_number; ++page_number) {
    auto& page_entry = page_table_[page_number];
    page_entry.current_protect = protect;
  }

  if (!SyncHostPageAccess(start_page_number, end_page_number)) {
    return false;
  }

  return true;
}

bool BaseHeap::QueryRegionInfo(uint32_t base_address, HeapAllocationInfo* out_info) {
  uint32_t start_page_number = (base_address - heap_base_) >> page_size_shift_;
  if (start_page_number > page_table_.size()) {
    REXSYS_ERROR("BaseHeap::QueryRegionInfo base page out of range");
    return false;
  }

  std::lock_guard<std::recursive_mutex> heap_lock(heap_mutex_);

  auto start_page_entry = page_table_[start_page_number];
  out_info->base_address = base_address;
  out_info->allocation_base = 0;
  out_info->allocation_protect = 0;
  out_info->region_size = 0;
  out_info->state = 0;
  out_info->protect = 0;
  if (start_page_entry.state) {
    out_info->allocation_base = heap_base_ + (start_page_entry.base_address << page_size_shift_);
    out_info->allocation_protect = start_page_entry.allocation_protect;
    out_info->allocation_size = start_page_entry.region_page_count << page_size_shift_;
    out_info->state = start_page_entry.state;
    out_info->protect = start_page_entry.current_protect;

    for (uint32_t page_number = start_page_number;
         page_number < start_page_entry.base_address + start_page_entry.region_page_count;
         ++page_number) {
      auto page_entry = page_table_[page_number];
      if (page_entry.base_address != start_page_entry.base_address ||
          page_entry.state != start_page_entry.state ||
          page_entry.current_protect != start_page_entry.current_protect) {
        break;
      }
      out_info->region_size += page_size_;
    }
  } else {
    for (uint32_t page_number = start_page_number; page_number < page_table_.size();
         ++page_number) {
      auto page_entry = page_table_[page_number];
      if (page_entry.state) {
        break;
      }
      out_info->region_size += page_size_;
    }
  }
  return true;
}

bool BaseHeap::QuerySize(uint32_t address, uint32_t* out_size) {
  uint32_t page_number = (address - heap_base_) >> page_size_shift_;
  if (page_number > page_table_.size()) {
    REXSYS_ERROR("BaseHeap::QuerySize base page out of range");
    *out_size = 0;
    return false;
  }
  std::lock_guard<std::recursive_mutex> heap_lock(heap_mutex_);
  auto page_entry = page_table_[page_number];
  *out_size = (page_entry.region_page_count << page_size_shift_);
  return true;
}

bool BaseHeap::QueryBaseAndSize(uint32_t* in_out_address, uint32_t* out_size) {
  uint32_t page_number = (*in_out_address - heap_base_) >> page_size_shift_;
  if (page_number > page_table_.size()) {
    REXSYS_ERROR("BaseHeap::QuerySize base page out of range");
    *out_size = 0;
    return false;
  }
  std::lock_guard<std::recursive_mutex> heap_lock(heap_mutex_);
  auto page_entry = page_table_[page_number];
  *in_out_address = (page_entry.base_address << page_size_shift_);
  *out_size = (page_entry.region_page_count << page_size_shift_);
  return true;
}

bool BaseHeap::QueryProtect(uint32_t address, uint32_t* out_protect) {
  uint32_t page_number = (address - heap_base_) >> page_size_shift_;
  if (page_number > page_table_.size()) {
    REXSYS_ERROR("BaseHeap::QueryProtect base page out of range");
    *out_protect = 0;
    return false;
  }
  std::lock_guard<std::recursive_mutex> heap_lock(heap_mutex_);
  auto page_entry = page_table_[page_number];
  *out_protect = page_entry.current_protect;
  return true;
}

rex::memory::PageAccess BaseHeap::QueryRangeAccess(uint32_t low_address, uint32_t high_address) {
  if (low_address > high_address || low_address < heap_base_ ||
      (high_address - heap_base_) >= heap_size_) {
    return rex::memory::PageAccess::kNoAccess;
  }
  uint32_t low_page_number = (low_address - heap_base_) >> page_size_shift_;
  uint32_t high_page_number = (high_address - heap_base_) >> page_size_shift_;
  bool all_readable = true;
  bool all_writable = true;
  {
    std::lock_guard<std::recursive_mutex> heap_lock(heap_mutex_);
    for (uint32_t i = low_page_number; i <= high_page_number; ++i) {
      uint32_t page_protect = page_table_[i].current_protect;
      if (!(page_protect & memory::kMemoryProtectRead)) {
        all_readable = false;
      }
      if (!(page_protect & memory::kMemoryProtectWrite) &&
          !(page_protect & memory::kMemoryProtectWriteCombine)) {
        all_writable = false;
      }
    }
  }
  if (all_readable && all_writable) {
    return rex::memory::PageAccess::kReadWrite;
  } else if (all_readable) {
    return rex::memory::PageAccess::kReadOnly;
  } else {
    return rex::memory::PageAccess::kNoAccess;
  }
}

}
