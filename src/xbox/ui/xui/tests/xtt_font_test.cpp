/**
 * @file        ui/xui/tests/xtt_font_test.cpp
 * @brief       The console's XTT fonts converted to TrueType (RG-GDK-061)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include <rex/ui/xui/system_update.h>
#include <rex/ui/xui/xtt_font.h>

using rex::ui::xui::SystemUpdate;
using rex::ui::xui::XttToTrueType;

namespace {

using Bytes = std::vector<uint8_t>;

void Put16(Bytes& out, uint16_t v) {
  out.push_back(uint8_t(v >> 8));
  out.push_back(uint8_t(v));
}

void Put32(Bytes& out, uint32_t v) {
  Put16(out, uint16_t(v >> 16));
  Put16(out, uint16_t(v));
}

uint32_t Be32(const Bytes& b, size_t at) {
  return uint32_t(b[at]) << 24 | uint32_t(b[at + 1]) << 16 | uint32_t(b[at + 2]) << 8 | b[at + 3];
}

uint16_t Be16(const Bytes& b, size_t at) {
  return uint16_t(b[at] << 8 | b[at + 1]);
}

Bytes Zlib(const Bytes& data) {
  Bytes out = {0x78, 0x01, 0x01};
  out.push_back(uint8_t(data.size()));
  out.push_back(uint8_t(data.size() >> 8));
  out.push_back(uint8_t(~data.size()));
  out.push_back(uint8_t(~data.size() >> 8));
  out.insert(out.end(), data.begin(), data.end());
  uint32_t a = 1, b = 0;
  for (uint8_t byte : data) {
    a = (a + byte) % 65521;
    b = (b + a) % 65521;
  }
  Put32(out, b << 16 | a);
  return out;
}

Bytes Glyph(uint16_t points) {
  Bytes g;
  Put16(g, 1);
  for (int16_t v : {0, 0, 100, 100}) {
    Put16(g, uint16_t(v));
  }
  Put16(g, uint16_t(points - 1));
  Put16(g, 0);
  for (uint16_t i = 0; i < points; ++i) {
    g.push_back(0x01 | 0x02 | 0x04);
  }
  for (int i = 0; i < 2 * points; ++i) {
    g.push_back(10);
  }
  return g;
}

Bytes MakeXtt() {
  const Bytes g0 = Glyph(3), g1 = Glyph(4), g2 = Glyph(5);
  Bytes block0 = g0;
  block0.insert(block0.end(), g1.begin(), g1.end());
  Bytes xglf = Zlib(block0);
  xglf.resize(4096, 0);
  const Bytes block1 = Zlib(g2);
  xglf.insert(xglf.end(), block1.begin(), block1.end());
  xglf.resize(8192, 0);

  Bytes xloc;
  Put32(xloc, 0);
  Put32(xloc, uint32_t(g0.size()));
  Put32(xloc, 1u << 16);
  Put32(xloc, 1u << 16 | uint32_t(g2.size()));

  Bytes head(54, 0);
  head[0] = 0x00;
  head[1] = 0x01;
  head[18] = 0x08;
  Bytes hhea(36, 0);
  hhea[1] = 0x01;
  hhea[4] = 0x08;
  hhea[35] = 3;
  Bytes hmtx(12, 0);
  Bytes cmap = {0, 0, 0, 0};
  Bytes name = {0, 0, 0, 0, 0, 6};

  std::map<std::string, Bytes> tables = {{"cmap", cmap}, {"head", head}, {"hhea", hhea},
                                         {"hmtx", hmtx}, {"name", name}, {"xloc", xloc}};
  const uint16_t count = uint16_t(tables.size() + 1);
  Bytes dir;
  Put32(dir, 0x00010000);
  Put16(dir, count);
  Put16(dir, 0);
  Put16(dir, 0);
  Put16(dir, 0);
  uint32_t at = 12 + count * 16;
  Bytes data;
  for (const auto& [tag, table] : tables) {
    dir.insert(dir.end(), tag.begin(), tag.end());
    Put32(dir, 0);
    Put32(dir, at + uint32_t(data.size()));
    Put32(dir, uint32_t(table.size()));
    data.insert(data.end(), table.begin(), table.end());
  }
  const uint32_t xglf_at = 0x1000;
  dir.insert(dir.end(), {'x', 'g', 'l', 'f'});
  Put32(dir, 0);
  Put32(dir, xglf_at);
  Put32(dir, uint32_t(xglf.size()));
  dir.insert(dir.end(), data.begin(), data.end());

  const Bytes compressed = Zlib(dir);
  Bytes xtt = {'x', 't', 't', 'f'};
  xtt.resize(0x10C, 0);
  Put32(xtt, uint32_t(compressed.size()));
  Put32(xtt, uint32_t(dir.size()));
  Put32(xtt, 0x10000);
  xtt.insert(xtt.end(), compressed.begin(), compressed.end());
  xtt.resize(xglf_at, 0);
  xtt.insert(xtt.end(), xglf.begin(), xglf.end());
  return xtt;
}

std::map<std::string, std::pair<uint32_t, uint32_t>> Tables(const Bytes& font) {
  std::map<std::string, std::pair<uint32_t, uint32_t>> tables;
  for (uint16_t i = 0; i < Be16(font, 4); ++i) {
    const size_t entry = 12 + size_t(i) * 16;
    tables[std::string(font.begin() + entry, font.begin() + entry + 4)] = {Be32(font, entry + 8),
                                                                           Be32(font, entry + 12)};
  }
  return tables;
}

}

TEST_CASE("XTT fonts convert to TrueType", "[xui][xtt]") {
  std::string error;
  const auto font = XttToTrueType(MakeXtt(), &error);
  REQUIRE(font);
  CHECK(error.empty());
  CHECK(Be32(*font, 0) == 0x00010000);
  const auto tables = Tables(*font);
  for (const char* tag :
       {"OS/2", "cmap", "glyf", "head", "hhea", "hmtx", "loca", "maxp", "name", "post"}) {
    INFO(tag);
    CHECK(tables.contains(tag));
  }
  CHECK_FALSE(tables.contains("xglf"));

  const auto [head_at, head_size] = tables.at("head");
  CHECK(Be16(*font, head_at + 50) == 1);
  const auto [loca_at, loca_size] = tables.at("loca");
  REQUIRE(loca_size == 4 * 4);
  const auto [glyf_at, glyf_size] = tables.at("glyf");
  const uint32_t g1 = Be32(*font, loca_at + 4), g2 = Be32(*font, loca_at + 8);
  CHECK(Be32(*font, loca_at) == 0);
  CHECK(Be32(*font, loca_at + 12) == glyf_size);
  CHECK(Be16(*font, glyf_at + g2 + 10) == 4);
  CHECK(Be16(*font, glyf_at + g1 + 10) == 3);

  const auto [maxp_at, maxp_size] = tables.at("maxp");
  CHECK(Be16(*font, maxp_at + 4) == 3);
  CHECK(Be16(*font, maxp_at + 6) == 5);

  uint32_t sum = 0;
  for (size_t i = 0; i < font->size(); i += 4) {
    sum += Be32(*font, i);
  }
  CHECK(sum == 0xB1B0AFBA);
}

TEST_CASE("Damaged XTT fonts are rejected", "[xui][xtt]") {
  std::string error;
  CHECK_FALSE(XttToTrueType(Bytes{'n', 'o', 'p', 'e'}, &error));
  CHECK(error == "not an XTT font");

  Bytes truncated = MakeXtt();
  truncated.resize(0x1000 + 100);
  CHECK_FALSE(XttToTrueType(truncated, &error));
  CHECK(error.find("xglf") != std::string::npos);
}

TEST_CASE("System fonts travel in the guide bundle", "[xui][xtt]") {
  SystemUpdate::Modules modules;
  modules["font/xenonjklatin"] = MakeXtt();
  const auto bundle = SystemUpdate::WriteBundle(modules);
  std::string error;
  const auto read = SystemUpdate::ReadBundle(bundle, &error);
  REQUIRE(read);
  SystemUpdate update;
  REQUIRE(update.AddFont("xenonjklatin", read->at("font/xenonjklatin"), &error));
  CHECK_FALSE(update.Font("XenonJKLatin").empty());
  CHECK(update.Font("segoexbox-light").empty());
}

TEST_CASE("The console's own fonts convert", "[xui][xtt]") {
  std::vector<std::filesystem::path> sources;
  for (const char* name : {"REXGLUE_GUIDE_FLASH", "REXGLUE_SYSTEM_UPDATE"}) {
    if (const char* path = std::getenv(name); path && *path) {
      sources.emplace_back(path);
    }
  }
  if (sources.empty()) {
    SKIP("REXGLUE_GUIDE_FLASH and REXGLUE_SYSTEM_UPDATE are not set");
  }
  std::string error;
  const auto modules = SystemUpdate::ReadModules(sources, &error);
  REQUIRE(modules);
  size_t fonts = 0;
  for (const auto& [module, bytes] : *modules) {
    if (!module.starts_with("font/")) {
      continue;
    }
    INFO(module);
    const auto font = XttToTrueType(bytes, &error);
    REQUIRE(font);
    const auto tables = Tables(*font);
    REQUIRE(tables.contains("maxp"));
    CHECK(Be16(*font, tables.at("maxp").first + 4) > 100);
    ++fonts;
  }
  CHECK(fonts > 0);
}
