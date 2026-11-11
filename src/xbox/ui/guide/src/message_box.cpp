// Copyright (c) 2026 Xbox Guide contributors. BSD 3-Clause License; see LICENSE.
#include <rex/ui/guide/message_box.h>
#include <fmt/format.h>

namespace rex::ui::guide {
std::unique_ptr<MessageBoxScene> MessageBoxScene::Create(const xui::SceneContext& context,
                                                         std::string title, std::string body,
                                                         std::span<const std::string> choices,
                                                         size_t initial_choice,
                                                         std::string_view visual_name) {
  if (!context.skin || choices.empty() || choices.size() > 4 || initial_choice >= choices.size())
    return nullptr;
  const xui::Node* visual = nullptr;
  for (const auto& node : context.skin->root.children)
    if (node.id() == visual_name) {
      visual = &node;
      break;
    }
  if (!visual || visual->children.empty())
    return nullptr;
  auto box = std::unique_ptr<MessageBoxScene>(new MessageBoxScene);
  box->context_ = context;
  box->root_ = xui::Element::Create(visual->children.front(), box->context_);
  if (!box->root_ || !box->root_->FindById("MessageText"))
    return nullptr;
  for (size_t i = 0; i < choices.size(); ++i) {
    auto* button = box->root_->FindById(fmt::format("Button{}", i));
    if (!button)
      return nullptr;
    button->SetVisible(true);
    if (!button->focusable())
      return nullptr;
    button->SetText(choices[i]);
    box->choices_.push_back(button);
  }
  for (size_t i = choices.size(); i < 4; ++i)
    if (auto* button = box->root_->FindById(fmt::format("Button{}", i)))
      button->SetVisible(false);
  for (const auto* id : {"Icon", "ProgressAnimation"})
    if (auto* element = box->root_->FindById(id))
      element->SetVisible(false);
  box->root_->SetText(std::move(title));
  box->SetBody(std::move(body));
  box->focused_ = initial_choice;
  xui::Element::MoveFocus(nullptr, box->choices_[initial_choice], true);
  return box;
}
void MessageBoxScene::SetBody(std::string body) {
  root_->FindById("MessageText")->SetText(std::move(body));
}
void MessageBoxScene::SetEnabled(size_t choice, bool enabled) {
  if (choice < choices_.size())
    choices_[choice]->Set("Enabled", xui::Value{enabled});
}
void MessageBoxScene::Move(int direction) {
  if (!direction)
    return;
  const auto next = (focused_ + (direction > 0 ? 1 : choices_.size() - 1)) % choices_.size();
  Focus(next);
}
void MessageBoxScene::Focus(size_t choice) {
  if (choice >= choices_.size() || choice == focused_)
    return;
  xui::Element::MoveFocus(choices_[focused_], choices_[choice]);
  focused_ = choice;
}
std::optional<size_t> MessageBoxScene::Activate() {
  auto* choice = choices_[focused_];
  choice->Press();
  return choice->enabled() ? std::optional(focused_) : std::nullopt;
}
}
