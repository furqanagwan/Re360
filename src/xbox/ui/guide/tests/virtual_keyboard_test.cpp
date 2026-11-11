/**
 * @file        ui/guide/tests/virtual_keyboard_test.cpp
 * @brief       The console's on-screen keyboard: layouts and editing (RG-GDK-059)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <catch2/catch_test_macros.hpp>

#include <rex/input/input.h>
#include <rex/ui/guide/guide_input.h>
#include <rex/ui/guide/virtual_keyboard.h>

using rex::ui::guide::GuideAction;
using rex::ui::guide::GuidePad;
using rex::ui::guide::VirtualKeyboard;
using Page = VirtualKeyboard::Page;

TEST_CASE("The keyboard's pages are the console's English layouts", "[guide][keyboard]") {
  VirtualKeyboard keyboard(u"", 10);
  CHECK(keyboard.KeyAt(0, 0) == u'1');
  CHECK(keyboard.KeyAt(1, 0) == u'q');
  CHECK(keyboard.KeyAt(2, 9) == u'-');
  CHECK(keyboard.KeyAt(3, 9) == u'.');
  CHECK(keyboard.KeyAt(5, 0) == 0);
  CHECK(keyboard.KeyAt(0, 10) == 0);

  keyboard.GoToPage(keyboard.NextPage());
  CHECK(keyboard.page() == Page::kSymbols);
  CHECK(keyboard.KeyAt(1, 0) == u',');
  CHECK(keyboard.KeyAt(3, 4) == u'€');
  keyboard.GoToPage(keyboard.NextPage());
  CHECK(keyboard.page() == Page::kAccents);
  CHECK(keyboard.KeyAt(1, 0) == u'à');
  CHECK(keyboard.NextPage() == Page::kAlphabet);
  CHECK(keyboard.PreviousPage() == Page::kSymbols);
  CHECK(VirtualKeyboard::PageName(Page::kAccents) == "Accents");

  // Caps gives capitals where there are any.
  keyboard.ToggleCaps();
  CHECK(keyboard.KeyAt(1, 0) == u'À');
  CHECK(keyboard.KeyAt(4, 0) == u'ß');  // ß has none
  CHECK(keyboard.KeyAt(0, 0) == u'1');
}

TEST_CASE("The keyboard edits at its cursor within the buffer's length", "[guide][keyboard]") {
  VirtualKeyboard keyboard(u"Bond", 6);
  CHECK(keyboard.cursor() == 4);
  CHECK(keyboard.Type(0, 6));  // 7
  CHECK(keyboard.text() == u"Bond7");
  keyboard.MoveCursor(-10);
  CHECK(keyboard.cursor() == 0);
  keyboard.ToggleCaps();
  CHECK(keyboard.Type(1, 1));  // W
  CHECK(keyboard.text() == u"WBond7");
  // Full: nothing more goes in.
  CHECK_FALSE(keyboard.Space());
  CHECK_FALSE(keyboard.Insert(u'x'));
  CHECK(keyboard.text() == u"WBond7");
  CHECK(keyboard.Backspace());
  CHECK(keyboard.text() == u"Bond7");
  CHECK_FALSE(keyboard.Backspace());  // nothing before the cursor
  keyboard.MoveCursor(+100);
  CHECK(keyboard.cursor() == 5);
  CHECK(keyboard.Space());
  CHECK(keyboard.text() == u"Bond7 ");

  // A default text longer than the buffer is cut.
  VirtualKeyboard short_one(u"Quantum", 3);
  CHECK(short_one.text() == u"Qua");
}

TEST_CASE("Start, the left stick and the triggers reach the keyboard", "[guide][keyboard]") {
  GuidePad pad(0);
  CHECK(pad.Update(rex::input::X_INPUT_GAMEPAD_START, 0, 0, 0) ==
        std::vector<GuideAction>{GuideAction::kStart});
  CHECK(pad.Update(rex::input::X_INPUT_GAMEPAD_LEFT_THUMB, 0, 0, 10) ==
        std::vector<GuideAction>{GuideAction::kLeftThumb});
  CHECK(pad.Update(0, 0, 0, 20).empty());
  // A trigger fires once as it passes half way.
  CHECK(pad.Update(0, 0, 0, 30, 200, 0) == std::vector<GuideAction>{GuideAction::kLeftTrigger});
  CHECK(pad.Update(0, 0, 0, 40, 255, 0).empty());
  CHECK(pad.Update(0, 0, 0, 50, 0, 140) == std::vector<GuideAction>{GuideAction::kRightTrigger});

  // A trigger held as the keyboard opens waits for its release.
  GuidePad held(0);
  CHECK(held.Update(0, 0, 0, 0, 255, 0).empty());
  CHECK(held.Update(0, 0, 0, 10, 0, 0).empty());
  CHECK(held.Update(0, 0, 0, 20, 255, 0) == std::vector<GuideAction>{GuideAction::kLeftTrigger});
}
