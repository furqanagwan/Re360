// Copyright (c) 2026 ReXGlue contributors. BSD 3-Clause License; see LICENSE.
#pragma once
#include <array>
#include <atomic>
#include <chrono>
#include <future>
#include <vector>
#include <rex/system/game_source.h>
#include <rex/ui/guide/file_browser.h>
#include <rex/ui/guide/guide_list_page.h>
#include <rex/ui/imgui_dialog.h>
#include <rex/ui/overlay/launch_settings.h>
#include <rex/ui/guide/xbox_guide.h>
#include <rex/ui/xui/renderer.h>

namespace rex::ui {
struct GameSourceVisuals {
  std::shared_ptr<const guide::GuideAssets> assets;
  xui::RenderResources resources;
  std::function<void(std::string_view, std::string_view)> play_sound;
  bool loading = false;
};
using GameSourceVisualsProvider = std::function<std::optional<GameSourceVisuals>()>;
class GameSourceConsoleBox;
class GameSourceGuidePage;
class GameSourceConsoleDownloads;

class GameSourceDialog : public ImGuiDialog {
 public:
  GameSourceDialog(ImGuiDrawer* drawer, system::GameSourceIdentity expected,
                   std::filesystem::path config,
                   std::function<void(std::filesystem::path)> completed,
                   std::filesystem::path initial = {}, std::string error = {},
                   std::string executable = "default.xex",
                   std::filesystem::path extraction_root = {}, LaunchPadSource pad_source = {},
                   GameSourceVisualsProvider visuals = {},
                   std::function<void(guide::GuideActivity)> activity_completed = {});
  ~GameSourceDialog() override;

 protected:
  void OnDraw(ImGuiIO& io) override;
  void OnClose() override;

 private:
  enum class ConsoleScreen {
    kSource,
    kBrowse,
    kDrives,
    kChecking,
    kExtracting,
    kVerified,
    kError,
    kSaveFailed,
    kCopied
  };

  void Browse(bool folder);

  void BrowseNative(bool folder);
  void ShowBrowseScreen();
  void ShowConsolePage(ConsoleScreen screen, std::string title,
                       std::vector<guide::GuideListRow> rows, size_t initial = 0,
                       std::string legend_b = "Back", std::string empty_details = {});

  void ShowConsoleScreen(ConsoleScreen screen, std::string title, std::string body,
                         std::vector<std::string> choices, size_t initial = 0);
  void ShowSourceScreen();
  void ShowDriveScreen();
  void HandleConsoleInput(const LaunchPadState& pad);
  void HandleConsoleCancel();

  void ReturnFromCheck();
  void BeginSourceCheck(std::filesystem::path path);
  void HandleConsoleChoice(size_t choice);
  void CompleteConsoleSelection(bool remember);
  void StartExtraction();
  void CollectOpticalDrives();
  system::GameSourceIdentity expected_;
  std::filesystem::path config_;
  std::function<void(std::filesystem::path)> completed_;
  std::filesystem::path initial_source_;
  std::array<char, 4096> path_{};
  std::future<system::GameSourceResult> checking_;
  std::shared_ptr<std::atomic<bool>> check_cancel_ = std::make_shared<std::atomic<bool>>(false);
  std::future<system::GameSourceExtraction> extracting_;
  struct CopyProgress {
    std::atomic<uint64_t> done = 0;
    std::atomic<uint64_t> total = 0;
    std::atomic<bool> cancel = false;
  };
  std::shared_ptr<CopyProgress> copy_progress_;
  std::string executable_;
  std::filesystem::path extraction_root_;
  std::filesystem::path checking_path_;
  std::filesystem::path validated_;
  std::filesystem::path selected_;
  std::string error_;
  bool remember_ = true;
  LaunchPadSource pad_source_;
  GameSourceVisualsProvider visuals_;
  std::unique_ptr<GameSourceGuidePage> console_page_;
  bool console_initialized_ = false;
  bool console_mode_ = false;
  ConsoleScreen console_screen_ = ConsoleScreen::kSource;
  ConsoleScreen check_origin_ = ConsoleScreen::kSource;
  std::vector<std::string> console_choices_;

  bool console_waiting_for_guide_ = false;
  std::string console_title_;
  std::vector<guide::GuideListRow> console_rows_;
  size_t console_initial_choice_ = 0;
  std::string console_legend_b_;
  std::string console_empty_details_;
  LaunchPadState previous_pad_;
  int previous_pad_direction_ = 0;
  std::chrono::steady_clock::time_point next_pad_navigation_{};
  bool browse_folder_ = false;
  std::unique_ptr<guide::GuideFileBrowser> browser_;
  std::vector<std::string> optical_drives_;
  std::unique_ptr<GameSourceConsoleDownloads> console_downloads_;
  std::function<void(guide::GuideActivity)> activity_completed_;
};
// Setup's last step, before the game starts: the GPU prepares the shaders earlier runs stored.
// Shown on the Guide's list page ("Preparing Shaders") when the Guide's assets are there, else as a
// plain bar. Closes itself and calls `finished` once the GPU reports it is done.
class ShaderPreparationDialog : public ImGuiDialog {
 public:
  using ProgressQuery = std::function<bool(uint32_t& done, uint32_t& total)>;
  ShaderPreparationDialog(ImGuiDrawer* drawer, ProgressQuery progress,
                          std::function<void()> finished, GameSourceVisualsProvider visuals = {});
  ~ShaderPreparationDialog() override;

 protected:
  void OnDraw(ImGuiIO& io) override;

 private:
  ProgressQuery progress_;
  std::function<void()> finished_;
  GameSourceVisualsProvider visuals_;
  std::unique_ptr<GameSourceGuidePage> page_;
  bool finished_called_ = false;
};
class GameMediaRecoveryDialog : public ImGuiDialog {
 public:
  GameMediaRecoveryDialog(ImGuiDrawer* drawer, bool optical, std::string error,
                          std::function<void(bool)> completed, LaunchPadSource pad_source = {},
                          GameSourceVisualsProvider visuals = {});
  ~GameMediaRecoveryDialog() override;

 protected:
  void OnDraw(ImGuiIO& io) override;
  void OnClose() override;

 private:
  bool optical_, retry_ = false;
  bool focus_leave_ = true;
  std::string error_;
  std::function<void(bool)> completed_;
  LaunchPadSource pad_source_;
  GameSourceVisualsProvider visuals_;
  std::unique_ptr<GameSourceConsoleBox> console_box_;
};
}
