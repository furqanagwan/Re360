/**
 * @file        ui/guide/src/guide_dlc.cpp
 * @brief       Games & Apps > Manage Game: a title's add-ons (RG-GDK-041, RG-GDK-050)
 *
 * Lists every add-on the title had in the marketplace, from its config's
 * [[dlc]] entries and the catalogue built into it (name, publisher,
 * description, banner), as the console's in-game marketplace listed offers:
 * Install where the price was. Installing takes the add-on's package from the
 * DLC folder beside the executable, or one the player picks on this PC, and
 * installs it with the content manager off the UI thread. An add-on that
 * needs a title update the build lacks says so and is not installed. Content
 * installed or found that the catalogue lacks is listed after it. The page is
 * the console's Options scene, its rows given the btn_Count visual for the
 * right-hand column.
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/guide/xbox_guide.h>

#include <algorithm>
#include <atomic>
#include <thread>

#include <windows.h>
#include <shobjidl.h>

#include <fmt/format.h>

#include <rex/filesystem.h>
#include <rex/filesystem/devices/stfs_container_device.h>
#include <rex/logging.h>
#include <rex/string/utf8.h>
#include <rex/system/kernel_state.h>
#include <rex/system/xam/content_device.h>
#include <rex/system/xam/content_manager.h>
#include <rex/system/xcontent.h>
#include <rex/ui/guide/dlc_catalog.h>
#include <rex/ui/guide/guide_layout.h>
#include <rex/ui/xui/schema.h>

namespace rex::ui::guide {
namespace {

constexpr float kRowHeight = 28.0f;
constexpr size_t kMaxRows = 11;

constexpr float kPaneX = 425.0f;
constexpr float kPaneWidth = 287.0f;
constexpr float kBannerY = 61.0f;
constexpr float kBannerHeight = kPaneWidth * 95.0f / 420.0f;
constexpr float kDetailsY = kBannerY + kBannerHeight + 8.0f;
constexpr float kPaneBottom = 405.0f;

bool SameName(std::string_view a, std::string_view b) {
  return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](char x, char y) {
    return std::tolower(uint8_t(x)) == std::tolower(uint8_t(y));
  });
}

std::vector<std::filesystem::path> PickPackages(const std::string& title) {
  std::vector<std::filesystem::path> picked;
  const HRESULT init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  IFileOpenDialog* dialog = nullptr;
  if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
                                 IID_PPV_ARGS(&dialog)))) {
    DWORD options = 0;
    dialog->GetOptions(&options);
    dialog->SetOptions(options | FOS_ALLOWMULTISELECT | FOS_FILEMUSTEXIST | FOS_FORCEFILESYSTEM);
    const std::u16string utf16 = string::to_utf16(title);
    dialog->SetTitle(reinterpret_cast<const wchar_t*>(utf16.c_str()));
    if (SUCCEEDED(dialog->Show(GetForegroundWindow()))) {
      IShellItemArray* items = nullptr;
      if (SUCCEEDED(dialog->GetResults(&items))) {
        DWORD count = 0;
        items->GetCount(&count);
        for (DWORD i = 0; i < count; ++i) {
          IShellItem* item = nullptr;
          PWSTR path = nullptr;
          if (SUCCEEDED(items->GetItemAt(i, &item)) &&
              SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
            picked.emplace_back(path);
            CoTaskMemFree(path);
          }
          if (item) {
            item->Release();
          }
        }
        items->Release();
      }
    }
    dialog->Release();
  }
  if (SUCCEEDED(init)) {
    CoUninitialize();
  }
  return picked;
}

}

struct XboxGuide::DlcJob {
  std::atomic<bool> done{false};
  bool pick = false;
  std::string entry_id;
  std::string file_name;
  X_RESULT result = X_ERROR_SUCCESS;
  std::vector<std::filesystem::path> picked;
};

std::vector<XboxGuide::DlcEntry> XboxGuide::FindDlc() const {
  std::vector<DlcEntry> out;
  for (const PPCTitleDlc* d = host_.dlc; d && d->id; ++d) {
    DlcEntry entry;
    entry.id = d->id;
    entry.requires_title_update = d->requires_title_update;
    if (const DlcCatalogEntry* info = FindEmbeddedDlc(entry.id)) {
      entry.name = info->title;
      entry.publisher = info->publisher;
      entry.description = info->description;
    }
    entry.package_name = d->package_name && *d->package_name ? d->package_name : entry.name;
    if (entry.name.empty()) {
      entry.name = entry.package_name.empty() ? entry.id : entry.package_name;
    }
    out.push_back(std::move(entry));
  }
  const size_t catalogued = out.size();

  auto entry_for = [&](std::string_view display_name) -> DlcEntry* {
    for (size_t i = 0; i < catalogued; ++i) {
      if (SameName(out[i].package_name, display_name) || SameName(out[i].name, display_name)) {
        return &out[i];
      }
    }
    return nullptr;
  };

  auto* kernel = host_.kernel_state;
  const uint32_t title_id = kernel ? kernel->title_id() : 0;
  if (kernel && kernel->content_manager()) {
    for (const auto& content : kernel->content_manager()->ListContent(
             static_cast<uint32_t>(system::xam::DummyDeviceId::HDD), 0,
             system::XContentType::kMarketplaceContent, title_id)) {
      const std::string name = string::to_utf8(content.display_name());
      DlcEntry* entry = entry_for(name);
      if (!entry) {
        out.push_back({});
        entry = &out.back();
        entry->name = name;
      }
      entry->file_name = content.file_name();
      entry->installed = true;
    }
  }
  auto add_package = [&](const std::filesystem::path& path) {
    auto header = filesystem::StfsContainerDevice::ReadPackageHeader(path);
    if (!header || header->metadata.content_type != system::XContentType::kMarketplaceContent ||
        header->metadata.execution_info.title_id != title_id) {
      return;
    }
    const std::string name =
        string::to_utf8(header->metadata.display_name(system::XLanguage::kEnglish));
    const std::string file_name = path_to_utf8(path.filename());
    DlcEntry* entry = entry_for(name);
    if (!entry) {
      auto it = std::find_if(out.begin(), out.end(), [&](const DlcEntry& e) {
        return SameName(e.file_name, file_name) || (!name.empty() && SameName(e.name, name));
      });
      if (it == out.end()) {
        out.push_back({});
        it = std::prev(out.end());
        it->name = name.empty() ? file_name : name;
        it->description =
            string::to_utf8(header->metadata.description(system::XLanguage::kEnglish));
      }
      entry = &*it;
    }
    if (entry->file_name.empty()) {
      entry->file_name = file_name;
    }
    entry->package = path;
  };
  std::error_code ec;
  const std::filesystem::path folder = filesystem::GetExecutableFolder() / "DLC";
  for (auto it = std::filesystem::recursive_directory_iterator(folder, ec);
       !ec && it != std::filesystem::recursive_directory_iterator(); it.increment(ec)) {
    if (it->is_regular_file(ec)) {
      add_package(it->path());
    }
  }
  for (const auto& path : picked_packages_) {
    add_package(path);
  }
  std::stable_sort(out.begin() + catalogued, out.end(),
                   [](const DlcEntry& a, const DlcEntry& b) { return a.name < b.name; });
  return out;
}

void XboxGuide::OpenManageGame() {
  SettingsPage& page = PushPage(assets_->options_notifications, "Manage Game");
  manage_scene_ = page.scene;
  manage_rows_.clear();
  dlc_status_.clear();
  if (!dlc_banner_node_) {
    dlc_banner_node_ = std::make_unique<xui::Node>();
    dlc_banner_node_->class_name = "XuiImage";
    dlc_banner_node_->cls = xui::FindClass("XuiImage");
  }
  dlc_banner_ = manage_scene_->AttachScene(*dlc_banner_node_, skin_context_);
  dlc_banner_->Set("Id", xui::Value{std::string("imgDlcBanner")});
  dlc_banner_->Set("Position", xui::Value{xui::Vec3{kPaneX, kBannerY, 0.0f}});
  dlc_banner_->Set("Width", xui::Value{kPaneWidth});
  dlc_banner_->Set("Height", xui::Value{kBannerHeight});
  dlc_banner_->SetVisible(false);
  auto place = [&](std::string_view id, float y, float height) {
    if (xui::Element* e = manage_scene_->FindById(id)) {
      xui::Vec3 p = e->GetVector("Position");
      p.y = y;
      e->Set("Position", xui::Value{p});
      e->Set("Height", xui::Value{height});
    }
  };
  place("XuiLabel1", kDetailsY, kPaneBottom - kDetailsY);

  page.on_focus = [this] { ShowDlc(focus_); };
  page.on_select = [this](xui::Element* row) {
    if (dlc_job_) {
      return;
    }
    const auto at = std::find(manage_rows_.begin(), manage_rows_.end(), row);
    if (at == manage_rows_.end() || size_t(at - manage_rows_.begin()) >= dlc_.size()) {
      return;
    }
    const DlcEntry& entry = dlc_[size_t(at - manage_rows_.begin())];
    if (entry.installed || !host_.kernel_state) {
      return;
    }
    if (entry.requires_title_update > host_.title_update) {
      media_->PlaySound("sharedres://btn_InactiveSelect.xma", "");
      return;
    }
    auto job = std::make_shared<DlcJob>();
    dlc_job_ = job;
    if (entry.package.empty()) {
      job->pick = true;
      job->entry_id = entry.id;
      const std::string title = fmt::format("Install {}", entry.name);
      std::thread([job, title] {
        job->picked = PickPackages(title);
        job->done = true;
      }).detach();
      return;
    }
    StartDlcInstall(entry);
  };
  FillManageGame();
}

void XboxGuide::StartDlcInstall(const DlcEntry& entry) {
  auto job = dlc_job_ ? dlc_job_ : std::make_shared<DlcJob>();
  job->pick = false;
  job->done = false;
  job->file_name = entry.file_name;
  dlc_job_ = job;
  dlc_status_ = fmt::format("Installing {}...", entry.name);
  ShowDlc(focus_);
  auto* content = host_.kernel_state->content_manager();
  const std::filesystem::path package = entry.package;

  std::thread([job, content, package] {
    job->result = content->InstallContent(package);
    job->done = true;
  }).detach();
}

void XboxGuide::FillManageGame() {
  xui::Element* scene = manage_scene_;
  xui::Element* model = scene->FindById("chkShow");
  if (!model) {
    return;
  }
  for (xui::Element* row : manage_rows_) {
    if (row == focus_) {
      focus_ = nullptr;
    }
    row->parent()->RemoveChild(row);
  }
  manage_rows_.clear();
  dlc_ = FindDlc();

  const float top = model->GetVector("Position").y;
  const size_t rows = std::min(dlc_.size(), kMaxRows);
  for (size_t i = 0; i < rows; ++i) {
    const DlcEntry& entry = dlc_[i];

    xui::Element* row = scene->CloneChild(*model, fmt::format("btnDlc{}", i), "btn_Count");
    xui::Vec3 p = model->GetVector("Position");
    p.y = top + float(i) * kRowHeight;
    row->Set("Position", xui::Value{p});
    row->SetText(entry.name);
    std::string action = "Install";
    if (entry.installed) {
      action = "Installed";
    } else if (entry.requires_title_update > host_.title_update) {
      action = "Needs Update";
    }
    row->SetSecondaryText(std::move(action));
    row->SetVisible(true);
    manage_rows_.push_back(row);
  }
  for (size_t i = 0; i < manage_rows_.size(); ++i) {
    manage_rows_[i]->Set(
        "NavUp", xui::Value{i > 0 ? std::string(manage_rows_[i - 1]->id()) : std::string()});
    manage_rows_[i]->Set(
        "NavDown", xui::Value{i + 1 < manage_rows_.size() ? std::string(manage_rows_[i + 1]->id())
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
  if (manage_rows_.empty()) {
    if (xui::Element* details = scene->FindById("XuiLabel1")) {
      details->SetText("There's no downloadable content for this game.");
    }
    SetLegends("", manage_scene_->GetString("LegendB"), "");
    return;
  }
  SetFocus(manage_rows_.front(), true);
  ShowDlc(manage_rows_.front());
}

void XboxGuide::ShowDlc(xui::Element* row) {
  if (!manage_scene_ || !row) {
    return;
  }
  auto set_text = [&](std::string_view id, std::string text) {
    if (xui::Element* e = manage_scene_->FindById(id)) {
      e->SetText(std::move(text));
    }
  };
  const auto at = std::find(manage_rows_.begin(), manage_rows_.end(), row);
  if (at == manage_rows_.end() || size_t(at - manage_rows_.begin()) >= dlc_.size()) {
    return;
  }
  const DlcEntry& entry = dlc_[size_t(at - manage_rows_.begin())];
  const DlcCatalogEntry* info = entry.id.empty() ? nullptr : FindEmbeddedDlc(entry.id);
  if (dlc_banner_) {
    const bool banner = info && !info->banner.empty();
    dlc_banner_->SetVisible(banner);
    if (banner) {
      dlc_banner_->Set("ImagePath", xui::Value{fmt::format("dlc://{}/banner", entry.id)});
    }
  }
  std::string status = dlc_status_;
  std::string action;
  if (status.empty()) {
    if (entry.installed) {
      status = "Installed. The game may need a restart to use it.";
    } else if (entry.requires_title_update > host_.title_update) {
      status = fmt::format("Install title update {} first.", entry.requires_title_update);
    } else {
      if (!entry.package.empty()) {
        status = "Ready to install.";
      }
      action = "Install";
    }
  }

  std::string details = entry.name;
  if (!entry.publisher.empty()) {
    details += "\r\n" + entry.publisher;
  }
  if (!status.empty()) {
    details += "\r\n" + status;
  }
  if (!entry.description.empty()) {
    details += "\r\n\r\n" + entry.description;
  }
  set_text("XuiLabel1", std::move(details));
  SetLegends(dlc_job_ ? "" : action, manage_scene_->GetString("LegendB"), "");
}

void XboxGuide::PollManageGame() {
  if (!dlc_job_ || !dlc_job_->done) {
    return;
  }
  const auto job = dlc_job_;
  if (!manage_scene_ || pages_.empty() || pages_.back().scene != manage_scene_) {
    dlc_job_.reset();
    return;
  }
  if (job->pick) {
    picked_packages_.insert(picked_packages_.end(), job->picked.begin(), job->picked.end());
    FillManageGame();
    auto it = std::find_if(dlc_.begin(), dlc_.end(),
                           [&](const DlcEntry& e) { return e.id == job->entry_id; });
    if (it != dlc_.end() && !it->installed && !it->package.empty()) {
      if (size_t(it - dlc_.begin()) < manage_rows_.size()) {
        SetFocus(manage_rows_[size_t(it - dlc_.begin())]);
      }
      StartDlcInstall(*it);
      return;
    }
    dlc_job_.reset();
    if (!job->picked.empty() && it != dlc_.end()) {
      dlc_status_ = fmt::format("The file chosen is not {} for this game.", it->name);
      ShowDlc(focus_);
      dlc_status_.clear();
    }
    return;
  }
  dlc_job_.reset();
  if (job->result == X_ERROR_SUCCESS) {
    REXLOG_INFO("Xbox guide: installed {}", job->file_name);
    dlc_status_ = "Installed. The game may need a restart to use it.";
    media_->PlaySound("btn_selectG.xma", "xam/skin");
  } else {
    REXLOG_WARN("Xbox guide: installing {} failed: {:08X}", job->file_name, job->result);
    dlc_status_ = fmt::format("The content could not be installed (error {:08X}).", job->result);
  }
  xui::Element* focused = focus_;
  const size_t index =
      size_t(std::find(manage_rows_.begin(), manage_rows_.end(), focused) - manage_rows_.begin());
  const std::string keep = dlc_status_;
  FillManageGame();
  if (index < manage_rows_.size()) {
    SetFocus(manage_rows_[index]);
  }
  dlc_status_ = keep;
  ShowDlc(focus_);
  dlc_status_.clear();
}

}
