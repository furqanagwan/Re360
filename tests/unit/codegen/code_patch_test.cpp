/**
 * @file        code_patch_test.cpp
 * @brief       [[patch]] parsing and codegen-time guest code patches
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <catch2/catch_test_macros.hpp>

#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <rex/codegen/analyze.h>
#include <rex/codegen/binary_view.h>
#include <rex/codegen/code_patches.h>
#include <rex/codegen/codegen_context.h>
#include <rex/codegen/config.h>
#include <rex/codegen/test_support.h>

#include <toml++/toml.hpp>

using namespace rex::codegen;
namespace fs = std::filesystem;

namespace {

constexpr uint32_t kBase = 0x82000000;

RecompilerConfig Parse(std::string_view text) {
  RecompilerConfig cfg;
  REQUIRE(cfg.LoadFromTable(toml::parse(text), fs::temp_directory_path()));
  return cfg;
}

const CodePatch& Named(const RecompilerConfig& cfg, std::string_view name) {
  for (const auto& patch : cfg.patches) {
    if (patch.name == name) {
      return patch;
    }
  }
  FAIL("no patch named " << name);
  return cfg.patches.front();
}

class Image : public TestModule {
 public:
  std::vector<uint8_t> bytes;
  explicit Image(bool executable = true) {
    const uint32_t words[] = {0x38600001, 0x4E800020, 0, 0};
    for (uint32_t w : words) {
      for (int shift = 24; shift >= 0; shift -= 8) {
        bytes.push_back(uint8_t(w >> shift));
      }
    }
    Load(kBase, bytes.data(), bytes.size());
    binary_sections_.front().executable = executable;
  }
};

uint32_t WordAt(const BinaryView& binary, uint32_t address) {
  const uint8_t* p = binary.translate(address);
  REQUIRE(p);
  return (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) | (uint32_t(p[2]) << 8) | p[3];
}

CodePatch Patch(std::string name, uint32_t address, std::vector<uint8_t> bytes,
                bool enabled = true) {
  CodePatch patch;
  patch.name = std::move(name);
  patch.enabled = enabled;
  patch.writes.push_back({address, std::move(bytes)});
  return patch;
}

}

TEST_CASE("[[patch]] writes are big-endian at their width", "[codegen][patch]") {
  auto cfg = Parse(R"(
    file_path = "default.xex"
    [[patch]]
    name = "Widths"
    [[patch.be8]]
    address = 0x82000003
    value = 0x02
    [[patch.be16]]
    address = 0x82000010
    value = 0x1234
    [[patch.be32]]
    address = 0x82000020
    value = 0x60000000
    [[patch.be64]]
    address = 0x82000030
    value = 0x0102030405060708
  )");
  const auto& patch = Named(cfg, "Widths");
  CHECK(patch.enabled);
  CHECK(patch.error.empty());
  REQUIRE(patch.writes.size() == 4);
  CHECK(patch.writes[0].bytes == std::vector<uint8_t>{0x02});
  CHECK(patch.writes[1].bytes == std::vector<uint8_t>{0x12, 0x34});
  CHECK(patch.writes[2].bytes == std::vector<uint8_t>{0x60, 0x00, 0x00, 0x00});
  CHECK(patch.writes[3].bytes == std::vector<uint8_t>{1, 2, 3, 4, 5, 6, 7, 8});
  CHECK(patch.writes[2].address == 0x82000020);
}

TEST_CASE("[[patch]] accepts Canary's is_enabled and reports bad entries", "[codegen][patch]") {
  auto cfg = Parse(R"(
    file_path = "default.xex"
    [[patch]]
    name = "Copied from Canary"
    is_enabled = false
    [[patch.be32]]
    address = 0x82000000
    value = 0x60000000

    [[patch]]
    name = "Too wide"
    [[patch.be8]]
    address = 0x82000000
    value = 0x100

    [[patch]]
    name = "No writes"
  )");
  CHECK_FALSE(Named(cfg, "Copied from Canary").enabled);
  CHECK(Named(cfg, "Too wide").error.find("does not fit") != std::string::npos);
  CHECK(Named(cfg, "No writes").error.find("no be8") != std::string::npos);
}

TEST_CASE("An including file can switch on a patch without repeating it", "[codegen][patch]") {
  const auto dir = fs::temp_directory_path() / "rexglue_code_patch_test";
  fs::create_directories(dir);
  std::ofstream(dir / "title.toml") << R"(
    [[patch]]
    name = "Unlock FPS"
    enabled = false
    [[patch.be32]]
    address = 0x82000000
    value = 0x60000000
  )";
  std::ofstream(dir / "local.toml") << R"(
    file_path = "default.xex"
    includes = ["title.toml"]
    [[patch]]
    name = "Unlock FPS"
    enabled = true
  )";
  RecompilerConfig cfg;
  REQUIRE(cfg.Load((dir / "local.toml").string()));
  REQUIRE(cfg.patches.size() == 1);
  CHECK(cfg.patches[0].enabled);
  CHECK(cfg.patches[0].error.empty());
  REQUIRE(cfg.patches[0].writes.size() == 1);
  CHECK(cfg.patches[0].writes[0].bytes == std::vector<uint8_t>{0x60, 0, 0, 0});
  fs::remove_all(dir);
}

TEST_CASE("Enabled patches are written into the code, disabled ones are not", "[codegen][patch]") {
  Image image;
  auto binary = BinaryView::fromModule(image);
  std::vector<CodePatch> patches{Patch("on", kBase, {0x38, 0x60, 0x00, 0x02}),
                                 Patch("off", kBase + 4, {0x60, 0, 0, 0}, false)};
  auto applied = ApplyCodePatches(binary, patches);
  REQUIRE(applied);
  CHECK(*applied == std::vector<std::string>{"on"});
  CHECK(WordAt(binary, kBase) == 0x38600002);
  CHECK(WordAt(binary, kBase + 4) == 0x4E800020);
  CHECK(image.bytes[3] == 0x01);
}

TEST_CASE("Patches that cannot reach the running code are refused", "[codegen][patch]") {
  SECTION("outside the image") {
    Image image;
    auto binary = BinaryView::fromModule(image);
    auto result = ApplyCodePatches(binary, {Patch("far", kBase + 0x1000, {0})});
    REQUIRE_FALSE(result);
    CHECK(result.error().message.find("outside the code sections") != std::string::npos);
  }
  SECTION("a data section") {
    Image image(false);
    auto binary = BinaryView::fromModule(image);
    REQUIRE_FALSE(ApplyCodePatches(binary, {Patch("data", kBase, {0})}));
  }
  SECTION("past the end of the section") {
    Image image;
    auto binary = BinaryView::fromModule(image);
    REQUIRE_FALSE(ApplyCodePatches(binary, {Patch("tail", kBase + 14, {1, 2, 3, 4})}));
  }
}

TEST_CASE("Nothing is written when enabled patches overlap or one is malformed",
          "[codegen][patch]") {
  Image image;
  auto binary = BinaryView::fromModule(image);
  auto overlap = ApplyCodePatches(
      binary, {Patch("first", kBase, {0x60, 0, 0, 0}), Patch("second", kBase + 2, {0xAA})});
  REQUIRE_FALSE(overlap);
  CHECK(overlap.error().message.find("both write") != std::string::npos);
  CHECK(WordAt(binary, kBase) == 0x38600001);

  CodePatch broken = Patch("broken", kBase, {0});
  broken.error = "a be8 entry needs an address and a value";
  auto result = ApplyCodePatches(binary, {Patch("good", kBase + 4, {0x60, 0, 0, 0}), broken});
  REQUIRE_FALSE(result);
  CHECK(result.error().message.find("broken") != std::string::npos);
  CHECK(WordAt(binary, kBase + 4) == 0x4E800020);

  broken.enabled = false;
  CHECK(ApplyCodePatches(binary, {Patch("good", kBase, {0x60, 0, 0, 0}), broken}));
}

TEST_CASE("Analysis patches the image before anything else", "[codegen][patch]") {
  Image image;
  RecompilerConfig config;
  config.patches.push_back(Patch("li 2", kBase, {0x38, 0x60, 0x00, 0x02}));
  auto ctx = CodegenContext::Create(BinaryView::fromModule(image), std::move(config));

  auto analyzed = Analyze(ctx);
  REQUIRE_FALSE(analyzed);
  CHECK(analyzed.error().message.find("Exception DataDirectory") != std::string::npos);
  CHECK(ctx.appliedPatches() == std::vector<std::string>{"li 2"});
  CHECK(WordAt(ctx.binary(), kBase) == 0x38600002);

  RecompilerConfig bad;
  bad.patches.push_back(Patch("far", kBase + 0x1000, {0}));
  auto badCtx = CodegenContext::Create(BinaryView::fromModule(image), std::move(bad));
  auto refused = Analyze(badCtx);
  REQUIRE_FALSE(refused);
  CHECK(refused.error().message.find("\"far\"") != std::string::npos);
  CHECK(badCtx.appliedPatches().empty());
}

TEST_CASE("[[patch]] switchable is parsed and can be set by an including file",
          "[codegen][patch]") {
  auto cfg = Parse(R"(
file_path = "default.xex"
[[patch]]
name = "Unlock FPS"
enabled = false
switchable = true
[[patch.be8]]
address = 0x82000003
value = 2
)");
  CHECK(Named(cfg, "Unlock FPS").switchable);
  CHECK_FALSE(Named(cfg, "Unlock FPS").enabled);
}

TEST_CASE("Switchable patches keep the image original and list both words", "[codegen][patch]") {
  Image image;
  auto binary = BinaryView::fromModule(image);
  CodePatch patch = Patch("Two", kBase + 3, {0x02}, false);
  patch.switchable = true;
  std::vector<CodePatch> patches = {patch};
  auto applied = ApplyCodePatches(binary, patches);
  REQUIRE(applied);
  CHECK(applied->empty());
  CHECK(WordAt(binary, kBase) == 0x38600001);

  auto switchable = PrepareSwitchablePatches(binary, patches);
  REQUIRE(switchable);
  REQUIRE(switchable->patches.size() == 1);
  CHECK(switchable->patches[0].name == "Two");
  CHECK_FALSE(switchable->patches[0].enabled);
  REQUIRE(switchable->words.size() == 1);
  const SwitchedWord& word = switchable->words.at(kBase);
  CHECK(word.original == 0x38600001);
  CHECK(word.patched == 0x38600002);
  CHECK(word.patch_index == 0);

  CodePatch same = Patch("Same", kBase + 3, {0x01});
  same.switchable = true;
  auto unchanged = PrepareSwitchablePatches(binary, {same});
  REQUIRE(unchanged);
  CHECK(unchanged->patches.size() == 1);
  CHECK(unchanged->words.empty());
}

TEST_CASE("Switchable patches may not touch branches", "[codegen][patch]") {
  Image image;
  auto binary = BinaryView::fromModule(image);

  CodePatch patch = Patch("Return", kBase + 4, {0x60, 0x00, 0x00, 0x00});
  patch.switchable = true;
  auto switchable = PrepareSwitchablePatches(binary, {patch});
  REQUIRE_FALSE(switchable);
  CHECK(switchable.error().message.find("branch") != std::string::npos);

  CodePatch jump = Patch("Jump", kBase, {0x48, 0x00, 0x00, 0x08});
  jump.switchable = true;
  CHECK_FALSE(PrepareSwitchablePatches(binary, {jump}));
}

TEST_CASE("[[cheat]] lists the title's own codes, keyed by name", "[codegen][patch]") {
  auto cfg = Parse(R"(
file_path = "default.xex"
[[cheat]]
name = "007 Pack"
code = "g3tb0nd"
description = "Walther PPK"
where = "Extras > Cheat Codes"
[[cheat]]
name = "No code"
[[cheat]]
name = "Skyfall Pack"
code = "l3g3nds"
[[cheat]]
name = "007 Pack"
code = "g3tb0nd"
description = "Walther PPK and Fast Switch"
)");
  REQUIRE(cfg.cheats.size() == 2);
  CHECK(cfg.cheats[0].name == "007 Pack");
  CHECK(cfg.cheats[0].description == "Walther PPK and Fast Switch");
  CHECK(cfg.cheats[0].where.empty());
  CHECK(cfg.cheats[1].code == "l3g3nds");
}

TEST_CASE("[[dlc]] lists the title's add-ons, keyed by id", "[codegen][patch]") {
  auto cfg = Parse(R"(
file_path = "default.xex"
[[dlc]]
id = "d4c83e1f-243b-4a68-ad70-abf3c0fbf372"
[[dlc]]
package_name = "No id"
[[dlc]]
id = "00000000-0000-400C-80CF-0001415607FF"
requires_title_update = 2
package_name = "Camille Map Pack"
[[dlc]]
id = "D4C83E1F-243B-4A68-AD70-ABF3C0FBF372"
package_name = "SKYFALL"
)");
  REQUIRE(cfg.dlc.size() == 2);

  CHECK(cfg.dlc[0].id == "D4C83E1F-243B-4A68-AD70-ABF3C0FBF372");
  CHECK(cfg.dlc[0].package_name == "SKYFALL");
  CHECK(cfg.dlc[0].requires_title_update == 0);
  CHECK(cfg.dlc[1].requires_title_update == 2);
  CHECK(cfg.dlc[1].package_name == "Camille Map Pack");
}

TEST_CASE("Switchable patches set registers, optionally keyed on lr", "[codegen][patch]") {
  auto cfg = Parse(R"(
file_path = "default.xex"
[[patch]]
name = "God Mode"
category = "cheat"
switchable = true
enabled = false
[[patch.set]]
address = 0x82000004
register = "r11"
value = 30000
lr = 0x82000000
[[patch]]
name = "Flag only"
switchable = true
[[patch]]
name = "Fixed set"
[[patch.set]]
address = 0x82000004
register = "r3"
value = 1
[[patch]]
name = "Bad register"
switchable = true
[[patch.set]]
address = 0x82000004
register = "r40"
value = 1
)");
  const CodePatch& god = Named(cfg, "God Mode");
  REQUIRE(god.sets.size() == 1);
  CHECK(god.sets[0].address == kBase + 4);
  CHECK(god.sets[0].reg == 11);
  CHECK(god.sets[0].value == 30000);
  CHECK(god.sets[0].lr == kBase);
  CHECK(god.category == "mod");

  CHECK(Named(cfg, "Flag only").error.empty());
  CHECK_FALSE(Named(cfg, "Fixed set").error.empty());
  CHECK_FALSE(Named(cfg, "Bad register").error.empty());

  Image image;
  auto binary = BinaryView::fromModule(image);
  std::vector<CodePatch> usable = {god, Named(cfg, "Flag only")};
  auto prepared = PrepareSwitchablePatches(binary, usable);
  REQUIRE(prepared);
  CHECK(prepared->patches.size() == 2);
  CHECK(prepared->words.empty());
  REQUIRE(prepared->sets.count(kBase + 4) == 1);
  CHECK(prepared->sets.find(kBase + 4)->second.patch_index == 0);

  CHECK_FALSE(PrepareSwitchablePatches(binary, usable, false));

  CodePatch far = god;
  far.sets[0].address = kBase + 0x1000;
  CHECK_FALSE(PrepareSwitchablePatches(binary, {far}));
}

TEST_CASE("Switchable sets can require registers and guest words to be non-zero",
          "[codegen][patch]") {
  auto cfg = Parse(R"(
file_path = "default.xex"
[[patch]]
name = "One-Hit Kills"
switchable = true
[[patch.set]]
address = 0x82000004
register = "r11"
value = 0
lr = 0x82000000
when_nonzero = ["r26", "r26+316", "r3+-8"]
[[patch]]
name = "Bad condition"
switchable = true
[[patch.set]]
address = 0x82000004
register = "r11"
value = 0
when_nonzero = ["f1"]
[[patch]]
name = "Bad offset"
switchable = true
[[patch.set]]
address = 0x82000004
register = "r11"
value = 0
when_nonzero = ["r26+x", "r3-8"]
)");
  const CodePatch& kills = Named(cfg, "One-Hit Kills");
  REQUIRE(kills.error.empty());
  REQUIRE(kills.sets.size() == 1);
  const auto& when = kills.sets[0].when_nonzero;
  REQUIRE(when.size() == 3);
  CHECK(when[0].reg == 26);
  CHECK_FALSE(when[0].offset);
  CHECK(when[1].reg == 26);
  CHECK(when[1].offset == 316);
  CHECK(when[2].reg == 3);
  CHECK(when[2].offset == -8);
  CHECK_FALSE(Named(cfg, "Bad condition").error.empty());
  CHECK_FALSE(Named(cfg, "Bad offset").error.empty());

  Image image;
  auto binary = BinaryView::fromModule(image);
  auto prepared = PrepareSwitchablePatches(binary, {kills});
  REQUIRE(prepared);
  REQUIRE(prepared->sets.count(kBase + 4) == 1);
  CHECK(prepared->sets.find(kBase + 4)->second.when_nonzero.size() == 3);
}
