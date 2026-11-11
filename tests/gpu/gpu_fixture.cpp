/**
 * @file        gpu_fixture.cpp
 * @brief       Headless PM4 fixture host for the Xenos GPU plugin (RG-GDK-006)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include "gpu_fixture.h"

#include <algorithm>
#include <crtdbg.h>
#include <cstdlib>
#include <string_view>
#include <thread>

#include <fmt/format.h>

#include <rex/cvar.h>
#include <rex/logging.h>
#include <rex/memory/utils.h>
#include <rex/system/gpu_plugin.h>
#include <rex/system/interfaces/graphics.h>
#include <rex/system/mmio_handler.h>
#include <rex/system/xmemory.h>

REXCVAR_DECLARE(int32_t, d3d12_adapter);

namespace rex::testing {

namespace {

namespace xenos = graphics::xenos;

const bool kCrtReportsToStderr = [] {
  for (int type : {_CRT_WARN, _CRT_ERROR, _CRT_ASSERT}) {
    _CrtSetReportMode(type, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(type, _CRTDBG_FILE_STDERR);
  }
  SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
  return true;
}();

constexpr uint32_t kGpuRegisterBase = 0x7FC80000;
constexpr uint32_t kCpRbWptr = 0x01C5;

}

std::unique_ptr<GpuFixture> GpuFixture::Create(std::string* error, CvarList cvars,
                                               std::string_view plugin, void* host_window) {
  static bool logging_initialized = [] {
    const char* log_file = std::getenv("REXGLUE_GPU_FIXTURE_LOG");
    rex::InitLogging(log_file && *log_file ? log_file : nullptr);
    return true;
  }();
  (void)logging_initialized;

  std::unique_ptr<GpuFixture> fixture(new GpuFixture());
  std::error_code ec;
  fixture->root_ = std::filesystem::temp_directory_path(ec) /
                   fmt::format("rexglue_gpu_fixture_{}", GetCurrentProcessId());
  std::filesystem::create_directories(fixture->root_, ec);
  fixture->runtime_ = std::make_unique<Runtime>(fixture->root_);

  if (const char* adapter = std::getenv("REXGLUE_GPU_FIXTURE_ADAPTER"); adapter && *adapter) {
    REXCVAR_SET(d3d12_adapter, std::atoi(adapter));
  }

  RuntimeConfig config;
  config.graphics = system::LoadGpuPlugin(plugin, "d3d12");
  if (!config.graphics) {
    *error = fmt::format("the {} GPU plugin could not be loaded", plugin);
    return nullptr;
  }
  fixture->plugin_runs_until_exit_ = plugin != system::kDefaultGpuPlugin;
  if (host_window) {
    config.graphics->SetHostWindow(host_window);
  }

  if (const char* cvars = std::getenv("REXGLUE_GPU_FIXTURE_CVARS"); cvars && *cvars) {
    std::string_view rest(cvars);
    while (!rest.empty()) {
      std::string_view item = rest.substr(0, rest.find(';'));
      rest.remove_prefix(std::min(rest.size(), item.size() + 1));
      size_t equals = item.find('=');
      if (equals == std::string_view::npos ||
          !cvar::SetFlagByName(item.substr(0, equals), item.substr(equals + 1))) {
        *error = fmt::format("invalid REXGLUE_GPU_FIXTURE_CVARS entry '{}'", item);
        return nullptr;
      }
    }
  }
  for (const auto& [name, value] : cvars) {
    if (!cvar::SetFlagByName(name, value)) {
      *error = fmt::format("invalid fixture cvar '{}={}'", name, value);
      return nullptr;
    }
  }
  if (XFAILED(fixture->runtime_->Setup(std::move(config)))) {
    *error = "the runtime or the D3D12 GPU could not be set up";
    return nullptr;
  }

  fixture->ring_ = fixture->AllocPhysical(kRingDwords * 4);
  fixture->fence_address_ = fixture->AllocPhysical(0x1000);
  if (!fixture->ring_ || !fixture->fence_address_) {
    *error = "guest physical memory for the ring buffer could not be allocated";
    return nullptr;
  }
  fixture->runtime_->graphics_system()->InitializeRingBuffer(fixture->ring_, kRingSizeLog2);
  if (fixture->plugin_runs_until_exit_) {
    fixture->runtime_->graphics_system()->SetInterruptCallback(0, 0);
  }

  if (!fixture->Flush()) {
    *error = "the command processor did not start";
    return nullptr;
  }
  return fixture;
}

GpuFixture::~GpuFixture() {
  if (plugin_runs_until_exit_) {
    Runtime* still_running = runtime_.release();
    (void)still_running;
    return;
  }
  runtime_.reset();
  std::error_code ec;
  std::filesystem::remove_all(root_, ec);
}

const ui::d3d12::D3D12Provider& GpuFixture::provider() const {
  return *static_cast<const ui::d3d12::D3D12Provider*>(runtime_->graphics_system()->provider());
}

uint32_t GpuFixture::AllocPhysical(uint32_t size, uint32_t alignment) {
  uint32_t address = memory()->SystemHeapAlloc(size, alignment, memory::kSystemHeapPhysical);
  if (!address) {
    return 0;
  }

  uint32_t physical = memory()->GetPhysicalAddress(address);
  physical_to_virtual_ = address - physical;
  std::memset(memory()->TranslatePhysical(physical), 0, size);
  return physical;
}

void GpuFixture::WriteDwords(uint32_t address, const std::vector<uint32_t>& dwords) {
  auto* dest = memory()->TranslatePhysical<uint8_t*>(address);
  for (size_t i = 0; i < dwords.size(); ++i) {
    memory::store_and_swap<uint32_t>(dest + i * 4, dwords[i]);
  }
}

void GpuFixture::WriteDwordsAsGuest(uint32_t address, const std::vector<uint32_t>& dwords) {
  auto* dest = memory()->TranslateVirtual<uint8_t*>(address + physical_to_virtual_);
  for (size_t i = 0; i < dwords.size(); ++i) {
    memory::store_and_swap<uint32_t>(dest + i * 4, dwords[i]);
  }
}

uint32_t GpuFixture::ReadDword(uint32_t address) const {
  return memory::load_and_swap<uint32_t>(memory()->TranslatePhysical(address));
}

bool GpuFixture::Submit(const std::vector<uint32_t>& dwords) {
  if (!read_pointer_writeback_) {
    assert_true(write_index_ + dwords.size() < kRingDwords);
    WriteDwords(ring_ + write_index_ * 4, dwords);
    write_index_ += uint32_t(dwords.size());
    runtime::MMIOHandler::global_handler()->CheckStore(kGpuRegisterBase + kCpRbWptr * 4,
                                                       write_index_);
    return true;
  }

  assert_true(dwords.size() < kRingDwords);
  auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
  while (true) {
    uint32_t read_index = ReadDword(read_pointer_writeback_);
    uint32_t free_dwords = (read_index + kRingDwords - write_index_ - 1) % kRingDwords;
    if (free_dwords >= dwords.size()) {
      break;
    }
    if (std::chrono::steady_clock::now() > deadline) {
      return false;
    }
    std::this_thread::yield();
  }
  size_t first = std::min<size_t>(dwords.size(), kRingDwords - write_index_);
  WriteDwords(ring_ + write_index_ * 4,
              std::vector<uint32_t>(dwords.begin(), dwords.begin() + first));
  if (first < dwords.size()) {
    WriteDwords(ring_, std::vector<uint32_t>(dwords.begin() + first, dwords.end()));
  }
  write_index_ = uint32_t((write_index_ + dwords.size()) % kRingDwords);
  runtime::MMIOHandler::global_handler()->CheckStore(kGpuRegisterBase + kCpRbWptr * 4,
                                                     write_index_);
  return true;
}

uint32_t GpuFixture::EnableReadPointerWriteBack(uint32_t block_size_log2) {
  if (!read_pointer_writeback_) {
    read_pointer_writeback_ = AllocPhysical(0x1000);
  }

  WriteDwords(read_pointer_writeback_, {write_index_});
  runtime_->graphics_system()->EnableReadPointerWriteBack(read_pointer_writeback_, block_size_log2);
  return read_pointer_writeback_;
}

bool GpuFixture::Flush(std::chrono::milliseconds timeout) {
  ++fence_value_;
  if (!Submit({xenos::MakePacketType3(xenos::PM4_EVENT_WRITE_SHD, 3), 0,
               fence_address_ | uint32_t(xenos::Endian::k8in32), fence_value_})) {
    return false;
  }
  auto deadline = std::chrono::steady_clock::now() + timeout;
  while (ReadDword(fence_address_) != fence_value_) {
    if (std::chrono::steady_clock::now() > deadline) {
      return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return true;
}

std::string GpuFixture::Metadata() const {
  if (!runtime_->graphics_system()->provider()) {
    return "the plugin owns its Direct3D 12 device";
  }
  const auto& p = provider();
  return fmt::format(
      "vendor 0x{:04X}, driver {}, software {}, feature level 0x{:X}, shader model 0x{:X}, "
      "ROV {}, PS stencil reference {}",
      uint32_t(p.GetAdapterVendorID()), p.GetDriverVersion(), p.IsAdapterSoftware(),
      uint32_t(p.GetMaxFeatureLevel()), uint32_t(p.GetHighestShaderModel()),
      p.AreRasterizerOrderedViewsSupported(), p.IsPSSpecifiedStencilReferenceSupported());
}

std::vector<uint32_t> GpuFixture::MemWrite(uint32_t address, std::initializer_list<uint32_t> values,
                                           xenos::Endian endian) {
  std::vector<uint32_t> packet;
  packet.push_back(xenos::MakePacketType3(xenos::PM4_MEM_WRITE, uint16_t(values.size() + 1)));
  packet.push_back(address | uint32_t(endian));
  packet.insert(packet.end(), values);
  return packet;
}

std::vector<uint32_t> GpuFixture::SetRegisters(uint32_t first_register,
                                               std::initializer_list<uint32_t> values) {
  std::vector<uint32_t> packet;
  packet.push_back(xenos::MakePacketType0(uint16_t(first_register), uint16_t(values.size())));
  packet.insert(packet.end(), values);
  return packet;
}

std::vector<uint32_t> GpuFixture::RegToMem(uint32_t reg, uint32_t address, xenos::Endian endian) {
  return {xenos::MakePacketType3(xenos::PM4_REG_TO_MEM, 2), reg, address | uint32_t(endian)};
}

std::vector<uint32_t> GpuFixture::IndirectBuffer(uint32_t address, uint32_t dword_count) {
  return {xenos::MakePacketType3(xenos::PM4_INDIRECT_BUFFER, 2), address, dword_count};
}

}
