/**
 * @file        rex/rex_app.h
 * @brief       ReXApp - base class for recompiled windowed applications
 *
 * @copyright   Copyright (c) 2026 Tom Clay <tomc@tctechstuff.com>
 *              All rights reserved.
 *
 * @license     BSD 3-Clause License
 *              See LICENSE file in the project root for full license text.
 */

#pragma once

#include <atomic>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string_view>
#include <thread>
#include <vector>
#include <rex/ui/guide/active_downloads.h>

#include <rex/image_info.h>
#include <rex/logging/types.h>
#include <rex/runtime.h>
#include <rex/system/gaming_runtime.h>
#include <rex/ui/imgui_dialog.h>
#include <rex/ui/imgui_drawer.h>
#include <rex/ui/immediate_drawer.h>
#include <rex/ui/overlay/debug_overlay.h>
#include <rex/ui/window.h>
#include <rex/ui/window_listener.h>
#include <rex/ui/windowed_app.h>

namespace rex {

class LogCaptureSink;
namespace system {
class GameMediaRecovery;
}

struct PathConfig {
  std::filesystem::path game_data_root;
  std::filesystem::path user_data_root;
  std::filesystem::path update_data_root;
  std::filesystem::path cache_root;
  std::filesystem::path metadata_root;
  std::filesystem::path config_path;
};

namespace ui {
class AchievementNotificationDialog;
class ConsoleDialog;
class SettingsDialog;
class LaunchSettingsDialog;
class GameSourceDialog;
class GameMediaRecoveryDialog;
struct GameSourceVisuals;
namespace guide {
struct GuideAssets;
class GuideMedia;
class XboxGuide;
}
}

class ReXApp : public ui::WindowedApp, public ui::WindowListener, public ui::WindowInputListener {
 public:
  ~ReXApp() override;

 protected:
  ReXApp(ui::WindowedAppContext& ctx, std::string_view name, PPCImageInfo ppc_info,
         std::string_view usage = "");

  virtual void OnPreSetup(RuntimeConfig& config) {}

  virtual void OnLoadXexImage(std::string& xex_image) {}

  virtual void OnPostSetup() {}

  virtual void OnCreateDialogs(ui::ImGuiDrawer* drawer) { (void)drawer; }

  virtual void OnShutdown() {}

  virtual void OnConfigurePaths(PathConfig& paths) { (void)paths; }

  virtual void OnConfigureLogging(LogConfig& config) { (void)config; }

  virtual std::optional<PathConfig> OnFinalizePaths(const PathConfig& defaults,
                                                    std::function<void(PathConfig)> resume) {
    (void)resume;
    return defaults;
  }

  virtual void OnConfigureFonts(ImFontAtlas* atlas) { (void)atlas; }

  virtual void OnConfigureStyle(ImGuiStyle& imgui_style, ui::Style& ui_style) {
    (void)imgui_style;
    (void)ui_style;
  }

  virtual void OnPostInitLogging() {}

  virtual bool OnGamingRuntimeInitialized(const system::GamingRuntimeResult& result,
                                          system::GamingRuntimePolicy policy) {
    return system::GamingRuntimeAllowsLaunch(policy, result);
  }

  virtual void OnPostLoadXexImage() {}

  virtual void OnPreLaunchModule() {}

  virtual void OnPostLaunchModule(system::XThread* thread) { (void)thread; }

  virtual void OnGuestThreadExit(system::XThread* thread) { (void)thread; }

  virtual std::unique_ptr<ui::ImmediateDrawer> OnCreateImmediateDrawer() { return nullptr; }

  virtual void OnWindowResized(uint32_t logical_width, uint32_t logical_height) {
    (void)logical_width;
    (void)logical_height;
  }

  virtual void OnWindowPixelSizeChanged(uint32_t pixel_width, uint32_t pixel_height) {
    (void)pixel_width;
    (void)pixel_height;
  }

  virtual bool OnWindowCloseRequested() { return true; }

  virtual void OnWindowFocusChanged(bool focused) { (void)focused; }

  virtual void OnDpiScaleChanged(float scale) { (void)scale; }

  virtual void OnWindowMinimized() {}
  virtual void OnWindowRestored() {}

  virtual std::unique_ptr<ui::ImGuiDialog> CreateAchievementsOverlay();

  virtual std::unique_ptr<ui::AchievementNotificationDialog> CreateAchievementNotificationDialog();

  virtual bool SetupEnvironment();
  void ConfigureGameUpdates();

  virtual bool ConstructRuntime(const PathConfig& paths);

  virtual bool SetupPresentation();

  virtual void LaunchModule();

  Runtime* runtime() const { return runtime_.get(); }

  system::GamingRuntime* gaming_runtime() const { return gaming_runtime_.get(); }
  ui::Window* window() const { return window_.get(); }
  ui::ImGuiDrawer* imgui_drawer() const { return imgui_drawer_.get(); }
  ui::ImmediateDrawer* immediate_drawer() const { return immediate_drawer_.get(); }
  system::AchievementManager& achievements() const;

  const std::filesystem::path& game_data_root() const { return game_data_root_; }
  const std::filesystem::path& user_data_root() const { return user_data_root_; }
  const std::filesystem::path& update_data_root() const { return update_data_root_; }
  const std::filesystem::path& cache_root() const { return cache_root_; }
  const std::filesystem::path& metadata_root() const { return metadata_root_; }

  void SetGuestFrameStats(ui::DebugOverlayDialog::FrameStatsProvider provider);

 private:
  std::function<void(PathConfig)> MakeResumeCallback();
  bool BeginLaunch(PathConfig paths);
  void InstallMediaRecovery(std::string executable);
  void ReleaseMediaRecoveryUi();
  std::optional<ui::GameSourceVisuals> GetGameSourceVisuals();

  bool InitializeGamingRuntime();

  void SetupOverlays(ui::Presenter* presenter, ui::ImmediateDrawer* drawer);

  bool OnInitialize() override;
  void OnDestroy() override;

  void OnClosing(ui::UIEvent& e) override;
  bool OnCloseRequested(ui::UIEvent& e) override;
  void OnResize(ui::UISetupEvent& e) override;
  void OnDpiChanged(ui::UISetupEvent& e) override;
  void OnGotFocus(ui::UISetupEvent& e) override;
  void OnLostFocus(ui::UISetupEvent& e) override;
  void OnMinimized(ui::UIEvent& e) override;
  void OnRestored(ui::UIEvent& e) override;

  void OnKeyDown(ui::KeyEvent& e) override;

  void SetupGuide();
  void StartGuidePoller();
  void StopGuide();
  void ToggleGuide();
  // The mouse cursor over the game: hidden in fullscreen play, shown while the guide or another
  // dialog is open (they take the mouse), hidden after a moment's stillness in a window.
  void UpdateCursorVisibility();

  PPCImageInfo ppc_info_;
  PathConfig resolved_defaults_;
  RuntimeConfig config_;
  std::filesystem::path game_data_root_;
  std::filesystem::path user_data_root_;
  std::filesystem::path update_data_root_;
  std::filesystem::path cache_root_;
  std::filesystem::path metadata_root_;

  std::unique_ptr<system::GamingRuntime> gaming_runtime_;
  std::unique_ptr<Runtime> runtime_;
  std::unique_ptr<ui::Window> window_;
  std::thread module_thread_;
  std::atomic<bool> shutting_down_{false};
  std::unique_ptr<ui::ImmediateDrawer> immediate_drawer_;
  std::unique_ptr<ui::ImGuiDrawer> imgui_drawer_;

  std::shared_ptr<LogCaptureSink> log_sink_;
  std::unique_ptr<ui::DebugOverlayDialog> debug_overlay_;
  std::unique_ptr<ui::ConsoleDialog> console_overlay_;
  std::unique_ptr<ui::SettingsDialog> settings_overlay_;
  ui::LaunchSettingsDialog* launch_settings_ = nullptr;
  ui::GameSourceDialog* game_source_dialog_ = nullptr;
  ui::GameMediaRecoveryDialog* media_recovery_dialog_ = nullptr;
  std::shared_ptr<system::GameMediaRecovery> media_recovery_;
  bool media_system_ui_ = false;
  std::vector<ui::guide::GuideActivity> source_activities_;
  std::unique_ptr<ui::ImGuiDialog> achievements_overlay_;
  std::shared_ptr<ui::AchievementNotificationDialog> achievement_notification_;
  uint64_t achievement_notification_listener_ = 0;
  ui::DebugOverlayDialog::FrameStatsProvider frame_stats_provider_;
  std::filesystem::path config_path_;

  std::mutex guide_mutex_;
  std::shared_ptr<const ui::guide::GuideAssets> guide_assets_;
  std::string guide_error_;
  std::thread guide_loader_;
  std::thread guide_poller_;
  std::atomic<bool> guide_stop_{false};
  std::unique_ptr<ui::guide::GuideMedia> guide_media_;
  ImFont* guide_font_regular_ = nullptr;
  ImFont* guide_font_bold_ = nullptr;
  ui::guide::XboxGuide* guide_ = nullptr;
  bool guide_unavailable_shown_ = false;

  bool handed_off_ = false;

  bool restart_on_exit_ = false;
  std::filesystem::path local_dir_;
};

}
