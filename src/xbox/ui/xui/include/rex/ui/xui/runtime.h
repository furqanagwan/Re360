/**
 * @file        ui/xui/include/rex/ui/xui/runtime.h
 * @brief       Live XUI elements: skin visuals, anchoring, named-frame timelines, focus
 * (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <rex/ui/xui/document.h>

namespace rex::ui::xui {

class SystemUpdate;

constexpr double kFramesPerSecond = 60.0;

enum AnchorFlags : uint32_t {
  kAnchorLeft = 0x01,
  kAnchorTop = 0x02,
  kAnchorRight = 0x04,
  kAnchorBottom = 0x08,
  kAnchorXCenter = 0x10,
  kAnchorYCenter = 0x20,
  kAnchorXScale = 0x40,
  kAnchorYScale = 0x80,
};

enum class NavDirection { kUp, kDown, kLeft, kRight };

std::span<const uint8_t> ResolveFile(const SystemUpdate& update, std::string_view path,
                                     std::string_view base_package);

float EaseProgress(float t, int8_t ease_in, int8_t ease_out);

struct SceneContext {
  const Document* skin = nullptr;
  std::string package;

  std::function<void(std::string_view file, std::string_view package)> play_sound;
};

class Element {
 public:
  static std::unique_ptr<Element> Create(const Node& node, const SceneContext& context);

  ~Element();
  Element(const Element&) = delete;
  Element& operator=(const Element&) = delete;

  std::string_view id() const;
  const std::string& class_name() const { return class_name_; }

  bool IsA(std::string_view name) const;
  Element* parent() const { return parent_; }
  const std::vector<std::unique_ptr<Element>>& children() const { return children_; }

  Element* FindById(std::string_view id);
  const std::string& package() const { return context_->package; }
  const SceneContext& context() const { return *context_; }

  const Value* Get(std::string_view name) const;
  float GetFloat(std::string_view name, float fallback = 0.0f) const;
  bool GetBool(std::string_view name, bool fallback = false) const;
  uint32_t GetUnsigned(std::string_view name, uint32_t fallback = 0) const;
  int32_t GetInteger(std::string_view name, int32_t fallback = 0) const;
  uint32_t GetColor(std::string_view name, uint32_t fallback = 0) const;
  std::string_view GetString(std::string_view name) const;
  Vec3 GetVector(std::string_view name, Vec3 fallback = {}) const;
  Quat GetQuaternion(std::string_view name) const;

  const PropertyBag* GetCompound(std::string_view name) const;
  void Set(std::string_view name, Value value);

  void SetText(std::string text) { Set("Text", Value{std::move(text)}); }
  std::string_view text() const { return GetString("Text"); }

  void SetSecondaryText(std::string text) { secondary_text_ = std::move(text); }
  const std::string& secondary_text() const { return secondary_text_; }

  bool visible() const { return !suppressed_ && GetBool("Show", true); }
  void SetVisible(bool show) { Set("Show", Value{show}); }

  void Suppress() { suppressed_ = true; }
  bool suppressed() const { return suppressed_; }

  float width() const;
  float height() const;

  Vec3 position() const;

  bool HasNamedFrame(std::string_view name) const;

  bool Play(std::string_view name, bool sounds = true);
  bool playing() const { return playing_; }
  double frame() const { return frame_; }

  void Seek(double frame);

  void Advance(double frames);

  Element* AttachScene(const Node& node, const SceneContext& context);

  Element* CloneChild(const Element& source, std::string id, std::string_view visual = {});
  void RemoveChild(Element* child);

  std::vector<Element*> PopulateList(size_t count, int columns = 1);
  const std::vector<Element*>& list_items() const { return list_items_; }

  bool focusable() const;
  bool enabled() const { return GetBool("Enabled", true); }

  void Press();

  void SetChecked(bool checked);
  bool checked() const { return checked_; }
  bool focused() const { return focused_; }

  Element* Navigate(NavDirection direction);

  static void MoveFocus(Element* from, Element* to, bool initial = false);

 private:
  struct TimelineSet {
    const std::vector<Timeline>* timelines = nullptr;
    const std::vector<NamedFrame>* frames = nullptr;
  };

  Element(const Node& node, Element* parent, const SceneContext* context);
  void Build(const Node& node);
  void ApplyVisual(const Node& visual);
  void AddTimelines(const Node& owner);
  void ApplyFrame(double frame);
  void FireSounds(double from, double to, bool inclusive);
  const PropDef* FindDef(std::string_view name) const;
  void SetPath(std::span<const PropDef* const> path, int32_t index, const Value& value);

  bool PlayState(std::string_view base, bool sounds);

  Element* parent_ = nullptr;
  const Node* node_ = nullptr;
  const SceneContext* context_ = nullptr;
  std::unique_ptr<SceneContext> owned_context_;
  const ClassDef* cls_ = nullptr;
  std::string class_name_;
  PropertyBag props_;
  std::vector<std::unique_ptr<Element>> children_;
  std::string secondary_text_;
  std::vector<Element*> list_items_;

  float design_parent_width_ = 0.0f;
  float design_parent_height_ = 0.0f;

  std::vector<TimelineSet> timeline_sets_;
  double frame_ = 0.0;
  bool playing_ = false;
  bool suppressed_ = false;
  bool checked_ = false;
  bool focused_ = false;
};

}
