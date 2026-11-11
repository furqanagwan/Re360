// Copyright (c) 2026 Xbox Guide contributors. BSD 3-Clause License; see LICENSE.
#pragma once

#include <optional>
#include <span>
#include <rex/ui/xui/runtime.h>

namespace rex::ui::guide {

class MessageBoxScene {
 public:
  MessageBoxScene(const MessageBoxScene&) = delete;
  MessageBoxScene& operator=(const MessageBoxScene&) = delete;
  MessageBoxScene(MessageBoxScene&&) = delete;
  MessageBoxScene& operator=(MessageBoxScene&&) = delete;
  static std::unique_ptr<MessageBoxScene> Create(const xui::SceneContext& context,
                                                 std::string title, std::string body,
                                                 std::span<const std::string> choices,
                                                 size_t initial_choice = 0,
                                                 std::string_view visual = "XuiMessageBox3");
  xui::Element& root() { return *root_; }
  void Move(int direction);
  void Focus(size_t choice);
  void SetEnabled(size_t choice, bool enabled);
  void SetBody(std::string body);
  std::optional<size_t> Activate();
  size_t focused_choice() const { return focused_; }

 private:
  MessageBoxScene() = default;
  xui::SceneContext context_;
  std::unique_ptr<xui::Element> root_;
  std::vector<xui::Element*> choices_;
  size_t focused_ = 0;
};
}
