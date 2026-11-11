/**
 * @file        ui/xui/include/rex/ui/xui/package.h
 * @brief       XUIZ (.xzp) packages and XUIS (.xus) string tables (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace rex::ui::xui {

class Package {
 public:
  struct Entry {
    std::string name;
    uint32_t offset = 0;
    uint32_t size = 0;
  };

  static std::optional<Package> Parse(std::vector<uint8_t> bytes, std::string* error);

  std::span<const uint8_t> Find(std::string_view name) const;
  bool Contains(std::string_view name) const { return FindEntry(name) != nullptr; }
  const std::vector<Entry>& entries() const { return entries_; }

 private:
  const Entry* FindEntry(std::string_view name) const;

  std::vector<uint8_t> bytes_;
  std::vector<Entry> entries_;
};

std::optional<std::vector<std::string>> ParseStringTable(std::span<const uint8_t> bytes,
                                                         std::string* error);

bool SamePath(std::string_view a, std::string_view b);

}
