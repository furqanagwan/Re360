/**
 * @file        ui/guide/include/rex/ui/guide/app_update.h
 * @brief       Updates of the recompiled game itself from its GitHub releases
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

namespace rex::ui::guide {

struct AppUpdateConfig {
  std::string current_version;
  std::string repository;
  std::string asset_pattern;
  std::filesystem::path local_dir;
  std::filesystem::path install_folder;
  std::filesystem::path executable;
};

enum class AppUpdateStatus {
  kOff,
  kChecking,
  kUpToDate,
  kAvailable,
  kDownloading,
  kReady,
  kFailed,
};

struct AppUpdateState {
  AppUpdateStatus status = AppUpdateStatus::kOff;
  std::string current_version;
  std::string latest_version;
  std::string notes;
  std::string error;
  uint64_t downloaded = 0;
  uint64_t download_size = 0;
  bool skipped = false;
  bool can_roll_back = false;
};

std::string PlainReleaseNotes(std::string_view markdown);

void ConfigureAppUpdate(const AppUpdateConfig& config);
void CheckForAppUpdate(bool force);
AppUpdateState GetAppUpdateState();
void DownloadAppUpdate();
void SkipAppUpdate();
bool InstallAppUpdate(std::string* error);
bool RollBackAppUpdate(std::string* error);
bool TakeAppUpdateNotice(std::string* version);

}
