// Copyright (c) 2026 Xbox Guide contributors. BSD 3-Clause License; see LICENSE.
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <rex/ui/xui/runtime.h>

namespace rex::ui::guide {
struct GuideListRow {
  std::string text;
  std::string secondary;
  std::string details;
};

class GuideListPage {
 public:
  static constexpr float kSceneWidth = 852.0f;
  static constexpr float kSceneHeight = 480.0f;
  static constexpr size_t kVisibleRows = 11;

  GuideListPage(const GuideListPage&) = delete;
  GuideListPage& operator=(const GuideListPage&) = delete;

  static std::unique_ptr<GuideListPage> Create(const xui::Document& frame,
                                               const xui::SceneContext& frame_context,
                                               const xui::Document& options,
                                               const xui::SceneContext& options_context);

  void Show(std::string heading, std::vector<GuideListRow> rows, size_t focus = 0,
            std::string legend_a = "Select", std::string legend_b = "Back",
            std::string empty_details = {});

  void Move(int direction);
  void Focus(size_t index);

  std::optional<size_t> Activate();
  size_t focused() const { return focused_; }
  size_t size() const { return rows_.size(); }
  size_t first_visible() const { return first_; }

  xui::Element* slot(size_t index) const {
    return index < slots_.size() && slots_[index]->visible() ? slots_[index] : nullptr;
  }
  xui::Element& root() { return *backdrop_; }
  xui::Element& scene() { return *scene_; }

  void PlayClose();
  bool animating() const { return hud_root_->playing(); }

 private:
  GuideListPage() = default;
  void Refresh();
  void SetLegend(const char* button, const char* label, const std::string& text);

  xui::SceneContext backdrop_context_, hud_context_;
  std::unique_ptr<xui::Element> backdrop_;
  xui::Element* hud_root_ = nullptr;
  xui::Element* scene_ = nullptr;
  xui::Element* details_ = nullptr;
  std::vector<xui::Element*> slots_;
  std::vector<GuideListRow> rows_;
  std::string empty_details_;
  size_t first_ = 0;
  size_t focused_ = 0;
  xui::Element* focus_ = nullptr;
};
}
