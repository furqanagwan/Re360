/**
 * @file        ui/guide/include/rex/ui/guide/virtual_keyboard.h
 * @brief       The console's on-screen keyboard: layouts and editing (RG-GDK-059)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace rex::ui::guide {

class VirtualKeyboard {
 public:
  static constexpr int kRows = 5;
  static constexpr int kColumns = 10;

  enum class Page { kAlphabet, kSymbols, kAccents };
  static constexpr int kPages = 3;

  VirtualKeyboard(std::u16string text, size_t max_length);

  char16_t KeyAt(int row, int column) const;
  static std::string_view PageName(Page page);
  Page page() const { return page_; }

  Page PreviousPage() const { return Page((int(page_) + kPages - 1) % kPages); }
  Page NextPage() const { return Page((int(page_) + 1) % kPages); }
  bool caps() const { return caps_; }

  bool Type(int row, int column);
  bool Insert(char16_t c);
  bool Backspace();
  bool Space() { return Insert(u' '); }

  void MoveCursor(int delta);
  void ToggleCaps() { caps_ = !caps_; }
  void GoToPage(Page page) { page_ = page; }

  const std::u16string& text() const { return text_; }
  size_t cursor() const { return cursor_; }
  size_t max_length() const { return max_length_; }

 private:
  std::u16string text_;
  size_t max_length_;
  size_t cursor_;
  Page page_ = Page::kAlphabet;
  bool caps_ = false;
};

}
