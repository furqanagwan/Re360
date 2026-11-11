/**
 * @file        ui/xui/include/rex/ui/xui/text_scroll.h
 * @brief       Scrolling for text taller than its box, as the console's long descriptions
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <string>

namespace rex::ui::xui {

struct TextScroll {
  std::string text;
  double start = 0;
};
struct TextScrollFrame {
  float offset = 0;
  float alpha = 1;
};

TextScrollFrame TextScrollAt(double elapsed, float overflow, float speed);

}
