/**
 * @file        ui/guide/tests/app_update_test.cpp
 * @brief       The game update service without the network
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <chrono>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <string_view>
#include <thread>

#include <catch2/catch_test_macros.hpp>

#include <rex/ui/guide/app_update.h>

using namespace rex::ui::guide;

namespace {

struct TemporaryFolder {
  std::filesystem::path path = std::filesystem::temp_directory_path() /
                               ("rex_app_update_" + std::to_string(std::random_device()()));
  TemporaryFolder() { std::filesystem::create_directories(path); }
  ~TemporaryFolder() {
    std::error_code code;
    std::filesystem::remove_all(path, code);
  }
};

AppUpdateConfig Config(const TemporaryFolder& folder, std::string version) {
  return {std::move(version),    "furqanagwan/007",       "007-QuantumOfSolace-*-win-x64.zip",
          folder.path / "local", folder.path / "install", folder.path / "install" / "game.exe"};
}

}

TEST_CASE("Game updates are off without a version or a repository", "[guide][app_update]") {
  TemporaryFolder folder;
  ConfigureAppUpdate(Config(folder, ""));
  CHECK(GetAppUpdateState().status == AppUpdateStatus::kOff);
  AppUpdateConfig no_repository = Config(folder, "0.1.0-alpha.1");
  no_repository.repository.clear();
  ConfigureAppUpdate(no_repository);
  CHECK(GetAppUpdateState().status == AppUpdateStatus::kOff);
  CheckForAppUpdate(true);
  CHECK(GetAppUpdateState().status == AppUpdateStatus::kOff);
}

TEST_CASE("A configured game reports its version and whether it can go back",
          "[guide][app_update]") {
  TemporaryFolder folder;
  ConfigureAppUpdate(Config(folder, "0.1.0-alpha.1"));
  AppUpdateState state = GetAppUpdateState();
  CHECK(state.status == AppUpdateStatus::kUpToDate);
  CHECK(state.current_version == "0.1.0-alpha.1");
  CHECK_FALSE(state.can_roll_back);

  std::filesystem::create_directories(folder.path / "install" / "previous");
  ConfigureAppUpdate(Config(folder, "0.1.0-alpha.1"));
  CHECK(GetAppUpdateState().can_roll_back);
}

TEST_CASE("Release notes are shown as plain text", "[guide][app_update]") {
  const std::string markdown =
      "\xEF\xBB\xBF"
      "First public alpha. Expect bugs; your saves are kept outside the game folder,\r\n"
      "so updating never touches them.\r\n"
      "\r\n"
      "### All games\r\n"
      "\r\n"
      "- Native builds, made with\r\n"
      "  [ReXGlue](https://github.com/furqanagwan/rexglue-sdk).\r\n"
      "* **Bold** and `code` lose their marks.\r\n"
      "1. Numbered items stay.\r\n"
      "\r\n"
      "#\r\n";
  CHECK(PlainReleaseNotes(markdown) ==
        "First public alpha. Expect bugs; your saves are kept outside the game folder, so "
        "updating never touches them.\r\n"
        "\r\n"
        "All games\r\n"
        "\r\n"
        "- Native builds, made with ReXGlue.\r\n"
        "- Bold and code lose their marks.\r\n"
        "1. Numbered items stay.");
  CHECK(PlainReleaseNotes("") == "");
  CHECK(PlainReleaseNotes("A [link without a target") == "A [link without a target");
}

TEST_CASE("Installing needs a downloaded update", "[guide][app_update]") {
  TemporaryFolder folder;
  ConfigureAppUpdate(Config(folder, "0.1.0-alpha.1"));
  std::string error;
  CHECK_FALSE(InstallAppUpdate(&error));
  CHECK_FALSE(error.empty());
  error.clear();
  CHECK_FALSE(RollBackAppUpdate(&error));
  CHECK_FALSE(error.empty());
  SkipAppUpdate();
  CHECK_FALSE(GetAppUpdateState().skipped);
}

namespace {

void WriteFile(const std::filesystem::path& path, std::string_view text) {
  std::filesystem::create_directories(path.parent_path());
  std::ofstream(path, std::ios::binary) << text;
}

AppUpdateState CheckFromCache() {
  CheckForAppUpdate(false);
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
  while (GetAppUpdateState().status == AppUpdateStatus::kChecking &&
         std::chrono::steady_clock::now() < deadline) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  return GetAppUpdateState();
}

}

TEST_CASE("A newer release is announced once, and not again after it is skipped",
          "[guide][app_update]") {
  TemporaryFolder folder;
  const auto updates = folder.path / "local" / "updates";
  WriteFile(updates / "releases.json",
            R"([{"tag_name": "v0.1.0-alpha.2", "prerelease": true, "body": "Notes",
                 "assets": [{"name": "007-QuantumOfSolace-0.1.0-alpha.2-win-x64.zip",
                             "browser_download_url": "https://example.invalid/a.zip",
                             "size": 100}]}])");
  WriteFile(updates / "last-check.txt",
            std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                               std::chrono::system_clock::now().time_since_epoch())
                               .count()));
  ConfigureAppUpdate(Config(folder, "0.1.0-alpha.1"));

  REQUIRE(CheckFromCache().status == AppUpdateStatus::kAvailable);
  std::string version;
  REQUIRE(TakeAppUpdateNotice(&version));
  CHECK(version == "0.1.0-alpha.2");
  CHECK_FALSE(TakeAppUpdateNotice(&version));

  REQUIRE(CheckFromCache().status == AppUpdateStatus::kAvailable);
  CHECK_FALSE(TakeAppUpdateNotice(&version));

  SkipAppUpdate();
  ConfigureAppUpdate(Config(folder, "0.1.0-alpha.1"));
  REQUIRE(CheckFromCache().skipped);
  CHECK_FALSE(TakeAppUpdateNotice(&version));
}
