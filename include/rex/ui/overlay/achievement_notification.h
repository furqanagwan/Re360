/**
 * @file        rex/ui/overlay/achievement_notification.h
 * @brief       Replaceable achievement notification dialog interface.
 *
 * @copyright   Copyright (c) 2026 Rien Gupta <rgupta9@scu.edu>
 *              All rights reserved.
 *
 * @license     BSD 3-Clause License
 *              See LICENSE file in the project root for full license text.
 */
#pragma once

#include <rex/system/achievement_manager.h>
#include <rex/ui/imgui_dialog.h>

namespace rex::ui {

class AchievementNotificationDialog : public ImGuiDialog {
 public:
  ~AchievementNotificationDialog() override = default;

  virtual void Push(const rex::system::AchievementEvent& event) = 0;
  bool UsesMouse() const override { return false; }  // always open, never clicked

 protected:
  explicit AchievementNotificationDialog(ImGuiDrawer* drawer) : ImGuiDialog(drawer) {}
};

}
