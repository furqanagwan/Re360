/**
 * @file        ui/guide/include/rex/ui/guide/guide_layout.h
 * @brief       Removing and adding menu entries in the console's scenes (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <cmath>
#include <string>
#include <string_view>

#include <rex/ui/xui/runtime.h>

struct ImFont;

namespace rex::ui::guide {

inline const xui::Node& SceneNode(const xui::Document& document) {
  return document.root.children.empty() ? document.root : document.root.children.front();
}

bool UseThreeTabs(xui::Node& guide_main);

void LayOutLegends(xui::Element& backdrop, const xui::Node& backdrop_root, ImFont* font);

inline void RemoveEntry(xui::Element* scene, std::string_view id) {
  xui::Element* entry = scene->FindById(id);
  if (!entry || entry->suppressed()) {
    return;
  }
  const std::string up(entry->GetString("NavUp"));
  const std::string down(entry->GetString("NavDown"));
  const float y = entry->GetVector("Position").y;
  const float height = entry->height();
  entry->Suppress();
  xui::Element* parent = entry->parent();
  for (const auto& sibling : parent->children()) {
    xui::Element* s = sibling.get();
    if (s == entry) {
      continue;
    }
    if (s->GetString("NavDown") == id) {
      s->Set("NavDown", xui::Value{down});
    }
    if (s->GetString("NavUp") == id) {
      s->Set("NavUp", xui::Value{up});
    }
    xui::Vec3 p = s->GetVector("Position");
    if (s->IsA("XuiControl") && !s->suppressed() && p.y > y + 0.5f) {
      p.y -= height;
      s->Set("Position", xui::Value{p});
    }
  }
}

inline xui::Element* AddEntry(xui::Element* scene, std::string_view model, std::string_view after,
                              std::string id, std::string text, std::string_view visual = {}) {
  xui::Element* model_entry = scene->FindById(model);
  xui::Element* after_entry = scene->FindById(after);
  if (!model_entry || !after_entry || model_entry->parent() != after_entry->parent()) {
    return nullptr;
  }
  xui::Element* parent = after_entry->parent();
  const float y = after_entry->GetVector("Position").y;
  const float height = after_entry->height();
  for (const auto& sibling : parent->children()) {
    xui::Vec3 p = sibling->GetVector("Position");
    if (sibling->IsA("XuiControl") && !sibling->suppressed() && p.y > y + 0.5f) {
      p.y += height;
      sibling->Set("Position", xui::Value{p});
    }
  }
  xui::Element* entry = parent->CloneChild(*model_entry, id, visual);
  xui::Vec3 p = model_entry->GetVector("Position");
  p.y = y + height;
  entry->Set("Position", xui::Value{p});
  entry->SetText(std::move(text));
  entry->SetVisible(true);
  const std::string below(after_entry->GetString("NavDown"));
  entry->Set("NavUp", xui::Value{std::string(after)});
  entry->Set("NavDown", xui::Value{below});
  after_entry->Set("NavDown", xui::Value{id});
  if (xui::Element* next = below.empty() ? nullptr : scene->FindById(below)) {
    next->Set("NavUp", xui::Value{id});
  }
  return entry;
}

inline void ScrollMenuTo(xui::Element* entry) {
  xui::Element* menu = entry ? entry->parent() : nullptr;
  if (!menu || !menu->IsA("XuiScene")) {
    return;
  }
  const float height = menu->height();
  bool overflows = false;
  for (const auto& child : menu->children()) {
    if (child->IsA("XuiControl") && child->visible() &&
        child->GetVector("Position").y + child->height() > height + 0.5f) {
      overflows = true;
    }
  }
  if (overflows) {
    menu->Set("ClipChildren", xui::Value{true});
  }
  const float row = entry->height();
  const float top = entry->GetVector("Position").y;
  float shift = 0.0f;
  if (top < -0.5f) {
    shift = -top;
  } else if (row > 0.0f && top + row > height + 0.5f) {
    shift = -std::ceil((top + row - height) / row) * row;
  }
  if (shift == 0.0f) {
    return;
  }
  for (const auto& child : menu->children()) {
    if (child->IsA("XuiControl") && !child->suppressed()) {
      xui::Vec3 p = child->GetVector("Position");
      p.y += shift;
      child->Set("Position", xui::Value{p});
    }
  }
}

}
