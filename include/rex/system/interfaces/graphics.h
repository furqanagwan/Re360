/**
 * @file        system/interfaces/graphics.h
 * @brief       Abstract graphics system interface for dependency injection
 *
 * @copyright   Copyright (c) 2026 Tom Clay <tomc@tctechstuff.com>
 *              All rights reserved.
 *
 * @license     BSD 3-Clause License
 *              See LICENSE file in the project root for full license text.
 */

#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include <rex/system/xtypes.h>

namespace rex::runtime {
class FunctionDispatcher;
}
namespace rex::ui {
class GraphicsProvider;
class Presenter;
class WindowedAppContext;
}
namespace rex::system {
class KernelState;
}

namespace rex::system {

class IGraphicsSystem {
 public:
  virtual ~IGraphicsSystem() = default;

  virtual X_STATUS SetupPresentation(ui::WindowedAppContext* app_context) = 0;

  virtual X_STATUS SetupGuestGpu(runtime::FunctionDispatcher* function_dispatcher,
                                 KernelState* kernel_state) = 0;

  virtual bool has_presentation() const = 0;

  virtual ui::GraphicsProvider* provider() const { return nullptr; }
  virtual ui::Presenter* presenter() const { return nullptr; }

  virtual void SetHostWindow(void* native_window_handle) { (void)native_window_handle; }
  virtual bool GetShaderPreparation(uint32_t& done, uint32_t& total) {
    done = total = 0;
    return false;
  }
  // What shader preparation is doing, for the preparation screen (GetShaderPreparation's count
  // plus the step, the threads at it and a heartbeat).
  struct ShaderPreparationStatus {
    std::string step;
    uint32_t done = 0, total = 0, working = 0;
    uint32_t threads = 0, max_threads = 0;  // max_threads 0: not adjustable
    double elapsed = 0, step_elapsed = 0, since_last_finished = 0;
    double seconds_left = -1;  // below 0: not known yet
  };
  virtual bool GetShaderPreparationStatus(ShaderPreparationStatus& status) {
    status = {};
    return GetShaderPreparation(status.done, status.total);
  }
  // Threads preparing shaders; 0 is the default. Applies straight away.
  virtual void SetShaderPreparationThreads(uint32_t threads) { (void)threads; }
  virtual uint32_t DefaultShaderPreparationThreads() const { return 0; }
  // Starts the game before preparation ends; the rest is prepared in the background.
  virtual void StartBeforeShadersPrepared() {}
  // The render_upscaler values this PC can run ("off", "fsr1", "fsr3", "fsr4", "dlss", "xess");
  // empty when the plugin has no upscalers or doesn't know yet.
  virtual std::vector<std::string> AvailableUpscalers() const { return {}; }
  virtual void FlushCaches() {}

  virtual void SetInterruptCallback(uint32_t callback, uint32_t user_data) {
    (void)callback;
    (void)user_data;
  }
  virtual void InitializeRingBuffer(uint32_t ptr, uint32_t size_log2) {
    (void)ptr;
    (void)size_log2;
  }
  virtual void EnableReadPointerWriteBack(uint32_t ptr, uint32_t block_size_log2) {
    (void)ptr;
    (void)block_size_log2;
  }

  virtual void InitializeShaderStorage(const std::filesystem::path& cache_root, uint32_t title_id,
                                       bool blocking) {
    (void)cache_root;
    (void)title_id;
    (void)blocking;
  }

  X_STATUS Setup(runtime::FunctionDispatcher* function_dispatcher, KernelState* kernel_state,
                 ui::WindowedAppContext* app_context, bool with_presentation) {
    if (with_presentation && !has_presentation()) {
      X_STATUS status = SetupPresentation(app_context);
      if (XFAILED(status)) {
        return status;
      }
    }
    return SetupGuestGpu(function_dispatcher, kernel_state);
  }

  virtual void Shutdown() = 0;
};

}
