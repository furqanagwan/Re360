/**
 * @file        ui/xui/src/xtt_font.cpp
 * @brief       The console's XTT system fonts as standard TrueType (RG-GDK-061)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/xui/xtt_font.h>

#include <algorithm>
#include <cstdlib>
#include <map>
#include <string_view>

#include <fmt/format.h>

#include <stb_image.h>

namespace rex::ui::xui {
namespace {

constexpr uint32_t kXttMagic = 0x78747466;

constexpr size_t kCompressedSizeAt = 0x10C;
constexpr size_t kUncompressedSizeAt = 0x110;
constexpr size_t kHeaderSize = 0x118;
constexpr size_t kGlyphBlockSize = 4096;

uint32_t Be32(std::span<const uint8_t> b, size_t at) {
  const uint8_t* p = b.data() + at;
  return uint32_t(p[0]) << 24 | uint32_t(p[1]) << 16 | uint32_t(p[2]) << 8 | p[3];
}

uint16_t Be16(std::span<const uint8_t> b, size_t at) {
  return uint16_t(b[at] << 8 | b[at + 1]);
}

void Put16(std::vector<uint8_t>& out, uint16_t v) {
  out.push_back(uint8_t(v >> 8));
  out.push_back(uint8_t(v));
}

void Put32(std::vector<uint8_t>& out, uint32_t v) {
  Put16(out, uint16_t(v >> 16));
  Put16(out, uint16_t(v));
}

void Set16(std::vector<uint8_t>& out, size_t at, uint16_t v) {
  out[at] = uint8_t(v >> 8);
  out[at + 1] = uint8_t(v);
}

void Set32(std::vector<uint8_t>& out, size_t at, uint32_t v) {
  Set16(out, at, uint16_t(v >> 16));
  Set16(out, at + 2, uint16_t(v));
}

std::optional<std::vector<uint8_t>> Inflate(std::span<const uint8_t> in, int size_hint) {
  int size = 0;
  char* data = stbi_zlib_decode_malloc_guesssize_headerflag(
      reinterpret_cast<const char*>(in.data()), int(in.size()), std::max(size_hint, 4096), &size,
      1);
  if (!data) {
    return std::nullopt;
  }
  std::vector<uint8_t> out(data, data + size);
  std::free(data);
  return out;
}

uint32_t Checksum(std::span<const uint8_t> table) {
  uint32_t sum = 0;
  for (size_t i = 0; i < table.size(); i += 4) {
    uint32_t word = 0;
    for (size_t j = 0; j < 4; ++j) {
      word = word << 8 | (i + j < table.size() ? table[i + j] : 0);
    }
    sum += word;
  }
  return sum;
}

std::vector<uint8_t> BuildMaxp(std::span<const uint8_t> glyf, const std::vector<uint32_t>& loca) {
  const size_t glyphs = loca.size() - 1;
  std::vector<uint16_t> points(glyphs), contours(glyphs);
  uint16_t max_points = 0, max_contours = 0, max_instructions = 0;
  for (size_t g = 0; g < glyphs; ++g) {
    const auto glyph = glyf.subspan(loca[g], loca[g + 1] - loca[g]);
    if (glyph.size() < 12 || int16_t(Be16(glyph, 0)) < 0) {
      continue;
    }
    const uint16_t count = Be16(glyph, 0);
    if (glyph.size() < 10 + size_t(count) * 2 + 2) {
      continue;
    }
    contours[g] = count;
    points[g] = count ? uint16_t(Be16(glyph, 10 + (count - 1) * 2) + 1) : 0;
    max_contours = std::max(max_contours, count);
    max_points = std::max(max_points, points[g]);
    max_instructions = std::max(max_instructions, Be16(glyph, 10 + count * 2));
  }
  uint16_t max_composite_points = 0, max_composite_contours = 0, max_components = 0;
  for (size_t g = 0; g < glyphs; ++g) {
    const auto glyph = glyf.subspan(loca[g], loca[g + 1] - loca[g]);
    if (glyph.size() < 10 || int16_t(Be16(glyph, 0)) >= 0) {
      continue;
    }
    constexpr uint16_t kArgsAreWords = 0x0001, kHaveScale = 0x0008, kMoreComponents = 0x0020,
                       kHaveXYScale = 0x0040, kHaveTwoByTwo = 0x0080;
    uint32_t sum_points = 0, sum_contours = 0;
    uint16_t components = 0;
    size_t at = 10;
    uint16_t flags = kMoreComponents;
    while ((flags & kMoreComponents) && at + 4 <= glyph.size()) {
      flags = Be16(glyph, at);
      const uint16_t index = Be16(glyph, at + 2);
      at += 4 + ((flags & kArgsAreWords) ? 4 : 2);
      at += (flags & kHaveTwoByTwo) ? 8 : (flags & kHaveXYScale) ? 4 : (flags & kHaveScale) ? 2 : 0;
      if (index < glyphs) {
        sum_points += points[index];
        sum_contours += contours[index];
      }
      ++components;
    }
    max_composite_points = std::max<uint16_t>(max_composite_points, uint16_t(sum_points));
    max_composite_contours = std::max<uint16_t>(max_composite_contours, uint16_t(sum_contours));
    max_components = std::max(max_components, components);
  }
  std::vector<uint8_t> maxp;
  Put32(maxp, 0x00010000);
  Put16(maxp, uint16_t(glyphs));
  Put16(maxp, max_points);
  Put16(maxp, max_contours);
  Put16(maxp, max_composite_points);
  Put16(maxp, max_composite_contours);
  Put16(maxp, 2);
  Put16(maxp, 0);
  Put16(maxp, 0);
  Put16(maxp, 0);
  Put16(maxp, 0);
  Put16(maxp, 0);
  Put16(maxp, max_instructions);
  Put16(maxp, max_components);
  Put16(maxp, max_components ? 1 : 0);
  return maxp;
}

std::vector<uint8_t> BuildOs2(std::span<const uint8_t> head, std::span<const uint8_t> hhea) {
  const int16_t ascent = int16_t(Be16(hhea, 4));
  const int16_t descent = int16_t(Be16(hhea, 6));
  const int16_t line_gap = int16_t(Be16(hhea, 8));
  const int16_t y_min = int16_t(Be16(head, 38));
  const int16_t y_max = int16_t(Be16(head, 42));
  std::vector<uint8_t> os2;
  Put16(os2, 4);
  Put16(os2, uint16_t(Be16(head, 18) / 2));
  Put16(os2, 400);
  Put16(os2, 5);
  Put16(os2, 0);
  os2.resize(os2.size() + 2 * 11 + 10 + 16, 0);
  os2.insert(os2.end(), {'X', 'B', 'O', 'X'});
  Put16(os2, 0x0040);
  Put16(os2, 0x0020);
  Put16(os2, 0xFFFF);
  Put16(os2, uint16_t(ascent));
  Put16(os2, uint16_t(descent));
  Put16(os2, uint16_t(line_gap));
  Put16(os2, uint16_t(std::max<int>(ascent, y_max)));
  Put16(os2, uint16_t(std::max<int>(-descent, -y_min)));
  Put32(os2, 1);
  Put32(os2, 0);
  Put16(os2, 0);
  Put16(os2, 0);
  Put16(os2, 0);
  Put16(os2, 0x0020);
  Put16(os2, 1);
  return os2;
}

std::vector<uint8_t> BuildPost() {
  std::vector<uint8_t> post;
  Put32(post, 0x00030000);
  post.resize(32, 0);
  return post;
}

}

std::optional<std::vector<uint8_t>> XttToTrueType(std::span<const uint8_t> xtt,
                                                  std::string* error) {
  auto fail = [&](std::string message) -> std::optional<std::vector<uint8_t>> {
    if (error) {
      *error = std::move(message);
    }
    return std::nullopt;
  };
  if (xtt.size() < kHeaderSize || Be32(xtt, 0) != kXttMagic) {
    return fail("not an XTT font");
  }
  const uint32_t compressed = Be32(xtt, kCompressedSizeAt);
  const uint32_t uncompressed = Be32(xtt, kUncompressedSizeAt);
  if (compressed > xtt.size() - kHeaderSize) {
    return fail("the XTT font is truncated");
  }
  auto sfnt = Inflate(xtt.subspan(kHeaderSize, compressed), int(uncompressed));
  if (!sfnt || sfnt->size() < 12) {
    return fail("the XTT font directory does not decompress");
  }
  const std::span<const uint8_t> dir(*sfnt);

  std::map<std::string, std::span<const uint8_t>> tables;
  const uint16_t table_count = Be16(dir, 4);
  if (dir.size() < 12 + size_t(table_count) * 16) {
    return fail("the XTT font directory is truncated");
  }
  for (uint16_t i = 0; i < table_count; ++i) {
    const size_t entry = 12 + size_t(i) * 16;
    const std::string tag(reinterpret_cast<const char*>(dir.data() + entry), 4);
    const uint32_t offset = Be32(dir, entry + 8);
    const uint32_t length = Be32(dir, entry + 12);
    const std::span<const uint8_t> source = tag == "xglf" ? xtt : dir;
    if (offset > source.size() || length > source.size() - offset) {
      return fail(fmt::format("the XTT font's {} table is out of range", tag));
    }
    tables[tag] = source.subspan(offset, length);
  }
  for (const char* tag : {"cmap", "head", "hhea", "hmtx", "name", "xglf", "xloc"}) {
    if (!tables.contains(tag)) {
      return fail(fmt::format("the XTT font has no {} table", tag));
    }
  }
  if (tables["head"].size() < 54 || tables["hhea"].size() < 36 || tables["xloc"].size() < 8) {
    return fail("the XTT font's head, hhea or xloc table is too short");
  }

  const std::span<const uint8_t> xglf = tables["xglf"];
  const std::span<const uint8_t> xloc = tables["xloc"];
  const size_t glyph_count = xloc.size() / 4 - 1;
  std::map<uint32_t, std::vector<uint8_t>> blocks;
  auto block = [&](uint32_t index) -> const std::vector<uint8_t>* {
    auto it = blocks.find(index);
    if (it == blocks.end()) {
      const size_t at = size_t(index) * kGlyphBlockSize;
      if (at >= xglf.size()) {
        return nullptr;
      }
      auto inflated = Inflate(xglf.subspan(at, std::min(kGlyphBlockSize, xglf.size() - at)), 16384);
      if (!inflated) {
        return nullptr;
      }
      it = blocks.emplace(index, std::move(*inflated)).first;
    }
    return &it->second;
  };
  std::vector<uint8_t> glyf;
  std::vector<uint32_t> loca;
  loca.reserve(glyph_count + 1);
  for (size_t g = 0; g < glyph_count; ++g) {
    const uint32_t start = Be32(xloc, g * 4);
    const uint32_t next = Be32(xloc, g * 4 + 4);
    loca.push_back(uint32_t(glyf.size()));
    const std::vector<uint8_t>* data = block(start >> 16);
    if (!data) {
      return fail(fmt::format("the XTT font's glyph block {} does not decompress", start >> 16));
    }
    const size_t begin = start & 0xFFFF;
    const size_t end = (next >> 16) == (start >> 16) ? (next & 0xFFFF) : data->size();
    if (begin > end || end > data->size()) {
      return fail(fmt::format("the XTT font's glyph {} is out of range", g));
    }
    glyf.insert(glyf.end(), data->begin() + begin, data->begin() + end);
    glyf.resize((glyf.size() + 3) & ~size_t(3), 0);
  }
  loca.push_back(uint32_t(glyf.size()));

  std::vector<uint8_t> head(tables["head"].begin(), tables["head"].end());
  Set32(head, 8, 0);
  Set16(head, 50, 1);
  std::vector<uint8_t> hhea(tables["hhea"].begin(), tables["hhea"].end());
  if (Be16(hhea, 34) > glyph_count) {
    Set16(hhea, 34, uint16_t(glyph_count));
  }
  std::vector<uint8_t> loca_table;
  loca_table.reserve(loca.size() * 4);
  for (uint32_t offset : loca) {
    Put32(loca_table, offset);
  }

  std::map<std::string, std::vector<uint8_t>> out_tables;
  for (const auto& [tag, table] : tables) {
    if (!tag.starts_with('x') && tag != "glyf" && tag != "loca") {
      out_tables[tag].assign(table.begin(), table.end());
    }
  }
  if (!out_tables.contains("OS/2")) {
    out_tables["OS/2"] = BuildOs2(head, hhea);
  }
  if (!out_tables.contains("maxp")) {
    out_tables["maxp"] = BuildMaxp(glyf, loca);
  }
  if (!out_tables.contains("post")) {
    out_tables["post"] = BuildPost();
  }
  out_tables["head"] = std::move(head);
  out_tables["hhea"] = std::move(hhea);
  out_tables["glyf"] = std::move(glyf);
  out_tables["loca"] = std::move(loca_table);

  const uint16_t count = uint16_t(out_tables.size());
  uint16_t power = 1, log2 = 0;
  while (power * 2 <= count) {
    power *= 2;
    ++log2;
  }
  std::vector<uint8_t> font;
  Put32(font, 0x00010000);
  Put16(font, count);
  Put16(font, uint16_t(power * 16));
  Put16(font, log2);
  Put16(font, uint16_t(count * 16 - power * 16));
  size_t data_at = 12 + size_t(count) * 16;
  size_t head_at = 0;
  for (const auto& [tag, table] : out_tables) {
    font.insert(font.end(), tag.begin(), tag.end());
    Put32(font, Checksum(table));
    Put32(font, uint32_t(data_at));
    Put32(font, uint32_t(table.size()));
    if (tag == "head") {
      head_at = data_at;
    }
    data_at += (table.size() + 3) & ~size_t(3);
  }
  for (const auto& [tag, table] : out_tables) {
    font.insert(font.end(), table.begin(), table.end());
    font.resize((font.size() + 3) & ~size_t(3), 0);
  }
  Set32(font, head_at + 8, 0xB1B0AFBA - Checksum(font));
  return font;
}

}
