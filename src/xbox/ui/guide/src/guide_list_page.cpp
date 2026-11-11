// Copyright (c) 2026 Xbox Guide contributors. BSD 3-Clause License; see LICENSE.
#include <rex/ui/guide/guide_list_page.h>

#include <algorithm>
#include <ctime>

#include <fmt/format.h>

#include <rex/ui/guide/guide_layout.h>

namespace rex::ui::guide {
namespace {
constexpr float kRowHeight = 28.0f;
constexpr float kPaneTop = 61.0f;
constexpr float kPaneBottom = 405.0f;
constexpr float kPaneMargin = 8.0f;
}

std::unique_ptr<GuideListPage> GuideListPage::Create(const xui::Document& frame,
                                                     const xui::SceneContext& frame_context,
                                                     const xui::Document& options,
                                                     const xui::SceneContext& options_context) {
  if (!frame_context.skin || !options_context.skin)
    return nullptr;
  auto page = std::unique_ptr<GuideListPage>(new GuideListPage);
  page->backdrop_context_ = frame_context;
  page->hud_context_ = options_context;
  page->backdrop_ = xui::Element::Create(frame.root, page->backdrop_context_);
  if (!page->backdrop_)
    return nullptr;
  page->hud_root_ = page->backdrop_->FindById("HUDRootScene");
  xui::Element* host = page->backdrop_->FindById("AppHostElementId");
  if (!page->hud_root_ || !host)
    return nullptr;
  page->scene_ = host->AttachScene(SceneNode(options), page->hud_context_);
  xui::Element* model = page->scene_ ? page->scene_->FindById("chkShow") : nullptr;
  page->details_ = page->scene_ ? page->scene_->FindById("XuiLabel1") : nullptr;
  if (!model || !page->details_)
    return nullptr;

  xui::Vec3 details = page->details_->GetVector("Position");
  details.y = kPaneTop + kPaneMargin;
  page->details_->Set("Position", xui::Value{details});
  page->details_->Set("Height", xui::Value{kPaneBottom - details.y});
  xui::Vec3 position = model->GetVector("Position");
  const float top = position.y;
  for (size_t i = 0; i < kVisibleRows; ++i) {
    xui::Element* row =
        model->parent()->CloneChild(*model, fmt::format("btnListRow{}", i), "btn_Count");
    position.y = top + float(i) * kRowHeight;
    row->Set("Position", xui::Value{position});
    row->SetVisible(false);
    page->slots_.push_back(row);
  }
  for (const char* id :
       {"chkShow", "chkSound", "chkShowMovies", "chkShowIPTV", "labelSoundDisabled", "XuiLabel2"})
    if (xui::Element* e = page->scene_->FindById(id))
      e->Suppress();
  page->SetLegend("XButton", "XText", {});
  page->SetLegend("YButton", "YText", {});
  if (xui::Element* clock = page->backdrop_->FindById("DateTimeTextId")) {
    const std::time_t now = std::time(nullptr);
    std::tm local = {};
    localtime_s(&local, &now);
    const int hour = local.tm_hour % 12 == 0 ? 12 : local.tm_hour % 12;
    clock->SetText(
        fmt::format("{}:{:02} {}", hour, local.tm_min, local.tm_hour < 12 ? "AM" : "PM"));
  }
  page->hud_root_->Play("ClosedToFull");
  return page;
}

void GuideListPage::SetLegend(const char* button, const char* label, const std::string& text) {
  xui::Element* glyph = backdrop_->FindById(button);
  xui::Element* words = backdrop_->FindById(label);
  if (!glyph || !words)
    return;
  glyph->SetVisible(!text.empty());
  words->SetVisible(!text.empty());
  words->SetText(text);
}

void GuideListPage::Show(std::string heading, std::vector<GuideListRow> rows, size_t focus,
                         std::string legend_a, std::string legend_b, std::string empty_details) {
  if (xui::Element* label = scene_->FindById("labelHeading"))
    label->SetText(std::move(heading));
  rows_ = std::move(rows);
  empty_details_ = std::move(empty_details);
  SetLegend("AButton", "AText", rows_.empty() ? std::string{} : legend_a);
  SetLegend("BButton", "BText", legend_b);
  focused_ = rows_.empty() ? 0 : std::min(focus, rows_.size() - 1);

  first_ = rows_.size() > kVisibleRows ? std::min(focused_, rows_.size() - kVisibleRows) : 0;
  Refresh();
}

void GuideListPage::Refresh() {
  for (size_t i = 0; i < slots_.size(); ++i) {
    const size_t index = first_ + i;
    xui::Element* row = slots_[i];
    row->SetVisible(index < rows_.size());
    row->SetText(index < rows_.size() ? rows_[index].text : std::string{});
    row->SetSecondaryText(index < rows_.size() ? rows_[index].secondary : std::string{});
  }
  xui::Element* target = rows_.empty() ? nullptr : slots_[focused_ - first_];
  if (target != focus_) {
    const bool initial = !focus_;
    if (target)
      xui::Element::MoveFocus(focus_, target, initial);
    else if (focus_)
      xui::Element::MoveFocus(focus_, nullptr);
    focus_ = target;
  }
  details_->SetText(rows_.empty() ? empty_details_ : rows_[focused_].details);
}

void GuideListPage::Move(int direction) {
  if (rows_.empty() || !direction)
    return;
  const size_t next =
      direction < 0 ? (focused_ ? focused_ - 1 : 0) : std::min(focused_ + 1, rows_.size() - 1);
  if (next == focused_)
    return;
  Focus(next);
}

void GuideListPage::Focus(size_t index) {
  if (index >= rows_.size() || index == focused_)
    return;
  focused_ = index;
  const bool scrolled = focused_ < first_ || focused_ >= first_ + kVisibleRows;
  if (focused_ < first_)
    first_ = focused_;
  else if (focused_ >= first_ + kVisibleRows)
    first_ = focused_ + 1 - kVisibleRows;
  Refresh();

  if (scrolled && hud_context_.play_sound)
    hud_context_.play_sound("sharedres://btn_Focus.xma", "");
}

std::optional<size_t> GuideListPage::Activate() {
  if (rows_.empty() || !focus_)
    return std::nullopt;
  focus_->Press();
  return focused_;
}

void GuideListPage::PlayClose() {
  hud_root_->Play("FullToClosed");
}
}
