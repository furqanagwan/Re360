// Copyright (c) 2026 Xbox Guide contributors. BSD 3-Clause License; see LICENSE.
#include <rex/ui/guide/active_downloads.h>

namespace rex::ui::guide {
void PrepareActiveDownloadsScene(xui::Element& root) {
  if (auto* heading = root.FindById("labelHeading"))
    heading->SetText("Active Downloads");
  for (const auto* id :
       {"chkShow", "chkSound", "chkShowMovies", "chkShowIPTV", "labelSoundDisabled", "XuiLabel2"})
    if (auto* element = root.FindById(id))
      element->Suppress();
}

std::unique_ptr<ActiveDownloadsScene> ActiveDownloadsScene::Create(
    const xui::Document& document, const xui::SceneContext& context) {
  auto model = std::unique_ptr<ActiveDownloadsScene>(new ActiveDownloadsScene);
  model->context_ = context;
  model->root_ = xui::Element::Create(document.root, model->context_);
  if (!model->root_ || !model->root_->FindById("chkShow"))
    return nullptr;
  PrepareActiveDownloadsScene(*model->root_);
  return model;
}
void ActiveDownloadsScene::Update(std::vector<GuideActivity> activities) {
  if (activities.size() > 11)
    activities.resize(11);
  activities_ = std::move(activities);
  if (rows_.size() != activities_.size()) {
    for (auto* row : rows_)
      row->parent()->RemoveChild(row);
    rows_.clear();
    auto* prototype = root_->FindById("chkShow");
    const auto top = prototype->GetVector("Position");
    for (size_t i = 0; i < activities_.size(); ++i) {
      auto* row =
          prototype->parent()->CloneChild(*prototype, "activity" + std::to_string(i), "btn_Count");
      auto position = top;
      position.y += float(i) * 28;
      row->Set("Position", xui::Value{position});
      row->SetVisible(true);
      rows_.push_back(row);
    }
    focused_ = rows_.empty() ? 0 : std::min(focused_, rows_.size() - 1);
    if (!rows_.empty())
      xui::Element::MoveFocus(nullptr, rows_[focused_], true);
  }
  for (size_t i = 0; i < rows_.size(); ++i) {
    rows_[i]->SetText(activities_[i].title);
    rows_[i]->SetSecondaryText(activities_[i].status);
  }
  if (auto* details = root_->FindById("XuiLabel1"))
    details->SetText(rows_.empty() ? "Nothing is downloading." : activities_[focused_].details);
}
void ActiveDownloadsScene::Focus(size_t index) {
  if (index >= rows_.size())
    return;
  xui::Element::MoveFocus(rows_[focused_], rows_[index]);
  focused_ = index;
  if (auto* details = root_->FindById("XuiLabel1"))
    details->SetText(activities_[index].details);
}
void ActiveDownloadsScene::Activate() {
  if (focused_ < activities_.size() && activities_[focused_].cancel) {
    rows_[focused_]->Press();
    activities_[focused_].cancel();
  }
}
}
