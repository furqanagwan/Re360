/**
 * @file        ui/guide/src/guide_notification.cpp
 * @brief       Achievement unlocks as the console shows them: XAM's notify scene (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/guide/guide_notification.h>

#include <algorithm>

#include <fmt/format.h>

#include <rex/ui/guide/app_update.h>

namespace rex::ui::guide {
namespace {

constexpr float kSceneWidth = 852.0f;
constexpr float kSceneHeight = 480.0f;

constexpr float kPopupX = (kSceneWidth - 460.0f) / 2.0f;
constexpr float kPopupY = 350.0f;

std::string UnlockText(const std::vector<std::string>& xam_strings,
                       const system::AchievementEvent& event) {
  std::string format =
      FindString(xam_strings, "Achievement unlocked", "Achievement unlocked\\n%sG - %s");
  for (size_t at = format.find("\\n"); at != std::string::npos; at = format.find("\\n", at)) {
    format.replace(at, 2, "\n");
  }
  const std::string values[] = {std::to_string(event.achievement.gamerscore),
                                event.achievement.label};
  size_t value = 0;
  for (size_t at = format.find("%s"); at != std::string::npos && value < 2;
       at = format.find("%s", at)) {
    format.replace(at, 2, values[value]);
    at += values[value].size();
    ++value;
  }
  return format;
}

}

GuideNotificationDialog::GuideNotificationDialog(
    ImGuiDrawer* drawer, MediaSource source, GuideFonts fonts,
    std::unique_ptr<AchievementNotificationDialog> fallback)
    : AchievementNotificationDialog(drawer),
      source_(std::move(source)),
      fonts_(fonts),
      fallback_(std::move(fallback)) {
  render_.regular_font = fonts_.regular;
  render_.bold_font = fonts_.bold;
  render_.texture = [this](std::string_view path, std::string_view package, int* w, int* h) {
    return media_ ? media_->Texture(path, package, w, h) : ImTextureID{};
  };
}

GuideNotificationDialog::~GuideNotificationDialog() = default;

void GuideNotificationDialog::Push(const system::AchievementEvent& event) {
  if (!REXCVAR_GET(notifications_show)) {
    return;
  }
  std::lock_guard<std::mutex> lock(mutex_);
  queue_.push_back({event, {}, {}});
}

void GuideNotificationDialog::PushMessage(std::string text, std::string image) {
  if (!REXCVAR_GET(notifications_show)) {
    return;
  }
  std::lock_guard<std::mutex> lock(mutex_);
  queue_.push_back({std::nullopt, std::move(text), std::move(image)});
}

void GuideNotificationDialog::Start(const Notice& notice, const Media& media) {
  assets_ = media.assets;
  media_ = media.media;
  context_.skin = &assets_->skin;
  context_.package = "xam/xam";
  context_.play_sound = [this](std::string_view file, std::string_view package) {
    if (media_ && REXCVAR_GET(notifications_sound)) {
      media_->PlaySound(file, package);
    }
  };
  scene_ = xui::Element::Create(assets_->notify.root, context_);
  popup_ = scene_->FindById("PopupControl");
  if (!popup_) {
    scene_.reset();
    return;
  }
  popup_->SetText(notice.achievement ? UnlockText(assets_->xam_strings, *notice.achievement)
                                     : notice.text);
  popup_->Set("ImagePath",
              xui::Value{notice.achievement ? std::string("xam://Achievement.png") : notice.image});

  popup_->Play("TransTo");
  last_tick_ = started_ = std::chrono::steady_clock::now();
}

void GuideNotificationDialog::OnDraw(ImGuiIO& io) {
  const auto now = std::chrono::steady_clock::now();
  if (std::string version; TakeAppUpdateNotice(&version)) {
    PushMessage(fmt::format("Game update available\nGet version {} from the Guide.", version),
                "xam://ico_32x_information.png");
  }
  if (!scene_) {
    std::optional<Notice> next;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (!queue_.empty()) {
        next = queue_.front();
      }
    }
    if (!next) {
      return;
    }
    Media media = source_ ? source_() : Media{};
    if (media.unavailable || (media.assets && !media.assets->has_notify)) {
      std::lock_guard<std::mutex> lock(mutex_);
      if (fallback_) {
        for (const auto& notice : queue_) {
          if (notice.achievement) {
            fallback_->Push(*notice.achievement);
          }
        }
      }
      queue_.clear();
      return;
    }
    if (!media.assets || !media.media) {
      return;
    }
    {
      std::lock_guard<std::mutex> lock(mutex_);
      queue_.pop_front();
    }
    Start(*next, media);
    if (!scene_) {
      return;
    }
  }

  const double seconds = std::chrono::duration<double>(now - last_tick_).count();
  last_tick_ = now;
  scene_->Advance(std::min(seconds, 0.25) * xui::kFramesPerSecond);

  constexpr double kHoldSeconds = 4.0;
  constexpr double kTransFromFrame = 186.0;
  if (popup_->frame() < kTransFromFrame &&
      std::chrono::duration<double>(now - started_).count() >= kHoldSeconds) {
    popup_->Play("TransFrom");
  }

  const float scale = std::min(io.DisplaySize.x / kSceneWidth, io.DisplaySize.y / kSceneHeight);
  const ImVec2 origin((io.DisplaySize.x - kSceneWidth * scale) / 2 + kPopupX * scale,
                      (io.DisplaySize.y - kSceneHeight * scale) / 2 + kPopupY * scale);
  render_.pixels_per_point = imgui_drawer()->PixelsPerPoint();
  xui::Render(ImGui::GetForegroundDrawList(), *scene_, origin, scale, 1.0f, render_);

  if (!popup_->playing()) {
    scene_.reset();
    popup_ = nullptr;
  }
}

}
