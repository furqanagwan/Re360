/**
 * @file        ui/xui/src/package.cpp
 * @brief       XUIZ (.xzp) packages and XUIS (.xus) string tables (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/xui/package.h>

#include <fmt/format.h>

namespace rex::ui::xui {
namespace {

uint32_t ReadBe32(const uint8_t* p) {
  return uint32_t(p[0]) << 24 | uint32_t(p[1]) << 16 | uint32_t(p[2]) << 8 | p[3];
}

uint16_t ReadBe16(const uint8_t* p) {
  return uint16_t(p[0] << 8 | p[1]);
}

char FoldPathChar(char c) {
  if (c == '\\') {
    return '/';
  }
  return (c >= 'A' && c <= 'Z') ? char(c - 'A' + 'a') : c;
}

constexpr uint32_t kXuizMagic = 0x5855495A;
constexpr size_t kTableStart = 0x16;

constexpr uint32_t kXuisMagic = 0x58554953;
constexpr size_t kXuisHeaderSize = 12;

}

bool SamePath(std::string_view a, std::string_view b) {
  if (a.size() != b.size()) {
    return false;
  }
  for (size_t i = 0; i < a.size(); ++i) {
    if (FoldPathChar(a[i]) != FoldPathChar(b[i])) {
      return false;
    }
  }
  return true;
}

std::optional<Package> Package::Parse(std::vector<uint8_t> bytes, std::string* error) {
  auto fail = [&](std::string message) -> std::optional<Package> {
    if (error) {
      *error = std::move(message);
    }
    return std::nullopt;
  };
  if (bytes.size() < kTableStart || ReadBe32(bytes.data()) != kXuizMagic) {
    return fail("not a XUIZ package");
  }
  const uint32_t table_size = ReadBe32(bytes.data() + 0x10);
  const uint16_t count = ReadBe16(bytes.data() + 0x14);
  const size_t data_start = kTableStart + size_t(table_size);
  if (data_start > bytes.size()) {
    return fail("XUIZ entry table runs past the end");
  }
  Package package;
  size_t pos = kTableStart;
  for (uint16_t i = 0; i < count; ++i) {
    if (pos + 9 > data_start) {
      return fail(fmt::format("XUIZ entry {} runs past the table", i));
    }
    Entry entry;
    entry.size = ReadBe32(bytes.data() + pos);
    entry.offset = ReadBe32(bytes.data() + pos + 4);
    const uint8_t name_length = bytes[pos + 8];
    pos += 9;
    if (pos + name_length > data_start) {
      return fail(fmt::format("XUIZ entry {} name runs past the table", i));
    }
    entry.name.assign(reinterpret_cast<const char*>(bytes.data() + pos), name_length);
    pos += name_length;
    if (uint64_t(data_start) + entry.offset + entry.size > bytes.size()) {
      return fail(fmt::format("XUIZ file {} runs past the end", entry.name));
    }
    entry.offset += uint32_t(data_start);
    package.entries_.push_back(std::move(entry));
  }
  package.bytes_ = std::move(bytes);
  return package;
}

const Package::Entry* Package::FindEntry(std::string_view name) const {
  for (const Entry& entry : entries_) {
    if (SamePath(entry.name, name)) {
      return &entry;
    }
  }
  return nullptr;
}

std::span<const uint8_t> Package::Find(std::string_view name) const {
  const Entry* entry = FindEntry(name);
  if (!entry) {
    return {};
  }
  return std::span<const uint8_t>(bytes_.data() + entry->offset, entry->size);
}

std::optional<std::vector<std::string>> ParseStringTable(std::span<const uint8_t> bytes,
                                                         std::string* error) {
  auto fail = [&](std::string message) -> std::optional<std::vector<std::string>> {
    if (error) {
      *error = std::move(message);
    }
    return std::nullopt;
  };

  if (bytes.size() < kXuisHeaderSize || ReadBe32(bytes.data()) != kXuisMagic) {
    return fail("not a XUIS string table");
  }
  const uint16_t count = ReadBe16(bytes.data() + 10);
  std::vector<std::string> strings;
  strings.reserve(count);
  size_t pos = kXuisHeaderSize;
  for (uint16_t i = 0; i < count; ++i) {
    size_t end = pos;
    while (end < bytes.size() && bytes[end] != 0) {
      ++end;
    }
    if (end >= bytes.size()) {
      return fail(fmt::format("XUIS string {} is not terminated", i));
    }
    strings.emplace_back(reinterpret_cast<const char*>(bytes.data() + pos), end - pos);
    pos = end + 1;
  }
  return strings;
}

}
