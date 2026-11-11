/**
 * @file        ui/guide/src/guide_game_update.cpp
 * @brief       The guide's Game Update page: new releases of the recompiled game
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/guide/xbox_guide.h>

#include <fmt/format.h>

#include <rex/logging.h>
#include <rex/ui/guide/app_update.h>

namespace rex::ui::guide {
namespace {

constexpr float kPaneTop = 61.0f;
constexpr float kPaneBottom = 405.0f;

std::string Megabytes(uint64_t bytes) {
  return fmt::format("{:.1f} MB", double(bytes) / (1024.0 * 1024.0));
}

std::string RowStatus(const AppUpdateState& state) {
  switch (state.status) {
    case AppUpdateStatus::kOff:
      return "Off";
    case AppUpdateStatus::kChecking:
      return "Checking";
    case AppUpdateStatus::kUpToDate:
      return "Up to Date";
    case AppUpdateStatus::kAvailable:
      return state.skipped ? "Skipped" : "Available";
    case AppUpdateStatus::kDownloading:
      return state.download_size ? fmt::format("{}%", state.downloaded * 100 / state.download_size)
                                 : "Downloading";
    case AppUpdateStatus::kReady:
      return "Ready";
    case AppUpdateStatus::kFailed:
      return "Failed";
  }
  return {};
}

}

void XboxGuide::OpenGameUpdate() {
  SettingsPage& page = PushPage(assets_->options_notifications, "Game Update");
  game_update_scene_ = page.scene;
  game_update_row_ = nullptr;
  if (xui::Element* e = game_update_scene_->FindById("XuiLabel1")) {
    xui::Vec3 p = e->GetVector("Position");
    p.y = kPaneTop;
    e->Set("Position", xui::Value{p});
    e->Set("Height", xui::Value{kPaneBottom - kPaneTop});
  }
  xui::Element* model = game_update_scene_->FindById("chkShow");
  if (model) {
    game_update_row_ = game_update_scene_->CloneChild(*model, "btnGameUpdate0", "btn_Count");
    game_update_row_->Set("Position", xui::Value{model->GetVector("Position")});
    game_update_row_->SetVisible(true);
  }
  for (std::string_view id :
       {"chkShow", "chkSound", "chkShowMovies", "chkShowIPTV", "labelSoundDisabled", "XuiLabel2"}) {
    if (xui::Element* e = game_update_scene_->FindById(id)) {
      e->Suppress();
    }
  }
  page.on_focus = [this] { ShowGameUpdate(); };
  page.on_select = [this](xui::Element*) { SelectGameUpdate(); };
  page.on_x = [this](xui::Element*) {
    if (GetAppUpdateState().can_roll_back) {
      OpenConfirm(Confirm::kGameRollBack);
    }
  };
  page.on_y = [this] {
    const AppUpdateState state = GetAppUpdateState();
    if (state.status == AppUpdateStatus::kAvailable && !state.skipped) {
      SkipAppUpdate();
      ShowGameUpdate();
    }
  };
  if (game_update_row_) {
    SetFocus(game_update_row_, true);
  }
  CheckForAppUpdate(false);
  ShowGameUpdate();
}

void XboxGuide::PollGameUpdate() {
  if (!game_update_scene_ || pages_.empty() || pages_.back().scene != game_update_scene_ ||
      screen_ != Screen::kSettings) {
    return;
  }
  const AppUpdateState state = GetAppUpdateState();
  if (game_update_row_ && RowStatus(state) != game_update_row_->secondary_text()) {
    ShowGameUpdate();
  } else if (state.status == AppUpdateStatus::kDownloading) {
    ShowGameUpdate();
  }
}

void XboxGuide::ShowGameUpdate() {
  const AppUpdateState state = GetAppUpdateState();
  if (game_update_row_) {
    game_update_row_->SetText(state.current_version.empty()
                                  ? std::string("This Game")
                                  : fmt::format("Version {}", state.current_version));
    game_update_row_->SetSecondaryText(RowStatus(state));
  }
  std::string details;
  std::string action;
  std::string y_action;
  switch (state.status) {
    case AppUpdateStatus::kOff:
      details = "Updates aren't available for this game.";
      break;
    case AppUpdateStatus::kChecking:
      details = "Checking for a newer version...";
      break;
    case AppUpdateStatus::kUpToDate:
      details = fmt::format("Version {} is the newest.", state.current_version);
      action = "Check Again";
      break;
    case AppUpdateStatus::kAvailable:
      details =
          fmt::format("Version {} is available. You have {}.{}", state.latest_version,
                      state.current_version, state.skipped ? "\r\nYou chose to skip it." : "");
      if (state.download_size) {
        details += fmt::format("\r\nDownload: {}", Megabytes(state.download_size));
      }
      details += "\r\n\r\n" +
                 (state.notes.empty() ? std::string("No notes are listed for it.") : state.notes);
      action = "Download";
      y_action = state.skipped ? "" : "Skip Version";
      break;
    case AppUpdateStatus::kDownloading:
      details = state.download_size
                    ? fmt::format("Downloading version {}: {} of {}", state.latest_version,
                                  Megabytes(state.downloaded), Megabytes(state.download_size))
                    : fmt::format("Downloading version {}...", state.latest_version);
      break;
    case AppUpdateStatus::kReady:
      details = fmt::format(
          "Version {} is ready to install. The game will close, update and start again. Your "
          "saved games and settings will be kept.",
          state.latest_version);
      action = "Install";
      break;
    case AppUpdateStatus::kFailed:
      details = state.error;
      action = "Check Again";
      break;
  }
  if (state.can_roll_back) {
    details += "\r\n\r\nTo go back to the version you had before, press X.";
  }
  details += "\r\n\r\nUpdates never install without asking.";
  if (xui::Element* e = game_update_scene_->FindById("XuiLabel1")) {
    e->SetText(std::move(details));
  }
  SetLegends(action, game_update_scene_->GetString("LegendB"), y_action,
             state.can_roll_back ? "Previous Version" : "");
}

void XboxGuide::SelectGameUpdate() {
  const AppUpdateState state = GetAppUpdateState();
  switch (state.status) {
    case AppUpdateStatus::kUpToDate:
    case AppUpdateStatus::kFailed:
      CheckForAppUpdate(true);
      break;
    case AppUpdateStatus::kAvailable:
      DownloadAppUpdate();
      break;
    case AppUpdateStatus::kReady:
      OpenConfirm(Confirm::kGameUpdate);
      return;
    default:
      media_->PlaySound("sharedres://btn_InactiveSelect.xma", "");
      return;
  }
  ShowGameUpdate();
}

void XboxGuide::ApplyGameUpdateChoice() {
  std::string error;
  const bool rollback = confirm_ == Confirm::kGameRollBack;
  const bool started = rollback ? RollBackAppUpdate(&error) : InstallAppUpdate(&error);
  if (!started) {
    REXLOG_WARN("Xbox guide: game update couldn't start: {}", error);
    CloseConfirm();
    if (game_update_scene_) {
      if (xui::Element* e = game_update_scene_->FindById("XuiLabel1")) {
        e->SetText("The update couldn't start:\r\n" + error);
      }
    }
    return;
  }
  REXLOG_INFO("Xbox guide: {}; the game closes for the updater",
              rollback ? "going back to the previous version" : "installing the update");
  BeginClose(true);
}

}
