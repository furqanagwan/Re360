/**
 * @file        ui/guide/include/rex/ui/guide/title_update.h
 * @brief       Optional title updates: packages, sources, install and launch (RG-GDK-057)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <rex/cvar.h>
#include <rex/image_info.h>

REXCVAR_DECLARE(int32_t, title_update);

REXCVAR_DECLARE(bool, title_update_handoff);

namespace rex::ui::guide {

constexpr uint32_t kTitleUpdateContentType = 0x000B0000;

struct TitleUpdatePackage {
  uint32_t content_type = 0;
  uint32_t title_id = 0;
  uint32_t media_id = 0;
  uint32_t base_version = 0;

  std::array<uint8_t, 20> content_id{};
  bool content_id_valid = false;
  std::string display_name;
};

std::optional<TitleUpdatePackage> ReadTitleUpdatePackage(std::span<const uint8_t> head,
                                                         std::string* error);
std::optional<TitleUpdatePackage> ReadTitleUpdatePackage(const std::filesystem::path& path,
                                                         std::string* error);

std::string HexString(std::span<const uint8_t> bytes);

std::string CheckTitleUpdatePackage(const TitleUpdatePackage& package,
                                    const PPCTitleUpdate& expected, uint32_t title_id);

std::filesystem::path TitleUpdateFolder(const std::filesystem::path& local_dir, uint32_t version);

std::filesystem::path FindInstalledTitleUpdate(const std::filesystem::path& local_dir,
                                               uint32_t version);

std::string InstallTitleUpdate(const std::filesystem::path& package_file,
                               const PPCTitleUpdate& expected, uint32_t title_id,
                               const std::filesystem::path& local_dir);

using DownloadProgress = std::function<void(uint64_t done, uint64_t total)>;

std::string DownloadFile(const std::string& url, const std::filesystem::path& destination,
                         const DownloadProgress& progress, const std::atomic<bool>& cancel);

std::optional<std::string> FetchText(const std::string& url, std::string* error);

struct TitleUpdateSource {
  std::string name;
  std::function<std::optional<std::string>(const PPCTitleUpdate& update, uint32_t title_id,
                                           std::string* error)>
      find_url;
};

TitleUpdateSource XboxUnitySource();

std::optional<std::string> FindXboxUnityUpdateId(std::string_view info_json,
                                                 std::string_view content_id);

std::vector<TitleUpdateSource> TitleUpdateSources();

bool DownloadTitleUpdate(const PPCTitleUpdate& update, uint32_t title_id,
                         const std::filesystem::path& local_dir,
                         const std::vector<TitleUpdateSource>& sources,
                         const DownloadProgress& progress, const std::atomic<bool>& cancel,
                         std::vector<std::string>* errors);

struct TitleUpdateJob {
  enum class State { kRunning, kInstalled, kFailed, kCancelled };
  PPCTitleUpdate update{};
  bool from_file = false;
  std::atomic<uint64_t> done{0};
  std::atomic<uint64_t> total{0};
  std::atomic<State> state{State::kRunning};
  std::atomic<bool> cancel{false};

  std::vector<std::string> errors;
};

std::shared_ptr<TitleUpdateJob> StartTitleUpdateDownload(const PPCTitleUpdate& update,
                                                         uint32_t title_id,
                                                         const std::filesystem::path& local_dir);

std::shared_ptr<TitleUpdateJob> StartTitleUpdateInstall(const PPCTitleUpdate& update,
                                                        uint32_t title_id,
                                                        const std::filesystem::path& local_dir,
                                                        const std::filesystem::path& package_file);

std::shared_ptr<TitleUpdateJob> FindTitleUpdateJob(uint32_t version);

std::vector<std::shared_ptr<TitleUpdateJob>> TitleUpdateJobs();

struct LaunchChoice {
  bool hand_off = false;
  std::filesystem::path executable;
  std::filesystem::path update;
  std::string note;
  bool cannot_run = false;
};
LaunchChoice ChooseLaunch(uint32_t built, uint32_t wanted, const std::filesystem::path& exe_path,
                          const std::filesystem::path& local_dir, bool handed_off);

std::filesystem::path TitleUpdateExecutable(const std::filesystem::path& exe_path, uint32_t built,
                                            uint32_t version);

}
