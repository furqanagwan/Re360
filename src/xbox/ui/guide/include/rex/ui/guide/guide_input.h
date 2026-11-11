/**
 * @file        ui/guide/include/rex/ui/guide/guide_input.h
 * @brief       Opening the Xbox guide and driving it from a pad (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <cstdint>
#include <vector>

namespace rex::ui::guide {

class GuideChord {
 public:
  bool Update(uint16_t buttons);

 private:
  bool latched_ = true;
};

enum class GuideAction : uint8_t {
  kUp,
  kDown,
  kLeft,
  kRight,
  kA,
  kB,
  kX,
  kY,
  kPreviousTab,
  kNextTab,
  kStart,
  kLeftThumb,
  kLeftTrigger,
  kRightTrigger,
};

class GuidePad {
 public:
  static constexpr uint64_t kRepeatDelayMs = 400;
  static constexpr uint64_t kRepeatIntervalMs = 110;
  static constexpr int16_t kStickThreshold = 16384;
  static constexpr uint8_t kTriggerThreshold = 128;

  explicit GuidePad(uint16_t held_buttons = 0xFFFF);

  std::vector<GuideAction> Update(uint16_t buttons, int16_t thumb_lx, int16_t thumb_ly,
                                  uint64_t now_ms, uint8_t left_trigger = 0,
                                  uint8_t right_trigger = 0);

 private:
  uint16_t previous_ = 0;
  uint16_t ignored_ = 0;
  int direction_ = -1;
  uint64_t next_repeat_ms_ = 0;

  bool left_trigger_ = true, right_trigger_ = true;
};

}
