/**
 * @file        ui/guide/include/rex/ui/guide/xbox_guide.h
 * @brief       The Xbox 360 guide, run from the console's own scenes (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <imgui.h>

#include <rex/audio/audio_outputs.h>
#include <rex/cvar.h>
#include <rex/image_info.h>
#include <rex/ui/guide/code_patch_states.h>
#include <rex/ui/guide/active_downloads.h>
#include <rex/system/achievement_store.h>
#include <rex/ui/display_info.h>
#include <rex/ui/guide/guide_input.h>
#include <rex/ui/imgui_dialog.h>
#include <rex/ui/overlay/achievement_icon_cache.h>
#include <rex/ui/xui/document.h>
#include <rex/ui/xui/renderer.h>
#include <rex/ui/xui/runtime.h>
#include <rex/ui/xui/system_update.h>

namespace rex {
class Runtime;
namespace audio {
struct PcmSound;
class UiSoundPlayer;
}
namespace input {
class InputSystem;
}
namespace system {
class AchievementManager;
class KernelState;
}
}

namespace rex::ui {
class ImmediateDrawer;
class ImmediateTexture;
}

REXCVAR_DECLARE(bool, xbox_guide);
REXCVAR_DECLARE(std::string, xbox_guide_system_update);
REXCVAR_DECLARE(bool, notifications_show);
REXCVAR_DECLARE(bool, notifications_sound);
REXCVAR_DECLARE(std::string, code_patch_states);
REXCVAR_DECLARE(bool, resolution_match_display);

namespace rex::ui::guide {

std::vector<std::filesystem::path> SystemUpdateLocations();

bool RegisterEmbeddedGuide(const uint8_t* data, size_t size);
std::span<const uint8_t> EmbeddedGuide();

enum class GuidePresentation { Xbox360, OriginalXbox };

struct GuideAssets {
  std::unique_ptr<xui::SystemUpdate> update;
  xui::Document skin;
  xui::Document backdrop;
  xui::Document main;
  xui::Document home_tab, games_tab, settings_tab;

  bool emulator_layout = false;
  xui::Document xbox_settings;
  bool has_xbox_settings = false;
  xui::Document achievements, achievement_details;
  bool has_achievement_scenes = false;
  xui::Document notify;
  bool has_notify = false;

  xui::Document options, options_vibration, options_notifications, options_voice;
  bool has_options = false;

  xui::Document keyboard_main, keyboard_base;
  bool has_keyboard = false;
  std::vector<std::string> hud_strings, xam_strings, profile_strings;

  static std::unique_ptr<GuideAssets> Load(
      const std::filesystem::path& path, std::string* error,
      GuidePresentation presentation = GuidePresentation::Xbox360);

  static std::unique_ptr<GuideAssets> LoadBundle(
      std::span<const uint8_t> bundle, std::string* error,
      GuidePresentation presentation = GuidePresentation::Xbox360);
  static std::unique_ptr<GuideAssets> FromUpdate(
      std::unique_ptr<xui::SystemUpdate> update, std::string* error,
      GuidePresentation presentation = GuidePresentation::Xbox360);
};

std::string FindString(const std::vector<std::string>& strings, std::string_view prefix,
                       std::string_view fallback);

struct GuideFonts {
  ImFont* regular = nullptr;
  ImFont* bold = nullptr;
};

bool DrawGuideVectorImage(ImDrawList& list, std::string_view path,
                          const std::function<ImVec2(ImVec2)>& to_screen, float opacity,
                          ImFont* bold);

void HideGuideButtonLetters(xui::Element* root);

GuideFonts AddGuideFonts(ImFontAtlas* atlas, int display_height = 0);

class GuideMedia {
 public:
  GuideMedia(ImmediateDrawer* immediate_drawer, std::shared_ptr<const GuideAssets> assets);
  ~GuideMedia();

  ImTextureID Texture(std::string_view path, std::string_view package, int* width, int* height);
  void PlaySound(std::string_view file, std::string_view package);

 private:
  struct Image {
    std::unique_ptr<ImmediateTexture> texture;
    int width = 0, height = 0;
  };

  ImmediateDrawer* immediate_drawer_;
  std::shared_ptr<const GuideAssets> assets_;
  std::map<std::string, Image, std::less<>> images_;
  std::map<std::string, std::shared_ptr<const audio::PcmSound>, std::less<>> sounds_;
  std::unique_ptr<audio::UiSoundPlayer> player_;
};

struct GuideHost {
  input::InputSystem* input = nullptr;
  system::KernelState* kernel_state = nullptr;
  system::AchievementManager* achievements = nullptr;
  Runtime* runtime = nullptr;
  ImmediateDrawer* immediate_drawer = nullptr;
  std::string title_name;

  const PPCSwitchablePatch* patches = nullptr;

  const PPCTitleCheat* cheats = nullptr;

  const PPCTitleDlc* dlc = nullptr;

  uint32_t title_update = 0;

  const PPCTitleUpdate* title_updates = nullptr;

  std::filesystem::path local_dir;

  std::function<void()> restart_title;

  int display_scale = 1;

  std::function<std::vector<audio::AudioOutput>()> audio_outputs;

  std::function<std::vector<DisplayInfo>()> displays;

  std::function<void()> save_settings;

  std::function<std::vector<GuideActivity>()> activities;

  std::function<void(bool exit_title)> on_closed;
};

class XboxGuide final : public ImGuiDialog {
 public:
  XboxGuide(ImGuiDrawer* drawer, std::shared_ptr<const GuideAssets> assets, GuideMedia* media,
            GuideFonts fonts, GuideHost host, uint16_t held_buttons);
  ~XboxGuide() override;

  void Dismiss();

 protected:
  void OnDraw(ImGuiIO& io) override;
  void OnClose() override;

 private:
  enum class Screen { kMain, kAchievements, kAchievementDetail, kConfirm, kSettings };
  enum class Confirm { kXboxHome, kTurnOff, kTitleUpdate, kDeleteSave, kGameUpdate, kGameRollBack };

  void Handle(GuideAction action);
  void HandleMain(GuideAction action);
  void HandleAchievements(GuideAction action);
  void HandleConfirm(GuideAction action);
  void Activate(xui::Element* control);

  std::span<const int> Tabs() const;
  int SettingsTab() const { return Tabs().back(); }

  void SwitchTab(int direction);
  xui::Element* FirstFocusable(xui::Element* root);
  void SetFocus(xui::Element* control, bool initial = false);
  bool IsTabMenuEntry(const xui::Element* control) const;
  void SetLegends(std::string_view a, std::string_view b, std::string_view y,
                  std::string_view x = {});
  void ConfigureMain();

  void OpenAchievements();
  void ShowAchievement(size_t index);
  void ScrollAchievements();
  void OpenAchievementDetail();
  void CloseAchievements();

  void OpenConfirm(Confirm confirm);
  void CloseConfirm();

  struct SettingsPage {
    xui::Element* scene = nullptr;
    xui::Element* return_focus = nullptr;
    std::function<void(xui::Element*)> on_select;
    std::function<void()> on_focus;
    std::function<void(xui::Element*, int)> on_adjust;
    std::function<void(xui::Element*)> on_x;
    std::function<void()> on_y;
  };
  SettingsPage& PushPage(const xui::Document& scene, std::string heading);
  void PopPage();
  void HandleSettings(GuideAction action);
  void OpenPreferences();
  void OpenVibration();
  void OpenVolume();
  void OpenNotifications();
  void OpenResolution();
  void OpenAudioOutput();
  void OpenDisplay();

  void OpenXboxSettings();
  void OpenPatches(std::string_view category);
  void OpenCheats();
  void SetSlider(xui::Element* slider, int value);

  struct DlcEntry {
    std::string id;
    std::filesystem::path package;
    std::string file_name;
    std::string name;
    std::string package_name;
    std::string publisher;
    std::string description;
    uint32_t requires_title_update = 0;
    bool installed = false;
  };
  struct DlcJob;
  void OpenManageGame();
  void FillManageGame();
  void StartDlcInstall(const DlcEntry& entry);
  void ShowDlc(xui::Element* row);
  void PollManageGame();
  std::vector<DlcEntry> FindDlc() const;

  struct SaveEntry {
    std::string name;
    std::string file_name;
    uint64_t xuid = 0;
    uint64_t size = 0;
    std::string modified;
  };
  void OpenManageStorage();
  void FillManageStorage();
  void ShowSave(xui::Element* row);
  void DeleteChosenSave();
  std::vector<SaveEntry> FindSaves() const;

  void OpenTitleUpdates();
  void FillTitleUpdates();
  void PollTitleUpdates();

  const PPCTitleUpdate* TitleUpdateAt(xui::Element* row) const;
  std::string TitleUpdateAction(const PPCTitleUpdate& update) const;
  void ShowTitleUpdate(const PPCTitleUpdate& update);
  void SelectTitleUpdate(const PPCTitleUpdate& update);
  void ChooseTitleUpdateFile(const PPCTitleUpdate& update);
  void ApplyTitleUpdateChoice();
  void OpenGameUpdate();
  void PollGameUpdate();
  void ShowGameUpdate();
  void SelectGameUpdate();
  void ApplyGameUpdateChoice();

  void OpenActiveDownloads();
  void FillActiveDownloads();
  void PollActiveDownloads();

  void BeginClose(bool exit_title);
  void UpdateClock();

  void ColourGamerscoreGlyph();

  void UpdateControllerBattery();
  ImTextureID Texture(std::string_view path, std::string_view package, int* width, int* height);

  std::shared_ptr<const GuideAssets> assets_;
  GuideMedia* media_;
  GuideFonts fonts_;
  GuideHost host_;
  GuidePad pad_;
  AchievementIconCache icons_;
  xui::RenderResources render_;

  xui::SceneContext backdrop_context_, hud_context_, skin_context_, profile_context_;
  std::unique_ptr<xui::Element> backdrop_;
  xui::Element* hud_root_ = nullptr;
  xui::Element* app_host_ = nullptr;
  xui::Element* error_host_ = nullptr;
  xui::Element* main_ = nullptr;
  xui::Element* tabs_ = nullptr;
  xui::Element* tab_scenes_[5] = {};
  xui::Element* tab_focus_[5] = {};
  int tab_ = 2;
  xui::Element* focus_ = nullptr;

  Screen screen_ = Screen::kMain;
  std::vector<system::AchievementInfo> achievement_list_;
  xui::Element* achievements_ = nullptr;
  xui::Element* details_ = nullptr;
  size_t selected_ = 0;
  size_t first_row_ = 0;
  int columns_ = 1;
  int visible_rows_ = 1;

  Confirm confirm_ = Confirm::kXboxHome;
  uint32_t confirm_title_update_ = 0;
  xui::Element* message_ = nullptr;
  xui::Element* return_focus_ = nullptr;
  std::vector<xui::Element*> pending_removal_;
  std::vector<SettingsPage> pages_;
  xui::Element* manage_scene_ = nullptr;
  std::vector<xui::Element*> manage_rows_;
  std::vector<DlcEntry> dlc_;
  std::vector<std::filesystem::path> picked_packages_;
  std::shared_ptr<DlcJob> dlc_job_;
  std::string dlc_status_;
  xui::Element* storage_scene_ = nullptr;
  std::vector<xui::Element*> storage_rows_;
  std::vector<SaveEntry> saves_;
  size_t confirm_save_ = 0;
  std::string storage_status_;
  std::shared_ptr<std::filesystem::path> picked_title_update_;
  std::shared_ptr<std::atomic<bool>> title_update_pick_;
  uint32_t title_update_pick_version_ = 0;
  xui::Element* updates_scene_ = nullptr;
  xui::Element* game_update_scene_ = nullptr;
  xui::Element* game_update_row_ = nullptr;
  std::vector<xui::Element*> update_rows_;
  xui::Element* downloads_scene_ = nullptr;
  std::vector<xui::Element*> download_rows_;
  std::vector<GuideActivity> download_items_;

  std::unique_ptr<xui::Node> dlc_banner_node_;
  xui::Element* dlc_banner_ = nullptr;

  bool closing_ = false;
  bool exit_title_ = false;
  float dim_ = 0.0f;
  std::chrono::steady_clock::time_point last_tick_;
  std::chrono::steady_clock::time_point opened_;
  int64_t clock_minute_ = -1;
  int64_t battery_second_ = -1;
};

}
