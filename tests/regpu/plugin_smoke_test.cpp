/**
 * @file        regpu/plugin_smoke_test.cpp
 * @brief       rexgpu-regpu runs a command stream through the plugin interface
 *
 * @license     BSD 3-Clause License
 *              See LICENSE file in the project root for full license text.
 */

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <functional>
#include <memory>
#include <thread>
#include <vector>

#include <fmt/format.h>

#include <rex/graphics/xenos.h>
#include <rex/logging.h>
#include <rex/memory/utils.h>
#include <rex/runtime.h>
#include <rex/system/gpu_plugin.h>
#include <rex/system/interfaces/graphics.h>
#include <rex/system/mmio_handler.h>
#include <rex/system/xmemory.h>

#include <windows.h>

namespace {

namespace xenos = rex::graphics::xenos;

constexpr uint32_t kGpuRegisterBase = 0x7FC80000;
constexpr uint32_t kCpRbWptr = 0x01C5;
constexpr uint32_t kEdramTiming = 0x0F00;
constexpr uint32_t kRingSizeLog2 = 13;
constexpr uint32_t kRingDwords = (uint32_t(1) << (kRingSizeLog2 + 3)) / 4;
constexpr int kSkipped = 4;

int failures = 0;

void Check(bool passed, const char* what) {
  std::printf("%s: %s\n", passed ? "ok" : "FAILED", what);
  std::fflush(stdout);
  if (!passed) {
    ++failures;
  }
}

bool WaitFor(const std::function<bool()>& condition) {
  auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(20);
  while (!condition()) {
    if (std::chrono::steady_clock::now() > deadline) {
      return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return true;
}

}

int main() {
  _putenv_s("REGPU_HEADLESS", "1");
  rex::InitLogging(nullptr);

  std::error_code ignored;
  std::filesystem::path root = std::filesystem::temp_directory_path(ignored) /
                               fmt::format("re360_regpu_smoke_{}", GetCurrentProcessId());
  std::filesystem::create_directories(root, ignored);
  _putenv_s("REGPU_LOG_DIR", root.string().c_str());
  auto* runtime = new rex::Runtime(root, {}, {}, root / "cache");

  rex::RuntimeConfig config;
  config.graphics = rex::system::LoadGpuPlugin("regpu");
  if (!config.graphics) {
    std::printf("skipped: the regpu GPU plugin could not be loaded\n");
    return kSkipped;
  }
  if (XFAILED(runtime->Setup(std::move(config)))) {
    std::printf("skipped: the runtime or a Direct3D 12 device could not be set up\n");
    return kSkipped;
  }
  auto* memory = runtime->memory();
  auto* graphics = runtime->graphics_system();

  auto alloc_physical = [&](uint32_t size) {
    uint32_t address = memory->SystemHeapAlloc(size, 0x1000, rex::memory::kSystemHeapPhysical);
    uint32_t physical = memory->GetPhysicalAddress(address);
    std::memset(memory->TranslatePhysical(physical), 0, size);
    return physical;
  };
  auto read_dword = [&](uint32_t physical) {
    return rex::memory::load_and_swap<uint32_t>(memory->TranslatePhysical(physical));
  };

  uint32_t ring = alloc_physical(kRingDwords * 4);
  uint32_t fence = alloc_physical(0x1000);
  uint32_t scratch = alloc_physical(0x1000);
  uint32_t writeback = alloc_physical(0x1000);

  graphics->InitializeRingBuffer(ring, kRingSizeLog2);
  graphics->EnableReadPointerWriteBack(writeback, 2);
  graphics->SetInterruptCallback(0, 0);

  std::vector<uint32_t> packets = {
      xenos::MakePacketType3(xenos::PM4_MEM_WRITE, 3),
      scratch | uint32_t(xenos::Endian::k8in32),
      0x52653336,
      0x30475055,
      xenos::MakePacketType2(),
      xenos::MakePacketType3(xenos::PM4_EVENT_WRITE_SHD, 3),
      0,
      fence | uint32_t(xenos::Endian::k8in32),
      0x00000001,
  };
  auto* ring_host = memory->TranslatePhysical<uint8_t*>(ring);
  for (size_t i = 0; i < packets.size(); ++i) {
    rex::memory::store_and_swap<uint32_t>(ring_host + i * 4, packets[i]);
  }
  uint32_t write_index = uint32_t(packets.size());
  auto* mmio = rex::runtime::MMIOHandler::global_handler();
  Check(mmio->CheckStore(kGpuRegisterBase + kCpRbWptr * 4, write_index),
        "the write pointer store reaches the plugin's register range");

  Check(WaitFor([&] { return read_dword(fence) == 1; }),
        "the command processor runs the stream to its fence");
  Check(read_dword(scratch) == 0x52653336 && read_dword(scratch + 4) == 0x30475055,
        "MEM_WRITE lands in guest physical memory");
  Check(WaitFor([&] { return read_dword(writeback) == write_index; }),
        "the read pointer write-back reaches the write pointer");

  uint32_t value = 0;
  Check(mmio->CheckLoad(kGpuRegisterBase + kCpRbWptr * 4, &value) && value == write_index,
        "a register load returns what the guest stored");
  Check(mmio->CheckLoad(kGpuRegisterBase + kEdramTiming * 4, &value) && value == 0x08100748,
        "the EDRAM timing register reads as the console's");

  graphics->FlushCaches();
  std::printf("%s\n",
              failures ? "regpu plugin smoke test FAILED" : "regpu plugin smoke test passed");
  std::fflush(stdout);
  rex::FlushLogging();
  std::_Exit(failures ? 1 : 0);
}
