/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2022 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 *
 * @modified    Tom Clay, 2026 - Adapted for ReXGlue runtime
 */

#ifndef REX_UI_IMGUI_DRAWER_H_
#define REX_UI_IMGUI_DRAWER_H_

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include <rex/ui/immediate_drawer.h>
#include <rex/ui/presenter.h>
#include <rex/ui/style.h>
#include <rex/ui/window.h>
#include <rex/ui/window_listener.h>

namespace rex {
namespace ui {

class ImGuiDialog;
class Window;

class ImGuiDrawer : public WindowInputListener, public UIDrawer {
 public:
  using FontSetupCallback = std::function<void(ImFontAtlas*)>;
  using StyleSetupCallback = std::function<void(ImGuiStyle&, Style&)>;
  ImGuiDrawer(Window* window, size_t z_order, FontSetupCallback font_setup = nullptr,
              StyleSetupCallback style_setup = nullptr);
  ~ImGuiDrawer();

  ImGuiIO& GetIO();
  float PixelsPerPoint() const;

  Style& style() { return style_; }
  const Style& style() const { return style_; }

  void AddDialog(ImGuiDialog* dialog);
  void RemoveDialog(ImGuiDialog* dialog);

  bool HasDialogs() const { return !dialogs_.empty(); }
  // Called after a dialog is added or removed (on the UI thread, possibly while dialogs draw).
  void SetDialogsChangedCallback(std::function<void()> callback) {
    dialogs_changed_ = std::move(callback);
  }

  void SetPresenter(Presenter* new_presenter);
  void SetImmediateDrawer(ImmediateDrawer* new_immediate_drawer);
  void SetPresenterAndImmediateDrawer(Presenter* new_presenter,
                                      ImmediateDrawer* new_immediate_drawer) {
    SetPresenter(new_presenter);
    SetImmediateDrawer(new_immediate_drawer);
  }

  void Draw(UIDrawContext& ui_draw_context) override;

 protected:
  void OnKeyDown(KeyEvent& e) override;
  void OnKeyUp(KeyEvent& e) override;
  void OnKeyChar(KeyEvent& e) override;
  void OnMouseDown(MouseEvent& e) override;
  void OnMouseMove(MouseEvent& e) override;
  void OnMouseUp(MouseEvent& e) override;
  void OnMouseWheel(MouseEvent& e) override;
  void OnTouchEvent(TouchEvent& e) override;

 private:
  void Initialize();

  void SetupFontTexture();

  void RenderDrawLists(ImDrawData* data, UIDrawContext& ui_draw_context);

  void ClearInput();
  void OnKey(KeyEvent& e, bool is_down);
  void UpdateMousePosition(float x, float y);
  void SwitchToPhysicalMouseAndUpdateMousePosition(const MouseEvent& e);

  bool IsDrawingDialogs() const { return dialog_loop_next_index_ != SIZE_MAX; }
  void DetachIfLastDialogRemoved();

  std::optional<ImGuiKey> VirtualKeyToImGuiKey(VirtualKey vkey);

  Window* window_;
  size_t z_order_;
  FontSetupCallback font_setup_;
  StyleSetupCallback style_setup_;
  Style style_;

  ImGuiContext* internal_state_ = nullptr;

  std::vector<ImGuiDialog*> dialogs_;
  std::function<void()> dialogs_changed_;

  size_t dialog_loop_next_index_ = SIZE_MAX;

  Presenter* presenter_ = nullptr;

  ImmediateDrawer* immediate_drawer_ = nullptr;

  std::unique_ptr<ImmediateTexture> font_texture_;

  uint32_t touch_pointer_id_ = TouchEvent::kPointerIDNone;

  bool reset_mouse_position_after_next_frame_ = false;

  double frame_time_tick_frequency_;
  uint64_t last_frame_time_ticks_;

  static void PlatformSetImeData(ImGuiContext* context, ImGuiViewport* viewport,
                                 ImGuiPlatformImeData* data);
  void SetWindowTextInputActive(bool active);

  bool text_input_active_ = false;
};

}
}

#endif
