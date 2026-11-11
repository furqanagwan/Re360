/**
 * @file        ui/xui/include/rex/ui/xui/renderer.h
 * @brief       Draws live XUI elements onto an ImGui draw list (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <string_view>

#include <imgui.h>

#include <rex/ui/xui/text_scroll.h>

namespace rex::ui::xui {

class Element;

enum TextStyleFlags : uint32_t {
  kTextBold = 0x0001,
  kTextNoWrap = 0x0010,
  kTextRight = 0x0200,
  kTextCenter = 0x0400,
  kTextVerticalCenter = 0x1000,
  kTextEllipsis = 0x4000,
};

constexpr float kPointToSceneUnits = 1.6f;

inline constexpr std::string_view kGamerscoreGlyph = "\xEE\x80\x8A";
inline constexpr std::string_view kGamerscoreImage = "rex://gamerscore";

struct RenderResources {
  std::function<ImTextureID(std::string_view path, std::string_view package, int* width,
                            int* height)>
      texture;
  ImFont* regular_font = nullptr;
  ImFont* bold_font = nullptr;

  std::function<bool(ImDrawList& list, std::string_view path,
                     const std::function<ImVec2(ImVec2)>& to_screen, float opacity)>
      vector_image;

  std::shared_ptr<std::unordered_map<const Element*, TextScroll>> text_scroll =
      std::make_shared<std::unordered_map<const Element*, TextScroll>>();

  float pixels_per_point = 1.0f;
};

struct GradientSpan {
  float start = 0;
  float end = 0;
};

GradientSpan EdgeFadeOnScreen(GradientSpan fade, float pixels_per_unit);

void Render(ImDrawList* list, const Element& root, ImVec2 origin, float scale, float opacity,
            const RenderResources& resources);

}
