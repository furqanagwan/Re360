/**
 * @file        ui/xui/include/rex/ui/xui/schema.h
 * @brief       XUI class and property order used to decode XUR v8 (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

namespace rex::ui::xui {

enum class PropType : uint8_t {
  kBool,
  kInteger,
  kUnsigned,
  kString,
  kFloat,
  kVector,
  kQuaternion,
  kColor,
  kObject,
  kCustom,
};

struct ClassDef;

struct PropDef {
  std::string_view name;
  PropType type;
  bool indexed = false;
};

struct ClassDef {
  std::string_view name;
  std::string_view base;
  std::span<const PropDef> props;
};

const ClassDef* FindClass(std::string_view name);

std::vector<const ClassDef*> ClassChain(const ClassDef* derived);

const ClassDef* CompoundClass(std::string_view property_name);

}
