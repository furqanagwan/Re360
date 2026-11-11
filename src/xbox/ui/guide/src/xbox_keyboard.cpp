/**
 * @file        ui/guide/src/xbox_keyboard.cpp
 * @brief       The console's own on-screen keyboard for XamShowKeyboardUI (RG-GDK-059)
 *
 * The 2.0.17559 keyboard (vk.xex): KeyboardMain, a HUD scene with the title,
 * hosts KeyboardBase: the description, the text field, a 5 x 10 grid of keys
 * with Backspace and Space below, and a column of three keys on each side
 * (cursor left, previous page, Caps; cursor right, next page, Done). The pad
 * works as on the console: A presses the focused key, X is Backspace, Y
 * Space, LB and RB move the cursor, LT and RT change the page, the left
 * stick pressed in is Caps, Start is Done and B cancels. A PC keyboard types
 * straight into the field.
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/guide/xbox_keyboard.h>

#include <algorithm>
#include <cfloat>
#include <cmath>

#include <fmt/format.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <rex/input/input_system.h>
#include <rex/string.h>
#include <rex/ui/guide/guide_layout.h>

namespace rex::ui::guide {
namespace {

constexpr float kSceneWidth = 852.0f;
constexpr float kSceneHeight = 480.0f;

constexpr float kDimOpacity = 0.75f;
constexpr double kDimInSeconds = 23 / xui::kFramesPerSecond;
constexpr double kDimOutSeconds = 16 / xui::kFramesPerSecond;

constexpr const char* kLeftColumn[] = {"Key.Left", "Key.Prev", "Key.Caps"};
constexpr const char* kRightColumn[] = {"Key.Right", "Key.Next", "Key.OK"};

}

XboxKeyboard::XboxKeyboard(ImGuiDrawer* drawer, std::shared_ptr<const GuideAssets> assets,
                           GuideMedia* media, GuideFonts fonts, input::InputSystem* input,
                           uint32_t user_index, Request request, Done done)
    : ImGuiDialog(drawer),
      assets_(std::move(assets)),
      media_(media),
      fonts_(fonts),
      input_(input),
      user_index_(user_index),
      done_(std::move(done)),
      keyboard_(std::move(request.default_text), request.max_length) {
  uint16_t held = 0;
  if (input_) {
    input::X_INPUT_STATE state = {};
    if (input_->GetStateForUI(user_index_, &state) == X_ERROR_SUCCESS) {
      held = state.gamepad.buttons;
    }
  }
  pad_ = GuidePad(held);

  render_.regular_font = fonts_.regular;
  render_.bold_font = fonts_.bold;
  render_.texture = [this](std::string_view path, std::string_view package, int* w, int* h) {
    return media_->Texture(path, package, w, h);
  };
  render_.vector_image = [this](ImDrawList& list, std::string_view path,
                                const std::function<ImVec2(ImVec2)>& to_screen, float opacity) {
    return DrawGuideVectorImage(list, path, to_screen, opacity, fonts_.bold);
  };
  auto sound = [this](std::string_view file, std::string_view package) {
    media_->PlaySound(file, package);
  };
  for (auto [context, package] :
       {std::pair{&backdrop_context_, "xam/xam"}, std::pair{&vk_context_, "vk/vk"}}) {
    context->skin = &assets_->skin;
    context->package = package;
    context->play_sound = sound;
  }

  backdrop_ = xui::Element::Create(assets_->backdrop.root, backdrop_context_);
  hud_root_ = backdrop_->FindById("HUDRootScene");
  xui::Element* host = backdrop_->FindById("AppHostElementId");
  xui::Element* main = host->AttachScene(SceneNode(assets_->keyboard_main), vk_context_);
  xui::Element* placeholder = main->FindById("KeyboardPlaceholder");
  xui::Element* base = placeholder->AttachScene(SceneNode(assets_->keyboard_base), vk_context_);

  if (xui::Element* header = main->FindById("Text_Header")) {
    header->SetText(rex::string::to_utf8(request.title));
  }
  if (xui::Element* description = base->FindById("XuiText1")) {
    description->SetText(rex::string::to_utf8(request.description));
  }

  for (const char* id : {"KeysGroupJP", "CandidateListGroup", "KanaModeRomaji", "KanaModeKana",
                         "FormattedText", "XuiText2"}) {
    if (xui::Element* e = base->FindById(id)) {
      e->Suppress();
    }
  }
  keys_ = base->FindById("KeysGroup");
  edit_ = base->FindById("XuiEdit1");
  for (const char* id : {"Key.JComma", "Key.JPeriod", "Key.Hidden", "KanaSeparator"}) {
    if (xui::Element* e = keys_->FindById(id)) {
      e->Suppress();
    }
  }

  struct SideKey {
    const char* id;
    const char* image;
  };
  for (const SideKey& key : {SideKey{"Key.Left", "LB.png"}, SideKey{"Key.Right", "RB.png"},
                             SideKey{"Key.Prev", "LT.png"}, SideKey{"Key.Next", "RT.png"},
                             SideKey{"Key.Caps", "Caps.png"}, SideKey{"Key.OK", "Done.png"}}) {
    if (xui::Element* e = keys_->FindById(key.id)) {
      e->Set("ImagePath", xui::Value{std::string(key.image)});
    }
  }
  for (const auto& [id, text] : {std::pair{"Key.Left", "Left"}, std::pair{"Key.Right", "Right"},
                                 std::pair{"Key.Caps", "Caps"}, std::pair{"Key.OK", "Done"},
                                 std::pair{"Key.BS", "Backspace"}, std::pair{"Key.Spc", "Space"}}) {
    if (xui::Element* e = keys_->FindById(id)) {
      e->SetText(text);
    }
  }

  auto legend = [&](const char* button, const char* label, const char* text) {
    xui::Element* glyph = backdrop_->FindById(button);
    xui::Element* words = backdrop_->FindById(label);
    if (glyph && words) {
      const bool show = text && *text;
      glyph->SetVisible(show);
      words->SetVisible(show);
      words->SetText(show ? text : "");
    }
  };
  legend("AButton", "AText", "Select");
  legend("BButton", "BText", "Back");
  legend("XButton", "XText", "");
  legend("YButton", "YText", "");

  Refresh();
  spot_ = {1, 0};
  Focus(true);

  hud_root_->Play("ClosedToFull");
  last_tick_ = opened_ = std::chrono::steady_clock::now();
}

xui::Element* XboxKeyboard::ElementAt(Spot spot) const {
  const char* id = nullptr;
  std::string key;
  if (spot.column < 0) {
    id = kLeftColumn[std::min(spot.row, 5) / 2];
  } else if (spot.column >= VirtualKeyboard::kColumns) {
    id = kRightColumn[std::min(spot.row, 5) / 2];
  } else if (spot.row >= VirtualKeyboard::kRows) {
    id = spot.column < 4 ? "Key.BS" : "Key.Spc";
  } else {
    key = fmt::format("Key.{}_{}", spot.row, spot.column);
    id = key.c_str();
  }
  return keys_->FindById(id);
}

void XboxKeyboard::Move(int rows, int columns) {
  constexpr int kWide = VirtualKeyboard::kColumns + 2;
  xui::Element* from = ElementAt(spot_);
  do {
    spot_.column = (spot_.column + 1 + columns + kWide) % kWide - 1;
    spot_.row = (spot_.row + rows + 6) % 6;

  } while (ElementAt(spot_) == from && (rows || columns));
  Focus();
}

void XboxKeyboard::Focus(bool initial) {
  xui::Element* control = ElementAt(spot_);
  if (!control || control == focus_) {
    return;
  }
  xui::Element::MoveFocus(focus_, control, initial);
  focus_ = control;
}

void XboxKeyboard::Refresh() {
  for (int row = 0; row < VirtualKeyboard::kRows; ++row) {
    for (int column = 0; column < VirtualKeyboard::kColumns; ++column) {
      if (xui::Element* key = keys_->FindById(fmt::format("Key.{}_{}", row, column))) {
        const char16_t c = keyboard_.KeyAt(row, column);
        key->SetText(c ? rex::string::to_utf8(std::u16string(1, c)) : std::string());
      }
    }
  }
  if (xui::Element* prev = keys_->FindById("Key.Prev")) {
    prev->SetText(std::string(VirtualKeyboard::PageName(keyboard_.PreviousPage())));
  }
  if (xui::Element* next = keys_->FindById("Key.Next")) {
    next->SetText(std::string(VirtualKeyboard::PageName(keyboard_.NextPage())));
  }
  if (!edit_) {
    return;
  }
  edit_->SetText(rex::string::to_utf8(keyboard_.text()));

  if (xui::Element* caret = edit_->FindById("Caret"); caret && fonts_.regular) {
    xui::Element* text = edit_->FindById("Text");
    const float size =
        (text ? text->GetFloat("PointSize", 11.5f) : 11.5f) * xui::kPointToSceneUnits;
    const std::string before = rex::string::to_utf8(keyboard_.text().substr(0, keyboard_.cursor()));
    const float width = fonts_.regular->CalcTextSizeA(size, FLT_MAX, 0.0f, before.c_str()).x;
    xui::Vec3 at = caret->GetVector("Position");
    at.x = (text ? text->GetVector("Position").x : 6.0f) + 4.0f + width;
    caret->Set("Position", xui::Value{at});
  }
  caret_shown_at_ = std::chrono::steady_clock::now();
}

void XboxKeyboard::Activate() {
  if (!focus_) {
    return;
  }
  focus_->Press();
  const std::string_view id = focus_->id();
  if (id == "Key.BS") {
    keyboard_.Backspace();
  } else if (id == "Key.Spc") {
    keyboard_.Space();
  } else if (id == "Key.Left") {
    keyboard_.MoveCursor(-1);
  } else if (id == "Key.Right") {
    keyboard_.MoveCursor(+1);
  } else if (id == "Key.Prev") {
    keyboard_.GoToPage(keyboard_.PreviousPage());
  } else if (id == "Key.Next") {
    keyboard_.GoToPage(keyboard_.NextPage());
  } else if (id == "Key.Caps") {
    keyboard_.ToggleCaps();
  } else if (id == "Key.OK") {
    Finish(true);
    return;
  } else {
    keyboard_.Type(spot_.row, spot_.column);
  }
  Refresh();
}

void XboxKeyboard::Handle(GuideAction action) {
  switch (action) {
    case GuideAction::kUp:
      Move(-1, 0);
      break;
    case GuideAction::kDown:
      Move(+1, 0);
      break;
    case GuideAction::kLeft:
      Move(0, -1);
      break;
    case GuideAction::kRight:
      Move(0, +1);
      break;
    case GuideAction::kA:
      Activate();
      break;
    case GuideAction::kB:
      Finish(false);
      break;
    case GuideAction::kX:
      keyboard_.Backspace();
      Refresh();
      break;
    case GuideAction::kY:
      keyboard_.Space();
      Refresh();
      break;
    case GuideAction::kPreviousTab:
      keyboard_.MoveCursor(-1);
      Refresh();
      break;
    case GuideAction::kNextTab:
      keyboard_.MoveCursor(+1);
      Refresh();
      break;
    case GuideAction::kLeftTrigger:
      keyboard_.GoToPage(keyboard_.PreviousPage());
      Refresh();
      break;
    case GuideAction::kRightTrigger:
      keyboard_.GoToPage(keyboard_.NextPage());
      Refresh();
      break;
    case GuideAction::kLeftThumb:
      keyboard_.ToggleCaps();
      Refresh();
      break;
    case GuideAction::kStart:
      Finish(true);
      break;
  }
}

void XboxKeyboard::Finish(bool accepted) {
  if (closing_) {
    return;
  }
  closing_ = true;
  accepted_ = accepted;
  hud_root_->Play("FullToClosed");
  media_->PlaySound(accepted ? "sharedres://btn_Select.xma" : "sharedres://btn_Back.xma", "");
}

void XboxKeyboard::OnDraw(ImGuiIO& io) {
  const auto now = std::chrono::steady_clock::now();
  const double seconds = std::chrono::duration<double>(now - last_tick_).count();
  last_tick_ = now;

  if (!keys_armed_) {
    keys_armed_ = !first_frame_;
    first_frame_ = false;
    for (ImGuiKey key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END;
         key = ImGuiKey(key + 1)) {
      if (ImGui::IsKeyDown(key)) {
        keys_armed_ = false;
        break;
      }
    }
    io.InputQueueCharacters.resize(0);
  }
  if (!closing_) {
    std::vector<GuideAction> actions;

    bool typed = false;
    for (ImWchar c : keys_armed_ ? io.InputQueueCharacters : ImVector<ImWchar>()) {
      if (c >= 0x20 && c != 0x7F && c <= 0xFFFF) {
        typed |= keyboard_.Insert(char16_t(c));
      }
    }
    if (keys_armed_ && ImGui::IsKeyPressed(ImGuiKey_Backspace, true)) {
      typed |= keyboard_.Backspace();
    }
    if (typed) {
      Refresh();
    }
    struct Key {
      ImGuiKey key;
      GuideAction action;
    };
    for (const Key& key :
         {Key{ImGuiKey_UpArrow, GuideAction::kUp}, Key{ImGuiKey_DownArrow, GuideAction::kDown},
          Key{ImGuiKey_LeftArrow, GuideAction::kLeft},
          Key{ImGuiKey_RightArrow, GuideAction::kRight}, Key{ImGuiKey_Enter, GuideAction::kStart},
          Key{ImGuiKey_Escape, GuideAction::kB}}) {
      if (keys_armed_ && ImGui::IsKeyPressed(key.key, true)) {
        actions.push_back(key.action);
      }
    }
    if (input_) {
      input::X_INPUT_STATE state = {};
      if (input_->GetStateForUI(user_index_, &state) == X_ERROR_SUCCESS) {
        const uint64_t now_ms =
            uint64_t(std::chrono::duration_cast<std::chrono::milliseconds>(now - opened_).count());
        for (GuideAction action :
             pad_.Update(state.gamepad.buttons, state.gamepad.thumb_lx, state.gamepad.thumb_ly,
                         now_ms, state.gamepad.left_trigger, state.gamepad.right_trigger)) {
          actions.push_back(action);
        }
      }
    }
    for (GuideAction action : actions) {
      Handle(action);
      if (closing_) {
        break;
      }
    }
  }

  GImGui->PlatformImeData.WantTextInput = !closing_;

  if (xui::Element* caret = edit_ ? edit_->FindById("Caret") : nullptr) {
    const double since = std::chrono::duration<double>(now - caret_shown_at_).count();
    caret->SetVisible(std::fmod(since, 1.0) < 0.5);
  }
  backdrop_->Advance(std::min(seconds, 0.25) * xui::kFramesPerSecond);
  dim_ = closing_ ? std::max(0.0f, dim_ - float(seconds / kDimOutSeconds))
                  : std::min(1.0f, dim_ + float(seconds / kDimInSeconds));
  ImDrawList* list = ImGui::GetForegroundDrawList();
  list->AddRectFilled(ImVec2(0, 0), io.DisplaySize,
                      IM_COL32(0, 0, 0, int(kDimOpacity * dim_ * 255.0f + 0.5f)));
  const float scale = std::min(io.DisplaySize.x / kSceneWidth, io.DisplaySize.y / kSceneHeight);
  const ImVec2 origin((io.DisplaySize.x - kSceneWidth * scale) / 2,
                      (io.DisplaySize.y - kSceneHeight * scale) / 2);
  render_.pixels_per_point = imgui_drawer()->PixelsPerPoint();
  xui::Render(list, *backdrop_, origin, scale, 1.0f, render_);

  if (closing_ && !hud_root_->playing() && dim_ == 0.0f) {
    Close();
  }
}

void XboxKeyboard::OnClose() {
  if (done_) {
    done_(accepted_ ? std::optional<std::u16string>(keyboard_.text()) : std::nullopt);
  }
}

}
