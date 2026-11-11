/**
 * @file        ui/guide/src/app_update.cpp
 * @brief       Updates of the recompiled game itself from its GitHub releases
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/guide/app_update.h>

#include <algorithm>
#include <atomic>
#include <charconv>
#include <chrono>
#include <fstream>
#include <iterator>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

#include <fmt/format.h>

#include <rex/logging.h>
#include <rex/system/release_update.h>
#include <rex/ui/guide/title_update.h>

namespace rex::ui::guide {
namespace {

namespace fs = std::filesystem;
namespace update = rex::system::update;

constexpr auto kCheckInterval = std::chrono::hours(24);

struct Updater {
  std::mutex mutex;
  AppUpdateConfig config;
  AppUpdateState state;
  std::optional<update::AvailableUpdate> available;
  fs::path package_root;
  bool busy = false;
  bool notice_pending = false;
  std::string noticed_version;
};

Updater& Instance() {
  static Updater updater;
  return updater;
}

fs::path UpdatesFolder(const AppUpdateConfig& config) {
  return config.local_dir / "updates";
}

std::string ReadLine(const fs::path& path) {
  std::ifstream stream(path);
  std::string line;
  std::getline(stream, line);
  return line;
}

void WriteLine(const fs::path& path, std::string_view line) {
  std::error_code code;
  fs::create_directories(path.parent_path(), code);
  std::ofstream(path) << line << "\n";
}

int64_t NowSeconds() {
  return std::chrono::duration_cast<std::chrono::seconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

bool CheckedRecently(const AppUpdateConfig& config) {
  const std::string last = ReadLine(UpdatesFolder(config) / "last-check.txt");
  int64_t checked_at = 0;
  if (std::from_chars(last.data(), last.data() + last.size(), checked_at).ec != std::errc()) {
    return false;
  }
  const int64_t elapsed = NowSeconds() - checked_at;
  return elapsed >= 0 && elapsed < std::chrono::seconds(kCheckInterval).count();
}

bool CanRollBack(const AppUpdateConfig& config) {
  std::error_code code;
  return fs::is_directory(config.install_folder / "previous", code);
}

void SetFailed(Updater& updater, std::string message, std::string_view detail) {
  std::lock_guard lock(updater.mutex);
  REXLOG_WARN("Game update: {} ({})", message, detail);
  updater.state.status = AppUpdateStatus::kFailed;
  updater.state.error = std::move(message);
  updater.busy = false;
}

std::optional<std::string> ReadCachedReleases(const AppUpdateConfig& config) {
  std::ifstream stream(UpdatesFolder(config) / "releases.json", std::ios::binary);
  if (!stream) {
    return std::nullopt;
  }
  return std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
}

void RunCheck(AppUpdateConfig config, bool use_cache) {
  Updater& updater = Instance();
  const auto current = update::ReleaseVersion::Parse(config.current_version);
  std::string error;
  std::optional<std::string> json = use_cache ? ReadCachedReleases(config) : std::nullopt;
  if (!json) {
    json = FetchText(
        fmt::format("https://api.github.com/repos/{}/releases?per_page=30", config.repository),
        &error);
    if (!json) {
      SetFailed(updater,
                "Unable to connect to the update service. Check your network connection and "
                "try again.",
                "GitHub: " + error);
      return;
    }
    WriteLine(UpdatesFolder(config) / "last-check.txt", std::to_string(NowSeconds()));
    std::ofstream(UpdatesFolder(config) / "releases.json", std::ios::binary) << *json;
  }
  const auto found = update::FindUpdate(update::ParseGitHubReleases(*json), *current,
                                        config.asset_pattern, !current->prerelease.empty());
  const std::string skipped = ReadLine(UpdatesFolder(config) / "skipped-version.txt");
  std::lock_guard lock(updater.mutex);
  updater.busy = false;
  updater.available = found;
  if (!found) {
    updater.state.status = AppUpdateStatus::kUpToDate;
    return;
  }
  updater.state.status = AppUpdateStatus::kAvailable;
  updater.state.latest_version = found->release.version.ToString();
  updater.state.notes = PlainReleaseNotes(found->release.notes);
  updater.state.download_size = found->package.size;
  updater.state.skipped = skipped == updater.state.latest_version;
  if (!updater.state.skipped && updater.noticed_version != updater.state.latest_version) {
    updater.noticed_version = updater.state.latest_version;
    updater.notice_pending = true;
  }
  REXLOG_INFO("Game update: {} is available (running {})", updater.state.latest_version,
              config.current_version);
}

void RunDownload(AppUpdateConfig config, update::AvailableUpdate available) {
  Updater& updater = Instance();
  const std::string version = available.release.version.ToString();
  const fs::path folder = UpdatesFolder(config) / version;
  const fs::path zip = folder / available.package.name;
  std::error_code code;
  fs::remove_all(folder, code);
  fs::create_directories(folder, code);
  std::atomic<bool> cancel = false;
  std::string error = DownloadFile(
      available.package.download_url, zip,
      [&updater](uint64_t done, uint64_t total) {
        std::lock_guard lock(updater.mutex);
        updater.state.downloaded = done;
        if (total) {
          updater.state.download_size = total;
        }
      },
      cancel);
  if (!error.empty()) {
    SetFailed(updater, "The download didn't finish. Check your network connection and try again.",
              error);
    return;
  }
  if (!available.checksums) {
    SetFailed(updater, "This update can't be checked, so it won't be installed.",
              "the release has no SHA256SUMS.txt");
    return;
  }
  const auto sums = FetchText(available.checksums->download_url, &error);
  if (!sums || !update::MatchesChecksum(zip, update::ParseChecksums(*sums))) {
    SetFailed(updater, "This update didn't download correctly. Try again.",
              "the download doesn't match the release's checksum");
    return;
  }
  if (!update::UnpackZip(zip, folder / "unpacked", &error)) {
    SetFailed(updater, "This update couldn't be unpacked. Try again.", error);
    return;
  }
  std::lock_guard lock(updater.mutex);
  updater.package_root = update::FindPackageRoot(folder / "unpacked");
  updater.state.status = AppUpdateStatus::kReady;
  updater.busy = false;
  REXLOG_INFO("Game update: {} downloaded and checked", version);
}

bool StartHelper(bool rollback, std::string* error) {
  Updater& updater = Instance();
  update::UpdateHelperLaunch launch;
  {
    std::lock_guard lock(updater.mutex);
    launch.helper = updater.config.install_folder / "rexglue-updater.exe";
    launch.install_folder = updater.config.install_folder;
    launch.package_root = updater.package_root;
    launch.previous_folder = updater.config.install_folder / "previous";
    launch.relaunch = updater.config.executable;
    launch.rollback = rollback;
  }
  return update::StartUpdateHelper(launch, UpdatesFolder(updater.config) / "rexglue-updater.exe",
                                   error);
}

std::string_view Trimmed(std::string_view text) {
  const size_t first = text.find_first_not_of(" \t");
  if (first == std::string_view::npos) {
    return {};
  }
  return text.substr(first, text.find_last_not_of(" \t") - first + 1);
}

bool StartsListItem(std::string_view text) {
  if (text.starts_with("- ") || text.starts_with("* ") || text.starts_with("+ ")) {
    return true;
  }
  size_t digits = 0;
  while (digits < text.size() && text[digits] >= '0' && text[digits] <= '9') {
    ++digits;
  }
  return digits > 0 && text.substr(digits).starts_with(". ");
}

std::string InlineText(std::string_view text) {
  std::string plain;
  for (size_t i = 0; i < text.size(); ++i) {
    const char c = text[i];
    if (c == '[') {
      const size_t middle = text.find("](", i);
      const size_t end = middle == std::string_view::npos ? middle : text.find(')', middle);
      if (end != std::string_view::npos) {
        plain += text.substr(i + 1, middle - i - 1);
        i = end;
        continue;
      }
    }
    if (c == '*' || c == '`') {
      continue;
    }
    plain += c;
  }
  return plain;
}

}

std::string PlainReleaseNotes(std::string_view markdown) {
  if (markdown.starts_with("\xEF\xBB\xBF")) {
    markdown.remove_prefix(3);
  }
  std::vector<std::string> lines;
  bool joins_previous = false;
  size_t start = 0;
  while (start <= markdown.size()) {
    size_t end = markdown.find('\n', start);
    if (end == std::string_view::npos) {
      end = markdown.size();
    }
    std::string_view text = Trimmed(markdown.substr(start, end - start));
    start = end + 1;
    if (text.ends_with('\r')) {
      text = Trimmed(text.substr(0, text.size() - 1));
    }
    if (text.empty()) {
      if (!lines.empty() && !lines.back().empty()) {
        lines.emplace_back();
      }
      joins_previous = false;
    } else if (text.starts_with('#')) {
      lines.push_back(
          InlineText(Trimmed(text.substr(std::min(text.find_first_not_of('#'), text.size())))));
      joins_previous = false;
    } else if (StartsListItem(text)) {
      const bool bullet = text[0] == '-' || text[0] == '*' || text[0] == '+';
      lines.push_back(bullet ? "- " + InlineText(Trimmed(text.substr(2))) : InlineText(text));
      joins_previous = true;
    } else if (joins_previous) {
      lines.back() += ' ' + InlineText(text);
    } else {
      lines.push_back(InlineText(text));
      joins_previous = true;
    }
  }
  while (!lines.empty() && lines.back().empty()) {
    lines.pop_back();
  }
  std::string plain;
  for (const std::string& line : lines) {
    plain += (plain.empty() ? "" : "\r\n") + line;
  }
  return plain;
}

void ConfigureAppUpdate(const AppUpdateConfig& config) {
  Updater& updater = Instance();
  std::lock_guard lock(updater.mutex);
  updater.config = config;
  const bool usable = !config.repository.empty() && !config.asset_pattern.empty() &&
                      update::ReleaseVersion::Parse(config.current_version).has_value();
  updater.state = {};
  updater.state.status = usable ? AppUpdateStatus::kUpToDate : AppUpdateStatus::kOff;
  updater.state.current_version = config.current_version;
  updater.state.can_roll_back = usable && CanRollBack(config);
}

void CheckForAppUpdate(bool force) {
  Updater& updater = Instance();
  std::lock_guard lock(updater.mutex);
  if (updater.state.status == AppUpdateStatus::kOff || updater.busy) {
    return;
  }
  updater.busy = true;
  updater.state.status = AppUpdateStatus::kChecking;
  updater.state.error.clear();
  std::thread(RunCheck, updater.config, !force && CheckedRecently(updater.config)).detach();
}

bool TakeAppUpdateNotice(std::string* version) {
  Updater& updater = Instance();
  std::lock_guard lock(updater.mutex);
  if (!updater.notice_pending) {
    return false;
  }
  updater.notice_pending = false;
  *version = updater.state.latest_version;
  return true;
}

AppUpdateState GetAppUpdateState() {
  Updater& updater = Instance();
  std::lock_guard lock(updater.mutex);
  return updater.state;
}

void DownloadAppUpdate() {
  Updater& updater = Instance();
  std::lock_guard lock(updater.mutex);
  if (updater.busy || !updater.available) {
    return;
  }
  updater.busy = true;
  updater.state.status = AppUpdateStatus::kDownloading;
  updater.state.downloaded = 0;
  updater.state.error.clear();
  std::thread(RunDownload, updater.config, *updater.available).detach();
}

void SkipAppUpdate() {
  Updater& updater = Instance();
  std::lock_guard lock(updater.mutex);
  if (updater.state.latest_version.empty()) {
    return;
  }
  WriteLine(UpdatesFolder(updater.config) / "skipped-version.txt", updater.state.latest_version);
  updater.state.skipped = true;
}

bool InstallAppUpdate(std::string* error) {
  if (GetAppUpdateState().status != AppUpdateStatus::kReady) {
    if (error) {
      *error = "The update hasn't been downloaded yet.";
    }
    return false;
  }
  return StartHelper(false, error);
}

bool RollBackAppUpdate(std::string* error) {
  if (!GetAppUpdateState().can_roll_back) {
    if (error) {
      *error = "There's no previous version to go back to.";
    }
    return false;
  }
  return StartHelper(true, error);
}

}
