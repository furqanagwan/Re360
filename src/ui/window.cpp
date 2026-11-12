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

#include <algorithm>
#include <iterator>

#include <rex/assert.h>
#include <rex/cvar.h>
#include <rex/graphics/video_mode_util.h>
#include <rex/logging.h>
#include <rex/ui/imgui_drawer.h>
#include <rex/ui/presenter.h>
#include <rex/ui/window.h>

#include <imgui.h>

REXCVAR_DEFINE_INT32(window_width, 0, "UI/Window",
                     "Window width in logical pixels (0 = use app default)")
    .range(0, 8192);

REXCVAR_DEFINE_INT32(window_height, 0, "UI/Window",
                     "Window height in logical pixels (0 = use app default)")
    .range(0, 8192);

REXCVAR_DEFINE_BOOL(fullscreen, true, "UI/Window", "Start the window in fullscreen mode");

REXCVAR_DEFINE_BOOL(fullscreen_exclusive, false, "UI/Window",
                    "Switch the display mode in fullscreen instead of going "
                    "borderless at the desktop mode");

REXCVAR_DEFINE_INT32(monitor, 0, "UI/Window",
                     "Monitor index to display on (0 = default, 1 = primary, 2 = "
                     "second monitor, etc.)")
    .range(0, 16);

REXCVAR_DEFINE_STRING(ui_backend, "win32", "UI/Window",
                      "Window and message loop: win32 (the only one since SDL was removed)")
    .allowed({"win32", "sdl"})
    .lifecycle(rex::cvar::Lifecycle::kRequiresRestart);

REXCVAR_DEFINE_INT32(video_mode_width, 1280, "Display", "Guest video mode width in pixels")
    .range(640, 0x0FFF)
    .lifecycle(rex::cvar::Lifecycle::kRequiresRestart);

REXCVAR_DEFINE_INT32(video_mode_height, 720, "Display", "Guest video mode height in pixels")
    .range(480, 0x0FFF)
    .lifecycle(rex::cvar::Lifecycle::kRequiresRestart);

REXCVAR_DEFINE_STRING(resolution, "", "Display",
                      "Common resolution preset for both guest video mode and window (for "
                      "example: 720p, 1080p, 1440p, 4k, 1280x720)");

// NOLINTNEXTLINE(readability-redundant-string-init)
REXCVAR_DEFINE_STRING(render_resolution, "", "Display",
                      "Resolution the regpu plugin draws at, scaled to the window: a preset "
                      "(720p, 1080p, 1440p, 4k) or WxH; empty matches the window. Changes "
                      "apply while the game runs; the game's own video mode is unaffected");

REXCVAR_DEFINE_STRING(
    render_upscaler, "off", "Display",
    "Upscaler the regpu plugin uses: off, fsr1, fsr3, fsr4 or dlss (one the GPU or "
    "build can't run falls back to none). Changes apply while the game runs");

REXCVAR_DEFINE_INT32(render_upscaler_quality, 1, "Display",
                     "The upscaler's render scale: 0 native (anti-aliasing only), 1 quality, 2 "
                     "balanced, 3 performance, 4 ultra performance")
    .range(0, 4);

REXCVAR_DEFINE_DOUBLE(video_mode_refresh_rate, 60.0, "Display",
                      "Guest video mode refresh rate in Hz")
    .range(24.0, 240.0)
    .lifecycle(rex::cvar::Lifecycle::kRequiresRestart);

namespace rex {
namespace ui {

Window::Window(WindowedAppContext& app_context, const std::string_view title,
               uint32_t desired_logical_width, uint32_t desired_logical_height)
    : app_context_(app_context),
      title_(title),
      desired_logical_width_(desired_logical_width),
      desired_logical_height_(desired_logical_height) {}

Window::~Window() {
  assert_true(phase_ == Phase::kDeleting);
  EnterDestructor();

  if (presenter_) {
    Presenter* old_presenter = presenter_;

    presenter_ = nullptr;
    old_presenter->SetWindowSurfaceFromUIThread(nullptr, nullptr);
    presenter_surface_.reset();
  }

  while (innermost_destruction_receiver_) {
    innermost_destruction_receiver_->window_ = nullptr;
    innermost_destruction_receiver_ = innermost_destruction_receiver_->outer_receiver_;
  }
}

void Window::AddListener(WindowListener* listener) {
  assert_not_null(listener);

  if (std::find(listeners_.cbegin(), listeners_.cend(), listener) != listeners_.cend()) {
    return;
  }
  listeners_.push_back(listener);
}

void Window::RemoveListener(WindowListener* listener) {
  assert_not_null(listener);
  auto it = std::find(listeners_.cbegin(), listeners_.cend(), listener);
  if (it == listeners_.cend()) {
    return;
  }

  ListenerIterationContext* iteration_context = innermost_listener_iteration_context_;
  if (iteration_context) {
    size_t existing_index = size_t(std::distance(listeners_.cbegin(), it));
    while (iteration_context) {
      if (iteration_context->next_index > existing_index) {
        --iteration_context->next_index;
      }
      iteration_context = iteration_context->outer_context;
    }
  }
  listeners_.erase(it);
}

void Window::AddInputListener(WindowInputListener* listener, size_t z_order) {
  assert_not_null(listener);

  for (auto it_existing = input_listeners_.rbegin(); it_existing != input_listeners_.rend();
       ++it_existing) {
    if (it_existing->second != listener) {
      continue;
    }
    if (it_existing->first == z_order) {
      return;
    }

    InputListenerIterationContext* iteration_context = innermost_input_listener_iteration_context_;
    while (iteration_context) {
      if (iteration_context->next_iterator == it_existing) {
        ++iteration_context->next_iterator;
      }
      iteration_context = iteration_context->outer_context;
    }
    input_listeners_.erase(std::prev(it_existing.base()));
  }
  auto it_new = std::prev(std::make_reverse_iterator(input_listeners_.emplace(z_order, listener)));

  {
    InputListenerIterationContext* iteration_context = innermost_input_listener_iteration_context_;
    while (iteration_context) {
      if (z_order < iteration_context->current_z_order &&
          (iteration_context->next_iterator == input_listeners_.crend() ||
           z_order >= iteration_context->next_iterator->first)) {
        iteration_context->next_iterator = it_new;
      }
      iteration_context = iteration_context->outer_context;
    }
  }
}

void Window::RemoveInputListener(WindowInputListener* listener) {
  assert_not_null(listener);
  for (auto it_existing = input_listeners_.rbegin(); it_existing != input_listeners_.rend();
       ++it_existing) {
    if (it_existing->second != listener) {
      continue;
    }

    InputListenerIterationContext* iteration_context = innermost_input_listener_iteration_context_;
    while (iteration_context) {
      if (iteration_context->next_iterator == it_existing) {
        ++iteration_context->next_iterator;
      }
      iteration_context = iteration_context->outer_context;
    }
    input_listeners_.erase(std::prev(it_existing.base()));
    return;
  }
}

bool Window::Open() {
  if (phase_ != Phase::kClosedOpenable) {
    return true;
  }

  actual_physical_width_ = 0;
  actual_physical_height_ = 0;
  has_focus_ = false;
  phase_ = Phase::kOpening;
  bool platform_open_result = OpenImpl();
  if (!platform_open_result) {
    phase_ = Phase::kClosedOpenable;
    return false;
  }
  if (phase_ != Phase::kOpening) {
    return true;
  }
  phase_ = Phase::kOpen;

  {
    MonitorUpdateEvent e(this, true, true);
    OnMonitorUpdate(e);
  }
  {
    UISetupEvent e(this, true);
    WindowDestructionReceiver destruction_receiver(this);
    SendEventToListeners([&e](auto listener) { listener->OnOpened(e); }, destruction_receiver);
    if (destruction_receiver.IsWindowDestroyedOrListenersUncallable()) {
      return true;
    }
    SendEventToListeners([&e](auto listener) { listener->OnDpiChanged(e); }, destruction_receiver);
    if (destruction_receiver.IsWindowDestroyedOrListenersUncallable()) {
      return true;
    }
    SendEventToListeners([&e](auto listener) { listener->OnResize(e); }, destruction_receiver);
    if (destruction_receiver.IsWindowDestroyedOrListenersUncallable()) {
      return true;
    }
    if (HasFocus()) {
      SendEventToListeners([&e](auto listener) { listener->OnGotFocus(e); }, destruction_receiver);
      if (destruction_receiver.IsWindowDestroyedOrListenersUncallable()) {
        return true;
      }
    }
  }

  OnSurfaceChanged(true);

  return true;
}

void Window::SetFullscreen(bool new_fullscreen) {
  if (fullscreen_ == new_fullscreen) {
    return;
  }
  fullscreen_ = new_fullscreen;
  if (!CanApplyState()) {
    return;
  }
  WindowDestructionReceiver destruction_receiver(this);
  ApplyNewFullscreen();
  if (destruction_receiver.IsWindowDestroyedOrStateInapplicable()) {
    return;
  }
}

void Window::RefreshFullscreen() {
  if (!CanApplyState()) {
    return;
  }
  WindowDestructionReceiver destruction_receiver(this);
  ApplyNewFullscreen();
  if (destruction_receiver.IsWindowDestroyedOrStateInapplicable()) {
    return;
  }
}

void Window::SetMonitor(int32_t new_monitor) {
  if (monitor_ == new_monitor) {
    return;
  }
  monitor_ = new_monitor;
  if (!CanApplyState()) {
    return;
  }
  WindowDestructionReceiver destruction_receiver(this);
  ApplyNewMonitor();
  if (destruction_receiver.IsWindowDestroyedOrStateInapplicable()) {
    return;
  }
}

void Window::SetDesiredLogicalSize(uint32_t new_desired_logical_width,
                                   uint32_t new_desired_logical_height) {
  if (desired_logical_width_ == new_desired_logical_width &&
      desired_logical_height_ == new_desired_logical_height) {
    return;
  }
  desired_logical_width_ = new_desired_logical_width;
  desired_logical_height_ = new_desired_logical_height;
  if (!CanApplyState()) {
    return;
  }
  WindowDestructionReceiver destruction_receiver(this);
  ApplyNewDesiredLogicalSize();
  if (destruction_receiver.IsWindowDestroyedOrStateInapplicable()) {
    return;
  }
}

void Window::ResolveConfiguredLogicalSize(uint32_t& width_out, uint32_t& height_out) {
  int32_t configured_width = REXCVAR_GET(window_width);
  int32_t configured_height = REXCVAR_GET(window_height);
  if (configured_width <= 0 || configured_height <= 0) {
    rex::graphics::video_mode_util::ResolveConfiguredSize(configured_width, configured_height);
  }
  width_out = uint32_t(std::clamp(configured_width, 1, 8192));
  height_out = uint32_t(std::clamp(configured_height, 1, 8192));
}

void Window::SetTitle(const std::string_view new_title) {
  if (title_ == new_title) {
    return;
  }
  title_ = new_title;
  if (!CanApplyState()) {
    return;
  }
  WindowDestructionReceiver destruction_receiver(this);
  ApplyNewTitle();
  if (destruction_receiver.IsWindowDestroyedOrStateInapplicable()) {
    return;
  }
}

void Window::SetIcon(const void* buffer, size_t size) {
  bool reset = !buffer || !size;
  WindowDestructionReceiver destruction_receiver(this);
  LoadAndApplyIcon(reset ? nullptr : buffer, reset ? 0 : size, CanApplyState());
  if (destruction_receiver.IsWindowDestroyedOrStateInapplicable()) {
    return;
  }
}

void Window::SetMainMenu(std::unique_ptr<MenuItem> new_main_menu) {
  if (main_menu_ == new_main_menu) {
    return;
  }

  std::unique_ptr<MenuItem> old_main_menu = std::move(main_menu_);
  main_menu_ = std::move(new_main_menu);
  if (!CanApplyState()) {
    return;
  }
  WindowDestructionReceiver destruction_receiver(this);
  ApplyNewMainMenu(old_main_menu.get());
  if (destruction_receiver.IsWindowDestroyedOrStateInapplicable()) {
    return;
  }
}

void Window::CompleteMainMenuItemsUpdate() {
  if (!main_menu_ || !CanApplyState()) {
    return;
  }
  WindowDestructionReceiver destruction_receiver(this);
  CompleteMainMenuItemsUpdateImpl();
  if (destruction_receiver.IsWindowDestroyedOrStateInapplicable()) {
    return;
  }
}

void Window::SetMainMenuEnabled(bool enabled) {
  if (!main_menu_) {
    return;
  }

  WindowDestructionReceiver destruction_receiver(this);
  main_menu_->SetEnabled(enabled);
  if (destruction_receiver.IsWindowDestroyed()) {
    return;
  }

  CompleteMainMenuItemsUpdate();
  if (destruction_receiver.IsWindowDestroyed()) {
    return;
  }
}

void Window::CaptureMouse() {
  ++mouse_capture_request_count_;
  if (!CanApplyState()) {
    return;
  }
  WindowDestructionReceiver destruction_receiver(this);

  ApplyNewMouseCapture();
  if (destruction_receiver.IsWindowDestroyedOrStateInapplicable()) {
    return;
  }
}

void Window::ReleaseMouse() {
  assert_not_zero(mouse_capture_request_count_);
  if (!mouse_capture_request_count_) {
    return;
  }
  if (--mouse_capture_request_count_) {
    return;
  }
  if (!CanApplyState()) {
    return;
  }
  WindowDestructionReceiver destruction_receiver(this);
  ApplyNewMouseRelease();
  if (destruction_receiver.IsWindowDestroyedOrStateInapplicable()) {
    return;
  }
}

void Window::SetTextInputActive(bool active) {
  if (text_input_active_ == active) {
    return;
  }
  text_input_active_ = active;
  if (!CanApplyState()) {
    return;
  }
  WindowDestructionReceiver destruction_receiver(this);
  ApplyNewTextInputActive();
  if (destruction_receiver.IsWindowDestroyedOrStateInapplicable()) {
    return;
  }
}

void Window::SetCursorVisibility(CursorVisibility new_cursor_visibility) {
  if (cursor_visibility_ == new_cursor_visibility) {
    return;
  }
  CursorVisibility old_cursor_visibility = cursor_visibility_;
  cursor_visibility_ = new_cursor_visibility;
  if (!CanApplyState()) {
    return;
  }
  WindowDestructionReceiver destruction_receiver(this);
  ApplyNewCursorVisibility(old_cursor_visibility);
  if (destruction_receiver.IsWindowDestroyedOrStateInapplicable()) {
    return;
  }
}

void Window::Focus() {
  if (!CanApplyState() || has_focus_) {
    return;
  }
  WindowDestructionReceiver destruction_receiver(this);
  FocusImpl();
  if (destruction_receiver.IsWindowDestroyedOrStateInapplicable()) {
    return;
  }
}

void Window::SetPresenter(Presenter* presenter) {
  if (presenter_ == presenter) {
    return;
  }
  if (presenter_) {
    presenter_->SetWindowSurfaceFromUIThread(nullptr, nullptr);
    presenter_surface_.reset();
  }
  presenter_ = presenter;
  if (presenter_) {
    presenter_surface_ = CreateSurface(presenter_->GetSupportedSurfaceTypes());
    presenter_->SetWindowSurfaceFromUIThread(this, presenter_surface_.get());
  }
}

void Window::OnSurfaceChanged(bool new_surface_potentially_exists) {
  if (!presenter_) {
    return;
  }

  if (presenter_surface_) {
    presenter_->SetWindowSurfaceFromUIThread(this, nullptr);
    presenter_surface_.reset();
  }

  if (!new_surface_potentially_exists) {
    return;
  }

  Surface::TypeFlags supported_types = presenter_->GetSupportedSurfaceTypes();
  presenter_surface_ = CreateSurface(supported_types);
  if (presenter_surface_) {
    presenter_->SetWindowSurfaceFromUIThread(this, presenter_surface_.get());
  } else if (phase_ == Phase::kOpen) {
    REXLOG_ERROR(
        "No presentable surface for this window. The graphics provider supports surface types "
        "{:#x}.",
        supported_types);
  }
}

void Window::OnBeforeClose(WindowDestructionReceiver& destruction_receiver) {
  bool was_open = phase_ == Phase::kOpen;
  bool was_open_or_opening = was_open || phase_ == Phase::kOpening;
  assert_true(was_open_or_opening);
  if (!was_open_or_opening) {
    return;
  }

  if (was_open) {
    phase_ = Phase::kOpenBeforeClosing;

    OnFocusUpdate(false, destruction_receiver);
    if (destruction_receiver.IsWindowDestroyed()) {
      return;
    }

    {
      UIEvent e(this);
      SendEventToListeners([&e](auto listener) { listener->OnClosing(e); }, destruction_receiver);
      if (destruction_receiver.IsWindowDestroyed()) {
        return;
      }
    }

    OnSurfaceChanged(false);
  }

  phase_ = Phase::kClosing;
}

void Window::OnAfterClose() {
  assert_true(phase_ == Phase::kClosing);
  if (phase_ != Phase::kClosing) {
    return;
  }
  phase_ = (innermost_listener_iteration_context_ || innermost_input_listener_iteration_context_)
               ? Phase::kClosedLeavingListeners
               : Phase::kClosedOpenable;
}

void Window::OnDpiChanged(UISetupEvent& e, WindowDestructionReceiver& destruction_receiver) {
  SendEventToListeners([&e](auto listener) { listener->OnDpiChanged(e); }, destruction_receiver);
  if (destruction_receiver.IsWindowDestroyed()) {
    return;
  }
}

void Window::OnMonitorUpdate(MonitorUpdateEvent& e) {
  if (presenter_surface_) {
    presenter_->OnSurfaceMonitorUpdateFromUIThread(e.old_monitor_potentially_disconnected());
  }
}

bool Window::OnActualSizeUpdate(uint32_t new_physical_width, uint32_t new_physical_height,
                                WindowDestructionReceiver& destruction_receiver) {
  if (actual_physical_width_ == new_physical_width &&
      actual_physical_height_ == new_physical_height) {
    return false;
  }
  actual_physical_width_ = new_physical_width;
  actual_physical_height_ = new_physical_height;

  if (presenter_surface_) {
    presenter_->OnSurfaceResizeFromUIThread();
  }
  UISetupEvent e(this);
  SendEventToListeners([&e](auto listener) { listener->OnResize(e); }, destruction_receiver);
  if (destruction_receiver.IsWindowDestroyed()) {
    return true;
  }
  return true;
}

void Window::OnFocusUpdate(bool new_has_focus, WindowDestructionReceiver& destruction_receiver) {
  if (has_focus_ == new_has_focus) {
    return;
  }
  has_focus_ = new_has_focus;
  UISetupEvent e(this);
  if (has_focus_) {
    SendEventToListeners([&e](auto listener) { listener->OnGotFocus(e); }, destruction_receiver);
  } else {
    SendEventToListeners([&e](auto listener) { listener->OnLostFocus(e); }, destruction_receiver);
  }
  if (destruction_receiver.IsWindowDestroyed()) {
    return;
  }
}

bool Window::SendCloseRequestToListeners(WindowDestructionReceiver& destruction_receiver) {
  if (!CanSendEventsToListeners()) {
    return true;
  }
  UIEvent e(this);

  std::vector<WindowListener*> listeners(listeners_);
  for (WindowListener* listener : listeners) {
    bool proceed = listener->OnCloseRequested(e);
    if (destruction_receiver.IsWindowDestroyed()) {
      return false;
    }
    if (!proceed) {
      return false;
    }
  }
  return true;
}

void Window::OnMinimized(WindowDestructionReceiver& destruction_receiver) {
  if (!CanSendEventsToListeners()) {
    return;
  }
  UIEvent e(this);
  SendEventToListeners([&e](auto listener) { listener->OnMinimized(e); }, destruction_receiver);
}

void Window::OnRestored(WindowDestructionReceiver& destruction_receiver) {
  if (!CanSendEventsToListeners()) {
    return;
  }
  UIEvent e(this);
  SendEventToListeners([&e](auto listener) { listener->OnRestored(e); }, destruction_receiver);
}

void Window::OnPaint(bool force_paint) {
  if (is_painting_) {
    paint_requested_while_painting_ = true;
    return;
  }
  is_painting_ = true;
  if (presenter_surface_) {
    presenter_->PaintFromUIThread(force_paint);
  }
  is_painting_ = false;
  if (std::exchange(paint_requested_while_painting_, false)) {
    RequestPaint();
  }
}

void Window::OnFileDrop(FileDropEvent& e, WindowDestructionReceiver& destruction_receiver) {
  SendEventToListeners([&e](auto listener) { listener->OnFileDrop(e); }, destruction_receiver);
  if (destruction_receiver.IsWindowDestroyed()) {
    return;
  }
}

void Window::OnKeyDown(KeyEvent& e, WindowDestructionReceiver& destruction_receiver) {
  PropagateEventThroughInputListeners(
      [&e](auto listener) {
        listener->OnKeyDown(e);
        return e.is_handled();
      },
      destruction_receiver);
  if (destruction_receiver.IsWindowDestroyed()) {
    return;
  }
}

void Window::OnKeyUp(KeyEvent& e, WindowDestructionReceiver& destruction_receiver) {
  PropagateEventThroughInputListeners(
      [&e](auto listener) {
        listener->OnKeyUp(e);
        return e.is_handled();
      },
      destruction_receiver);
  if (destruction_receiver.IsWindowDestroyed()) {
    return;
  }
}

void Window::OnKeyChar(KeyEvent& e, WindowDestructionReceiver& destruction_receiver) {
  PropagateEventThroughInputListeners(
      [&e](auto listener) {
        listener->OnKeyChar(e);
        return e.is_handled();
      },
      destruction_receiver);
  if (destruction_receiver.IsWindowDestroyed()) {
    return;
  }
}

void Window::OnMouseDown(MouseEvent& e, WindowDestructionReceiver& destruction_receiver) {
  PropagateEventThroughInputListeners(
      [&e](auto listener) {
        listener->OnMouseDown(e);
        return e.is_handled();
      },
      destruction_receiver);
  if (destruction_receiver.IsWindowDestroyed()) {
    return;
  }
}

void Window::OnMouseMove(MouseEvent& e, WindowDestructionReceiver& destruction_receiver) {
  PropagateEventThroughInputListeners(
      [&e](auto listener) {
        listener->OnMouseMove(e);
        return e.is_handled();
      },
      destruction_receiver);
  if (destruction_receiver.IsWindowDestroyed()) {
    return;
  }
}

void Window::OnMouseUp(MouseEvent& e, WindowDestructionReceiver& destruction_receiver) {
  PropagateEventThroughInputListeners(
      [&e](auto listener) {
        listener->OnMouseUp(e);
        return e.is_handled();
      },
      destruction_receiver);
  if (destruction_receiver.IsWindowDestroyed()) {
    return;
  }
}

void Window::OnMouseWheel(MouseEvent& e, WindowDestructionReceiver& destruction_receiver) {
  PropagateEventThroughInputListeners(
      [&e](auto listener) {
        listener->OnMouseWheel(e);
        return e.is_handled();
      },
      destruction_receiver);
  if (destruction_receiver.IsWindowDestroyed()) {
    return;
  }
}

void Window::OnTouchEvent(TouchEvent& e, WindowDestructionReceiver& destruction_receiver) {
  PropagateEventThroughInputListeners(
      [&e](auto listener) {
        listener->OnTouchEvent(e);
        return e.is_handled();
      },
      destruction_receiver);
  if (destruction_receiver.IsWindowDestroyed()) {
    return;
  }
}

void Window::SendEventToListeners(std::function<void(WindowListener*)> fn,
                                  WindowDestructionReceiver& destruction_receiver) {
  if (!CanSendEventsToListeners()) {
    return;
  }
  ListenerIterationContext iteration_context(innermost_listener_iteration_context_);
  innermost_listener_iteration_context_ = &iteration_context;
  while (iteration_context.next_index < listeners_.size()) {
    fn(listeners_[iteration_context.next_index++]);
    if (destruction_receiver.IsWindowDestroyed()) {
      return;
    }
    if (!CanSendEventsToListeners()) {
      break;
    }
  }
  assert_true(innermost_listener_iteration_context_ == &iteration_context);
  innermost_listener_iteration_context_ = innermost_listener_iteration_context_->outer_context;
  if (phase_ == Phase::kClosedLeavingListeners && !innermost_listener_iteration_context_ &&
      !innermost_input_listener_iteration_context_) {
    phase_ = Phase::kClosedOpenable;
  }
}

void Window::PropagateEventThroughInputListeners(std::function<bool(WindowInputListener*)> fn,
                                                 WindowDestructionReceiver& destruction_receiver) {
  if (!CanSendEventsToListeners()) {
    return;
  }
  InputListenerIterationContext iteration_context(innermost_input_listener_iteration_context_,
                                                  input_listeners_.crbegin());
  innermost_input_listener_iteration_context_ = &iteration_context;
  while (iteration_context.next_iterator != input_listeners_.crend()) {
    iteration_context.current_z_order = iteration_context.next_iterator->first;
    bool event_handled = fn((iteration_context.next_iterator++)->second);
    if (destruction_receiver.IsWindowDestroyed()) {
      return;
    }
    if (event_handled) {
      break;
    }
    if (!CanSendEventsToListeners()) {
      break;
    }
  }
  assert_true(innermost_input_listener_iteration_context_ == &iteration_context);
  innermost_input_listener_iteration_context_ =
      innermost_input_listener_iteration_context_->outer_context;
  if (phase_ == Phase::kClosedLeavingListeners && !innermost_listener_iteration_context_ &&
      !innermost_input_listener_iteration_context_) {
    phase_ = Phase::kClosedOpenable;
  }
}

}
}
