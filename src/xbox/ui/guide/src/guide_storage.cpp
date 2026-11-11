/**
 * @file        ui/guide/src/guide_storage.cpp
 * @brief       The Xbox guide's Manage Storage page: the title's saved games
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 *
 * @remarks     RG-GDK-061. The backward compatibility Home tab's Manage
 *              Storage (dash command 75 on the console) opened the storage
 *              screen; here it lists this title's saves on this PC.
 */

#include <rex/ui/guide/xbox_guide.h>

#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>

#include <fmt/format.h>

#include <rex/logging.h>
#include <rex/system/kernel_state.h>
#include <rex/system/xam/content_device.h>
#include <rex/system/xam/content_manager.h>
#include <rex/string/utf8.h>
#include <rex/system/xcontent.h>

namespace rex::ui::guide {
namespace {

constexpr float kRowHeight = 28.0f;
constexpr size_t kMaxRows = 11;

std::string FormatSize(uint64_t bytes) {
  if (bytes >= 1024 * 1024) {
    return fmt::format("{:.1f} MB", double(bytes) / (1024.0 * 1024.0));
  }
  return fmt::format("{} KB", std::max<uint64_t>(1, (bytes + 1023) / 1024));
}

uint64_t FolderSize(const std::filesystem::path& folder, std::filesystem::file_time_type& newest) {
  uint64_t size = 0;
  std::error_code ec;
  for (auto it = std::filesystem::recursive_directory_iterator(folder, ec);
       !ec && it != std::filesystem::recursive_directory_iterator(); it.increment(ec)) {
    if (it->is_regular_file(ec)) {
      size += it->file_size(ec);
      newest = std::max(newest, it->last_write_time(ec));
    }
  }
  return size;
}

system::xam::XCONTENT_AGGREGATE_DATA SaveContent(std::string_view name, std::string_view file_name,
                                                 uint64_t xuid, uint32_t title_id) {
  system::xam::XCONTENT_AGGREGATE_DATA data;
  data.device_id = static_cast<uint32_t>(system::xam::DummyDeviceId::HDD);
  data.content_type = system::XContentType::kSavedGame;
  data.set_display_name(string::to_utf16(name));
  data.set_file_name(file_name);
  data.xuid = xuid;
  data.title_id = title_id;
  return data;
}

}

std::vector<XboxGuide::SaveEntry> XboxGuide::FindSaves() const {
  std::vector<SaveEntry> saves;
  system::KernelState* kernel_state = host_.kernel_state;
  if (!kernel_state || !kernel_state->content_manager()) {
    return saves;
  }
  auto* content = kernel_state->content_manager();
  const uint64_t profile_xuid =
      kernel_state->user_profile() ? kernel_state->user_profile()->xuid() : 0;

  for (uint64_t xuid : {profile_xuid, uint64_t(0)}) {
    if (xuid == 0 && profile_xuid == 0 && !saves.empty()) {
      break;
    }
    for (const auto& data :
         content->ListContent(static_cast<uint32_t>(system::xam::DummyDeviceId::HDD), xuid,
                              system::XContentType::kSavedGame, kernel_state->title_id())) {
      SaveEntry save;
      save.name = string::to_utf8(data.display_name());
      save.file_name = std::string(data.file_name());
      if (save.name.empty()) {
        save.name = save.file_name;
      }
      save.xuid = xuid;
      std::filesystem::file_time_type newest{};
      save.size = FolderSize(content->GetPackagePath(xuid, data), newest);
      if (newest != std::filesystem::file_time_type{}) {
        const auto system_time = std::chrono::clock_cast<std::chrono::system_clock>(newest);
        const std::time_t t = std::chrono::system_clock::to_time_t(system_time);
        std::tm local{};
        localtime_s(&local, &t);
        save.modified = fmt::format("{:04}-{:02}-{:02} {:02}:{:02}", local.tm_year + 1900,
                                    local.tm_mon + 1, local.tm_mday, local.tm_hour, local.tm_min);
      }
      saves.push_back(std::move(save));
    }
    if (profile_xuid == 0) {
      break;
    }
  }
  std::stable_sort(saves.begin(), saves.end(),
                   [](const SaveEntry& a, const SaveEntry& b) { return a.name < b.name; });
  return saves;
}

void XboxGuide::OpenManageStorage() {
  SettingsPage& page = PushPage(assets_->options_notifications, "Manage Storage");
  storage_scene_ = page.scene;
  storage_rows_.clear();
  storage_status_.clear();
  page.on_focus = [this] { ShowSave(focus_); };
  page.on_select = [this](xui::Element* row) {
    const auto at = std::find(storage_rows_.begin(), storage_rows_.end(), row);
    if (at == storage_rows_.end() || size_t(at - storage_rows_.begin()) >= saves_.size()) {
      return;
    }
    confirm_save_ = size_t(at - storage_rows_.begin());
    OpenConfirm(Confirm::kDeleteSave);
  };
  FillManageStorage();
}

void XboxGuide::FillManageStorage() {
  xui::Element* scene = storage_scene_;
  xui::Element* model = scene ? scene->FindById("chkShow") : nullptr;
  if (!model) {
    return;
  }
  for (xui::Element* row : storage_rows_) {
    if (row == focus_) {
      focus_ = nullptr;
    }
    row->parent()->RemoveChild(row);
  }
  storage_rows_.clear();
  saves_ = FindSaves();

  const float top = model->GetVector("Position").y;
  const size_t rows = std::min(saves_.size(), kMaxRows);
  for (size_t i = 0; i < rows; ++i) {
    xui::Element* row = scene->CloneChild(*model, fmt::format("btnSave{}", i), "btn_Count");
    xui::Vec3 p = model->GetVector("Position");
    p.y = top + float(i) * kRowHeight;
    row->Set("Position", xui::Value{p});
    row->SetText(saves_[i].name);
    row->SetSecondaryText(FormatSize(saves_[i].size));
    row->SetVisible(true);
    storage_rows_.push_back(row);
  }
  for (size_t i = 0; i < storage_rows_.size(); ++i) {
    storage_rows_[i]->Set(
        "NavUp", xui::Value{i > 0 ? std::string(storage_rows_[i - 1]->id()) : std::string()});
    storage_rows_[i]->Set(
        "NavDown", xui::Value{i + 1 < storage_rows_.size() ? std::string(storage_rows_[i + 1]->id())
                                                           : std::string()});
  }
  for (std::string_view id :
       {"chkShow", "chkSound", "chkShowMovies", "chkShowIPTV", "labelSoundDisabled"}) {
    if (xui::Element* e = scene->FindById(id)) {
      e->Suppress();
    }
  }
  if (xui::Element* status = scene->FindById("XuiLabel2")) {
    status->Suppress();
  }
  if (storage_rows_.empty()) {
    if (xui::Element* details = scene->FindById("XuiLabel1")) {
      details->SetText(storage_status_.empty() ? "This game has no saved games on this PC."
                                               : storage_status_);
    }
    SetLegends("", scene->GetString("LegendB"), "");
    return;
  }
  SetFocus(storage_rows_.front(), true);
  ShowSave(storage_rows_.front());
}

void XboxGuide::ShowSave(xui::Element* row) {
  if (!storage_scene_ || !row) {
    return;
  }
  const auto at = std::find(storage_rows_.begin(), storage_rows_.end(), row);
  if (at == storage_rows_.end() || size_t(at - storage_rows_.begin()) >= saves_.size()) {
    return;
  }
  const SaveEntry& save = saves_[size_t(at - storage_rows_.begin())];
  std::string details = save.name;
  details += "\r\n" + FormatSize(save.size);
  if (!save.modified.empty()) {
    details += "\r\nSaved " + save.modified;
  }
  details += save.xuid ? "\r\nThis profile's save" : "\r\nShared by every profile";
  if (!storage_status_.empty()) {
    details += "\r\n\r\n" + storage_status_;
  }
  if (xui::Element* e = storage_scene_->FindById("XuiLabel1")) {
    e->SetText(std::move(details));
  }
  SetLegends("Delete", storage_scene_->GetString("LegendB"), "");
}

void XboxGuide::DeleteChosenSave() {
  if (confirm_save_ >= saves_.size() || !host_.kernel_state) {
    return;
  }
  const SaveEntry save = saves_[confirm_save_];
  auto* content = host_.kernel_state->content_manager();
  const auto data =
      SaveContent(save.name, save.file_name, save.xuid, host_.kernel_state->title_id());
  if (content->IsContentOpen(data)) {
    storage_status_ = fmt::format("{} is in use by the game.", save.name);
    media_->PlaySound("sharedres://btn_InactiveSelect.xma", "");
    ShowSave(focus_);
    storage_status_.clear();
    return;
  }
  const X_RESULT result = content->DeleteContent(save.xuid, data);
  if (result == X_ERROR_SUCCESS) {
    REXLOG_INFO("Xbox guide: deleted the saved game {} ({})", save.name, save.file_name);
    storage_status_ = fmt::format("{} was deleted.", save.name);
  } else {
    REXLOG_WARN("Xbox guide: deleting the saved game {} failed: {:08X}", save.file_name,
                uint32_t(result));
    storage_status_ = fmt::format("{} could not be deleted.", save.name);
  }
  FillManageStorage();
  if (!storage_rows_.empty()) {
    ShowSave(focus_);
  }
  storage_status_.clear();
}

}
