/**
 * @file        regpu/plugin/graphics_system.cpp
 * @brief       rexgpu-regpu: ReGPU behind the GPU plugin interface
 *
 * @license     BSD 3-Clause License
 *              See LICENSE file in the project root for full license text.
 */

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <mutex>
#include <vector>

#include <rex/graphics/registers.h>
#include <rex/kernel/xboxkrnl/video.h>
#include <rex/logging.h>
#include <rex/runtime.h>
#include <rex/system/function_dispatcher.h>
#include <rex/system/gpu_plugin.h>
#include <rex/system/kernel_state.h>
#include <rex/system/kernel_thread.h>
#include <rex/system/thread_state.h>
#include <rex/system/xmemory.h>
#include <rex/system/xvideo.h>
#include <rex/ui/d3d12/d3d12_presenter.h>
#include <rex/ui/d3d12/d3d12_provider.h>
#include <rex/ui/windowed_app_context.h>

#include <regpu/driver.h>
#include <regpu/regpu.h>

#include <windows.h>

namespace rex::regpu_plugin {

namespace {

constexpr uint32_t kRegisterBase = 0x7FC80000;
constexpr uint32_t kRegisterMask = 0xFFFF0000;
constexpr uint32_t kRegisterRangeSize = 0x0000FFFF;
constexpr uint32_t kGuestThreadStackSize = 256 * 1024;
constexpr uint32_t kEdramTimingRegister = 0x0F00;
constexpr uint32_t kEdramTimingValue = 0x08100748;
constexpr uint32_t kHsioAlignerValue = 0x00BBBBBB;
constexpr uint32_t kFallbackWidth = 1280;
constexpr uint32_t kFallbackHeight = 720;

class ReGpuGraphicsSystem;
ReGpuGraphicsSystem* active_system = nullptr;

class ReGpuGraphicsSystem final : public system::IGraphicsSystem {
 public:
  ~ReGpuGraphicsSystem() override {
    if (active_system == this) {
      active_system = nullptr;
    }
  }

  X_STATUS SetupPresentation(ui::WindowedAppContext* app_context) override {
    if (presenter_) {
      return X_STATUS_SUCCESS;
    }
    if (!provider_) {
      provider_ = ui::d3d12::D3D12Provider::Create();
      if (!provider_) {
        REXLOG_ERROR("rexgpu-regpu: no Direct3D 12 device");
        return X_STATUS_UNSUCCESSFUL;
      }
    }
    auto create = [this] { presenter_ = provider_->CreatePresenter(); };
    if (app_context) {
      app_context->CallInUIThreadSynchronous(create);
    } else {
      create();
    }
    if (!presenter_) {
      REXLOG_ERROR("rexgpu-regpu: the presenter could not be created");
      return X_STATUS_UNSUCCESSFUL;
    }
    return X_STATUS_SUCCESS;
  }

  bool has_presentation() const override { return presenter_ != nullptr; }
  ui::GraphicsProvider* provider() const override { return provider_.get(); }
  ui::Presenter* presenter() const override { return presenter_.get(); }

  void SetHostWindow(void* native_window_handle) override { host_window_ = native_window_handle; }

  X_STATUS SetupGuestGpu(runtime::FunctionDispatcher* function_dispatcher,
                         system::KernelState* kernel_state) override {
    if (active_system) {
      REXLOG_ERROR("rexgpu-regpu: ReGPU keeps one renderer per process and one is already running");
      return X_STATUS_UNSUCCESSFUL;
    }
    function_dispatcher_ = function_dispatcher;
    kernel_state_ = kernel_state;
    memory_ = function_dispatcher->memory();
    active_system = this;
    if (!provider_) {
      provider_ = ui::d3d12::D3D12Provider::Create();
    }

    FillConfig();
    FillHost();
    regpu::Start();

    system::X_VIDEO_MODE video_mode;
    kernel::xboxkrnl::VdQueryVideoMode(&video_mode);
    regpu::driver::SetRefreshRate(double(float(video_mode.refresh_rate)));

    memory_->AddVirtualMappedRange(kRegisterBase, kRegisterMask, kRegisterRangeSize, this,
                                   ReadRegisterThunk, WriteRegisterThunk);
    REXLOG_INFO("rexgpu-regpu: ReGPU started at {}x{}", regpu::GetConfig().width,
                regpu::GetConfig().height);
    return X_STATUS_SUCCESS;
  }

  void SetInterruptCallback(uint32_t callback, uint32_t user_data) override {
    regpu::driver::SetInterruptCallback(callback, user_data);
  }

  void InitializeRingBuffer(uint32_t ptr, uint32_t size_log2) override {
    regpu::driver::InitializeRingBuffer(ptr, size_log2);
  }

  void EnableReadPointerWriteBack(uint32_t ptr, uint32_t block_size_log2) override {
    regpu::driver::EnableReadPointerWriteBack(ptr, block_size_log2);
  }

  void InitializeShaderStorage(const std::filesystem::path& cache_root, uint32_t title_id,
                               bool blocking) override {
    (void)cache_root;
    (void)title_id;
    (void)blocking;
    regpu::PrepareShaders();
  }

  bool GetShaderPreparation(uint32_t& done, uint32_t& total) override {
    return regpu::ShaderPreparation(done, total);
  }

  void FlushCaches() override {
    if (active_system == this) {
      regpu::SaveCaches();
    }
  }

  void Shutdown() override {
    FlushCaches();
    regpu::GetHost().presentFrame = nullptr;
  }

 private:
  void FillConfig() {
    regpu::Config& config = regpu::GetConfig();
    config.hostWindow = host_window_;
    if (provider_) {
      config.hostDevice = provider_->GetDevice();
      config.hostQueue = provider_->GetDirectQueue();
    }
    config.width = kFallbackWidth;
    config.height = kFallbackHeight;
    RECT client{};
    if (host_window_ && GetClientRect(HWND(host_window_), &client) && client.right > 0 &&
        client.bottom > 0) {
      config.width = uint32_t(client.right);
      config.height = uint32_t(client.bottom);
    }
    std::filesystem::path cache_root = kernel_state_->emulator()->cache_root();
    if (cache_root.empty()) {
      cache_root = std::filesystem::current_path();
    }
    config.userDir = cache_root / "regpu";
    std::error_code ignored;
    std::filesystem::create_directories(config.userDir, ignored);
  }

  void FillHost() {
    regpu::Host& host = regpu::GetHost();
    host.base = memory_->virtual_membase();
    host.alloc = AllocateGuestMemory;
    host.createThread = CreateGuestThread;
    host.currentPcr = CurrentPcr;
    host.guestCall = CallGuest;
    host.interruptLockAcquire = EnterInterruptCpu;
    host.interruptLockRelease = LeaveInterruptCpu;
    if (presenter_) {
      host.presentFrame = PresentFrame;
    }
  }

  static void PresentFrame(uint32_t width, uint32_t height) {
    ui::Presenter* presenter = active_system ? active_system->presenter_.get() : nullptr;
    if (!presenter) {
      return;
    }
    presenter->RefreshGuestOutput(width, height, width, height,
                                  [](ui::Presenter::GuestOutputRefreshContext& context) {
                                    context.SetIs8bpc(true);
                                    auto& d3d12_context = static_cast<
                                        ui::d3d12::D3D12Presenter::D3D12GuestOutputRefreshContext&>(
                                        context);
                                    return regpu::CopyFrameTo(d3d12_context.resource_uav_capable());
                                  });
  }

  static uint32_t AllocateGuestMemory(bool physical, uint32_t size, uint32_t alignment) {
    uint32_t address = active_system->memory_->SystemHeapAlloc(
        size, std::max(alignment, uint32_t(0x20)),
        physical ? memory::kSystemHeapPhysical : memory::kSystemHeapVirtual);
    if (!address) {
      REXLOG_ERROR("rexgpu-regpu: guest allocation of {} bytes failed (physical={})", size,
                   physical);
    }
    return address;
  }

  static void CreateGuestThread(const char* name, void (*body)(PPCContext&)) {
    auto thread = system::object_ref<system::XHostThread>(
        new system::XHostThread(active_system->kernel_state_, kGuestThreadStackSize, 0, [body]() {
          body(*runtime::current_ppc_context());
          return 0;
        }));
    thread->set_name(name);
    thread->Create();
    std::lock_guard<std::mutex> lock(active_system->threads_mutex_);
    active_system->threads_.push_back(std::move(thread));
  }

  static uint32_t CurrentPcr() { return system::XThread::GetCurrentThread()->pcr_ptr(); }

  static void CallGuest(PPCContext& context, uint32_t address) {
    uint64_t args[] = {context.r3.u64, context.r4.u64};
    active_system->function_dispatcher_->ExecuteInterrupt(runtime::ThreadState::Get(), address,
                                                          args, 2);
  }

  static void EnterInterruptCpu(int cpu) {
    system::XThread::GetCurrentThread()->SetActiveCpu(uint8_t(cpu));
  }

  static void LeaveInterruptCpu() {}

  static uint32_t ReadRegisterThunk(void* ppc_context, void* self, uint32_t address) {
    (void)ppc_context;
    (void)self;
    uint32_t index = (address & 0xFFFF) / 4;
    switch (index) {
      case kEdramTimingRegister:
        return kEdramTimingValue;
      case graphics::XE_GPU_REG_RB_HSIO_INTERFACE_ALIGNER_VALUE:
        return kHsioAlignerValue;
      default:
        return regpu::driver::ReadRegister(index);
    }
  }

  static void WriteRegisterThunk(void* ppc_context, void* self, uint32_t address, uint32_t value) {
    (void)ppc_context;
    (void)self;
    regpu::driver::WriteRegister((address & 0xFFFF) / 4, value);
  }

  runtime::FunctionDispatcher* function_dispatcher_ = nullptr;
  system::KernelState* kernel_state_ = nullptr;
  memory::Memory* memory_ = nullptr;
  void* host_window_ = nullptr;
  std::unique_ptr<ui::d3d12::D3D12Provider> provider_;
  std::unique_ptr<ui::Presenter> presenter_;
  std::mutex threads_mutex_;
  std::vector<system::object_ref<system::XHostThread>> threads_;
};

}

}

extern "C" REX_GPU_PLUGIN_EXPORT uint32_t rex_gpu_abi_version(void) {
  return rex::system::kGpuPluginAbiVersion;
}

extern "C" REX_GPU_PLUGIN_EXPORT rex::system::IGraphicsSystem* rex_gpu_create(
    uint32_t abi_version, const rex::system::GpuCreateInfo* info) {
  if (abi_version != rex::system::kGpuPluginAbiVersion) {
    REXLOG_ERROR("rexgpu-regpu: host requested ABI {}, plugin is ABI {}", abi_version,
                 rex::system::kGpuPluginAbiVersion);
    return nullptr;
  }
  if (!info || info->struct_size < sizeof(rex::system::GpuCreateInfo)) {
    REXLOG_ERROR("rexgpu-regpu: invalid GpuCreateInfo");
    return nullptr;
  }
  return new rex::regpu_plugin::ReGpuGraphicsSystem();
}
