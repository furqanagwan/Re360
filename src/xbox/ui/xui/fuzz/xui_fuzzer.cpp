// Copyright (c) 2026 ReXGlue contributors. BSD 3-Clause License; see LICENSE.
#include <cstdint>
#include <span>
#include <string>
#include <vector>

#include <rex/ui/xui/document.h>
#include <rex/ui/xui/package.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  using namespace rex::ui::xui;  // NOLINT
  const std::span<const uint8_t> bytes(data, size);
  std::string error;
  ParseXur(bytes, &error);
  ParseStringTable(bytes, &error);
  auto package = Package::Parse(std::vector<uint8_t>(data, data + size), &error);
  if (!package) {
    return 0;
  }
  for (const auto& entry : package->entries()) {
    const auto contents = package->Find(entry.name);
    ParseXur(contents, &error);
    ParseStringTable(contents, &error);
  }
  return 0;
}
