/**
 * @file        ui/guide/src/guide_input.cpp
 * @brief       Opening the Xbox guide and driving it from a pad (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/guide/guide_input.h>

#include <rex/input/input.h>

namespace rex::ui::guide {
namespace {

using namespace rex::input;

constexpr uint16_t kChordButtons = X_INPUT_GAMEPAD_BACK | X_INPUT_GAMEPAD_START;

}

bool GuideChord::Update(uint16_t buttons) {
  const bool chord = (buttons & kChordButtons) == kChordButtons;
  if (!(buttons & kChordButtons)) {
    latched_ = false;
  }
  if (chord && !latched_) {
    latched_ = true;
    return true;
  }
  return false;
}

GuidePad::GuidePad(uint16_t held_buttons) : previous_(held_buttons), ignored_(held_buttons) {}

std::vector<GuideAction> GuidePad::Update(uint16_t buttons, int16_t thumb_lx, int16_t thumb_ly,
                                          uint64_t now_ms, uint8_t left_trigger,
                                          uint8_t right_trigger) {
  std::vector<GuideAction> actions;
  ignored_ &= buttons;
  const uint16_t held = buttons & ~ignored_;
  const uint16_t pressed = held & ~previous_;
  previous_ = held;

  struct Press {
    uint16_t button;
    GuideAction action;
  };
  constexpr Press kPresses[] = {
      {X_INPUT_GAMEPAD_A, GuideAction::kA},
      {X_INPUT_GAMEPAD_B, GuideAction::kB},
      {X_INPUT_GAMEPAD_X, GuideAction::kX},
      {X_INPUT_GAMEPAD_Y, GuideAction::kY},
      {X_INPUT_GAMEPAD_LEFT_SHOULDER, GuideAction::kPreviousTab},
      {X_INPUT_GAMEPAD_RIGHT_SHOULDER, GuideAction::kNextTab},
      {X_INPUT_GAMEPAD_START, GuideAction::kStart},
      {X_INPUT_GAMEPAD_LEFT_THUMB, GuideAction::kLeftThumb},
  };
  for (const Press& press : kPresses) {
    if (pressed & press.button) {
      actions.push_back(press.action);
    }
  }

  auto trigger = [&](uint8_t value, bool& was_down, GuideAction action) {
    const bool down = value >= kTriggerThreshold;
    if (down && !was_down) {
      actions.push_back(action);
    }
    was_down = down;
  };
  trigger(left_trigger, left_trigger_, GuideAction::kLeftTrigger);
  trigger(right_trigger, right_trigger_, GuideAction::kRightTrigger);

  int direction = -1;
  if (held & X_INPUT_GAMEPAD_DPAD_UP) {
    direction = int(GuideAction::kUp);
  } else if (held & X_INPUT_GAMEPAD_DPAD_DOWN) {
    direction = int(GuideAction::kDown);
  } else if (held & X_INPUT_GAMEPAD_DPAD_LEFT) {
    direction = int(GuideAction::kLeft);
  } else if (held & X_INPUT_GAMEPAD_DPAD_RIGHT) {
    direction = int(GuideAction::kRight);
  } else if (thumb_ly > kStickThreshold) {
    direction = int(GuideAction::kUp);
  } else if (thumb_ly < -kStickThreshold) {
    direction = int(GuideAction::kDown);
  } else if (thumb_lx < -kStickThreshold) {
    direction = int(GuideAction::kLeft);
  } else if (thumb_lx > kStickThreshold) {
    direction = int(GuideAction::kRight);
  }
  if (direction != direction_) {
    direction_ = direction;
    if (direction >= 0) {
      actions.push_back(GuideAction(direction));
      next_repeat_ms_ = now_ms + kRepeatDelayMs;
    }
  } else if (direction >= 0 && now_ms >= next_repeat_ms_) {
    actions.push_back(GuideAction(direction));
    next_repeat_ms_ = now_ms + kRepeatIntervalMs;
  }
  return actions;
}

}
