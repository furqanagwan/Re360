/**
 * @file        ui/guide/include/rex/ui/guide/xbox_keyboard.h
 * @brief       The console's own on-screen keyboard for XamShowKeyboardUI (RG-GDK-059)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string>

#include <rex/ui/guide/guide_input.h>
#include <rex/ui/guide/virtual_keyboard.h>
#include <rex/ui/guide/xbox_guide.h>
#include <rex/ui/imgui_dialog.h>
#include <rex/ui/xui/renderer.h>
#include <rex/ui/xui/runtime.h>

namespace rex::input {
class InputSystem;
}

namespace rex::ui::guide {

class XboxKeyboard final : public ImGuiDialog {
 public:
  struct Request {
    std::u16string title, description, default_text;
    size_t max_length = 0;
  };
  using Done = std::function<void(std::optional<std::u16string>)>;

  XboxKeyboard(ImGuiDrawer* drawer, std::shared_ptr<const GuideAssets> assets, GuideMedia* media,
               GuideFonts fonts, input::InputSystem* input, uint32_t user_index, Request request,
               Done done);

 protected:
  void OnDraw(ImGuiIO& io) override;
  void OnClose() override;

 private:
  struct Spot {
    int row = 0, column = 0;
  };
  xui::Element* ElementAt(Spot spot) const;
  void Move(int rows, int columns);
  void Focus(bool initial = false);
  void Activate();
  void Handle(GuideAction action);
  void Refresh();
  void Finish(bool accepted);

  std::shared_ptr<const GuideAssets> assets_;
  GuideMedia* media_;
  GuideFonts fonts_;
  input::InputSystem* input_;
  uint32_t user_index_;
  Done done_;
  VirtualKeyboard keyboard_;
  GuidePad pad_;
  xui::RenderResources render_;
  xui::SceneContext backdrop_context_, vk_context_;
  std::unique_ptr<xui::Element> backdrop_;
  xui::Element* hud_root_ = nullptr;
  xui::Element* keys_ = nullptr;
  xui::Element* edit_ = nullptr;
  xui::Element* focus_ = nullptr;
  Spot spot_;
  bool closing_ = false;

  bool keys_armed_ = false;
  bool first_frame_ = true;
  bool accepted_ = false;
  float dim_ = 0.0f;
  std::chrono::steady_clock::time_point opened_, last_tick_, caret_shown_at_;
};

}
