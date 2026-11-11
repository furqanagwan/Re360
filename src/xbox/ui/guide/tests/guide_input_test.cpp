/**
 * @file        ui/guide/tests/guide_input_test.cpp
 * @brief       Opening the Xbox guide and driving it from a pad (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <catch2/catch_test_macros.hpp>

#include <vector>

#include <rex/input/input.h>
#include <rex/cvar.h>
#include <rex/ui/guide/guide_input.h>
#include <rex/ui/guide/code_patch_states.h>

using namespace rex::ui::guide;
using namespace rex::input;

namespace {
constexpr uint16_t kBack = X_INPUT_GAMEPAD_BACK;
constexpr uint16_t kStart = X_INPUT_GAMEPAD_START;
}

TEST_CASE("Back and Start together open the guide once per press", "[guide]") {
  GuideChord chord;
  CHECK_FALSE(chord.Update(0));
  CHECK_FALSE(chord.Update(kBack));
  CHECK(chord.Update(kBack | kStart));
  CHECK_FALSE(chord.Update(kBack | kStart));

  CHECK_FALSE(chord.Update(kStart));
  CHECK_FALSE(chord.Update(kBack | kStart));
  CHECK_FALSE(chord.Update(0));
  CHECK(chord.Update(kStart | kBack));
}

TEST_CASE("The Xbox button does not open the guide (it is Game Bar's)", "[guide]") {
  GuideChord chord;
  chord.Update(0);
  CHECK_FALSE(chord.Update(X_INPUT_GAMEPAD_GUIDE));
  CHECK_FALSE(chord.Update(X_INPUT_GAMEPAD_GUIDE | kBack));
  CHECK(chord.Update(X_INPUT_GAMEPAD_GUIDE | kBack | kStart));
}

TEST_CASE("A chord held when polling starts does not fire", "[guide]") {
  GuideChord chord;
  CHECK_FALSE(chord.Update(kBack | kStart));
  CHECK_FALSE(chord.Update(0));
  CHECK(chord.Update(kBack | kStart));
}

TEST_CASE("Buttons held when the guide opens are ignored until released", "[guide]") {
  GuidePad pad(kBack | kStart | X_INPUT_GAMEPAD_A);
  CHECK(pad.Update(kBack | kStart | X_INPUT_GAMEPAD_A, 0, 0, 0).empty());
  CHECK(pad.Update(0, 0, 0, 10).empty());
  CHECK(pad.Update(X_INPUT_GAMEPAD_A, 0, 0, 20) == std::vector<GuideAction>{GuideAction::kA});
  CHECK(pad.Update(X_INPUT_GAMEPAD_A, 0, 0, 30).empty());
}

TEST_CASE("Held directions repeat after a delay", "[guide]") {
  GuidePad pad(0);
  const uint16_t down = X_INPUT_GAMEPAD_DPAD_DOWN;
  CHECK(pad.Update(down, 0, 0, 0) == std::vector<GuideAction>{GuideAction::kDown});
  CHECK(pad.Update(down, 0, 0, GuidePad::kRepeatDelayMs - 1).empty());
  CHECK(pad.Update(down, 0, 0, GuidePad::kRepeatDelayMs).size() == 1);
  CHECK(pad.Update(down, 0, 0, GuidePad::kRepeatDelayMs + 10).empty());
  CHECK(pad.Update(down, 0, 0, GuidePad::kRepeatDelayMs + GuidePad::kRepeatIntervalMs).size() == 1);

  CHECK(pad.Update(0, -30000, 0, 2000) == std::vector<GuideAction>{GuideAction::kLeft});
  CHECK(pad.Update(0, 0, 0, 2010).empty());
}

TEST_CASE("Bumpers switch tabs", "[guide]") {
  GuidePad pad(0);
  CHECK(pad.Update(X_INPUT_GAMEPAD_RIGHT_SHOULDER, 0, 0, 0) ==
        std::vector<GuideAction>{GuideAction::kNextTab});
  CHECK(pad.Update(X_INPUT_GAMEPAD_LEFT_SHOULDER, 0, 0, 10) ==
        std::vector<GuideAction>{GuideAction::kPreviousTab});
}

TEST_CASE("Switchable patch states are saved and restored by name", "[guide]") {
  uint8_t flags[2] = {0, 1};
  const rex::PPCSwitchablePatch patches[] = {
      {"Unlock FPS", &flags[0], "patch"}, {"Ammo", &flags[1], "mod"}, {nullptr, nullptr, nullptr}};
  REQUIRE(rex::cvar::SetFlagByName("code_patch_states", "Unlock FPS=1;Gone=0"));
  ApplySavedCodePatches(patches);
  CHECK(flags[0] == 1);
  CHECK(flags[1] == 1);
  flags[1] = 0;
  CHECK(SaveCodePatchStates(patches) == "Unlock FPS=1;Ammo=0");
  rex::cvar::SetFlagByName("code_patch_states", "");
}
