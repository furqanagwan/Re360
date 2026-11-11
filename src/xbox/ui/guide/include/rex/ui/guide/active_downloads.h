// Copyright (c) 2026 Xbox Guide contributors. BSD 3-Clause License; see LICENSE.
#pragma once
#include <functional>
#include <rex/ui/xui/runtime.h>

namespace rex::ui::guide {

struct GuideActivity {
  std::string title;
  std::string status;
  std::string details;
  std::function<void()> cancel;
};

void PrepareActiveDownloadsScene(xui::Element& root);

class ActiveDownloadsScene {
 public:
  ActiveDownloadsScene(const ActiveDownloadsScene&) = delete;
  ActiveDownloadsScene& operator=(const ActiveDownloadsScene&) = delete;
  ActiveDownloadsScene(ActiveDownloadsScene&&) = delete;
  ActiveDownloadsScene& operator=(ActiveDownloadsScene&&) = delete;

  static std::unique_ptr<ActiveDownloadsScene> Create(const xui::Document& document,
                                                      const xui::SceneContext& context);
  xui::Element& root() { return *root_; }
  void Update(std::vector<GuideActivity> activities);
  void Focus(size_t index);
  void Activate();
  size_t size() const { return rows_.size(); }
  xui::Element* row(size_t index) const { return index < rows_.size() ? rows_[index] : nullptr; }

 private:
  ActiveDownloadsScene() = default;
  xui::SceneContext context_;
  std::unique_ptr<xui::Element> root_;
  std::vector<GuideActivity> activities_;
  std::vector<xui::Element*> rows_;
  size_t focused_ = 0;
};
}
