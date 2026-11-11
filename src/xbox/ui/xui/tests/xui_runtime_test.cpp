/**
 * @file        ui/xui/tests/xui_runtime_test.cpp
 * @brief       Live XUI elements: visuals, anchoring, timelines, sounds, focus (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cfloat>
#include <cstdlib>
#include <filesystem>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <imgui.h>

#include <rex/ui/guide/guide_layout.h>
#include <rex/ui/guide/xbox_guide.h>
#include <rex/ui/xui/document.h>
#include <rex/ui/xui/renderer.h>
#include <rex/ui/xui/text_scroll.h>
#include <rex/ui/xui/runtime.h>
#include <rex/ui/xui/system_update.h>

#include "xui_test_data.h"

using namespace rex::ui::xui;
using Catch::Approx;

namespace {

const PropDef* Def(std::string_view cls, std::string_view name) {
  for (const ClassDef* c : ClassChain(FindClass(cls))) {
    for (const PropDef& def : c->props) {
      if (def.name == name) {
        return &def;
      }
    }
  }
  FAIL("no property " << name << " on " << cls);
  return nullptr;
}

using Props = std::vector<std::pair<std::string_view, Value>>;

Node MakeNode(std::string_view cls, Props props, std::vector<Node> children = {}) {
  Node node;
  node.class_name = std::string(cls);
  node.cls = FindClass(cls);
  REQUIRE(node.cls);
  auto bag = std::make_shared<PropertyBag>();
  for (auto& [name, value] : props) {
    bag->entries.push_back({Def(cls, name), std::move(value)});
  }
  node.props = bag;
  node.children = std::move(children);
  return node;
}

Value Str(std::string s) {
  return Value{std::move(s)};
}

Keyframe Key(int32_t frame, Value value, Interpolation interpolation = Interpolation::kLinear,
             int8_t ease_in = 0, int8_t ease_out = 0) {
  Keyframe k;
  k.frame = frame;
  k.interpolation = interpolation;
  k.ease_in = ease_in;
  k.ease_out = ease_out;
  k.values.push_back(std::move(value));
  return k;
}

Timeline Track(std::string target, std::vector<const PropDef*> path, std::vector<Keyframe> keys) {
  Timeline t;
  t.target_id = std::move(target);
  AnimatedProperty prop;
  prop.path = std::move(path);
  t.props.push_back(prop);
  t.keyframes = std::move(keys);
  return t;
}

NamedFrame Frame(std::string name, int32_t frame, FrameCommand command = FrameCommand::kPlay) {
  NamedFrame f;
  f.name = std::move(name);
  f.frame = frame;
  f.command = command;
  return f;
}

Document MakeSkin() {
  auto fill = std::make_shared<PropertyBag>();
  fill->entries.push_back({Def("XuiFigureFill", "FillColor"), Value{Color{0xFF008A00}}});
  Node visual = MakeNode(
      "XuiVisual", {{"Id", Str("Btn")}, {"Width", Value{100.0f}}, {"Height", Value{20.0f}}},
      {
          MakeNode("XuiFigure", {{"Id", Str("hl")},
                                 {"Width", Value{100.0f}},
                                 {"Height", Value{20.0f}},
                                 {"Anchor", Value{uint32_t(15)}},
                                 {"Fill", Value{std::shared_ptr<const PropertyBag>(fill)}}}),
          MakeNode("XuiTextPresenter", {{"Id", Str("text_Label")},
                                        {"Width", Value{80.0f}},
                                        {"Anchor", Value{uint32_t(kAnchorLeft | kAnchorRight)}}}),
          MakeNode("XuiImagePresenter", {{"Id", Str("icon")},
                                         {"Position", Value{Vec3{90.0f, 0.0f, 0.0f}}},
                                         {"Width", Value{10.0f}},
                                         {"Anchor", Value{uint32_t(kAnchorRight)}}}),
          MakeNode("XuiSoundXAudio", {{"Id", Str("snd")}}),
      });
  const PropDef* opacity = Def("XuiFigure", "Opacity");
  visual.timelines.push_back(
      Track("hl", {opacity},
            {Key(0, Value{0.0f}), Key(5, Value{0.0f}, Interpolation::kEase, -100, 100),
             Key(15, Value{1.0f}), Key(16, Value{0.5f})}));
  visual.timelines.push_back(Track(
      "hl", {Def("XuiFigure", "Fill"), Def("XuiFigureFill", "FillColor")},
      {Key(0, Value{Color{0xFF008A00}}), Key(16, Value{Color{0xFF1CB61C}}, Interpolation::kNone)}));
  visual.timelines.push_back(Track(
      "snd", {Def("XuiSoundXAudio", "File")},
      {Key(0, Str(""), Interpolation::kNone), Key(5, Str("focus.xma"), Interpolation::kNone),
       Key(6, Str(""), Interpolation::kNone), Key(16, Str("press.xma"), Interpolation::kNone)}));
  visual.named_frames = {Frame("KillFocus", 0), Frame("EndKillFocus", 4, FrameCommand::kStop),
                         Frame("Focus", 5),     Frame("EndFocus", 15, FrameCommand::kStop),
                         Frame("Press", 16),    Frame("EndPress", 18, FrameCommand::kStop)};

  Node label = MakeNode("XuiVisual", {{"Id", Str("XuiLabel")}, {"Width", Value{100.0f}}},
                        {MakeNode("XuiTextPresenter", {{"Id", Str("Text")}})});
  Document skin;
  skin.root = MakeNode("XuiCanvas", {}, {std::move(visual), std::move(label)});
  return skin;
}

Node Button(std::string id, Props extra) {
  Props props = {{"Id", Str(std::move(id))},
                 {"Visual", Str("Btn")},
                 {"Width", Value{150.0f}},
                 {"Height", Value{20.0f}}};
  for (auto& p : extra) {
    props.push_back(std::move(p));
  }
  return MakeNode("XuiButton", std::move(props));
}

Document MakeScene() {
  Document doc;
  doc.root =
      MakeNode("XuiCanvas", {{"Width", Value{300.0f}}, {"Height", Value{200.0f}}},
               {MakeNode("XuiScene", {{"Id", Str("scene")}, {"Width", Value{300.0f}}},
                         {Button("a", {{"NavDown", Str("b")}}),
                          Button("b", {{"NavDown", Str("c")}, {"Show", Value{false}}}),
                          Button("c", {{"NavUp", Str("a")}}),
                          MakeNode("XuiLabel", {{"Id", Str("lbl")}, {"Text", Str("Hi")}})})});
  return doc;
}

struct Fixture {
  Document skin = MakeSkin();
  Document scene = MakeScene();
  std::vector<std::string> sounds;
  SceneContext context;
  std::unique_ptr<Element> root;

  Fixture() {
    context.skin = &skin;
    context.package = "hud/hud";
    context.play_sound = [this](std::string_view file, std::string_view) {
      sounds.emplace_back(file);
    };
    root = Element::Create(scene.root, context);
  }
  Element* Find(std::string_view id) { return root->FindById(id); }
};

}

TEST_CASE("Controls take their skin visual and anchor it to their own size", "[xui]") {
  Fixture f;
  Element* a = f.Find("a");
  REQUIRE(a);
  Element* hl = a->FindById("hl");
  REQUIRE(hl);

  CHECK(hl->width() == Approx(150.0f));
  CHECK(a->FindById("text_Label")->width() == Approx(130.0f));
  CHECK(a->FindById("icon")->position().x == Approx(140.0f));
  CHECK(a->FindById("icon")->width() == Approx(10.0f));

  REQUIRE(f.Find("lbl")->FindById("Text"));
}

TEST_CASE("Timelines show frame 0 when built and play named frames with easing", "[xui]") {
  Fixture f;
  Element* a = f.Find("a");
  Element* hl = a->FindById("hl");
  CHECK(hl->GetFloat("Opacity", -1.0f) == Approx(0.0f));
  CHECK(f.sounds.empty());

  REQUIRE(a->Play("Focus"));
  CHECK(f.sounds == std::vector<std::string>{"focus.xma"});
  a->Advance(5.0);

  CHECK(hl->GetFloat("Opacity") == Approx(0.75f));
  CHECK(a->playing());
  a->Advance(20.0);

  CHECK_FALSE(a->playing());
  CHECK(a->frame() == Approx(15.0));
  CHECK(hl->GetFloat("Opacity") == Approx(1.0f));
  CHECK(f.sounds.size() == 1);
}

TEST_CASE("Sound cues fire once as the playhead crosses them", "[xui]") {
  Fixture f;
  Element* a = f.Find("a");
  REQUIRE(a->Play("Press"));
  a->Advance(1.0);
  a->Advance(5.0);
  CHECK(f.sounds == std::vector<std::string>{"press.xma"});
  CHECK_FALSE(a->Play("NoSuchFrame"));
}

TEST_CASE("Compound properties animate per element, not in the shared scene data", "[xui]") {
  Fixture f;
  Element* a = f.Find("a");
  Element* c = f.Find("c");
  a->Play("Press");
  auto fill_color = [](Element* e) {
    const Value* v = e->FindById("hl")->GetCompound("Fill")->Find("FillColor");
    return v->get<Color>()->argb;
  };
  CHECK(fill_color(a) == 0xFF1CB61C);
  CHECK(fill_color(c) == 0xFF008A00);

  const auto* doc_fill =
      f.skin.root.children[0].children[0].Find("Fill")->get<std::shared_ptr<const PropertyBag>>();
  CHECK((*doc_fill)->Find("FillColor")->get<Color>()->argb == 0xFF008A00);
}

TEST_CASE("Navigation passes over hidden controls and focus plays the visual", "[xui]") {
  Fixture f;
  Element* a = f.Find("a");
  Element* c = f.Find("c");
  CHECK(a->focusable());
  CHECK_FALSE(f.Find("b")->focusable());
  CHECK(a->Navigate(NavDirection::kDown) == c);
  CHECK(c->Navigate(NavDirection::kUp) == a);
  CHECK(c->Navigate(NavDirection::kDown) == nullptr);

  Element::MoveFocus(a, c);
  CHECK(c->frame() == Approx(5.0));
  CHECK(a->frame() == Approx(0.0));
  CHECK(f.sounds == std::vector<std::string>{"focus.xma"});
}

TEST_CASE("Seek poses an element at a frame without playing", "[xui]") {
  Fixture f;
  Element* a = f.Find("a");
  a->Seek(16.0);
  CHECK_FALSE(a->playing());
  CHECK(a->FindById("hl")->GetCompound("Fill")->Find("FillColor")->get<Color>()->argb ==
        0xFF1CB61C);
  CHECK(f.sounds.empty());
}

TEST_CASE("Menu entries are removed and added with the list closed up", "[xui][guide]") {
  Document skin = MakeSkin();
  Document doc;
  doc.root = MakeNode(
      "XuiCanvas", {},
      {MakeNode(
          "XuiScene", {{"Id", Str("scene")}},
          {Button("a", {{"NavDown", Str("b")}}),
           Button("b", {{"Position", Value{Vec3{0.0f, 20.0f, 0.0f}}},
                        {"NavUp", Str("a")},
                        {"NavDown", Str("c")}}),
           Button("c", {{"Position", Value{Vec3{0.0f, 40.0f, 0.0f}}}, {"NavUp", Str("b")}})})});
  SceneContext context;
  context.skin = &skin;
  auto root = Element::Create(doc.root, context);
  Element* scene = root->FindById("scene");
  Element* a = scene->FindById("a");
  Element* c = scene->FindById("c");

  rex::ui::guide::RemoveEntry(scene, "b");
  CHECK_FALSE(scene->FindById("b")->visible());
  CHECK(c->GetVector("Position").y == Approx(20.0f));
  CHECK(a->Navigate(NavDirection::kDown) == c);
  CHECK(c->Navigate(NavDirection::kUp) == a);

  Element* d = rex::ui::guide::AddEntry(scene, "a", "a", "d", "Dee");
  REQUIRE(d);
  CHECK(d->text() == "Dee");
  CHECK(d->visible());
  CHECK(d->GetVector("Position").y == Approx(20.0f));
  CHECK(c->GetVector("Position").y == Approx(40.0f));
  CHECK(a->Navigate(NavDirection::kDown) == d);
  CHECK(d->Navigate(NavDirection::kDown) == c);
  CHECK(c->Navigate(NavDirection::kUp) == d);
  CHECK(d->FindById("hl"));

  Element* e = rex::ui::guide::AddEntry(scene, "a", "d", "e", "Ee", "XuiLabel");
  REQUIRE(e);
  CHECK(e->GetString("Visual") == "XuiLabel");
  CHECK_FALSE(e->FindById("hl"));
  CHECK(e->GetVector("Position").y == Approx(40.0f));
  CHECK(c->GetVector("Position").y == Approx(60.0f));
  CHECK(d->Navigate(NavDirection::kDown) == e);
}

TEST_CASE("A menu taller than its scene scrolls the focused entry into view", "[xui][guide]") {
  Document skin = MakeSkin();
  std::vector<Node> entries;
  for (int i = 0; i < 5; ++i) {
    entries.push_back(Button(std::string(1, char('a' + i)),
                             {{"Position", Value{Vec3{0.0f, float(i) * 20.0f, 0.0f}}}}));
  }
  Document doc;
  doc.root = MakeNode(
      "XuiCanvas", {},
      {MakeNode("XuiScene", {{"Id", Str("scene")}, {"Height", Value{50.0f}}}, std::move(entries))});
  SceneContext context;
  context.skin = &skin;
  auto root = Element::Create(doc.root, context);
  Element* scene = root->FindById("scene");
  auto y = [&](std::string_view id) { return scene->FindById(id)->GetVector("Position").y; };

  rex::ui::guide::ScrollMenuTo(scene->FindById("a"));
  CHECK(scene->GetBool("ClipChildren"));
  CHECK(y("a") == Approx(0.0f));

  rex::ui::guide::ScrollMenuTo(scene->FindById("c"));
  CHECK(y("a") == Approx(-20.0f));
  CHECK(y("c") == Approx(20.0f));

  rex::ui::guide::ScrollMenuTo(scene->FindById("e"));
  CHECK(y("e") == Approx(20.0f));
  CHECK(y("a") == Approx(-60.0f));

  rex::ui::guide::ScrollMenuTo(scene->FindById("d"));
  CHECK(y("e") == Approx(20.0f));

  rex::ui::guide::ScrollMenuTo(scene->FindById("b"));
  CHECK(y("b") == Approx(0.0f));
  CHECK(y("a") == Approx(-20.0f));
}

TEST_CASE("A menu that fits its scene is neither scrolled nor clipped", "[xui][guide]") {
  Document skin = MakeSkin();
  Document doc;
  doc.root = MakeNode(
      "XuiCanvas", {},
      {MakeNode("XuiScene", {{"Id", Str("scene")}, {"Height", Value{50.0f}}},
                {Button("a", {}), Button("b", {{"Position", Value{Vec3{0.0f, 20.0f, 0.0f}}}})})});
  SceneContext context;
  context.skin = &skin;
  auto root = Element::Create(doc.root, context);
  Element* scene = root->FindById("scene");
  rex::ui::guide::ScrollMenuTo(scene->FindById("b"));
  CHECK_FALSE(scene->GetBool("ClipChildren"));
  CHECK(scene->FindById("b")->GetVector("Position").y == Approx(20.0f));
}

TEST_CASE("A long button legend pushes the next one along instead of running into it",
          "[xui][guide]") {
  auto at = [](float x) { return Value{Vec3{x, 0.0f, 0.0f}}; };
  auto legend_text = [&](std::string id, float x) {
    return MakeNode("XuiText", {{"Id", Str(std::move(id))},
                                {"Position", at(x)},
                                {"Width", Value{64.0f}},
                                {"PointSize", Value{10.0f}}});
  };
  Document doc;
  doc.root = MakeNode("XuiCanvas", {},
                      {MakeNode("XuiGroup", {{"Id", Str("AButton")}, {"Position", at(137.0f)}}),
                       legend_text("AText", 161.0f),
                       MakeNode("XuiGroup", {{"Id", Str("BButton")}, {"Position", at(224.0f)}}),
                       legend_text("BText", 249.0f)});
  SceneContext context;
  auto root = Element::Create(doc.root, context);
  ImGuiContext* imgui = ImGui::CreateContext();
  ImFont* font = ImGui::GetIO().Fonts->AddFontDefault();
  unsigned char* pixels = nullptr;
  int width = 0, height = 0;
  ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

  root->FindById("BText")->SetText("Back");
  SECTION("A short label keeps the console's spacing") {
    root->FindById("AText")->SetText("Select");
    rex::ui::guide::LayOutLegends(*root, doc.root, font);
    CHECK(root->FindById("BButton")->GetVector("Position").x == Approx(224.0f));
    CHECK(root->FindById("BText")->GetVector("Position").x == Approx(249.0f));
  }
  SECTION("A long label moves B past its end, and back when it is short again") {
    root->FindById("AText")->SetText("Check for Updates Again");
    rex::ui::guide::LayOutLegends(*root, doc.root, font);
    const float a_end =
        161.0f +
        font->CalcTextSizeA(10.0f * kPointToSceneUnits, FLT_MAX, 0.0f, "Check for Updates Again").x;
    CHECK(root->FindById("BButton")->GetVector("Position").x > a_end);
    CHECK(root->FindById("BText")->GetVector("Position").x -
              root->FindById("BButton")->GetVector("Position").x ==
          Approx(25.0f));
    root->FindById("AText")->SetText("Select");
    rex::ui::guide::LayOutLegends(*root, doc.root, font);
    CHECK(root->FindById("BButton")->GetVector("Position").x == Approx(224.0f));
  }
  ImGui::DestroyContext(imgui);
}

TEST_CASE("A disc's soft edge keeps the console's width on screen", "[xui]") {
  const GradientSpan console = EdgeFadeOnScreen({0.8f, 1.0f}, 1.0f);
  CHECK(console.start == Approx(0.8f));
  CHECK(console.end == Approx(1.0f));
  const GradientSpan at_4k = EdgeFadeOnScreen({0.8f, 1.0f}, 3.0f);
  CHECK((at_4k.start + at_4k.end) / 2 == Approx(0.9f));
  CHECK(at_4k.end - at_4k.start == Approx(0.2f / 3.0f));
  const GradientSpan below_720p = EdgeFadeOnScreen({0.421f, 0.561f}, 0.5f);
  CHECK(below_720p.start == Approx(0.421f));
  CHECK(below_720p.end == Approx(0.561f));
}

TEST_CASE("The XUI ease curve", "[xui]") {
  CHECK(EaseProgress(0.0f, -100, 100) == Approx(0.0f));
  CHECK(EaseProgress(1.0f, -100, 100) == Approx(1.0f));
  CHECK(EaseProgress(0.5f, 0, 0) == Approx(0.5f));
  CHECK(EaseProgress(0.25f, 0, 0) == Approx(0.25f));
  CHECK(EaseProgress(0.5f, -100, 100) == Approx(0.75f));
  CHECK(EaseProgress(0.5f, 100, -100) == Approx(0.25f));
}

TEST_CASE("Scene paths resolve by protocol, then the scene's package, then shared ones", "[xui]") {
  using namespace xui_test;
  SystemUpdate update;
  std::string error;
  REQUIRE(update.AddModule("hud", XexWithResource("hud", Xuiz({{"a.png", "A"}}), 0), &error));
  REQUIRE(update.AddModule("xam", XexWithResource("shrdres", Xuiz({{"b.png", "B"}}), 0), &error));
  REQUIRE(
      update.AddModule("huduiskin", XexWithResource("skin", Xuiz({{"c.png", "C"}}), 0), &error));
  auto text = [](std::span<const uint8_t> bytes) {
    return std::string(bytes.begin(), bytes.end());
  };
  CHECK(text(ResolveFile(update, "sharedres://b.png", "hud/hud")) == "B");
  CHECK(text(ResolveFile(update, "a.png", "hud/hud")) == "A");
  CHECK(text(ResolveFile(update, "c.png", "hud/hud")) == "C");
  CHECK(ResolveFile(update, "missing.png", "hud/hud").empty());
  CHECK(ResolveFile(update, "sharedres://a.png", "hud/hud").empty());
}

TEST_CASE("The console's guide scene plays its tab transitions", "[xui][local]") {
  const char* path = std::getenv("REXGLUE_SYSTEM_UPDATE");
  if (!path || !*path) {
    SKIP("REXGLUE_SYSTEM_UPDATE is not set");
  }
  std::string error;
  auto update = SystemUpdate::Load(path, &error);
  REQUIRE(update);
  auto skin = ParseXur(update->Find("huduiskin/skin")->Find("skin.xur"), &error);
  REQUIRE(skin);
  auto guide = ParseXur(update->Find("hud/hud")->Find("GuideMain.xur"), &error);
  REQUIRE(guide);
  std::vector<std::string> sounds;
  SceneContext context;
  context.skin = &*skin;
  context.package = "hud/hud";
  context.play_sound = [&](std::string_view file, std::string_view package) {
    sounds.emplace_back(file);
    CHECK_FALSE(ResolveFile(*update, file, package).empty());
  };
  auto root = Element::Create(guide->root, context);
  Element* tabs = root->FindById("Tabscene");
  REQUIRE(tabs);
  Element* blade7 = root->FindById("Blade7");
  REQUIRE(blade7);
  const float x_before = blade7->position().x;
  REQUIRE(tabs->Play("2To3"));
  tabs->Advance(40.0);
  CHECK_FALSE(tabs->playing());
  CHECK(tabs->frame() == Approx(36.0));
  CHECK(blade7->position().x != Approx(x_before));
  CHECK(sounds == std::vector<std::string>{"BladeSwitch_2.xma"});
}

namespace {

Node MakeGuideMain(bool with_left_blade) {
  const PropDef* position = Def("XuiElement", "Position");
  const PropDef* show = Def("XuiElement", "Show");
  constexpr int32_t kFrames[] = {1, 12, 13, 24, 25, 36, 49, 60, 61, 72, 73, 180, 232, 283};
  Node tabs = MakeNode("XuiTabScene", {{"Id", Str("Tabscene")}});
  const std::string_view ids[] = {"Blade5",      "Blade6",      "txt_Settings", "txt_Media",
                                  "txt_home",    "txt_Games",   "Tab2",         "Tab3",
                                  "txt_homeSel", "txt_MediaSel"};
  for (size_t i = 0; i < std::size(ids); ++i) {
    if (ids[i] == "Blade6" && !with_left_blade) {
      continue;
    }
    Timeline t;
    t.target_id = std::string(ids[i]);
    t.props.push_back({{position}, -1});
    t.props.push_back({{show}, -1});
    for (int32_t frame : kFrames) {
      Keyframe k;
      k.frame = frame;
      k.values = {Value{Vec3{float(i * 1000 + frame), 0.0f, 0.0f}}, Value{true}};
      t.keyframes.push_back(std::move(k));
    }
    tabs.timelines.push_back(std::move(t));
  }
  for (std::string_view name : {"1To2", "2To1", "2To3", "2To3End", "3To2", "3To4", "3To4End",
                                "4To3", "4To3End", "4Close"}) {
    tabs.named_frames.push_back(Frame(std::string(name), 0));
  }
  Node main = MakeNode("XuiScene", {{"Id", Str("HUDScene")}}, {std::move(tabs)});
  return MakeNode("XuiCanvas", {}, {std::move(main)});
}

const Timeline& TrackOf(const Node& main, std::string_view id) {
  const Node* tabs = main.FindById("Tabscene");
  REQUIRE(tabs);
  for (const Timeline& t : tabs->timelines) {
    if (t.target_id == id) {
      return t;
    }
  }
  FAIL("no track " << id);
  return tabs->timelines.front();
}

std::pair<int, bool> At(const Node& main, std::string_view id, int32_t frame) {
  for (const Keyframe& k : TrackOf(main, id).keyframes) {
    if (k.frame == frame) {
      return {int(k.values[0].get<Vec3>()->x - float(frame)) / 1000, *k.values[1].get<bool>()};
    }
  }
  FAIL("no keyframe " << frame << " on " << id);
  return {};
}

}

TEST_CASE("GuideMain's blades are rewritten for three tabs", "[xui][guide]") {
  Node main = MakeGuideMain(true);
  REQUIRE(rex::ui::guide::UseThreeTabs(main));
  constexpr int kMedia = 3, kHome = 4, kMediaSel = 9;

  for (int32_t frame : {1, 12, 13, 24, 73, 180}) {
    CHECK(At(main, "txt_Settings", frame).first == kMedia);
    CHECK_FALSE(At(main, "Blade5", frame).second);
    CHECK(At(main, "Blade6", frame).second);
  }

  for (int32_t frame : {49, 60, 61, 72, 232, 283}) {
    CHECK(At(main, "txt_Games", frame).first == kHome);
    CHECK(At(main, "txt_home", frame).first == kMedia);
    CHECK_FALSE(At(main, "Blade6", frame).second);
    CHECK(At(main, "Blade5", frame).second);
  }
  CHECK(At(main, "Tab2", 49).first == kMedia + 4);
  CHECK(At(main, "txt_homeSel", 60).first == kMediaSel);
  CHECK(At(main, "Tab2", 232).first == 6);

  CHECK(At(main, "txt_Settings", 25).first == 2);
  CHECK(At(main, "txt_home", 1).first == kHome);

  std::vector<std::string> names;
  for (const NamedFrame& f : main.FindById("Tabscene")->named_frames) {
    names.push_back(f.name);
  }
  CHECK(names ==
        std::vector<std::string>{"1To2", "2To1", "2To4", "2To4End", "4To2", "4To2End", "4Close"});
}

TEST_CASE("A GuideMain of another layout is left as it is", "[xui][guide]") {
  Node main = MakeGuideMain(false);
  CHECK_FALSE(rex::ui::guide::UseThreeTabs(main));
  CHECK(At(main, "txt_Settings", 1).first == 2);
  CHECK(main.FindById("Tabscene")->named_frames.size() == 10);
}

TEST_CASE("The console's guide scene switches between three tabs", "[xui][local]") {
  const char* path = std::getenv("REXGLUE_SYSTEM_UPDATE");
  if (!path || !*path) {
    SKIP("REXGLUE_SYSTEM_UPDATE is not set");
  }
  std::string error;
  auto update = SystemUpdate::Load(path, &error);
  REQUIRE(update);
  auto skin = ParseXur(update->Find("huduiskin/skin")->Find("skin.xur"), &error);
  REQUIRE(skin);
  auto guide = ParseXur(update->Find("hud/hud")->Find("GuideMain.xur"), &error);
  REQUIRE(guide);
  REQUIRE(rex::ui::guide::UseThreeTabs(guide->root));
  SceneContext context;
  context.skin = &*skin;
  context.package = "hud/hud";
  auto root = Element::Create(guide->root, context);
  Element* tabs = root->FindById("Tabscene");
  REQUIRE(tabs);
  auto play = [&](std::string_view name) {
    REQUIRE(tabs->Play(name, false));
    tabs->Advance(60.0);
    CHECK_FALSE(tabs->playing());
  };
  auto x = [&](std::string_view id) { return root->FindById(id)->position().x; };
  auto shown = [&](std::string_view id) { return root->FindById(id)->visible(); };

  play("2Close");
  CHECK_FALSE(shown("Blade5"));
  CHECK(x("txt_Settings") == Approx(638.0f));
  CHECK(x("txt_Games") == Approx(242.0f));
  CHECK(shown("Tab2"));

  play("2To4");
  CHECK_FALSE(shown("Blade6"));
  CHECK(shown("Blade7"));
  CHECK(shown("Blade8"));
  CHECK(x("txt_Games") == Approx(212.0f));
  CHECK(x("txt_home") == Approx(242.0f));
  CHECK(root->FindById("Tab4")->GetFloat("Opacity") == Approx(1.0f));
  CHECK(root->FindById("Tab2")->GetFloat("Opacity") == Approx(0.0f));

  play("4To2");
  CHECK(x("txt_Settings") == Approx(638.0f));
  CHECK(x("txt_Games") == Approx(242.0f));
  CHECK(root->FindById("Tab2")->GetFloat("Opacity") == Approx(1.0f));

  play("2To1");
  CHECK_FALSE(shown("Blade5"));
  CHECK(x("txt_Settings") == Approx(668.0f));
  CHECK(x("txt_home") == Approx(638.0f));
  CHECK_FALSE(tabs->Play("2To3"));
}

TEST_CASE("The backward-compatibility guide has its own three tabs", "[xui][guide][local]") {
  const char* path = std::getenv("REXGLUE_GUIDE_FLASH");
  if (!path || !*path) {
    SKIP("REXGLUE_GUIDE_FLASH is not set");
  }
  std::string error;
  auto modules = SystemUpdate::ReadModules(std::filesystem::path(path), &error);
  REQUIRE(modules);
  auto assets =
      rex::ui::guide::GuideAssets::FromUpdate(SystemUpdate::FromModules(*modules, &error), &error,
                                              rex::ui::guide::GuidePresentation::OriginalXbox);
  REQUIRE(assets);
  CHECK(assets->emulator_layout);
  CHECK(assets->has_xbox_settings);
  CHECK(assets->has_options);

  SceneContext context;
  context.skin = &assets->skin;
  context.package = "hud/hud";
  auto root = Element::Create(assets->main.root, context);
  CHECK(root->FindById("btnY")->text() == "Leave Game");
  auto home = Element::Create(assets->home_tab.root, context);
  CHECK(home->FindById("btnDashboard"));
  CHECK(home->FindById("btnManageStorage"));
  CHECK_FALSE(home->FindById("btnConnectToLive"));
  Element* tabs = root->FindById("Tabscene");
  REQUIRE(tabs);
  auto play = [&](std::string_view name) {
    REQUIRE(tabs->Play(name, false));
    tabs->Advance(60.0);
    CHECK_FALSE(tabs->playing());
  };
  auto opacity = [&](std::string_view id) { return root->FindById(id)->GetFloat("Opacity"); };
  play("2Close");
  CHECK(opacity("Tab2") == Approx(1.0f));
  play("2To3");
  CHECK(opacity("Tab3") == Approx(1.0f));
  CHECK(opacity("Tab2") == Approx(0.0f));
  play("3To2");
  play("2To1");
  CHECK(opacity("Tab1") == Approx(1.0f));
  CHECK_FALSE(root->FindById("Tab4"));
}

TEST_CASE("Title presentation never changes with available Guide scenes",
          "[xui][guide][guide_presentation][local]") {
  using rex::ui::guide::GuideAssets;
  using rex::ui::guide::GuidePresentation;
  const char* console = std::getenv("REXGLUE_SYSTEM_UPDATE");
  const char* flash = std::getenv("REXGLUE_GUIDE_FLASH");
  if (!console || !*console || !flash || !*flash) {
    SKIP("Both console system update and BC Flash assets are required");
  }
  std::string error;
  auto original = GuideAssets::Load(console, &error);
  INFO(error);
  REQUIRE(original);
  CHECK_FALSE(original->emulator_layout);
  auto explicit_console = GuideAssets::Load(console, &error, GuidePresentation::Xbox360);
  REQUIRE(explicit_console);
  CHECK_FALSE(explicit_console->emulator_layout);
  CHECK_FALSE(GuideAssets::Load(console, &error, GuidePresentation::OriginalXbox));
  CHECK(error.find("Original Xbox Guide requires") != std::string::npos);
  auto bc = GuideAssets::Load(flash, &error, GuidePresentation::OriginalXbox);
  REQUIRE(bc);
  CHECK(bc->emulator_layout);

  auto default_bc = GuideAssets::Load(flash, &error);
  if (default_bc) {
    CHECK_FALSE(default_bc->emulator_layout);
  } else {
    CHECK_FALSE(error.empty());
  }
}

TEST_CASE("Text taller than its box scrolls, holds and fades back to the top", "[xui]") {
  using rex::ui::xui::TextScrollAt;

  CHECK(TextScrollAt(1.0, 30, 10).offset == 0);
  CHECK(TextScrollAt(1.0, 30, 10).alpha == 1);
  CHECK(TextScrollAt(4.0, 30, 10).offset == Catch::Approx(15));
  CHECK(TextScrollAt(6.5, 30, 10).offset == 30);
  CHECK(TextScrollAt(6.5, 30, 10).alpha == 1);
  CHECK(TextScrollAt(7.8, 30, 10).alpha == Catch::Approx(0.5).margin(0.01));
  const double cycle = 2.5 + 3 + 2 + 0.6;
  CHECK(TextScrollAt(cycle + 0.3, 30, 10).offset == 0);
  CHECK(TextScrollAt(cycle + 0.3, 30, 10).alpha == Catch::Approx(0.5).margin(0.01));

  CHECK(TextScrollAt(5.0, 0, 10).offset == 0);
}
