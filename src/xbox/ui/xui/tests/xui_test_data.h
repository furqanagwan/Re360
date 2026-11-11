/**
 * @file        ui/xui/tests/xui_test_data.h
 * @brief       Byte builders for XUIZ, XUIS and XEX2 test data (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace xui_test {

using Bytes = std::vector<uint8_t>;

inline void Be16(Bytes& out, uint16_t v) {
  out.push_back(uint8_t(v >> 8));
  out.push_back(uint8_t(v));
}

inline void Be32(Bytes& out, uint32_t v) {
  for (int shift = 24; shift >= 0; shift -= 8) {
    out.push_back(uint8_t(v >> shift));
  }
}

inline void Packed(Bytes& out, uint32_t v) {
  if (v < 0xF0) {
    out.push_back(uint8_t(v));
  } else if (v <= 0xFFF) {
    out.push_back(uint8_t(0xF0 | (v >> 8)));
    out.push_back(uint8_t(v));
  } else {
    out.push_back(0xFF);
    Be32(out, v);
  }
}

inline void Append(Bytes& out, const Bytes& more) {
  out.insert(out.end(), more.begin(), more.end());
}

inline Bytes Xuiz(const std::vector<std::pair<std::string, std::string>>& files) {
  Bytes table, data;
  for (const auto& [name, contents] : files) {
    Be32(table, uint32_t(contents.size()));
    Be32(table, uint32_t(data.size()));
    table.push_back(uint8_t(name.size()));
    table.insert(table.end(), name.begin(), name.end());
    data.insert(data.end(), contents.begin(), contents.end());
  }
  Bytes out;
  Be32(out, 0x5855495A);
  Be32(out, 3);
  Be32(out, uint32_t(0x16 + table.size() + data.size()));
  Be32(out, 0);
  Be32(out, uint32_t(table.size()));
  Be16(out, uint16_t(files.size()));
  Append(out, table);
  Append(out, data);
  return out;
}

inline Bytes Xuis(const std::vector<std::string>& strings) {
  Bytes body;
  for (const std::string& s : strings) {
    body.insert(body.end(), s.begin(), s.end());
    body.push_back(0);
  }
  Bytes out;
  Be32(out, 0x58554953);
  Be16(out, 0x0202);
  Be32(out, uint32_t(12 + body.size()));
  Be16(out, uint16_t(strings.size()));
  Append(out, body);
  return out;
}

inline Bytes XexWithResource(const std::string& resource_name, const Bytes& resource,
                             uint16_t compression, uint16_t encryption = 0) {
  const uint32_t load = 0x82000000;
  Bytes image(0x100, 0xCC);
  Append(image, resource);
  image.resize(image.size() + 0x10, 0);

  Bytes out;
  const uint32_t header_size = 0x400;
  Be32(out, 0x58455832);
  Be32(out, 0);
  Be32(out, header_size);
  Be32(out, 0);
  Be32(out, 0x200);
  Be32(out, 2);
  Be32(out, 0x2FF);
  Be32(out, 0x100);
  Be32(out, 0x3FF);
  Be32(out, 0x180);
  out.resize(0x100, 0);
  Be32(out, 4 + 16);
  std::string name = resource_name;
  name.resize(8, '\0');
  out.insert(out.end(), name.begin(), name.end());
  Be32(out, load + 0x100);
  Be32(out, uint32_t(resource.size()));
  out.resize(0x180, 0);
  if (compression == 1) {
    Be32(out, 16);
    Be16(out, encryption);
    Be16(out, 1);
    Be32(out, uint32_t(image.size() - 0x10));
    Be32(out, 0x10);
  } else {
    Be32(out, 8);
    Be16(out, encryption);
    Be16(out, 0);
  }
  out.resize(0x200, 0);
  Be32(out, 0);
  Be32(out, uint32_t(image.size()));
  out.resize(0x200 + 0x110, 0);
  Be32(out, load);
  out.resize(header_size, 0);
  if (compression == 1) {
    out.insert(out.end(), image.begin(), image.end() - 0x10);
  } else {
    Append(out, image);
  }
  return out;
}

}
