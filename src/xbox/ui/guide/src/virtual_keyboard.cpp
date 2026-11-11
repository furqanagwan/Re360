/**
 * @file        ui/guide/src/virtual_keyboard.cpp
 * @brief       The console's on-screen keyboard: layouts and editing (RG-GDK-059)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/guide/virtual_keyboard.h>

#include <algorithm>

namespace rex::ui::guide {
namespace {

using namespace std::string_view_literals;

constexpr std::u16string_view kLayouts[VirtualKeyboard::kPages] = {
    u"1234567890"
    u"qwertyuiop"
    u"asdfghjkl-"
    u"zxcvbnm_@."
    u"\0\0\0\0\0\0\0\0\0\0"sv,
    u"1234567890"
    u",;:'\"!?¡¿%"
    u"[]{}`$£«»#"
    u"<>()€¥¤~^\\"
    u"|=*/+-@_&."sv,
    u"1234567890"
    u"àáâãäåñèéê"
    u"ëþçýìíîïùú"
    u"ûòóôõðø×œæ"
    u"ßÿºª.üµš_ö"sv,
};
static_assert(kLayouts[0].size() == VirtualKeyboard::kRows * VirtualKeyboard::kColumns);
static_assert(kLayouts[1].size() == VirtualKeyboard::kRows * VirtualKeyboard::kColumns);
static_assert(kLayouts[2].size() == VirtualKeyboard::kRows * VirtualKeyboard::kColumns);

char16_t Upper(char16_t c) {
  if ((c >= u'a' && c <= u'z') || (c >= 0xE0 && c <= 0xFE && c != 0xF7)) {
    return char16_t(c - 0x20);
  }
  if (c == 0xFF) {
    return 0x178;
  }
  if (c >= 0x100 && c <= 0x17F) {
    // Pairs, capital first, except 0x139-0x148 and 0x179-0x17E, which start
    // one later.
    const bool odd_capitals = (c >= 0x139 && c <= 0x148) || (c >= 0x179 && c <= 0x17E);
    const bool lower = odd_capitals ? (c % 2 == 0) : (c % 2 == 1);
    if (lower && c != 0x131 && c != 0x138 && c != 0x149 && c != 0x17F) {
      return char16_t(c - 1);
    }
  }
  return c;
}

}  // namespace

VirtualKeyboard::VirtualKeyboard(std::u16string text, size_t max_length)
    : text_(std::move(text)), max_length_(max_length) {
  if (text_.size() > max_length_) {
    text_.resize(max_length_);
  }
  cursor_ = text_.size();
}

char16_t VirtualKeyboard::KeyAt(int row, int column) const {
  if (row < 0 || row >= kRows || column < 0 || column >= kColumns) {
    return 0;
  }
  const char16_t c = kLayouts[int(page_)][size_t(row) * kColumns + column];
  // Caps gives the capital of a letter that has one.
  return caps_ ? Upper(c) : c;
}

std::string_view VirtualKeyboard::PageName(Page page) {
  switch (page) {
    case Page::kAlphabet:
      return "Alphabet";
    case Page::kSymbols:
      return "Symbols";
    case Page::kAccents:
      return "Accents";
  }
  return {};
}

bool VirtualKeyboard::Type(int row, int column) {
  const char16_t c = KeyAt(row, column);
  return c && Insert(c);
}

bool VirtualKeyboard::Insert(char16_t c) {
  if (text_.size() >= max_length_) {
    return false;
  }
  text_.insert(text_.begin() + ptrdiff_t(cursor_), c);
  ++cursor_;
  return true;
}

bool VirtualKeyboard::Backspace() {
  if (!cursor_) {
    return false;
  }
  text_.erase(--cursor_, 1);
  return true;
}

void VirtualKeyboard::MoveCursor(int delta) {
  cursor_ = size_t(std::clamp(ptrdiff_t(cursor_) + delta, ptrdiff_t(0), ptrdiff_t(text_.size())));
}

}  // namespace rex::ui::guide
