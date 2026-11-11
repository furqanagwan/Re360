/**
 * @file        ui/guide/tests/title_update_test.cpp
 * @brief       Optional title updates: packages, sources, install and launch (RG-GDK-057)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <rex/cvar.h>
#include <rex/ui/guide/title_update.h>

#include "crypto/TinySHA1.hpp"

namespace fs = std::filesystem;
using namespace rex::ui::guide;
using rex::PPCTitleUpdate;

namespace {

void Put32(std::vector<uint8_t>& b, size_t at, uint32_t v) {
  b[at] = uint8_t(v >> 24);
  b[at + 1] = uint8_t(v >> 16);
  b[at + 2] = uint8_t(v >> 8);
  b[at + 3] = uint8_t(v);
}

std::vector<uint8_t> MakePackage(uint32_t content_type = kTitleUpdateContentType) {
  std::vector<uint8_t> b(0xB000 + 0x1000, 0);
  std::copy_n("LIVE", 4, b.begin());
  Put32(b, 0x340, 0xAD0E);
  Put32(b, 0x344, content_type);
  Put32(b, 0x354, 0x06DD88A0);
  Put32(b, 0x358, 7);
  Put32(b, 0x35C, 7);
  Put32(b, 0x360, 0x415607FF);
  const char16_t name[] = u"Quantum of Solace";
  for (size_t i = 0; name[i]; ++i) {
    b[0x411 + i * 2 + 1] = uint8_t(name[i]);
  }
  for (size_t i = 0x1000; i < b.size(); ++i) {
    b[i] = uint8_t(i * 31);
  }
  sha1::SHA1 hash;
  hash.processBytes(&b[0x344], 0xB000 - 0x344);
  hash.finalize(&b[0x32C]);
  return b;
}

std::string ContentId(const std::vector<uint8_t>& b) {
  return HexString(std::span(&b[0x32C], 20));
}

PPCTitleUpdate Expected(const std::string& content_id) {
  return {2, "06DD88A0", 7, content_id.c_str(), 2420, "2012-06-18", "Fixes."};
}

struct TempDir {
  fs::path path;
  explicit TempDir(const std::string& name) : path(fs::temp_directory_path() / name) {
    fs::remove_all(path);
    fs::create_directories(path);
  }
  ~TempDir() {
    std::error_code ec;
    fs::remove_all(path, ec);
  }
};

void WriteFile(const fs::path& path, const std::vector<uint8_t>& bytes) {
  fs::create_directories(path.parent_path());
  std::ofstream(path, std::ios::binary)
      .write(reinterpret_cast<const char*>(bytes.data()), std::streamsize(bytes.size()));
}

}

TEST_CASE("A title update package's header is read and its content ID checked",
          "[guide][title_update]") {
  auto bytes = MakePackage();
  std::string error;
  auto package = ReadTitleUpdatePackage(bytes, &error);
  REQUIRE(package);
  CHECK(package->content_type == kTitleUpdateContentType);
  CHECK(package->title_id == 0x415607FF);
  CHECK(package->media_id == 0x06DD88A0);
  CHECK(package->base_version == 7);
  CHECK(package->display_name == "Quantum of Solace");
  CHECK(package->content_id_valid);

  bytes[0x9000] ^= 1;
  package = ReadTitleUpdatePackage(bytes, &error);
  REQUIRE(package);
  CHECK_FALSE(package->content_id_valid);

  std::vector<uint8_t> other(0x2000, 0);
  CHECK_FALSE(ReadTitleUpdatePackage(other, &error));
  CHECK_FALSE(error.empty());
}

TEST_CASE("A package is accepted only as the update the title lists", "[guide][title_update]") {
  const auto bytes = MakePackage();
  const std::string id = ContentId(bytes);
  const auto good = *ReadTitleUpdatePackage(bytes, nullptr);
  CHECK(CheckTitleUpdatePackage(good, Expected(id), 0x415607FF).empty());

  CHECK_FALSE(CheckTitleUpdatePackage(good, Expected(id), 0x4156081F).empty());
  auto other_disc = Expected(id);
  other_disc.media_id = "76ECEE40";
  CHECK_FALSE(CheckTitleUpdatePackage(good, other_disc, 0x415607FF).empty());
  auto other_base = Expected(id);
  other_base.base_version = 0xA;
  CHECK_FALSE(CheckTitleUpdatePackage(good, other_base, 0x415607FF).empty());
  const std::string another(40, 'A');
  CHECK_FALSE(CheckTitleUpdatePackage(good, Expected(another), 0x415607FF).empty());

  const auto dlc = *ReadTitleUpdatePackage(MakePackage(0x00000002), nullptr);
  CHECK_FALSE(
      CheckTitleUpdatePackage(dlc, Expected(ContentId(MakePackage(2))), 0x415607FF).empty());
}

TEST_CASE("Xbox Unity's listing gives the update ID for a content ID", "[guide][title_update]") {
  const std::string qos =
      R"({"Type":1,"MediaIDS":[{"MediaID":"06DD88A0","Updates":[{"TitleUpdateID":"22386",)"
      R"("Version":"2","hash":"B59A1F29F4FB82AB35DC7AEC8975F83F620A8290","Size":"2420",)"
      R"("UploadDate":"2012-06-18 00:00:00","Name":"Quantum of Solace","BaseVersion":"00000007"}],)"
      R"("Count":1},{"MediaID":"76ECEE40","Updates":[{"TitleUpdateID":"21248","Version":"2",)"
      R"("hash":"8FFAE973B21854157E385306A385A3F427634CD5","Size":"1516"}],"Count":1}]})";
  CHECK(FindXboxUnityUpdateId(qos, "b59a1f29f4fb82ab35dc7aec8975f83f620a8290") == "22386");
  CHECK(FindXboxUnityUpdateId(qos, "8FFAE973B21854157E385306A385A3F427634CD5") == "21248");
  CHECK_FALSE(FindXboxUnityUpdateId(qos, std::string(40, '0')));

  CHECK_FALSE(FindXboxUnityUpdateId(R"({"Type":2,"Updates":[]})", std::string(40, '0')));
  CHECK_FALSE(FindXboxUnityUpdateId("not json", std::string(40, '0')));
}

TEST_CASE("More download sources come from title_update_sources", "[guide][title_update]") {
  rex::cvar::SetFlagByName("title_update_sources",
                           "https://example.invalid/{title_id}/{media_id}/{version}/{content_id};");
  const auto sources = TitleUpdateSources();
  rex::cvar::SetFlagByName("title_update_sources", "");
  REQUIRE(sources.size() == 2);
  CHECK(sources[0].name == "Xbox Unity");
  const std::string id(40, 'b');
  const auto url = sources[1].find_url(Expected(id), 0x415607FF, nullptr);
  CHECK(url == "https://example.invalid/415607FF/06DD88A0/2/" + std::string(40, 'B'));
}

TEST_CASE("A checked package is installed into the update's folder", "[guide][title_update]") {
  TempDir dir("rex_title_update_install");
  const auto bytes = MakePackage();
  const std::string id = ContentId(bytes);
  const fs::path picked = dir.path / "picked" / "TU_10LC1VV_000000S000000.00000000000G7";
  WriteFile(picked, bytes);

  CHECK(FindInstalledTitleUpdate(dir.path, 2).empty());
  CHECK(InstallTitleUpdate(picked, Expected(id), 0x415607FF, dir.path).empty());
  const fs::path installed = FindInstalledTitleUpdate(dir.path, 2);
  CHECK(installed == TitleUpdateFolder(dir.path, 2) / picked.filename());
  CHECK(fs::file_size(installed) == bytes.size());

  auto bad = bytes;
  bad[0x9000] ^= 1;
  const fs::path damaged = dir.path / "picked" / "damaged";
  WriteFile(damaged, bad);
  CHECK_FALSE(InstallTitleUpdate(damaged, Expected(id), 0x415607FF, dir.path).empty());
  CHECK(FindInstalledTitleUpdate(dir.path, 2) == installed);

  const fs::path part = TitleUpdateFolder(dir.path, 2) / "title_update_2.part";
  WriteFile(part, bytes);
  CHECK(InstallTitleUpdate(part, Expected(id), 0x415607FF, dir.path).empty());
  CHECK(FindInstalledTitleUpdate(dir.path, 2) == TitleUpdateFolder(dir.path, 2) / "title_update_2");
  size_t files = 0;
  for (const auto& entry : fs::directory_iterator(TitleUpdateFolder(dir.path, 2))) {
    (void)entry;
    ++files;
  }
  CHECK(files == 1);
}

TEST_CASE("Download sources are tried in order until one installs the update",
          "[guide][title_update]") {
  TempDir dir("rex_title_update_sources");
  const std::string id(40, 'C');
  int asked = 0;
  std::vector<TitleUpdateSource> sources{
      {"down",
       [&](const PPCTitleUpdate&, uint32_t, std::string* error) {
         ++asked;
         *error = "the server answered 503";
         return std::optional<std::string>();
       }},
      {"missing", [&](const PPCTitleUpdate&, uint32_t, std::string* error) {
         ++asked;
         *error = "it doesn't list title update 2";
         return std::optional<std::string>();
       }}};
  std::atomic<bool> cancel{false};
  std::vector<std::string> errors;
  CHECK_FALSE(
      DownloadTitleUpdate(Expected(id), 0x415607FF, dir.path, sources, nullptr, cancel, &errors));
  CHECK(asked == 2);
  REQUIRE(errors.size() == 2);
  CHECK(errors[0] == "down: the server answered 503");
  CHECK(FindInstalledTitleUpdate(dir.path, 2).empty());
}

TEST_CASE("The player's title update choice picks the executable, the original otherwise",
          "[guide][title_update]") {
  TempDir dir("rex_title_update_launch");
  const fs::path original = dir.path / "bin" / "qos.exe";
  const fs::path update = dir.path / "bin" / "qos_tu2.exe";
  const fs::path local = dir.path / "local";
  WriteFile(original, {0});

  CHECK(TitleUpdateExecutable(original, 0, 2) == update);
  CHECK(TitleUpdateExecutable(update, 2, 0) == original);

  auto choice = ChooseLaunch(0, 0, original, local, false);
  CHECK_FALSE(choice.hand_off);
  CHECK_FALSE(choice.cannot_run);

  choice = ChooseLaunch(0, 2, original, local, false);
  CHECK_FALSE(choice.hand_off);
  CHECK_FALSE(choice.note.empty());
  WriteFile(TitleUpdateFolder(local, 2) / "title_update_2", {1});
  choice = ChooseLaunch(0, 2, original, local, false);
  CHECK_FALSE(choice.hand_off);

  WriteFile(update, {0});
  choice = ChooseLaunch(0, 2, original, local, false);
  CHECK(choice.hand_off);
  CHECK(choice.executable == update);

  choice = ChooseLaunch(2, 2, update, local, true);
  CHECK_FALSE(choice.hand_off);
  CHECK(choice.update == TitleUpdateFolder(local, 2) / "title_update_2");

  choice = ChooseLaunch(2, 0, update, local, false);
  CHECK(choice.hand_off);
  CHECK(choice.executable == original);

  choice = ChooseLaunch(0, 2, original, local, true);
  CHECK_FALSE(choice.hand_off);

  fs::remove_all(TitleUpdateFolder(local, 2));
  choice = ChooseLaunch(2, 2, update, local, false);
  CHECK(choice.hand_off);
  CHECK(choice.executable == original);
  fs::remove(original);
  choice = ChooseLaunch(2, 2, update, local, false);
  CHECK(choice.cannot_run);
}

TEST_CASE("Quantum of Solace's title update 2 downloads from Xbox Unity and installs",
          "[.network][guide][title_update]") {
  TempDir dir("rex_title_update_network");
  const PPCTitleUpdate update{2,    "06DD88A0",   7, "B59A1F29F4FB82AB35DC7AEC8975F83F620A8290",
                              2420, "2012-06-18", ""};
  std::atomic<bool> cancel{false};
  uint64_t last_done = 0, last_total = 0;
  std::vector<std::string> errors;
  const bool installed = DownloadTitleUpdate(
      update, 0x415607FF, dir.path, {XboxUnitySource()},
      [&](uint64_t done, uint64_t total) {
        last_done = done;
        last_total = total;
      },
      cancel, &errors);
  for (const auto& e : errors) {
    UNSCOPED_INFO(e);
  }
  REQUIRE(installed);
  CHECK(last_total == 2478080);
  CHECK(last_done == last_total);
  const fs::path package = FindInstalledTitleUpdate(dir.path, 2);
  CHECK(package.filename() == "title_update_2");
  CHECK(fs::file_size(package) == 2478080);
}
