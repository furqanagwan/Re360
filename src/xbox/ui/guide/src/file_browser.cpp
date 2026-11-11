// Copyright (c) 2026 Xbox Guide contributors. BSD 3-Clause License; see LICENSE.
#include <rex/ui/guide/file_browser.h>

#include <algorithm>
#include <cwctype>

#include <fmt/format.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace rex::ui::guide {
namespace {

std::string Utf8(const std::filesystem::path& path) {
  const auto text = path.u8string();
  return std::string(text.begin(), text.end());
}

std::wstring Lower(std::wstring text) {
  std::transform(text.begin(), text.end(), text.begin(),
                 [](wchar_t c) { return wchar_t(std::towlower(c)); });
  return text;
}

bool Hidden(const std::filesystem::path& path) {
#ifdef _WIN32
  const DWORD attributes = GetFileAttributesW(path.c_str());
  return attributes != INVALID_FILE_ATTRIBUTES &&
         (attributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM));
#else
  const auto name = path.filename().native();
  return !name.empty() && name.front() == '.';
#endif
}

std::vector<std::filesystem::path> LocalDrives() {
  std::vector<std::filesystem::path> drives;
#ifdef _WIN32
  const DWORD present = GetLogicalDrives();
  for (int i = 0; i < 26; ++i) {
    if (!(present & (1u << i)))
      continue;
    const std::wstring root{wchar_t(L'A' + i), L':', L'\\'};
    const UINT type = GetDriveTypeW(root.c_str());
    if (type == DRIVE_FIXED || type == DRIVE_REMOVABLE || type == DRIVE_REMOTE)
      drives.emplace_back(root);
  }
#else
  drives.emplace_back("/");
#endif
  return drives;
}

// "Local Disk (C:)" as Explorer names a drive, or "C:" without a label.
std::string DriveName(const std::filesystem::path& drive) {
  std::string label;
#ifdef _WIN32
  wchar_t name[MAX_PATH + 1] = {};
  if (GetVolumeInformationW(drive.c_str(), name, MAX_PATH + 1, nullptr, nullptr, nullptr, nullptr,
                            0))
    label = Utf8(std::filesystem::path(name));
#endif
  const auto letter = Utf8(drive.root_name().empty() ? drive : drive.root_name());
  return label.empty() ? letter : fmt::format("{} ({})", label, letter);
}

}  // namespace

std::string FormatGuideSize(uint64_t bytes) {
  constexpr double kKilobyte = 1024.0;
  constexpr double kMegabyte = kKilobyte * 1024.0;
  constexpr double kGigabyte = kMegabyte * 1024.0;
  if (double(bytes) >= kGigabyte)
    return fmt::format("{:.1f} GB", double(bytes) / kGigabyte);
  if (double(bytes) >= kMegabyte)
    return fmt::format("{:.1f} MB", double(bytes) / kMegabyte);
  return fmt::format("{} KB", std::max<uint64_t>(1, (bytes + 1023) / 1024));
}

GuideFileBrowser::GuideFileBrowser(Pick pick, std::vector<std::wstring> extensions)
    : list_drives(LocalDrives), pick_(pick) {
  for (auto& extension : extensions)
    extensions_.push_back(Lower(std::move(extension)));
}

void GuideFileBrowser::Open(std::filesystem::path folder, const std::filesystem::path& focus) {
  folder_ = std::move(folder);
  entries_.clear();
  rows_.clear();
  if (use_folder_row())
    rows_.push_back(
        {"Use This Folder", {}, Utf8(folder_) + "\r\n\r\nUse the files in this folder."});
  if (folder_.empty()) {
    for (auto& drive : list_drives ? list_drives() : std::vector<std::filesystem::path>{}) {
      rows_.push_back({DriveName(drive), "Drive", Utf8(drive)});
      entries_.push_back(std::move(drive));
    }
  } else {
    struct Entry {
      std::filesystem::path path;
      bool folder;
      uint64_t size;
    };
    std::vector<Entry> found;
    std::error_code ec;
    for (std::filesystem::directory_iterator it(folder_, ec), end;
         !ec && it != end && found.size() < kMaxEntries; it.increment(ec)) {
      if (Hidden(it->path()))
        continue;
      std::error_code entry_ec;
      if (it->is_directory(entry_ec)) {
        found.push_back({it->path(), true, 0});
      } else if (pick_ == Pick::kFile && it->is_regular_file(entry_ec) &&
                 std::find(extensions_.begin(), extensions_.end(),
                           Lower(it->path().extension().wstring())) != extensions_.end()) {
        found.push_back({it->path(), false, it->file_size(entry_ec)});
      }
    }
    std::sort(found.begin(), found.end(), [](const Entry& left, const Entry& right) {
      if (left.folder != right.folder)
        return left.folder;
      return Lower(left.path.filename().wstring()) < Lower(right.path.filename().wstring());
    });
    for (auto& entry : found) {
      rows_.push_back({Utf8(entry.path.filename()),
                       entry.folder ? "Folder" : FormatGuideSize(entry.size), Utf8(entry.path)});
      entries_.push_back(std::move(entry.path));
    }
  }
  focus_ = 0;
  for (size_t i = 0; i < entries_.size(); ++i)
    if (!focus.empty() && entries_[i] == focus)
      focus_ = i + (use_folder_row() ? 1 : 0);
}

GuideFileBrowser::Result GuideFileBrowser::Select(size_t row) {
  if (row >= rows_.size())
    return {};
  if (use_folder_row() && row == 0)
    return {Result::Kind::kChosen, folder_};
  const auto& path = entries_[row - (use_folder_row() ? 1 : 0)];
  std::error_code ec;
  if (folder_.empty() || std::filesystem::is_directory(path, ec)) {
    Open(path);
    return {Result::Kind::kOpened, folder_};
  }
  return {Result::Kind::kChosen, path};
}

bool GuideFileBrowser::Up() {
  if (folder_.empty())
    return false;
  const auto from = folder_;
  const auto parent = folder_.parent_path();
  // A drive root is its own parent: the drives come next.
  if (parent == folder_ || parent.empty())
    Open({}, from.root_path().empty() ? from : from.root_path());
  else
    Open(parent, from);
  return true;
}

std::string GuideFileBrowser::empty_details() const {
  if (folder_.empty())
    return "No drives are available.";
  return Utf8(folder_) + (pick_ == Pick::kFolder ? "\r\n\r\nThis folder has no folders."
                                                 : "\r\n\r\nThis folder has no matching files.");
}

}  // namespace rex::ui::guide
