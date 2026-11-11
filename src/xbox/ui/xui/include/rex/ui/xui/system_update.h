/**
 * @file        ui/xui/include/rex/ui/xui/system_update.h
 * @brief       The dashboard XUI packages, read from the user's own system update (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <rex/ui/xui/package.h>

namespace rex::ui::xui {

struct XexResource {
  std::string name;
  std::vector<uint8_t> bytes;
};

std::optional<std::vector<XexResource>> ReadXexResources(std::span<const uint8_t> xex,
                                                         std::string* error);

class SystemUpdate {
 public:
  static constexpr std::string_view kModules[] = {"hud", "huduiskin", "xam", "gamerprofile", "vk"};

  static constexpr std::string_view kFonts[] = {"xenonjklatin", "xenonclatin", "xenonsclatin",
                                                "SegoeXbox-Light"};

  using Modules = std::map<std::string, std::vector<uint8_t>>;

  static std::unique_ptr<SystemUpdate> Load(const std::filesystem::path& path, std::string* error);

  static std::optional<Modules> ReadModules(const std::filesystem::path& path, std::string* error);

  static std::optional<Modules> ReadModules(std::span<const std::filesystem::path> paths,
                                            std::string* error);

  static std::unique_ptr<SystemUpdate> FromModules(const Modules& modules, std::string* error);

  static std::vector<uint8_t> WriteBundle(const Modules& modules);
  static std::optional<Modules> ReadBundle(std::span<const uint8_t> bundle, std::string* error);

  bool AddModule(std::string_view module, std::span<const uint8_t> xex, std::string* error);

  bool AddFont(std::string_view name, std::span<const uint8_t> xtt, std::string* error);

  const Package* Find(std::string_view module_resource) const;
  const std::map<std::string, Package>& packages() const { return packages_; }

  std::span<const uint8_t> Font(std::string_view name) const;

 private:
  std::map<std::string, Package> packages_;
  std::map<std::string, std::vector<uint8_t>> fonts_;
};

}
