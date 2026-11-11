// Copyright (c) 2026 ReXGlue contributors. BSD 3-Clause License; see LICENSE.
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

#include "xui_test_data.h"

namespace {

bool Write(const std::filesystem::path& path, const xui_test::Bytes& bytes) {
  std::filesystem::create_directories(path.parent_path());
  std::ofstream out(path, std::ios::binary | std::ios::trunc);
  out.write(reinterpret_cast<const char*>(bytes.data()), std::streamsize(bytes.size()));
  return bool(out);
}

}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::fprintf(stderr, "usage: xui_fuzz_seeds OUTPUT_DIRECTORY\n");
    return 2;
  }
  const std::filesystem::path root = argv[1];
  const auto strings = xui_test::Xuis({"Seed", "Strings"});
  const std::string table(strings.begin(), strings.end());
  const bool written =
      Write(root / "strings.xus", strings) &&
      Write(root / "package.xzp", xui_test::Xuiz({{"skin/strings.xus", table}, {"empty.txt", ""}}));
  return written ? 0 : 1;
}
