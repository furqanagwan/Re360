/**
 * Copyright (c) 2026 Tom Clay
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <rex/ui/xui/runtime.h>
#include <rex/ui/xui/system_update.h>

namespace rex::ui::xui {
namespace {

constexpr std::string_view kFallbackPackages[] = {
    "xam/skin", "huduiskin/skin", "xam/xam", "xam/shrdres", "hud/hud", "gamerprofile/gp"};

}

std::span<const uint8_t> ResolveFile(const SystemUpdate& update, std::string_view path,
                                     std::string_view base_package) {
  struct Protocol {
    std::string_view prefix;
    std::string_view package;
  };
  constexpr Protocol kProtocols[] = {
      {"sharedres://", "xam/shrdres"}, {"xam://", "xam/xam"}, {"skin://", "xam/skin"}};
  for (const Protocol& protocol : kProtocols) {
    if (path.starts_with(protocol.prefix)) {
      const Package* package = update.Find(protocol.package);
      return package ? package->Find(path.substr(protocol.prefix.size()))
                     : std::span<const uint8_t>();
    }
  }
  if (const Package* base = update.Find(base_package)) {
    if (auto bytes = base->Find(path); !bytes.empty()) {
      return bytes;
    }
  }
  for (std::string_view fallback : kFallbackPackages) {
    if (const Package* package = update.Find(fallback)) {
      if (auto bytes = package->Find(path); !bytes.empty()) {
        return bytes;
      }
    }
  }
  return {};
}

}
