/**
 * @file        ui/guide/src/guide_title_update.cpp
 * @brief       The guide's Title Updates and Active Downloads pages (RG-GDK-057)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/guide/xbox_guide.h>

#include <algorithm>
#include <thread>

#include <windows.h>
#include <shobjidl.h>

#include <fmt/format.h>

#include <rex/filesystem.h>
#include <rex/logging.h>
#include <rex/string/utf8.h>
#include <rex/system/kernel_state.h>
#include <rex/ui/guide/title_update.h>

namespace rex::ui::guide {
namespace {

constexpr float kRowHeight = 28.0f;
constexpr size_t kMaxRows = 11;

constexpr float kPaneTop = 61.0f;
constexpr float kPaneBottom = 405.0f;

uint32_t TitleId(const GuideHost& host) {
  return host.kernel_state ? host.kernel_state->title_id() : 0;
}

std::string Megabytes(uint64_t bytes) {
  return fmt::format("{:.1f} MB", double(bytes) / (1024.0 * 1024.0));
}

std::filesystem::path PickPackage(const std::string& title) {
  std::filesystem::path picked;
  const HRESULT init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  IFileOpenDialog* dialog = nullptr;
  if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
                                 IID_PPV_ARGS(&dialog)))) {
    DWORD options = 0;
    dialog->GetOptions(&options);
    dialog->SetOptions(options | FOS_FILEMUSTEXIST | FOS_FORCEFILESYSTEM);
    const std::u16string utf16 = string::to_utf16(title);
    dialog->SetTitle(reinterpret_cast<const wchar_t*>(utf16.c_str()));
    IShellItem* item = nullptr;
    PWSTR path = nullptr;
    if (SUCCEEDED(dialog->Show(GetForegroundWindow())) && SUCCEEDED(dialog->GetResult(&item)) &&
        SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
      picked = path;
      CoTaskMemFree(path);
    }
    if (item) {
      item->Release();
    }
    dialog->Release();
  }
  if (SUCCEEDED(init)) {
    CoUninitialize();
  }
  return picked;
}

}

const PPCTitleUpdate* XboxGuide::TitleUpdateAt(xui::Element* row) const {
  const auto at = std::find(update_rows_.begin(), update_rows_.end(), row);
  return at != update_rows_.end() ? &host_.title_updates[size_t(at - update_rows_.begin())]
                                  : nullptr;
}

void XboxGuide::OpenTitleUpdates() {
  SettingsPage& page = PushPage(assets_->options_notifications, "Title Updates");
  updates_scene_ = page.scene;
  update_rows_.clear();

  if (xui::Element* e = updates_scene_->FindById("XuiLabel1")) {
    xui::Vec3 p = e->GetVector("Position");
    p.y = kPaneTop;
    e->Set("Position", xui::Value{p});
    e->Set("Height", xui::Value{kPaneBottom - kPaneTop});
  }
  page.on_focus = [this] {
    if (const PPCTitleUpdate* update = TitleUpdateAt(focus_)) {
      ShowTitleUpdate(*update);
    }
  };
  page.on_select = [this](xui::Element* row) {
    if (const PPCTitleUpdate* update = TitleUpdateAt(row)) {
      SelectTitleUpdate(*update);
    }
  };
  page.on_x = [this](xui::Element* row) {
    if (const PPCTitleUpdate* update = TitleUpdateAt(row)) {
      ChooseTitleUpdateFile(*update);
    }
  };
  FillTitleUpdates();
}

void XboxGuide::FillTitleUpdates() {
  xui::Element* scene = updates_scene_;
  xui::Element* model = scene->FindById("chkShow");
  if (!model) {
    return;
  }
  const float top = model->GetVector("Position").y;
  for (const PPCTitleUpdate* u = host_.title_updates;
       u && u->version && update_rows_.size() < kMaxRows; ++u) {
    const size_t i = update_rows_.size();
    xui::Element* row = scene->CloneChild(*model, fmt::format("btnTitleUpdate{}", i), "btn_Count");
    xui::Vec3 p = model->GetVector("Position");
    p.y = top + float(i) * kRowHeight;
    row->Set("Position", xui::Value{p});
    row->SetText(fmt::format("Title Update {}", u->version));
    row->SetSecondaryText(TitleUpdateAction(*u));
    row->SetVisible(true);
    update_rows_.push_back(row);
  }
  for (size_t i = 0; i < update_rows_.size(); ++i) {
    update_rows_[i]->Set(
        "NavUp", xui::Value{i > 0 ? std::string(update_rows_[i - 1]->id()) : std::string()});
    update_rows_[i]->Set(
        "NavDown", xui::Value{i + 1 < update_rows_.size() ? std::string(update_rows_[i + 1]->id())
                                                          : std::string()});
  }
  for (std::string_view id :
       {"chkShow", "chkSound", "chkShowMovies", "chkShowIPTV", "labelSoundDisabled", "XuiLabel2"}) {
    if (xui::Element* e = scene->FindById(id)) {
      e->Suppress();
    }
  }
  if (update_rows_.empty()) {
    if (xui::Element* details = scene->FindById("XuiLabel1")) {
      details->SetText("There are no title updates for this game.");
    }
    SetLegends("", scene->GetString("LegendB"), "");
    return;
  }
  SetFocus(update_rows_.front(), true);
  ShowTitleUpdate(host_.title_updates[0]);
}

void XboxGuide::PollTitleUpdates() {
  if (title_update_pick_ && title_update_pick_->load()) {
    title_update_pick_.reset();
    const auto picked = std::move(picked_title_update_);
    for (const PPCTitleUpdate* u = host_.title_updates; u && u->version; ++u) {
      if (u->version == title_update_pick_version_ && picked && !picked->empty()) {
        REXLOG_INFO("Xbox guide: installing title update {} from {}", u->version,
                    path_to_utf8(*picked));
        StartTitleUpdateInstall(*u, TitleId(host_), host_.local_dir, *picked);
      }
    }
  }
  if (!updates_scene_ || pages_.empty() || pages_.back().scene != updates_scene_) {
    return;
  }

  for (size_t i = 0; i < update_rows_.size(); ++i) {
    const PPCTitleUpdate& update = host_.title_updates[i];
    const std::string action = TitleUpdateAction(update);
    const auto job = FindTitleUpdateJob(update.version);
    const bool running = job && job->state.load() == TitleUpdateJob::State::kRunning;
    if (action != update_rows_[i]->secondary_text()) {
      update_rows_[i]->SetSecondaryText(action);
    } else if (!running) {
      continue;
    }
    if (update_rows_[i] == focus_) {
      ShowTitleUpdate(update);
    }
  }
}

namespace {

struct TitleUpdateState {
  bool built = false;
  bool installed = false;
  bool on = false;
  bool running = false;
  std::shared_ptr<TitleUpdateJob> job;
};

TitleUpdateState StateOf(const PPCTitleUpdate& update, const GuideHost& host) {
  TitleUpdateState s;
  std::error_code ec;
  s.running = host.title_update == update.version;
  s.built = s.running || std::filesystem::is_regular_file(
                             TitleUpdateExecutable(filesystem::GetExecutablePath(),
                                                   host.title_update, update.version),
                             ec);
  s.installed = !FindInstalledTitleUpdate(host.local_dir, update.version).empty();
  s.on = uint32_t(std::max(0, REXCVAR_GET(title_update))) == update.version;
  s.job = FindTitleUpdateJob(update.version);
  return s;
}

}

std::string XboxGuide::TitleUpdateAction(const PPCTitleUpdate& update) const {
  const TitleUpdateState s = StateOf(update, host_);
  if (s.job && s.job->state.load() == TitleUpdateJob::State::kRunning) {
    const uint64_t total = s.job->total.load();
    return s.job->from_file ? "Installing"
           : total          ? fmt::format("{}%", s.job->done.load() * 100 / total)
                            : "Downloading";
  }
  if (!s.built) {
    return "Not Available";
  }
  if (!s.installed) {
    return "Download";
  }
  return s.on ? "On" : "Off";
}

void XboxGuide::ShowTitleUpdate(const PPCTitleUpdate& update) {
  const TitleUpdateState s = StateOf(update, host_);
  const bool busy = s.job && s.job->state.load() == TitleUpdateJob::State::kRunning;
  std::string status;
  std::string action, x_action;
  if (busy) {
    const uint64_t done = s.job->done.load(), total = s.job->total.load();
    status = s.job->from_file ? "Installing the file you chose..."
             : total ? fmt::format("Downloading: {} of {}", Megabytes(done), Megabytes(total))
                     : "Downloading...";
  } else if (!s.built) {
    status = "This title update can't be used with this version of the game.";
  } else if (!s.installed) {
    status = "Not installed. You don't need it to play.";
    if (s.job && s.job->state.load() == TitleUpdateJob::State::kFailed) {
      status = s.job->from_file
                   ? "That file isn't a title update for this game."
                   : "The title update couldn't be downloaded. Check your network connection "
                     "and try again.";
    }
    action = "Download";
    x_action = "Choose File";
  } else if (s.on) {
    status = s.running ? "On." : "On the next time the game starts.";
    action = "Turn Off";
  } else {
    status = s.running ? "Off the next time the game starts." : "Installed. Off.";
    action = "Turn On";
  }
  std::string facts;
  if (update.date && *update.date) {
    facts = fmt::format("Released {}", update.date);
  }
  if (update.size_kb) {
    facts += (facts.empty() ? "" : " · ") + Megabytes(uint64_t(update.size_kb) * 1024);
  }
  std::string details = fmt::format("Title Update {}\r\n{}", update.version, status);
  if (!facts.empty()) {
    details += "\r\n\r\n" + facts;
  }
  details += "\r\n\r\n";
  details +=
      update.changelog && *update.changelog ? update.changelog : "No changelog is listed for it.";
  details +=
      "\r\n\r\nMods made for the original version are left out while it's on; turn it off "
      "to use them again.";
  if (xui::Element* e = updates_scene_->FindById("XuiLabel1")) {
    e->SetText(std::move(details));
  }
  SetLegends(busy ? "" : action, updates_scene_->GetString("LegendB"), "", busy ? "" : x_action);
}

void XboxGuide::SelectTitleUpdate(const PPCTitleUpdate& update) {
  const TitleUpdateState s = StateOf(update, host_);
  if ((s.job && s.job->state.load() == TitleUpdateJob::State::kRunning) || !s.built) {
    media_->PlaySound("sharedres://btn_InactiveSelect.xma", "");
    return;
  }
  if (!s.installed) {
    REXLOG_INFO("Xbox guide: downloading title update {}", update.version);
    StartTitleUpdateDownload(update, TitleId(host_), host_.local_dir);
    ShowTitleUpdate(update);
    return;
  }

  confirm_title_update_ = s.on ? 0 : update.version;
  OpenConfirm(Confirm::kTitleUpdate);
}

void XboxGuide::ChooseTitleUpdateFile(const PPCTitleUpdate& update) {
  const TitleUpdateState s = StateOf(update, host_);
  if (!s.built || s.installed || title_update_pick_ ||
      (s.job && s.job->state.load() == TitleUpdateJob::State::kRunning)) {
    return;
  }
  auto done = std::make_shared<std::atomic<bool>>(false);
  title_update_pick_ = done;
  title_update_pick_version_ = update.version;
  auto picked = std::make_shared<std::filesystem::path>();
  const std::string title = fmt::format("Install title update {}", update.version);

  std::thread([done, picked, title] {
    *picked = PickPackage(title);
    done->store(true);
  }).detach();
  picked_title_update_ = picked;
}

void XboxGuide::ApplyTitleUpdateChoice() {
  REXCVAR_SET(title_update, int32_t(confirm_title_update_));
  if (host_.save_settings) {
    host_.save_settings();
  }
  REXLOG_INFO("Xbox guide: title update {} chosen; restarting",
              confirm_title_update_ ? fmt::format("{}", confirm_title_update_) : "off");
  if (host_.restart_title) {
    host_.restart_title();
  }
  BeginClose(true);
}

void XboxGuide::OpenActiveDownloads() {
  SettingsPage& page = PushPage(assets_->options_notifications, "Active Downloads");
  downloads_scene_ = page.scene;
  PrepareActiveDownloadsScene(*downloads_scene_);
  download_rows_.clear();
  page.on_focus = [this] { FillActiveDownloads(); };
  page.on_select = [this](xui::Element* row) {
    const auto at = std::find(download_rows_.begin(), download_rows_.end(), row);
    const size_t index = size_t(at - download_rows_.begin());
    if (index < download_items_.size() && download_items_[index].cancel)
      download_items_[index].cancel();
  };
  FillActiveDownloads();
}

void XboxGuide::FillActiveDownloads() {
  xui::Element* scene = downloads_scene_;
  xui::Element* model = scene ? scene->FindById("chkShow") : nullptr;
  if (!model)
    return;
  auto jobs = TitleUpdateJobs();
  std::reverse(jobs.begin(), jobs.end());
  download_items_.clear();
  for (const auto& job : jobs) {
    GuideActivity item;
    item.title = fmt::format("Title Update {}", job->update.version);
    item.details = fmt::format("{}\r\n{}", item.title,
                               job->from_file ? "From a file on this PC" : "From the internet");

    const auto state = job->state.load();
    const uint64_t done = job->done.load(), total = job->total.load();
    switch (state) {
      case TitleUpdateJob::State::kRunning:
        item.status =
            job->from_file ? "Installing"
            : total
                ? fmt::format("{}%", unsigned(std::min(100.0, double(done) / double(total) * 100)))
                : "Downloading";
        if (!job->from_file)
          item.cancel = [job] { job->cancel = true; };
        break;
      case TitleUpdateJob::State::kInstalled:
        item.status = "Installed";
        break;
      case TitleUpdateJob::State::kFailed:
        item.status = "Failed";
        break;
      case TitleUpdateJob::State::kCancelled:
        item.status = "Cancelled";
        break;
    }
    if (total)
      item.details += fmt::format("\r\n{} of {}", Megabytes(done), Megabytes(total));
    if (state == TitleUpdateJob::State::kFailed)
      item.details += job->from_file ? "\r\n\r\nThat file isn't a title update for this game."
                                     : "\r\n\r\nCheck your network connection and try again.";
    if (state == TitleUpdateJob::State::kInstalled)
      item.details += "\r\n\r\nTurn it on in Title Updates.";
    download_items_.push_back(std::move(item));
  }
  if (host_.activities) {
    auto activities = host_.activities();
    download_items_.insert(download_items_.end(), std::make_move_iterator(activities.begin()),
                           std::make_move_iterator(activities.end()));
  }
  const size_t rows = std::min(download_items_.size(), kMaxRows);
  if (download_rows_.size() != rows) {
    const size_t focused = size_t(std::find(download_rows_.begin(), download_rows_.end(), focus_) -
                                  download_rows_.begin());
    for (xui::Element* row : download_rows_) {
      if (row == focus_) {
        focus_ = nullptr;
      }
      row->parent()->RemoveChild(row);
    }
    download_rows_.clear();
    const float top = model->GetVector("Position").y;
    for (size_t i = 0; i < rows; ++i) {
      xui::Element* row = scene->CloneChild(*model, fmt::format("btnDownload{}", i), "btn_Count");
      xui::Vec3 p = model->GetVector("Position");
      p.y = top + float(i) * kRowHeight;
      row->Set("Position", xui::Value{p});
      row->SetVisible(true);
      download_rows_.push_back(row);
    }
    for (size_t i = 0; i < download_rows_.size(); ++i) {
      download_rows_[i]->Set(
          "NavUp", xui::Value{i > 0 ? std::string(download_rows_[i - 1]->id()) : std::string()});
      download_rows_[i]->Set("NavDown", xui::Value{i + 1 < download_rows_.size()
                                                       ? std::string(download_rows_[i + 1]->id())
                                                       : std::string()});
    }
    if (!download_rows_.empty()) {
      SetFocus(download_rows_[std::min(focused, download_rows_.size() - 1)], true);
    }
  }
  std::string details = "Nothing is downloading. Choose game-source extraction or a title update.";
  for (size_t i = 0; i < rows; ++i) {
    download_rows_[i]->SetText(download_items_[i].title);
    download_rows_[i]->SetSecondaryText(download_items_[i].status);
    if (download_rows_[i] == focus_)
      details = download_items_[i].details;
  }
  if (auto* label = scene->FindById("XuiLabel1"))
    label->SetText(std::move(details));
  const auto focused = std::find(download_rows_.begin(), download_rows_.end(), focus_);
  const size_t index = size_t(focused - download_rows_.begin());
  const bool cancellable = focused != download_rows_.end() && index < download_items_.size() &&
                           bool(download_items_[index].cancel);
  SetLegends(cancellable ? "Cancel" : "", scene->GetString("LegendB"), "");
}

void XboxGuide::PollActiveDownloads() {
  if (downloads_scene_ && !pages_.empty() && pages_.back().scene == downloads_scene_)
    FillActiveDownloads();
}

}
