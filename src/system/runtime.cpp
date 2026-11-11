/**
 * @file        runtime/runtime.cpp
 * @brief       Runtime subsystem implementation
 *
 * @copyright   Copyright (c) 2026 Tom Clay <tomc@tctechstuff.com>
 *              All rights reserved.
 *
 * @license     BSD 3-Clause License
 *              See LICENSE file in the project root for full license text.
 */

#include <rex/chrono/clock.h>
#include <rex/cvar.h>
#include <rex/filesystem/devices/host_path_device.h>
#include <rex/filesystem/devices/disc_image_device.h>
#include <rex/filesystem/devices/optical_disc_reader.h>
#include <rex/filesystem/devices/null_device.h>
#include <rex/filesystem/devices/stfs_container_device.h>
#include <rex/filesystem/vfs.h>
#include <rex/logging.h>
#include <rex/perf/counter.h>
#include <rex/ppc/context.h>
#include <rex/platform/exceptions.h>
#include <rex/kernel/crt/heap.h>
#include <rex/runtime.h>
#include <rex/system/export_resolver.h>
#include <rex/system/kernel_state.h>
#include <rex/system/function_dispatcher.h>
#include <rex/system/mmio_handler.h>
#include <rex/system/thread_state.h>
#include <rex/system/user_module.h>
#include <rex/system/xmemory.h>
#include <rex/system/kernel_thread.h>
#include <rex/thread.h>

#include <windows.h>

REXCVAR_DEFINE_STRING(game_data_root, "", "Runtime", "Override game data path");
REXCVAR_DEFINE_STRING(game_source, "", "Runtime", "First-run game folder or XDVDFS image")
    .lifecycle(rex::cvar::Lifecycle::kRequiresRestart);
REXCVAR_DEFINE_STRING(user_data_root, "", "Runtime", "Override user data path");
REXCVAR_DEFINE_STRING(update_data_root, "", "Runtime", "Override update data path");
REXCVAR_DEFINE_STRING(cache_root, "", "Runtime", "Override shader cache path");
REXCVAR_DEFINE_STRING(metadata_root, "", "Runtime", "Override metadata path");

REXCVAR_DEFINE_BOOL(mount_cache, true, "Runtime",
                    "Mount the console's cache partitions (cache:, cache0:, cache1:) in the "
                    "cache folder");

namespace rex {

namespace {

void ReportUnhandledGuestFault(void* context, uint64_t host_pc) {
  auto* dispatcher = static_cast<runtime::FunctionDispatcher*>(context);
  uint64_t host_entry = 0;
  const uint32_t guest = dispatcher->FindGuestFunctionByHostPc(host_pc, &host_entry);
  std::string module = "?";
  uint64_t module_offset = 0;
  HMODULE handle = nullptr;
  if (GetModuleHandleExW(
          GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
          reinterpret_cast<LPCWSTR>(host_pc), &handle)) {
    char path[MAX_PATH] = {};
    GetModuleFileNameA(handle, path, MAX_PATH);
    module = std::filesystem::path(path).filename().string();
    module_offset = host_pc - reinterpret_cast<uint64_t>(handle);
  }
  auto* thread_state = runtime::ThreadState::Get();
  const ::PPCContext* ctx = thread_state ? thread_state->context() : nullptr;
  auto* xthread = system::XThread::IsInThread() ? system::XThread::GetCurrentThread() : nullptr;
  REXSYS_ERROR(
      "Unhandled guest fault at host {}+0x{:X}: in sub_{:08X}+0x{:X} (host), guest lr "
      "0x{:08X}, r1 0x{:08X}, r3 0x{:08X}; guest thread {:X} stack 0x{:08X}-0x{:08X}",
      module, module_offset, guest, guest ? host_pc - host_entry : 0, ctx ? uint32_t(ctx->lr) : 0,
      ctx ? ctx->r1.u32 : 0, ctx ? ctx->r3.u32 : 0, xthread ? xthread->thread_id() : 0,
      xthread ? xthread->stack_limit() : 0, xthread ? xthread->stack_base() : 0);
  if (ctx && xthread) {
    std::string chain;
    const uint32_t low = xthread->stack_limit(), high = xthread->stack_base();
    uint32_t frame = ctx->r1.u32;
    for (int i = 0; i < 24 && frame >= low && frame < high - 4; ++i) {
      const uint32_t next =
          memory::load_and_swap<uint32_t>(dispatcher->memory()->TranslateVirtual(frame));
      if (next <= frame || next > high || next < low + 8) {
        break;
      }
      chain += fmt::format(" 0x{:08X}", memory::load_and_swap<uint32_t>(
                                            dispatcher->memory()->TranslateVirtual(next - 8)));
      frame = next;
    }
    REXSYS_ERROR("Guest return addresses, innermost first (stack back chain):{}", chain);
  }
  rex::FlushLogging();
}

}

Runtime* Runtime::instance_ = nullptr;

Runtime* Runtime::instance() {
  return instance_;
}

Runtime::Runtime(const std::filesystem::path& game_data_root,
                 const std::filesystem::path& user_data_root,
                 const std::filesystem::path& update_data_root,
                 const std::filesystem::path& cache_root,
                 const std::filesystem::path& metadata_root)
    : game_data_root_(game_data_root),
      user_data_root_(user_data_root.empty() ? game_data_root : user_data_root),
      update_data_root_(update_data_root),
      cache_root_(cache_root),
      metadata_root_(metadata_root) {}

Runtime::~Runtime() {
  Shutdown();
}

std::optional<std::filesystem::path> Runtime::FindMetadataPath(
    const std::filesystem::path& relative_path) const {
  if (!metadata_root_.empty()) {
    std::filesystem::path candidate = metadata_root_ / relative_path;
    std::error_code ec;
    if (std::filesystem::exists(candidate, ec)) {
      return candidate;
    }
    return std::nullopt;
  }

  const std::filesystem::path candidates[] = {
      game_data_root_ / "metadata" / relative_path,
      game_data_root_.parent_path() / "metadata" / relative_path,
      game_data_root_ / relative_path,
  };
  for (const auto& candidate : candidates) {
    std::error_code ec;
    if (std::filesystem::exists(candidate, ec)) {
      return candidate;
    }
  }
  return std::nullopt;
}

std::optional<EmbeddedMetadataAsset> Runtime::FindEmbeddedMetadata(
    const std::filesystem::path& relative_path) const {
  return FindEmbeddedMetadataAsset(relative_path);
}

X_STATUS Runtime::Setup(RuntimeConfig config) {
  if (instance_ != nullptr) {
    REXSYS_ERROR("Runtime::Setup() called but global instance already exists");
    return X_STATUS_UNSUCCESSFUL;
  }
  instance_ = this;

  auto fail = [this](X_STATUS status, std::string_view reason) {
    REXSYS_ERROR("Runtime::Setup failed: {}", reason);
    Shutdown();
    return status;
  };

  rex::initialize_seh();

  chrono::Clock::set_guest_tick_frequency(50000000);
  chrono::Clock::set_guest_system_time_base(chrono::Clock::QueryHostSystemTime());
  chrono::Clock::set_guest_time_scalar(1.0);

  thread::EnableAffinityConfiguration();

  tool_mode_ = config.tool_mode;

  memory_ = std::make_unique<memory::Memory>();
  if (!memory_->Initialize()) {
    return fail(X_STATUS_UNSUCCESSFUL, "memory init failed");
  }

  export_resolver_ = std::make_unique<runtime::ExportResolver>();

  function_dispatcher_ =
      std::make_unique<runtime::FunctionDispatcher>(memory_.get(), export_resolver_.get());
  if (auto* mmio = runtime::MMIOHandler::global_handler()) {
    mmio->SetUnhandledFaultReporter(&ReportUnhandledGuestFault, function_dispatcher_.get());
  }

  file_system_ = std::make_unique<rex::filesystem::VirtualFileSystem>();

  kernel_state_ = std::make_unique<system::KernelState>(this);

  if (config.input_factory) {
    input_system_ = config.input_factory(tool_mode_);
    if (input_system_) {
      X_STATUS input_status = input_system_->Setup();
      if (XFAILED(input_status)) {
        REXSYS_WARN("Failed to initialize input system (status {:08X}) - input disabled",
                    input_status);
        input_system_.reset();
      } else {
        REXSYS_DEBUG("Input system initialized");
      }
    }
  }

  if (config.kernel_init) {
    config.kernel_init(this, kernel_state_.get());
  }

  if (config.audio_factory) {
    audio_system_ = config.audio_factory(function_dispatcher_.get());
    if (audio_system_) {
      X_STATUS audio_status = audio_system_->Setup(kernel_state_.get());
      if (XFAILED(audio_status)) {
        REXSYS_WARN("Failed to initialize audio system (status {:08X}) - audio disabled",
                    audio_status);
        audio_system_.reset();
      } else {
        REXSYS_DEBUG("Audio system initialized");
      }
    }
  }

  if (!SetupVfs()) {
    return fail(X_STATUS_UNSUCCESSFUL, "VFS setup failed");
  }

  if (tool_mode_) {
    REXSYS_DEBUG("Runtime initialized in tool mode (no GPU)");
    setup_complete_ = true;
    return X_STATUS_SUCCESS;
  }

  if (config.graphics) {
    graphics_system_ = std::move(config.graphics);
    bool with_presentation = (app_context_ != nullptr);
    X_STATUS gpu_status = graphics_system_->Setup(function_dispatcher_.get(), kernel_state_.get(),
                                                  app_context_, with_presentation);
    if (XFAILED(gpu_status)) {
      return fail(gpu_status, "GPU setup failed");
    }
    REXSYS_DEBUG("GPU system initialized (presentation={})", with_presentation);
  } else {
    REXSYS_DEBUG("Runtime initialized without graphics system (native rendering mode)");
  }

  REXSYS_DEBUG("Runtime initialized successfully");
  setup_complete_ = true;
  return X_STATUS_SUCCESS;
}

X_STATUS Runtime::Setup(const rex::PPCImageInfo& image_info, RuntimeConfig config) {
  X_STATUS status = Setup(std::move(config));
  if (status != X_STATUS_SUCCESS) {
    return status;
  }

  codegen_flags_ = image_info.codegen_flags;

  if (!function_dispatcher_->InitializeFunctionTable(image_info.code_base, image_info.code_size,
                                                     image_info.image_base, image_info.image_size,
                                                     true, image_info.function_table_base)) {
    REXSYS_ERROR("Failed to initialize function table");
    Shutdown();
    return X_STATUS_UNSUCCESSFUL;
  }

  if (image_info.func_mappings) {
    int count = 0;
    int duplicates = 0;
    int rejected = 0;
    for (int i = 0; image_info.func_mappings[i].guest != 0; ++i) {
      uint32_t guest = static_cast<uint32_t>(image_info.func_mappings[i].guest);
      auto* host = image_info.func_mappings[i].host;
      if (!host) {
        continue;
      }
      if (function_dispatcher_->GetFunction(guest)) {
        REXSYS_WARN("func_mappings: duplicate guest address {:08X}", guest);
        ++duplicates;
      }
      if (!function_dispatcher_->SetFunction(guest, host)) {
        ++rejected;
      } else {
        ++count;
      }
    }
    REXSYS_DEBUG("Registered {} recompiled functions ({} duplicates, {} rejected)", count,
                 duplicates, rejected);
    if (rejected > 0) {
      REXSYS_ERROR("PPCImageInfo registration: {} func_mappings entries rejected", rejected);
      Shutdown();
      return X_STATUS_UNSUCCESSFUL;
    }
  }

  REXSYS_DEBUG("Runtime setup complete (code: {:08X}-{:08X}, image: {:08X}-{:08X})",
               image_info.code_base, image_info.code_base + image_info.code_size,
               image_info.image_base, image_info.image_base + image_info.image_size);

  const auto& flags = image_info.codegen_flags;
  REXSYS_DEBUG(
      "Codegen config: skip_lr={} skip_msr={} ctr={} xer={} cr={} reserved={} "
      "non_argument={} non_volatile={}",
      flags.skip_lr, flags.skip_msr, flags.ctr_as_local, flags.xer_as_local, flags.cr_as_local,
      flags.reserved_as_local, flags.non_argument_as_local, flags.non_volatile_as_local);

  return X_STATUS_SUCCESS;
}

void Runtime::Shutdown() {
  if (!instance_ && !setup_complete_ && !memory_) {
    return;
  }

  if (instance_ == this) {
    instance_ = nullptr;
  }

  if (graphics_system_) {
    graphics_system_->Shutdown();
    graphics_system_.reset();
  }
  if (audio_system_) {
    audio_system_->Shutdown();
    audio_system_.reset();
  }
  if (input_system_) {
    input_system_->Shutdown();
    input_system_.reset();
  }
  kernel_state_.reset();
  function_dispatcher_.reset();
  export_resolver_.reset();
  file_system_.reset();
  memory_.reset();

  rex::perf::Profiler::Shutdown();
  setup_complete_ = false;
}

uint8_t* Runtime::virtual_membase() const {
  return memory_ ? memory_->virtual_membase() : nullptr;
}

bool Runtime::MountFileSystem() {
  if (!file_system_) {
    file_system_ = std::make_unique<rex::filesystem::VirtualFileSystem>();
  }
  return SetupVfs();
}

bool Runtime::SetupVfs() {
  if (game_data_root_.empty()) {
    REXSYS_WARN("Runtime::SetupVfs: No game_data_root specified, skipping VFS setup");
    return true;
  }

  auto abs_game_root = std::filesystem::absolute(game_data_root_);
  if (!rex::filesystem::IsOpticalDiscPath(abs_game_root) &&
      !std::filesystem::exists(abs_game_root)) {
    REXSYS_ERROR("Runtime::SetupVfs: game_data_root does not exist: {}", abs_game_root.string());
    return false;
  }

  auto mount_path = "\\Device\\Harddisk0\\Partition1";
  std::unique_ptr<rex::filesystem::Device> device;
  if (rex::filesystem::IsOpticalDiscPath(abs_game_root) ||
      std::filesystem::is_regular_file(abs_game_root)) {
    device = std::make_unique<rex::filesystem::DiscImageDevice>(mount_path, abs_game_root);
  } else {
    device = std::make_unique<rex::filesystem::HostPathDevice>(
        mount_path, abs_game_root, !REXCVAR_GET(allow_game_relative_writes));
  }
  if (!device->Initialize()) {
    REXSYS_ERROR("Runtime::SetupVfs: Failed to initialize host path device");
    return false;
  }
  if (!file_system_->RegisterDevice(std::move(device))) {
    REXSYS_ERROR("Runtime::SetupVfs: Failed to register host path device");
    return false;
  }
  REXSYS_DEBUG("  Mounted {} at {}", abs_game_root.string(), mount_path);

  file_system_->RegisterSymbolicLink("game:", mount_path);
  file_system_->RegisterSymbolicLink("d:", mount_path);
  REXSYS_DEBUG("  Registered symbolic links: game:, d:");

  if (!update_data_root_.empty()) {
    auto abs_update_root = std::filesystem::absolute(update_data_root_);
    std::error_code ec;
    if (std::filesystem::exists(abs_update_root, ec)) {
      auto update_mount = "\\Device\\Harddisk0\\PartitionUpdate";
      std::unique_ptr<rex::filesystem::Device> update_device;
      if (std::filesystem::is_regular_file(abs_update_root, ec)) {
        update_device =
            std::make_unique<rex::filesystem::StfsContainerDevice>(update_mount, abs_update_root);
      } else {
        update_device =
            std::make_unique<rex::filesystem::HostPathDevice>(update_mount, abs_update_root, true);
      }
      if (update_device->Initialize() && file_system_->RegisterDevice(std::move(update_device))) {
        file_system_->RegisterSymbolicLink("update:", update_mount);
        REXSYS_DEBUG("  Mounted {} at update:", abs_update_root.string());
      } else {
        REXSYS_ERROR("Runtime::SetupVfs: could not mount the title update {}",
                     abs_update_root.string());
      }
    }
  }

  auto null_paths = {std::string("\\Partition0"), std::string("\\Cache0"), std::string("\\Cache1")};
  auto null_device =
      std::make_unique<rex::filesystem::NullDevice>("\\Device\\Harddisk0", null_paths);
  if (null_device->Initialize()) {
    file_system_->RegisterDevice(std::move(null_device));
    REXSYS_DEBUG("  Registered NullDevice for \\Device\\Harddisk0\\{{Partition0,Cache0,Cache1}}");
  }

  if (REXCVAR_GET(mount_cache) && !cache_root_.empty()) {
    struct Partition {
      const char* device;
      const char* folder;
      const char* link;
    };
    for (const Partition& p :
         {Partition{"\\CACHE0", "cache0", "cache0:"}, Partition{"\\CACHE1", "cache1", "cache1:"},
          Partition{"\\CACHE", "cache", "cache:"}}) {
      const std::filesystem::path folder = cache_root_ / "partitions" / p.folder;
      std::error_code ec;
      std::filesystem::create_directories(folder, ec);
      auto partition = std::make_unique<rex::filesystem::HostPathDevice>(p.device, folder, false);
      if (partition->Initialize() && file_system_->RegisterDevice(std::move(partition))) {
        file_system_->RegisterSymbolicLink(p.link, p.device);
        REXSYS_DEBUG("  Mounted {} at {}", folder.string(), p.link);
      } else {
        REXSYS_WARN("Runtime::SetupVfs: could not mount {} at {}", folder.string(), p.link);
      }
    }
  }

  return true;
}

X_STATUS Runtime::LoadXexImage(const std::string_view module_path) {
  REXSYS_DEBUG("Loading XEX image: {}", std::string(module_path));

  auto module = system::object_ref<system::UserModule>(new system::UserModule(kernel_state_.get()));
  X_STATUS status = module->LoadFromFile(module_path);
  if (XFAILED(status)) {
    REXSYS_ERROR("Runtime::LoadXexImage: Failed to load module, status {:08X}", status);
    return status;
  }

  kernel_state_->SetExecutableModule(module);
  REXSYS_DEBUG("  XEX image loaded successfully");
  return X_STATUS_SUCCESS;
}

system::object_ref<system::XThread> Runtime::PrepareModuleLaunch() {
  auto executable = kernel_state_->GetExecutableModule();
  if (!executable) {
    REXSYS_ERROR("Runtime::PrepareModuleLaunch: No executable module loaded");
    return nullptr;
  }

  auto thread = kernel_state_->PrepareModuleLaunch(executable);
  if (!thread) {
    REXSYS_ERROR("Runtime::PrepareModuleLaunch: Failed to prepare module");
    return nullptr;
  }

  REXSYS_DEBUG("  Module prepared on thread '{}'", thread->name());
  return thread;
}

system::object_ref<system::XThread> Runtime::LaunchModule() {
  auto thread = PrepareModuleLaunch();
  if (thread) {
    thread->Resume();
    REXSYS_DEBUG("  Module launched on thread '{}'", thread->name());
  }
  return thread;
}

}
