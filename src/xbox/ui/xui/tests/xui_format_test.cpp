/**
 * @file        ui/xui/tests/xui_format_test.cpp
 * @brief       XUIZ, XUIS, XUR v8 and system-XEX resource reading (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <catch2/catch_test_macros.hpp>

#include <bit>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include <rex/ui/xui/document.h>
#include <rex/ui/xui/package.h>
#include <rex/ui/xui/system_update.h>

#include "xui_test_data.h"

using namespace rex::ui::xui;
using namespace xui_test;

namespace {

struct XurWriter {
  std::vector<std::string> strings;
  std::vector<float> floats;
  std::vector<uint32_t> colors;
  Bytes keyp, keyd, name, data;
  uint32_t objects = 0;

  uint32_t Str(const std::string& s) {
    for (size_t i = 0; i < strings.size(); ++i) {
      if (strings[i] == s) {
        return uint32_t(i + 1);
      }
    }
    strings.push_back(s);
    return uint32_t(strings.size());
  }
  uint32_t Flt(float f) {
    floats.push_back(f);
    return uint32_t(floats.size() - 1);
  }
  uint32_t Col(uint32_t c) {
    colors.push_back(c);
    return uint32_t(colors.size() - 1);
  }
  void Object(const std::string& cls, uint8_t flags) {
    ++objects;
    Packed(data, Str(cls));
    data.push_back(flags);
  }

  Bytes Build() const {
    struct Section {
      const char* magic;
      Bytes bytes;
    };
    std::vector<Section> sections;
    Bytes strn;
    Be32(strn, 0);
    Be16(strn, uint16_t(strings.size()));
    for (const std::string& s : strings) {
      strn.insert(strn.end(), s.begin(), s.end());
      strn.push_back(0);
    }
    sections.push_back({"STRN", strn});
    Bytes flot, colr;
    for (float f : floats) {
      Be32(flot, std::bit_cast<uint32_t>(f));
    }
    for (uint32_t c : colors) {
      Be32(colr, c);
    }
    sections.push_back({"FLOT", flot});
    sections.push_back({"COLR", colr});
    if (!keyp.empty()) {
      sections.push_back({"KEYP", keyp});
      sections.push_back({"KEYD", keyd});
    }
    if (!name.empty()) {
      sections.push_back({"NAME", name});
    }
    sections.push_back({"DATA", data});

    Bytes counts;
    Packed(counts, objects);
    for (int i = 1; i < 12; ++i) {
      Packed(counts, 0);
    }
    const size_t header_size = 20 + counts.size() + sections.size() * 12;
    Bytes out;
    Be32(out, 0x58554942);
    Be32(out, 8);
    Be32(out, 0);
    Be16(out, 0x0E);
    size_t total = header_size;
    for (const Section& s : sections) {
      total += s.bytes.size();
    }
    Be32(out, uint32_t(total));
    Be16(out, uint16_t(sections.size()));
    Append(out, counts);
    size_t offset = header_size;
    for (const Section& s : sections) {
      out.insert(out.end(), s.magic, s.magic + 4);
      Be32(out, uint32_t(offset));
      Be32(out, uint32_t(s.bytes.size()));
      offset += s.bytes.size();
    }
    for (const Section& s : sections) {
      Append(out, s.bytes);
    }
    return out;
  }
};

Bytes SampleScene() {
  XurWriter w;

  w.Object("XuiCanvas", 0x1 | 0x2 | 0x4);
  Packed(w.data, 2);
  Packed(w.data, 0b110);
  Packed(w.data, w.Flt(852.0f));
  Packed(w.data, w.Flt(480.0f));
  Packed(w.data, 0);
  Packed(w.data, 3);

  w.Object("XuiText", 0x1);
  Packed(w.data, 4);
  Packed(w.data, 0b1);
  Packed(w.data, w.Str("label"));
  Packed(w.data, 0b1011);
  Packed(w.data, w.Str("Xbox Guide"));
  Packed(w.data, w.Col(0xFFEBEBEB));
  Packed(w.data, w.Flt(12.0f));

  w.Object("XuiFigure", 0x1);
  Packed(w.data, 2);
  Packed(w.data, 0b1);
  Packed(w.data, w.Str("fig"));
  Packed(w.data, 0b10);
  Packed(w.data, 0);
  Packed(w.data, 3);
  Packed(w.data, 0b1011);
  Packed(w.data, 2);
  Packed(w.data, w.Col(0xFF008A00));
  Packed(w.data, 0);
  Packed(w.data, 5);
  Packed(w.data, 0b1110);
  Packed(w.data, 2);
  w.data.push_back(2);
  Packed(w.data, w.Col(0xFF51860F));
  Packed(w.data, w.Col(0xFF77BA0F));
  w.data.push_back(2);
  Packed(w.data, w.Flt(0.0f));
  Packed(w.data, w.Flt(1.0f));

  w.Object("XuiText", 0x8);
  Packed(w.data, 1);

  Packed(w.name, w.Str("Show"));
  Packed(w.name, 0);
  w.name.push_back(1);
  Packed(w.name, w.Str("Hide"));
  Packed(w.name, 10);
  w.name.push_back(4);
  Packed(w.name, w.Str("Show"));
  Packed(w.data, 2);
  Packed(w.data, 0);
  Packed(w.data, 2);

  Packed(w.data, w.Str("label"));
  Packed(w.data, 1);
  w.data.push_back(1);
  w.data.push_back(1);
  w.data.push_back(6);
  Packed(w.data, 2);
  Packed(w.data, 0);

  Packed(w.data, w.Str("fig"));
  Packed(w.data, 1);
  w.data.push_back(2);
  w.data.push_back(0);
  w.data.push_back(1);
  w.data.push_back(1);
  Packed(w.data, 1);
  Packed(w.data, 2);

  Packed(w.keyp, w.Flt(0.0f));
  Packed(w.keyp, w.Flt(1.0f));
  Packed(w.keyp, w.Col(0xFF1CB61C));
  Packed(w.keyd, 0);
  w.keyd.push_back(0);
  Packed(w.keyd, 0);
  Packed(w.keyd, 10);
  w.keyd.push_back(2);
  w.keyd.push_back(uint8_t(-100));
  w.keyd.push_back(100);
  w.keyd.push_back(0);
  Packed(w.keyd, 1);
  Packed(w.keyd, 5);
  w.keyd.push_back(1);
  Packed(w.keyd, 2);
  return w.Build();
}

float FloatOf(const Value* v) {
  REQUIRE(v);
  REQUIRE(v->get<float>());
  return *v->get<float>();
}

}

TEST_CASE("XUIZ packages find files by path, ignoring case and separator", "[xui]") {
  std::string error;
  auto package =
      Package::Parse(Xuiz({{"Strings.xus", "abc"}, {"de-de\\Strings.xus", "defg"}}), &error);
  REQUIRE(package);
  CHECK(package->entries().size() == 2);
  auto a = package->Find("strings.XUS");
  CHECK(std::string(a.begin(), a.end()) == "abc");
  auto b = package->Find("de-de/strings.xus");
  CHECK(std::string(b.begin(), b.end()) == "defg");
  CHECK(package->Find("missing.png").empty());
}

TEST_CASE("XUIZ packages reject truncated tables and data", "[xui]") {
  Bytes good = Xuiz({{"a.png", "12345"}});
  std::string error;
  CHECK_FALSE(Package::Parse(Bytes(good.begin(), good.end() - 2), &error));
  CHECK(error.find("past the end") != std::string::npos);
  CHECK_FALSE(Package::Parse(Bytes{'X', 'U', 'I', 'S'}, &error));
}

TEST_CASE("XUIS string tables read UTF-8 strings in order", "[xui]") {
  std::string error;
  auto strings = ParseStringTable(Xuis({"%d hours",
                                        "M\xC3\xB6"
                                        "chten",
                                        ""}),
                                  &error);
  REQUIRE(strings);
  REQUIRE(strings->size() == 3);
  CHECK((*strings)[0] == "%d hours");
  CHECK((*strings)[1] ==
        "M\xC3\xB6"
        "chten");
  Bytes cut = Xuis({"unterminated"});
  cut.pop_back();
  CHECK_FALSE(ParseStringTable(cut, &error));
}

TEST_CASE("XUR v8 scenes decode elements, compounds and shared properties", "[xui]") {
  std::string error;
  auto doc = ParseXur(SampleScene(), &error);
  INFO(error);
  REQUIRE(doc);
  const Node& root = doc->root;
  CHECK(root.class_name == "XuiCanvas");
  CHECK(FloatOf(root.Find("Width")) == 852.0f);
  REQUIRE(root.children.size() == 3);

  const Node& label = root.children[0];
  CHECK(label.id() == "label");
  CHECK(*label.Find("Text")->get<std::string>() == "Xbox Guide");
  CHECK(label.Find("TextColor")->get<Color>()->argb == 0xFFEBEBEB);
  CHECK(root.FindById("fig") == &root.children[1]);

  const auto* fill = root.children[1].Find("Fill")->get<std::shared_ptr<const PropertyBag>>();
  REQUIRE(fill);
  CHECK(*(*fill)->Find("FillType")->get<uint32_t>() == 2);
  const auto* gradient = (*fill)->Find("Gradient")->get<std::shared_ptr<const PropertyBag>>();
  REQUIRE(gradient);
  const auto* stops =
      (*gradient)->Find("StopColor")->get<std::shared_ptr<const std::vector<Value>>>();
  REQUIRE(stops);
  REQUIRE((*stops)->size() == 2);
  CHECK((*stops)->at(1).get<Color>()->argb == 0xFF77BA0F);

  CHECK(root.children[2].props == label.props);
}

TEST_CASE("XUR v8 timelines resolve property paths and keyframe values", "[xui]") {
  std::string error;
  auto doc = ParseXur(SampleScene(), &error);
  REQUIRE(doc);
  const Node& root = doc->root;
  REQUIRE(root.named_frames.size() == 2);
  CHECK(root.named_frames[1].name == "Hide");
  CHECK(root.named_frames[1].command == FrameCommand::kGoToAndStop);
  CHECK(root.named_frames[1].target == "Show");

  REQUIRE(root.timelines.size() == 2);
  const Timeline& opacity = root.timelines[0];
  CHECK(opacity.target_id == "label");
  REQUIRE(opacity.props.size() == 1);
  REQUIRE(opacity.props[0].path.size() == 1);
  CHECK(opacity.props[0].path[0]->name == "Opacity");
  REQUIRE(opacity.keyframes.size() == 2);
  CHECK(FloatOf(&opacity.keyframes[1].values[0]) == 1.0f);
  CHECK(opacity.keyframes[1].frame == 10);
  CHECK(opacity.keyframes[1].interpolation == Interpolation::kEase);
  CHECK(opacity.keyframes[1].ease_in == -100);
  CHECK(opacity.keyframes[1].ease_out == 100);

  const Timeline& color = root.timelines[1];
  REQUIRE(color.props[0].path.size() == 2);
  CHECK(color.props[0].path[0]->name == "Fill");
  CHECK(color.props[0].path[1]->name == "FillColor");
  REQUIRE(color.keyframes.size() == 1);
  CHECK(color.keyframes[0].interpolation == Interpolation::kNone);
  CHECK(color.keyframes[0].values[0].get<Color>()->argb == 0xFF1CB61C);
}

TEST_CASE("XUR v8 reading fails on truncation and on classes the schema lacks", "[xui]") {
  Bytes scene = SampleScene();
  std::string error;
  CHECK_FALSE(ParseXur(Bytes(scene.begin(), scene.begin() + 30), &error));
  CHECK_FALSE(error.empty());

  XurWriter w;
  w.Object("XuiMysteryWidget", 0);
  error.clear();
  CHECK_FALSE(ParseXur(w.Build(), &error));
  CHECK(error.find("XuiMysteryWidget") != std::string::npos);
}

TEST_CASE("XUR v8 skips properties past a class's known list", "[xui]") {
  XurWriter w;
  w.Object("XuiGroup", 0x1);
  Packed(w.data, 2);
  Packed(w.data, 0b1);
  Packed(w.data, w.Str("btn"));
  Packed(w.data, 0b1);
  Packed(w.data, 0x123);
  std::string error;
  auto doc = ParseXur(w.Build(), &error);
  INFO(error);
  REQUIRE(doc);
  CHECK(doc->root.id() == "btn");
  REQUIRE(doc->root.props->entries.size() == 2);
  CHECK(doc->root.props->entries[1].def == nullptr);
}

TEST_CASE("XUR v8 reading checks the declared object count", "[xui]") {
  XurWriter w;
  w.Object("XuiGroup", 0);
  w.objects = 2;
  std::string error;
  CHECK_FALSE(ParseXur(w.Build(), &error));
  CHECK(error.find("declares 2 objects") != std::string::npos);
}

TEST_CASE("System XEX resources are extracted from plain and basic-compressed images", "[xui]") {
  const Bytes package = Xuiz({{"GuideMain.xur", "scene"}});
  for (uint16_t compression : {uint16_t(0), uint16_t(1)}) {
    CAPTURE(compression);
    std::string error;
    auto resources = ReadXexResources(XexWithResource("hud", package, compression), &error);
    INFO(error);
    REQUIRE(resources);
    REQUIRE(resources->size() == 1);
    CHECK((*resources)[0].name == "hud");
    CHECK((*resources)[0].bytes == package);
  }
}

TEST_CASE("SystemUpdate keeps a module's XUIZ resources by module/resource", "[xui]") {
  SystemUpdate update;
  std::string error;
  REQUIRE(update.AddModule("hud", XexWithResource("hud", Xuiz({{"a.xur", "x"}}), 0), &error));
  REQUIRE(update.Find("HUD/hud"));
  CHECK(update.Find("hud/hud")->Contains("A.XUR"));

  REQUIRE(update.AddModule("xam", XexWithResource("icon", Bytes{1, 2, 3, 4}, 0), &error));
  CHECK_FALSE(update.Find("xam/icon"));
}

TEST_CASE("The guide bundle carries the system modules a title embeds", "[xui]") {
  SystemUpdate::Modules modules;
  modules["hud"] = XexWithResource("hud", Xuiz({{"a.xur", "x"}}), 0);
  modules["huduiskin"] = XexWithResource("skin", Xuiz({{"skin.xur", "s"}}), 0);
  modules["xam"] = XexWithResource("skin", Xuiz({{"b.xur", "y"}}), 0);
  const auto bundle = SystemUpdate::WriteBundle(modules);
  std::string error;
  auto read = SystemUpdate::ReadBundle(bundle, &error);
  REQUIRE(read);
  CHECK(*read == modules);

  CHECK_FALSE(SystemUpdate::FromModules(*read, &error));
  CHECK(error.find("xam/shrdres") != std::string::npos);
  modules["xam"] = XexWithResource("shrdres", Xuiz({{"c.png", "z"}}), 0);

  CHECK_FALSE(SystemUpdate::FromModules(modules, &error));

  for (size_t cut : {size_t(0), size_t(5), bundle.size() / 2, bundle.size() - 1}) {
    INFO("cut " << cut);
    CHECK_FALSE(SystemUpdate::ReadBundle(std::span(bundle).first(cut), &error));
  }
  Bytes foreign = bundle;
  foreign[0] ^= 0xFF;
  CHECK_FALSE(SystemUpdate::ReadBundle(foreign, &error));
}

TEST_CASE("Encrypted XEXs are refused", "[xui]") {
  std::string error;
  CHECK_FALSE(ReadXexResources(XexWithResource("hud", Bytes{}, 0, 1), &error));
  CHECK(error.find("encrypted") != std::string::npos);
}

TEST_CASE("The console's own guide scenes decode", "[xui][local]") {
  const char* path = std::getenv("REXGLUE_SYSTEM_UPDATE");
  if (!path || !*path) {
    SKIP("REXGLUE_SYSTEM_UPDATE is not set");
  }
  std::string error;
  auto update = SystemUpdate::Load(path, &error);
  INFO(error);
  REQUIRE(update);
  const std::pair<const char*, const char*> scenes[] = {
      {"hud/hud", "GuideMain.xur"},
      {"hud/hud", "HomeTabSignedIn.xur"},
      {"hud/hud", "GamesTabSignedIn.xur"},
      {"hud/hud", "SettingsTabSignedIn.xur"},
      {"hud/hud", "SettingsTabSignedOut.xur"},
      {"hud/hud", "InfoMessage.xur"},
      {"huduiskin/skin", "skin.xur"},
      {"xam/xam", "hudbkgnd.xur"},
      {"xam/xam", "notify.xur"},
      {"gamerprofile/gp", "802_Achievements.xur"},
      {"gamerprofile/gp", "828_AchievDetails.xur"},
  };
  for (const auto& [package_name, scene] : scenes) {
    CAPTURE(package_name, scene);
    const Package* package = update->Find(package_name);
    REQUIRE(package);
    REQUIRE(package->Contains(scene));
  }

  int decoded = 0;
  for (const char* package_name : {"hud/hud", "huduiskin/skin", "xam/xam", "gamerprofile/gp"}) {
    for (const Package::Entry& entry : update->Find(package_name)->entries()) {
      if (!entry.name.ends_with(".xur") || entry.name.starts_with("Nui") ||
          entry.name == "nuihud.xur") {
        continue;
      }
      CAPTURE(package_name, entry.name);
      std::string scene_error;
      auto doc = ParseXur(update->Find(package_name)->Find(entry.name), &scene_error);
      INFO(scene_error);
      CHECK(doc);
      decoded += doc ? 1 : 0;
    }
  }
  CHECK(decoded >= 55);
}

TEST_CASE("The console's system update round-trips through a guide bundle", "[xui][local]") {
  const char* path = std::getenv("REXGLUE_SYSTEM_UPDATE");
  if (!path) {
    SKIP("REXGLUE_SYSTEM_UPDATE is not set");
  }
  std::string error;
  auto modules = SystemUpdate::ReadModules(path, &error);
  REQUIRE(modules);
  for (std::string_view module : SystemUpdate::kModules) {
    CHECK(modules->contains(std::string(module)));
  }
  const auto bundle = SystemUpdate::WriteBundle(*modules);
  auto read = SystemUpdate::ReadBundle(bundle, &error);
  REQUIRE(read);
  CHECK(read->size() == modules->size());
  auto update = SystemUpdate::FromModules(*read, &error);
  INFO(error);
  REQUIRE(update);
  CHECK(update->Find("hud/hud")->Contains("GuideMain.xur"));

  CHECK_FALSE(update->Font("SegoeXbox-Light").empty());
}
