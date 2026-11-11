/**
 * @file        ui/guide/src/xbox_guide.cpp
 * @brief       The Xbox 360 guide, run from the console's own scenes (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/guide/xbox_guide.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iterator>

#include <fmt/format.h>

#include <rex/audio/ui_sound.h>
#include <rex/data_locations.h>
#include <rex/filesystem.h>
#include <rex/input/input.h>
#include <rex/input/input_system.h>
#include <rex/kernel/xam/module.h>
#include <rex/logging.h>
#include <rex/system/achievement_manager.h>
#include <rex/system/kernel_state.h>
#include <rex/system/xam/user_profile.h>
#include <rex/ui/image_decode.h>
#include <rex/ui/immediate_drawer.h>

#include <rex/ui/guide/app_update.h>
#include <rex/ui/guide/dlc_catalog.h>
#include <rex/ui/guide/guide_layout.h>

REXCVAR_DEFINE_BOOL(xbox_guide, true, "UI",
                    "View+Menu (Back+Start) or Home opens the Xbox guide (needs the "
                    "console's system update, see xbox_guide_system_update)");
REXCVAR_DEFINE_STRING(xbox_guide_system_update, "", "UI",
                      "The console's $SystemUpdate folder (dashboard 2.0.17559) or its "
                      "su*_00000000 package, for the Xbox guide's scenes and sounds");

REXCVAR_DEFINE_BOOL(notifications_show, true, "UI",
                    "Show notifications (achievement unlocks) over the title");
REXCVAR_DEFINE_BOOL(notifications_sound, true, "UI", "Play the notification sound");
REXCVAR_DEFINE_STRING(code_patch_states, "", "UI",
                      "Switchable code patches the player turned on or off in the Xbox guide "
                      "(Name=1;Name=0)");
REXCVAR_DEFINE_BOOL(resolution_match_display, false, "GPU",
                    "Experimental: render at the display's resolution, setting resolution_scale "
                    "from the monitor at startup (3 on a 4K display: nine times the pixels, "
                    "much slower). Off by default: titles draw at their own resolution, as on "
                    "the console");

namespace rex::ui::guide {
namespace {

constexpr std::string_view kRemovedEntries[] = {
    "btnFamilySettings", "btnAccountManagement", "btnKinectTuner",
    "btnShutdown",       "btnConnectToLive",     "btnDiscInTray",
};

constexpr std::string_view kLeaveGame = "Leave Game";
constexpr std::string_view kLeaveGameWarning =
    "Are you sure you want to close the game? Any unsaved progress will be lost.";

constexpr std::string_view kXboxSettings = "Xbox Settings";

constexpr std::string_view kHomeTabLabels[] = {"txt_home", "txt_homeSel"};

constexpr int kEmulatorTabs[] = {1, 2, 3};
constexpr int kTabs[] = {1, 2, 4};
constexpr int kRemovedTab = 3;

struct ButtonGlyph {
  std::string_view path;
  const char* letter;
  uint32_t rgb;
};
constexpr ButtonGlyph kButtonGlyphs[] = {
    {"sharedres://A-Button.png", "A", 0x5BA929},
    {"sharedres://B-Button.png", "B", 0xB73333},
    {"sharedres://X-Button.png", "X", 0x2369A0},
    {"sharedres://Y-Button.png", "Y", 0xE39402},
};

constexpr float kDimOpacity = 0.75f;
constexpr double kDimInSeconds = 23 / xui::kFramesPerSecond;
constexpr double kDimOutSeconds = 16 / xui::kFramesPerSecond;

constexpr float kSceneWidth = 852.0f;
constexpr float kSceneHeight = 480.0f;
constexpr uint32_t kXnSysUi = 0x00000009;
constexpr std::string_view kAchievementScheme = "achievement://";

constexpr uint32_t kAchievementShowUnachieved = 0x8;

constexpr std::string_view kDefaultGamerPicture = "sharedres://64_fffe07d10002000000010000.png";

const xui::Node* FindVisual(const xui::Document& skin, std::string_view id) {
  for (const xui::Node& visual : skin.root.children) {
    if (visual.id() == id) {
      return &visual;
    }
  }
  return nullptr;
}

std::string FormatCount(std::string format, uint32_t first, uint32_t second) {
  auto replace = [&](std::string_view token, uint32_t value) {
    if (size_t at = format.find(token); at != std::string::npos) {
      format.replace(at, token.size(), std::to_string(value));
    }
  };
  replace("%1!u!", first);
  replace("%2!u!", second);
  return format;
}

void ForEach(xui::Element* root, const std::function<void(xui::Element*)>& fn) {
  fn(root);
  for (const auto& child : root->children()) {
    ForEach(child.get(), fn);
  }
}

constexpr uint32_t kRingOfLightGreen = 0x00ACF124;

void TintGradient(xui::Element* figure, uint32_t rgb) {
  const xui::PropertyBag* fill = figure->GetCompound("Fill");
  if (!fill) {
    return;
  }
  auto tinted = std::make_shared<xui::PropertyBag>(*fill);
  for (xui::PropertyBag::Entry& entry : tinted->entries) {
    const auto* gradient = entry.value.get<std::shared_ptr<const xui::PropertyBag>>();
    if (!entry.def || entry.def->name != "Gradient" || !gradient || !*gradient) {
      continue;
    }
    auto stops = std::make_shared<xui::PropertyBag>(**gradient);
    for (xui::PropertyBag::Entry& stop : stops->entries) {
      const auto* colors = stop.value.get<std::shared_ptr<const std::vector<xui::Value>>>();
      if (!stop.def || stop.def->name != "StopColor" || !colors || !*colors) {
        continue;
      }
      auto recoloured = std::make_shared<std::vector<xui::Value>>(**colors);
      for (xui::Value& value : *recoloured) {
        if (const xui::Color* c = value.get<xui::Color>()) {
          value = xui::Value{xui::Color{(c->argb & 0xFF000000u) | rgb}};
        }
      }
      stop.value = xui::Value{std::shared_ptr<const std::vector<xui::Value>>(recoloured)};
    }
    entry.value = xui::Value{std::shared_ptr<const xui::PropertyBag>(stops)};
  }
  figure->Set("Fill", xui::Value{std::shared_ptr<const xui::PropertyBag>(tinted)});
}

bool DrawControllerBattery(ImDrawList& list, std::string_view path,
                           const std::function<ImVec2(ImVec2)>& to_screen, float opacity) {
  int bars = 0;
  if (path == "Controller_OneFourth.xur") {
    bars = 1;
  } else if (path == "Controller_Half.xur") {
    bars = 2;
  } else if (path == "Controller_ThreeFourths.xur") {
    bars = 3;
  } else if (path == "Controller_Full.xur") {
    bars = 4;
  } else {
    return false;
  }
  auto color = [&](float alpha) {
    return IM_COL32(0xEB, 0xEB, 0xEB, int(alpha * std::clamp(opacity, 0.0f, 1.0f) * 255 + 0.5f));
  };
  auto rect = [&](float x0, float y0, float x1, float y1, float alpha) {
    const ImVec2 points[] = {to_screen({x0, y0}), to_screen({x1, y0}), to_screen({x1, y1}),
                             to_screen({x0, y1})};
    list.AddConvexPolyFilled(points, 4, color(alpha));
  };

  constexpr ImVec2 kPad[] = {
      {2.0f, 1.0f},  {2.6f, 0.2f},   {6.2f, 0.2f},   {6.7f, 1.0f},  {10.3f, 1.0f},
      {10.8f, 0.2f}, {13.6f, 0.2f},  {14.2f, 1.0f},  {14.6f, 3.0f}, {14.7f, 7.5f},
      {14.6f, 9.6f}, {14.2f, 10.7f}, {13.3f, 10.7f}, {12.5f, 9.4f}, {11.4f, 8.6f},
      {9.0f, 8.4f},  {6.6f, 8.6f},   {5.3f, 9.2f},   {4.4f, 10.7f}, {3.6f, 11.9f},
      {1.5f, 11.9f}, {0.5f, 11.0f},  {0.4f, 8.5f},   {0.7f, 5.0f},  {1.3f, 2.2f},
  };
  ImVec2 pad[std::size(kPad)];
  for (size_t i = 0; i < std::size(kPad); ++i) {
    pad[i] = to_screen(kPad[i]);
  }

  const ImDrawListFlags flags = list.Flags;
  list.Flags &= ~ImDrawListFlags_AntiAliasedFill;
  list.AddConcavePolyFilled(pad, int(std::size(pad)), color(0.6f));
  list.Flags = flags;

  const ImVec2 centre = to_screen({7.5f, 3.6f});
  const ImVec2 edge = to_screen({8.6f, 3.6f});
  list.AddCircleFilled(centre, std::hypot(edge.x - centre.x, edge.y - centre.y),
                       IM_COL32(0, 0, 0, int(0.45f * std::clamp(opacity, 0.0f, 1.0f) * 255)), 16);

  rect(16.9f, 1.9f, 34.1f, 10.0f, 0.12f);
  rect(16.0f, 1.0f, 35.0f, 1.9f, 0.6f);
  rect(16.0f, 10.0f, 35.0f, 10.9f, 0.6f);
  rect(16.0f, 1.9f, 16.9f, 10.0f, 0.6f);
  rect(34.1f, 1.9f, 35.0f, 10.0f, 0.6f);
  rect(35.0f, 4.0f, 37.2f, 7.9f, 0.6f);
  for (int i = 0; i < bars; ++i) {
    rect(18.7f + 4.0f * float(i), 3.0f, 21.1f + 4.0f * float(i), 8.9f, 1.0f);
  }
  return true;
}

bool DrawButtonGlyph(ImDrawList& list, std::string_view path,
                     const std::function<ImVec2(ImVec2)>& to_screen, float opacity, ImFont* font) {
  const auto glyph = std::find_if(std::begin(kButtonGlyphs), std::end(kButtonGlyphs),
                                  [&](const ButtonGlyph& g) { return g.path == path; });
  if (glyph == std::end(kButtonGlyphs)) {
    return false;
  }
  const float alpha = std::clamp(opacity, 0.0f, 1.0f);
  const ImVec2 centre = to_screen({9.0f, 9.0f});
  const ImVec2 right = to_screen({10.0f, 9.0f});
  const float scale = std::hypot(right.x - centre.x, right.y - centre.y);
  constexpr float kRadius = 8.0f;
  constexpr int kSegments = 96;
  ImVec2 disc[kSegments];
  for (int i = 0; i < kSegments; ++i) {
    const float a = 2.0f * 3.14159265f * float(i) / float(kSegments);
    disc[i] = to_screen({9.0f + kRadius * std::cos(a), 9.0f + kRadius * std::sin(a)});
  }
  const uint32_t rgb = glyph->rgb;
  list.AddConvexPolyFilled(
      disc, kSegments,
      IM_COL32((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF, int(alpha * 255.0f + 0.5f)));
  if (!font || scale <= 0.0f) {
    return true;
  }

  const float size = 11.0f * scale;
  ImFontBaked* baked = font->GetFontBaked(size);
  const ImFontGlyph* g = baked ? baked->FindGlyph(ImWchar(glyph->letter[0])) : nullptr;
  if (!g) {
    return true;
  }

  const float k = size / baked->Size;
  const ImVec2 at(centre.x - (g->X0 + g->X1) / 2.0f * k, centre.y - (g->Y0 + g->Y1) / 2.0f * k);
  list.AddImage(font->OwnerAtlas->TexRef, ImVec2(at.x + g->X0 * k, at.y + g->Y0 * k),
                ImVec2(at.x + g->X1 * k, at.y + g->Y1 * k), ImVec2(g->U0, g->V0),
                ImVec2(g->U1, g->V1), IM_COL32(0xF5, 0xF5, 0xF5, int(alpha * 255.0f + 0.5f)));
  return true;
}

void CoverBladeEdge(xui::Element* column) {
  if (!column) {
    return;
  }
  const xui::Vec3 p = column->GetVector("Position");
  column->Set("Position", xui::Value{xui::Vec3{p.x - 1.0f, p.y - 1.0f, p.z}});
  column->Set("Width", xui::Value{column->width() + 1.0f});
  column->Set("Height", xui::Value{column->height() + 1.0f});
}

void HideButtonLetters(xui::Element* root) {
  ForEach(root, [](xui::Element* e) {
    const std::string_view path = e->GetString("ImagePath");
    if (!e->IsA("XuiImage") || !e->parent() ||
        std::none_of(std::begin(kButtonGlyphs), std::end(kButtonGlyphs),
                     [&](const ButtonGlyph& g) { return g.path == path; })) {
      return;
    }
    for (const auto& sibling : e->parent()->children()) {
      const std::string_view text = sibling->text();
      if (sibling->IsA("XuiText") && text.size() == 1 &&
          std::string_view("ABXY").find(text) != std::string_view::npos) {
        sibling->Suppress();
      }
    }
  });
}

int BatteryFrame(int percent) {
  if (percent < 15) {
    return 0;
  }
  if (percent < 45) {
    return 1;
  }
  return percent < 75 ? 2 : 3;
}

constexpr int kGamerscoreImageSize = 256;

bool InGamerscoreGlyph(float x, float y) {
  const float dx = x - 16.0f, dy = y - 16.0f;
  const float r = std::sqrt(dx * dx + dy * dy);
  if (r > 14.5f) {
    return false;
  }

  if ((y >= 15.3f && y <= 17.3f && x >= 16.0f && x <= 22.7f) ||
      (x >= 20.5f && x <= 22.7f && y >= 15.3f && y <= 24.5f)) {
    return false;
  }

  if (r >= 7.0f && r <= 9.3f) {
    const float angle = std::atan2(-dy, dx) * 180.0f / 3.14159265f;
    return angle > 0.0f && angle < 70.0f && dx > 0.0f;
  }
  return true;
}

std::vector<uint8_t> GamerscoreGlyphRGBA(int size) {
  constexpr int kSamples = 4;
  std::vector<uint8_t> rgba(size_t(size) * size * 4, 0xFF);
  for (int py = 0; py < size; ++py) {
    for (int px = 0; px < size; ++px) {
      int covered = 0;
      for (int sy = 0; sy < kSamples; ++sy) {
        for (int sx = 0; sx < kSamples; ++sx) {
          const float x = (px + (sx + 0.5f) / kSamples) * 32.0f / size;
          const float y = (py + (sy + 0.5f) / kSamples) * 32.0f / size;
          covered += InGamerscoreGlyph(x, y) ? 1 : 0;
        }
      }
      rgba[(size_t(py) * size + px) * 4 + 3] = uint8_t(covered * 255 / (kSamples * kSamples));
    }
  }
  return rgba;
}

void ShowControllerStatus(xui::Element* main) {
  xui::Element* ring = nullptr;
  for (const auto& child : main->children()) {
    if (child->id() == "ROL") {
      ring = child.get();
    }
  }
  if (!ring) {
    return;
  }

  for (const auto& light : ring->children()) {
    const std::string_view id = light->id();
    if (id == "light1") {
      for (const auto& figure : light->children()) {
        TintGradient(figure.get(), kRingOfLightGreen);
      }
    } else if (id == "light2" || id == "light3" || id == "light4") {
      light->SetVisible(false);
    }
  }
}

}

bool DrawGuideVectorImage(ImDrawList& list, std::string_view path,
                          const std::function<ImVec2(ImVec2)>& to_screen, float opacity,
                          ImFont* bold) {
  return DrawControllerBattery(list, path, to_screen, opacity) ||
         DrawButtonGlyph(list, path, to_screen, opacity, bold);
}

void HideGuideButtonLetters(xui::Element* root) {
  HideButtonLetters(root);
}

void ApplySavedCodePatches(const PPCSwitchablePatch* patches) {
  const std::string& saved = REXCVAR_GET(code_patch_states);
  for (const PPCSwitchablePatch* p = patches; p && p->name; ++p) {
    const std::string key = std::string(p->name) + "=";
    for (size_t at = 0; at < saved.size();) {
      const size_t end = std::min(saved.find(';', at), saved.size());
      const std::string_view item(saved.data() + at, end - at);
      if (item.starts_with(key) && item.size() == key.size() + 1) {
        __atomic_store_n(p->active, uint8_t(item.back() == '1'), __ATOMIC_RELAXED);
      }
      at = end + 1;
    }
  }
}

std::string SaveCodePatchStates(const PPCSwitchablePatch* patches) {
  std::string out;
  for (const PPCSwitchablePatch* p = patches; p && p->name; ++p) {
    if (!out.empty()) {
      out += ';';
    }
    out += fmt::format("{}={}", p->name, __atomic_load_n(p->active, __ATOMIC_RELAXED) ? 1 : 0);
  }
  return out;
}

std::vector<std::filesystem::path> SystemUpdateLocations() {
  std::vector<std::filesystem::path> locations;
  if (const std::string& configured = REXCVAR_GET(xbox_guide_system_update); !configured.empty()) {
    locations.emplace_back(configured);
  }
  locations.push_back(rex::filesystem::GetExecutableFolder() / "$SystemUpdate");
  locations.push_back(rex::filesystem::GetLocalAppDataFolder() / "ReXGlue" / "$SystemUpdate");
  return locations;
}

std::string FindString(const std::vector<std::string>& strings, std::string_view prefix,
                       std::string_view fallback) {
  for (const std::string& s : strings) {
    if (s == prefix) {
      return s;
    }
  }
  for (const std::string& s : strings) {
    if (s.starts_with(prefix)) {
      return s;
    }
  }
  return std::string(fallback);
}

namespace {
std::span<const uint8_t> g_embedded_guide;
}

bool RegisterEmbeddedGuide(const uint8_t* data, size_t size) {
  g_embedded_guide = {data, size};
  return true;
}

std::span<const uint8_t> EmbeddedGuide() {
  return g_embedded_guide;
}

std::unique_ptr<GuideAssets> GuideAssets::Load(const std::filesystem::path& path,
                                               std::string* error, GuidePresentation presentation) {
  return FromUpdate(xui::SystemUpdate::Load(path, error), error, presentation);
}

std::unique_ptr<GuideAssets> GuideAssets::LoadBundle(std::span<const uint8_t> bundle,
                                                     std::string* error,
                                                     GuidePresentation presentation) {
  auto modules = xui::SystemUpdate::ReadBundle(bundle, error);
  return modules ? FromUpdate(xui::SystemUpdate::FromModules(*modules, error), error, presentation)
                 : nullptr;
}

std::unique_ptr<GuideAssets> GuideAssets::FromUpdate(std::unique_ptr<xui::SystemUpdate> update,
                                                     std::string* error,
                                                     GuidePresentation presentation) {
  if (!update) {
    return nullptr;
  }
  auto assets = std::make_unique<GuideAssets>();
  assets->update = std::move(update);
  auto scene = [&](xui::Document& out, std::string_view package, std::string_view name) {
    const xui::Package* p = assets->update->Find(package);
    std::string scene_error = "missing";
    auto doc = p ? xui::ParseXur(p->Find(name), &scene_error) : std::nullopt;
    if (!doc) {
      if (error) {
        *error = fmt::format("{} in {}: {}", name, package, scene_error);
      }
      return false;
    }
    out = std::move(*doc);
    return true;
  };
  auto strings = [&](std::vector<std::string>& out, std::string_view package,
                     std::string_view name) {
    if (const xui::Package* p = assets->update->Find(package)) {
      if (auto table = xui::ParseStringTable(p->Find(name), nullptr)) {
        out = std::move(*table);
      }
    }
  };
  if (!scene(assets->skin, "huduiskin/skin", "skin.xur") ||
      !scene(assets->backdrop, "xam/xam", "hudbkgnd.xur")) {
    return nullptr;
  }

  assets->emulator_layout = presentation == GuidePresentation::OriginalXbox;
  if (assets->emulator_layout) {
    if (!scene(assets->main, "hud/hud", "GuideMainEmulator.xur") ||
        !(scene(assets->home_tab, "hud/hud", "HomeTabEmulatorSignedInLocal.xur") ||
          scene(assets->home_tab, "hud/hud", "HomeTabEmulatorSignedIn.xur")) ||
        !scene(assets->games_tab, "hud/hud", "GamesTabEmulatorSignedIn.xur") ||
        !scene(assets->settings_tab, "hud/hud", "SettingsTabEmulatorSignedIn.xur")) {
      if (error) {
        *error = "Original Xbox Guide requires backward-compatibility emulator scenes: " + *error;
      }
      return nullptr;
    }
  } else {
    if (!scene(assets->main, "hud/hud", "GuideMain.xur") ||
        !scene(assets->home_tab, "hud/hud", "HomeTabSignedIn.xur") ||
        !scene(assets->games_tab, "hud/hud", "GamesTabSignedIn.xur") ||
        !scene(assets->settings_tab, "hud/hud", "SettingsTabSignedIn.xur")) {
      return nullptr;
    }
    if (!UseThreeTabs(assets->main.root)) {
      if (error) {
        *error = "GuideMain.xur in hud/hud: not the 2.0.17559 tab layout";
      }
      return nullptr;
    }
  }
  assets->has_xbox_settings =
      assets->emulator_layout && scene(assets->xbox_settings, "hud/hud", "XboxOneXSettings.xur");
  std::string ignored;
  assets->has_achievement_scenes =
      scene(assets->achievements, "gamerprofile/gp", "802_Achievements.xur") &&
      scene(assets->achievement_details, "gamerprofile/gp", "828_AchievDetails.xur");
  if (!assets->has_achievement_scenes && error) {
    REXLOG_WARN("Xbox guide: no achievement scenes ({}); Achievements stays disabled", *error);
    error->clear();
  }
  assets->has_notify = scene(assets->notify, "xam/xam", "notify.xur");

  assets->has_options =
      ((assets->emulator_layout && scene(assets->options, "hud/hud", "OptionsEmulator.xur")) ||
       scene(assets->options, "hud/hud", "Options.xur")) &&
      scene(assets->options_vibration, "hud/hud", "OptionsController.xur") &&
      scene(assets->options_notifications, "hud/hud", "OptionsNotifications.xur") &&
      scene(assets->options_voice, "hud/hud", "OptionsVoice.xur");
  assets->has_keyboard = scene(assets->keyboard_main, "vk/vk", "KeyboardMain.xur") &&
                         scene(assets->keyboard_base, "vk/vk", "KeyboardBase.xur");
  if (error) {
    error->clear();
  }
  strings(assets->hud_strings, "hud/hud", "Strings.xus");
  strings(assets->xam_strings, "huduiskin/xam", "XamStrings.xus");
  strings(assets->profile_strings, "gamerprofile/gp", "GamerProfile_Custom.xus");
  return assets;
}

GuideFonts AddGuideFonts(ImFontAtlas* atlas, int display_height) {
  GuideFonts fonts;

  static const ImWchar kBaseRanges[] = {0x0020, 0x024F, 0x0370, 0x03FF, 0x0400, 0x04FF, 0x2010,
                                        0x2027, 0x20AC, 0x20AC, 0x2122, 0x2122, 0};

  static ImVector<ImWchar> ranges;
  ImFontGlyphRangesBuilder ranges_builder;
  ranges_builder.AddRanges(kBaseRanges);
  ranges.clear();
  ranges_builder.BuildRanges(&ranges);
  const ImWchar* kRanges = ranges.Data;

  const float guide_scale = float(std::max(display_height, 1080)) / 480.0f;
  const float baked_size =
      std::clamp(std::ceil(20.0f * 4.0f / 3.0f * guide_scale / 8.0f) * 8.0f, 48.0f, 128.0f);
  ImFontConfig config;
  config.OversampleH = baked_size > 64.0f ? 1 : 2;
  config.OversampleV = 1;

  if (const auto bundle = EmbeddedGuide(); !bundle.empty()) {
    std::string error;
    if (auto modules = xui::SystemUpdate::ReadBundle(bundle, &error)) {
      if (auto packages = xui::SystemUpdate::FromModules(*modules, &error)) {
        for (const auto& [package, name] :
             {std::pair{"hud/hud", "Strings.xus"}, std::pair{"huduiskin/xam", "XamStrings.xus"},
              std::pair{"gamerprofile/gp", "GamerProfile_Custom.xus"}}) {
          if (const xui::Package* p = packages->Find(package)) {
            if (auto table = xui::ParseStringTable(p->Find(name), nullptr)) {
              for (const std::string& text : *table) {
                ranges_builder.AddText(text.c_str());
              }
            }
          }
        }
        ranges.clear();
        ranges_builder.BuildRanges(&ranges);
        kRanges = ranges.Data;
      }
      xui::SystemUpdate update;
      const auto xtt = modules->find("font/xenonjklatin");
      if (xtt != modules->end() && update.AddFont("xenonjklatin", xtt->second, &error)) {
        const std::span<const uint8_t> ttf = update.Font("xenonjklatin");

        void* data = IM_ALLOC(ttf.size());
        std::memcpy(data, ttf.data(), ttf.size());
        fonts.regular =
            atlas->AddFontFromMemoryTTF(data, int(ttf.size()), baked_size, &config, kRanges);
      } else if (xtt != modules->end()) {
        REXLOG_WARN("Xbox guide: the console font does not load ({})", error);
      }
    }
  }
  if (fonts.regular) {
    fonts.bold = fonts.regular;
    return fonts;
  }

  const char* windows = std::getenv("WINDIR");
  if (!windows) {
    return fonts;
  }
  auto add = [&](const char* file) -> ImFont* {
    const std::filesystem::path path = std::filesystem::path(windows) / "Fonts" / file;
    std::error_code ec;
    if (!std::filesystem::is_regular_file(path, ec)) {
      return nullptr;
    }
    return atlas->AddFontFromFileTTF(path.string().c_str(), baked_size, &config, kRanges);
  };
  fonts.regular = add("segoeui.ttf");
  fonts.bold = add("seguisb.ttf");
  if (!fonts.bold) {
    fonts.bold = fonts.regular;
  }
  return fonts;
}

GuideMedia::GuideMedia(ImmediateDrawer* immediate_drawer, std::shared_ptr<const GuideAssets> assets)
    : immediate_drawer_(immediate_drawer),
      assets_(std::move(assets)),
      player_(audio::UiSoundPlayer::Create()) {}

GuideMedia::~GuideMedia() = default;

ImTextureID GuideMedia::Texture(std::string_view path, std::string_view package, int* width,
                                int* height) {
  const std::string key = fmt::format("{}|{}", package, path);
  auto it = images_.find(key);
  if (it == images_.end()) {
    Image image;
    std::span<const uint8_t> bytes = xui::ResolveFile(*assets_->update, path, package);

    if (path.starts_with("dlc://")) {
      const std::string_view rest = path.substr(6);
      const size_t slash = rest.find('/');
      if (const DlcCatalogEntry* entry = FindEmbeddedDlc(rest.substr(0, slash))) {
        const std::string_view kind =
            slash == std::string_view::npos ? std::string_view() : rest.substr(slash + 1);
        const std::vector<uint8_t>& art = kind == "tile" ? entry->tile : entry->banner;
        bytes = std::span<const uint8_t>(art.data(), art.size());
      }
    }
    if (path == xui::kGamerscoreImage && immediate_drawer_) {
      image.width = image.height = kGamerscoreImageSize;
      const std::vector<uint8_t> rgba = GamerscoreGlyphRGBA(kGamerscoreImageSize);
      image.texture =
          immediate_drawer_->CreateTexture(uint32_t(image.width), uint32_t(image.height),
                                           ImmediateTextureFilter::kLinear, false, rgba.data());
    } else if (!bytes.empty() && immediate_drawer_) {
      std::vector<uint8_t> rgba =
          DecodeImageRGBA(bytes.data(), bytes.size(), image.width, image.height);
      if (!rgba.empty()) {
        image.texture =
            immediate_drawer_->CreateTexture(uint32_t(image.width), uint32_t(image.height),
                                             ImmediateTextureFilter::kLinear, false, rgba.data());
      }
    }
    it = images_.emplace(key, std::move(image)).first;
  }
  *width = it->second.width;
  *height = it->second.height;
  return reinterpret_cast<ImTextureID>(it->second.texture.get());
}

void GuideMedia::PlaySound(std::string_view file, std::string_view package) {
  if (!player_) {
    return;
  }
  const std::string key = fmt::format("{}|{}", package, file);
  auto it = sounds_.find(key);
  if (it == sounds_.end()) {
    std::shared_ptr<const audio::PcmSound> sound;
    std::string error;
    if (auto decoded =
            audio::DecodeXmaFile(xui::ResolveFile(*assets_->update, file, package), &error)) {
      sound = std::make_shared<audio::PcmSound>(std::move(*decoded));
    } else {
      REXLOG_WARN("Xbox guide: sound {} did not decode: {}", file, error);
    }
    it = sounds_.emplace(key, std::move(sound)).first;
  }
  if (it->second) {
    player_->Play(it->second);
  }
}

XboxGuide::XboxGuide(ImGuiDrawer* drawer, std::shared_ptr<const GuideAssets> assets,
                     GuideMedia* media, GuideFonts fonts, GuideHost host, uint16_t held_buttons)
    : ImGuiDialog(drawer),
      assets_(std::move(assets)),
      media_(media),
      fonts_(fonts),
      host_(std::move(host)),
      pad_(held_buttons),
      icons_(host_.immediate_drawer, host_.runtime) {
  render_.regular_font = fonts_.regular;
  render_.bold_font = fonts_.bold;
  render_.texture = [this](std::string_view path, std::string_view package, int* w, int* h) {
    return Texture(path, package, w, h);
  };
  render_.vector_image = [this](ImDrawList& list, std::string_view path,
                                const std::function<ImVec2(ImVec2)>& to_screen, float opacity) {
    return DrawControllerBattery(list, path, to_screen, opacity) ||
           DrawButtonGlyph(list, path, to_screen, opacity, fonts_.bold);
  };
  auto sound = [this](std::string_view file, std::string_view package) {
    media_->PlaySound(file, package);
  };
  auto context = [&](xui::SceneContext& c, std::string package) {
    c.skin = &assets_->skin;
    c.package = std::move(package);
    c.play_sound = sound;
  };
  context(backdrop_context_, "xam/xam");
  context(hud_context_, "hud/hud");
  context(skin_context_, "huduiskin/skin");
  context(profile_context_, "gamerprofile/gp");

  backdrop_ = xui::Element::Create(assets_->backdrop.root, backdrop_context_);
  hud_root_ = backdrop_->FindById("HUDRootScene");
  app_host_ = backdrop_->FindById("AppHostElementId");
  error_host_ = backdrop_->FindById("ErrorHostElement");
  main_ = app_host_->AttachScene(SceneNode(assets_->main), hud_context_);
  tabs_ = main_->FindById("Tabscene");
  for (int tab = 1; tab <= 4; ++tab) {
    tab_scenes_[tab] = main_->FindById(fmt::format("Tab{}", tab));
  }
  tab_scenes_[1]->AttachScene(SceneNode(assets_->games_tab), hud_context_);
  tab_scenes_[2]->AttachScene(SceneNode(assets_->home_tab), hud_context_);
  tab_scenes_[SettingsTab()]->AttachScene(SceneNode(assets_->settings_tab), hud_context_);
  ConfigureMain();

  if (host_.input) {
    host_.input->AddUIInputBlocker();
  }
  kernel::xam::xeXamAddSystemUI();
  if (host_.kernel_state) {
    host_.kernel_state->BroadcastNotification(kXnSysUi, 1);
  }

  hud_root_->Play("ClosedToHalf");
  main_->Play("2Close");
  tabs_->Play("2Close");
  SetFocus(FirstFocusable(tab_scenes_[2]), true);
  last_tick_ = opened_ = std::chrono::steady_clock::now();
}

XboxGuide::~XboxGuide() {
  if (host_.input) {
    host_.input->RemoveUIInputBlocker();
  }
  kernel::xam::xeXamRemoveSystemUI();
  if (host_.kernel_state) {
    host_.kernel_state->BroadcastNotification(kXnSysUi, 0);
  }
}

void XboxGuide::ConfigureMain() {
  for (std::string_view id : kRemovedEntries) {
    RemoveEntry(main_, id);
  }
  if (!assets_->emulator_layout && tab_scenes_[kRemovedTab]) {
    tab_scenes_[kRemovedTab]->Suppress();
  }
  for (std::string_view id : {"txt_Media", "txt_MediaSel"}) {
    if (xui::Element* label = main_->FindById(id)) {
      label->Suppress();
    }
  }
  if (!assets_->emulator_layout) {
    CoverBladeEdge(main_->FindById("Blade_Focus"));
  }
  if (assets_->emulator_layout) {
    if (xui::Element* awards = main_->FindById("btnAvatarAwards")) {
      xui::Element* games = awards->parent();
      AddEntry(games, "btnAvatarAwards", "btnAvatarAwards", "btnManageGame", "Manage Game",
               "XuiButtonGuide");
      AddEntry(games, "btnAvatarAwards", "btnManageGame", "btnTitleUpdates", "Title Updates",
               "XuiButtonGuide");
      AddEntry(games, "btnAvatarAwards", "btnTitleUpdates", "btnGameUpdate", "Game Update",
               "XuiButtonGuide");
      AddEntry(games, "btnAvatarAwards", "btnGameUpdate", "btnActiveDownloads", "Active Downloads",
               "XuiButtonGuide");
    }
  } else if (xui::Element* recent = main_->FindById("btnQuickLaunch")) {
    AddEntry(recent->parent(), "btnQuickLaunch", "btnAchievements", "btnManageGame", "Manage Game");

    AddEntry(recent->parent(), "btnQuickLaunch", "btnManageGame", "btnTitleUpdates",
             "Title Updates");
    AddEntry(recent->parent(), "btnQuickLaunch", "btnTitleUpdates", "btnGameUpdate", "Game Update");
  }

  const std::string_view system_settings =
      assets_->emulator_layout ? "btnXboxOneXSettings" : "btnSystemSettings";
  if (xui::Element* preferences = main_->FindById("btnPersonalSettings")) {
    xui::Element* settings = preferences->parent();
    AddEntry(settings, "btnPersonalSettings", system_settings, "btnPatches", "Patches");
    AddEntry(settings, "btnPersonalSettings", "btnPatches", "btnMods", "Mods");
    AddEntry(settings, "btnPersonalSettings", "btnMods", "btnCheats", "Cheats");
  }
  if (xui::Element* home = main_->FindById("btnDashboard")) {
    home->SetText(std::string(kLeaveGame));
  }
  main_->Set("LegendY", xui::Value{std::string(kLeaveGame)});
  if (xui::Element* system = main_->FindById(system_settings)) {
    system->SetText(std::string(kXboxSettings));
  }
  const auto* profile = host_.kernel_state ? host_.kernel_state->user_profile() : nullptr;
  if (profile && !profile->name().empty()) {
    for (std::string_view id : kHomeTabLabels) {
      if (xui::Element* label = main_->FindById(id)) {
        label->SetText(profile->name());
      }
    }
  }
  HideButtonLetters(backdrop_.get());
  auto handled = [&](std::string_view id) {
    if (assets_->has_options &&
        (id == "btnPersonalSettings" || id == "btnPatches" || id == "btnMods" ||
         id == "btnCheats" || id == "btnManageGame" || id == "btnTitleUpdates" ||
         id == "btnGameUpdate" || id == "btnActiveDownloads" || id == "btnManageStorage")) {
      return true;
    }
    if (id == "btnXboxOneXSettings") {
      return assets_->has_xbox_settings;
    }
    return id == "btnDashboard" ||
           (id == "btnAchievements" && assets_->has_achievement_scenes && host_.achievements);
  };
  ForEach(main_, [&](xui::Element* e) {
    if (e->focusable() && !handled(e->id())) {
      e->Set("Enabled", xui::Value{false});
      e->Play("NormalDisable");
    }
  });
  for (xui::Element* tab : tab_scenes_) {
    ScrollMenuTo(tab ? FirstFocusable(tab) : nullptr);
  }

  if (xui::Element* button = main_->FindById("btnAchievements"); button && host_.achievements) {
    uint32_t earned = 0;
    for (const auto& a : host_.achievements->ListAchievements()) {
      if (host_.achievements->IsUnlocked(a.id)) {
        earned += a.gamerscore;
      }
    }
    button->SetSecondaryText(std::to_string(earned));
  }
  if (xui::Element* picture = backdrop_->FindById("GamerPic")) {
    picture->Set("ImagePath", xui::Value{std::string(kDefaultGamerPicture)});
  }
  ShowControllerStatus(main_);
  battery_second_ = -1;
  UpdateControllerBattery();
  SetLegends(main_->GetString("LegendA"), main_->GetString("LegendB"), main_->GetString("LegendY"));
  UpdateClock();
}

void XboxGuide::SetLegends(std::string_view a, std::string_view b, std::string_view y,
                           std::string_view x) {
  auto legend = [&](std::string_view button, std::string_view text_id, std::string_view text) {
    xui::Element* glyph = backdrop_->FindById(button);
    xui::Element* label = backdrop_->FindById(text_id);
    if (!glyph || !label) {
      return;
    }
    glyph->SetVisible(!text.empty());
    label->SetVisible(!text.empty());
    label->SetText(std::string(text));
  };
  legend("AButton", "AText", a);
  legend("BButton", "BText", b);
  legend("XButton", "XText", x);
  legend("YButton", "YText", y);
  LayOutLegends(*backdrop_, assets_->backdrop.root, fonts_.regular);
}

void XboxGuide::ColourGamerscoreGlyph() {
  xui::Element* button = main_->FindById("btnAchievements");
  xui::Element* label = button ? button->FindById("text_Label2") : nullptr;
  xui::Element* glyph = button ? button->FindById("glyph_presenter") : nullptr;
  if (label && glyph) {
    glyph->Set("TextColor", xui::Value{xui::Color{label->GetColor("TextColor", 0xFFEBEBEB)}});
  }
}

void XboxGuide::UpdateControllerBattery() {
  const std::time_t now = std::time(nullptr);
  if (now == battery_second_) {
    return;
  }
  battery_second_ = now;
  xui::Element* icon = main_->FindById("imgControllerBattery");
  if (!icon) {
    return;
  }

  input::PadBattery battery;
  const bool known = host_.input && host_.input->GetBattery(0, &battery) && battery.wireless &&
                     battery.percent >= 0;
  icon->SetVisible(known);
  if (known) {
    icon->Seek(BatteryFrame(battery.percent));
  }
}

void XboxGuide::UpdateClock() {
  const std::time_t now = std::time(nullptr);
  if (now / 60 == clock_minute_) {
    return;
  }
  clock_minute_ = now / 60;
  std::tm local = {};
  localtime_s(&local, &now);
  const int hour = local.tm_hour % 12 == 0 ? 12 : local.tm_hour % 12;
  if (xui::Element* clock = backdrop_->FindById("DateTimeTextId")) {
    clock->SetText(
        fmt::format("{}:{:02} {}", hour, local.tm_min, local.tm_hour < 12 ? "AM" : "PM"));
  }
}

ImTextureID XboxGuide::Texture(std::string_view path, std::string_view package, int* width,
                               int* height) {
  if (path.starts_with(kAchievementScheme) && host_.achievements) {
    const uint32_t id = uint32_t(
        std::strtoul(std::string(path.substr(kAchievementScheme.size())).c_str(), nullptr, 10));
    if (auto info = host_.achievements->FindAchievement(id)) {
      if (ImmediateTexture* icon = icons_.GetIcon(*info)) {
        *width = int(icon->width);
        *height = int(icon->height);
        return reinterpret_cast<ImTextureID>(icon);
      }
    }
    return ImTextureID{};
  }
  return media_->Texture(path, package, width, height);
}

xui::Element* XboxGuide::FirstFocusable(xui::Element* root) {
  xui::Element* found = nullptr;
  ForEach(root, [&](xui::Element* e) {
    if (!found && e != root && e->focusable() && !e->IsA("XuiBackButton")) {
      found = e;
    }
  });
  return found;
}

void XboxGuide::SetFocus(xui::Element* control, bool initial) {
  if (!control || control == focus_) {
    return;
  }
  xui::Element::MoveFocus(focus_, control, initial);
  focus_ = control;
  if (IsTabMenuEntry(control)) {
    ScrollMenuTo(control);
  }
}

bool XboxGuide::IsTabMenuEntry(const xui::Element* control) const {
  const xui::Element* menu = control->parent();
  return menu && std::find(std::begin(tab_scenes_), std::end(tab_scenes_), menu->parent()) !=
                     std::end(tab_scenes_);
}

void XboxGuide::Dismiss() {
  BeginClose(false);
}

void XboxGuide::BeginClose(bool exit_title) {
  if (closing_) {
    return;
  }
  closing_ = true;
  exit_title_ = exit_title;
  if (screen_ == Screen::kConfirm) {
    hud_root_->Play("ErrorToClosed");
  } else if (screen_ == Screen::kAchievements || screen_ == Screen::kAchievementDetail ||
             screen_ == Screen::kSettings) {
    hud_root_->Play("FullToClosed");
  } else {
    hud_root_->Play("HalfToClosed");
    main_->Play(fmt::format("{}Open", tab_));
    tabs_->Play(fmt::format("{}Open", tab_));
  }
}

void XboxGuide::OnDraw(ImGuiIO& io) {
  const auto now = std::chrono::steady_clock::now();
  const double seconds = std::chrono::duration<double>(now - last_tick_).count();
  last_tick_ = now;

  if (!closing_) {
    struct Key {
      ImGuiKey key;
      GuideAction action;
    };
    constexpr Key kKeys[] = {
        {ImGuiKey_UpArrow, GuideAction::kUp},
        {ImGuiKey_DownArrow, GuideAction::kDown},
        {ImGuiKey_LeftArrow, GuideAction::kLeft},
        {ImGuiKey_RightArrow, GuideAction::kRight},
        {ImGuiKey_Enter, GuideAction::kA},
        {ImGuiKey_Space, GuideAction::kA},
        {ImGuiKey_Escape, GuideAction::kB},
        {ImGuiKey_Backspace, GuideAction::kB},
        {ImGuiKey_X, GuideAction::kX},
        {ImGuiKey_Y, GuideAction::kY},
        {ImGuiKey_PageUp, GuideAction::kPreviousTab},
        {ImGuiKey_PageDown, GuideAction::kNextTab},
    };
    std::vector<GuideAction> actions;
    for (const Key& key : kKeys) {
      if (ImGui::IsKeyPressed(key.key, true)) {
        actions.push_back(key.action);
      }
    }
    if (host_.input) {
      input::X_INPUT_STATE state = {};
      const uint32_t user = host_.input->GetLastUsedUser();
      if (host_.input->GetStateForUI(user, &state) == X_ERROR_SUCCESS) {
        const uint64_t now_ms =
            uint64_t(std::chrono::duration_cast<std::chrono::milliseconds>(now - opened_).count());
        for (GuideAction action : pad_.Update(state.gamepad.buttons, state.gamepad.thumb_lx,
                                              state.gamepad.thumb_ly, now_ms)) {
          actions.push_back(action);
        }
      }
    }
    for (GuideAction action : actions) {
      Handle(action);
      if (closing_) {
        break;
      }
    }
  }

  UpdateClock();
  UpdateControllerBattery();
  backdrop_->Advance(std::min(seconds, 0.25) * xui::kFramesPerSecond);
  ColourGamerscoreGlyph();
  PollManageGame();
  PollTitleUpdates();
  PollGameUpdate();
  PollActiveDownloads();
  if (!hud_root_->playing()) {
    for (xui::Element* element : pending_removal_) {
      element->parent()->RemoveChild(element);
    }
    pending_removal_.clear();
  }

  dim_ = closing_ ? std::max(0.0f, dim_ - float(seconds / kDimOutSeconds))
                  : std::min(1.0f, dim_ + float(seconds / kDimInSeconds));
  ImGui::GetForegroundDrawList()->AddRectFilled(
      ImVec2(0, 0), io.DisplaySize, IM_COL32(0, 0, 0, int(kDimOpacity * dim_ * 255.0f + 0.5f)));

  const float scale = std::min(io.DisplaySize.x / kSceneWidth, io.DisplaySize.y / kSceneHeight);
  const ImVec2 origin((io.DisplaySize.x - kSceneWidth * scale) / 2,
                      (io.DisplaySize.y - kSceneHeight * scale) / 2);
  render_.pixels_per_point = imgui_drawer()->PixelsPerPoint();
  xui::Render(ImGui::GetForegroundDrawList(), *backdrop_, origin, scale, 1.0f, render_);

  if (closing_ && !hud_root_->playing() && !tabs_->playing() && dim_ == 0.0f) {
    Close();
  }
}

void XboxGuide::OnClose() {
  if (host_.on_closed) {
    host_.on_closed(exit_title_);
  }
}

void XboxGuide::Handle(GuideAction action) {
  switch (screen_) {
    case Screen::kMain:
      HandleMain(action);
      break;
    case Screen::kAchievements:
    case Screen::kAchievementDetail:
      HandleAchievements(action);
      break;
    case Screen::kConfirm:
      HandleConfirm(action);
      break;
    case Screen::kSettings:
      HandleSettings(action);
      break;
  }
}

void XboxGuide::HandleMain(GuideAction action) {
  switch (action) {
    case GuideAction::kUp:
    case GuideAction::kDown:
      if (focus_) {
        SetFocus(focus_->Navigate(action == GuideAction::kUp ? xui::NavDirection::kUp
                                                             : xui::NavDirection::kDown));
      }
      break;
    case GuideAction::kLeft:
    case GuideAction::kPreviousTab:
      SwitchTab(-1);
      break;
    case GuideAction::kRight:
    case GuideAction::kNextTab:
      SwitchTab(+1);
      break;
    case GuideAction::kA:
      Activate(focus_);
      break;
    case GuideAction::kB:
      BeginClose(false);
      break;
    case GuideAction::kY:
      OpenConfirm(Confirm::kXboxHome);
      break;
    case GuideAction::kX:
      break;
  }
}

std::span<const int> XboxGuide::Tabs() const {
  return assets_->emulator_layout ? std::span<const int>(kEmulatorTabs)
                                  : std::span<const int>(kTabs);
}

void XboxGuide::SwitchTab(int direction) {
  const std::span<const int> tabs = Tabs();
  const auto at = std::find(tabs.begin(), tabs.end(), tab_);
  const ptrdiff_t next = (at - tabs.begin()) + direction;
  if (at == tabs.end() || next < 0 || next >= ptrdiff_t(tabs.size())) {
    return;
  }
  const int tab = tabs[next];
  if (tabs_->playing() || !tabs_->Play(fmt::format("{}To{}", tab_, tab))) {
    return;
  }
  tab_focus_[tab_] = focus_;
  xui::Element::MoveFocus(focus_, nullptr);
  focus_ = nullptr;
  tab_ = tab;
  xui::Element* target = tab_focus_[tab] ? tab_focus_[tab] : FirstFocusable(tab_scenes_[tab]);
  if (target) {
    xui::Element::MoveFocus(nullptr, target, true);
    focus_ = target;
  }
}

void XboxGuide::Activate(xui::Element* control) {
  if (!control) {
    return;
  }
  control->Press();
  if (!control->enabled()) {
    return;
  }
  const std::string_view id = control->id();
  if (id == "btnDashboard") {
    OpenConfirm(Confirm::kXboxHome);

  } else if (id == "btnAchievements") {
    OpenAchievements();
  } else if (id == "btnPersonalSettings") {
    OpenPreferences();
  } else if (id == "btnXboxOneXSettings") {
    OpenXboxSettings();
  } else if (id == "btnPatches") {
    OpenPatches("patch");
  } else if (id == "btnMods") {
    OpenPatches("mod");
  } else if (id == "btnCheats") {
    OpenCheats();
  } else if (id == "btnManageGame") {
    OpenManageGame();
  } else if (id == "btnManageStorage") {
    OpenManageStorage();
  } else if (id == "btnTitleUpdates") {
    OpenTitleUpdates();
  } else if (id == "btnGameUpdate") {
    OpenGameUpdate();
  } else if (id == "btnActiveDownloads") {
    OpenActiveDownloads();
  }
}

void XboxGuide::OpenAchievements() {
  achievement_list_ = host_.achievements->ListAchievements();

  std::stable_partition(
      achievement_list_.begin(), achievement_list_.end(),
      [&](const system::AchievementInfo& a) { return host_.achievements->IsUnlocked(a.id); });
  return_focus_ = focus_;
  hud_root_->Play("HalfToFull");
  tabs_->Play(fmt::format("{}Open", tab_));
  main_->SetVisible(false);
  achievements_ = app_host_->AttachScene(SceneNode(assets_->achievements), profile_context_);
  screen_ = Screen::kAchievements;

  uint32_t unlocked = 0;
  for (const auto& a : achievement_list_) {
    unlocked += host_.achievements->IsUnlocked(a.id) ? 1 : 0;
  }
  auto set_text = [&](std::string_view id, std::string text) {
    if (xui::Element* e = achievements_->FindById(id)) {
      e->SetText(std::move(text));
    }
  };
  set_text("labGameTitle", host_.title_name);
  set_text("labCountText",
           FormatCount(FindString(assets_->profile_strings, "%1!u! of %2!u! Achievements",
                                  "%1!u! of %2!u! Achievements"),
                       unlocked, uint32_t(achievement_list_.size())));

  if (xui::Element* loading = achievements_->FindById("ctlLoading")) {
    loading->SetVisible(false);
  }
  if (xui::Element* header = achievements_->FindById("HeaderShader")) {
    header->SetVisible(true);
  }
  xui::Element* list = achievements_->FindById("AchievementList");
  if (achievement_list_.empty() || !list) {
    if (xui::Element* empty = achievements_->FindById("labEmpty")) {
      empty->SetVisible(true);
    }
    SetLegends("", achievements_->GetString("LegendB"), "");
    return;
  }
  list->SetVisible(true);
  list->Set("ClipChildren", xui::Value{true});
  std::vector<xui::Element*> items = list->PopulateList(achievement_list_.size(), 1);
  const float item_width = items.front()->width();
  const float item_height = items.front()->height();
  columns_ = std::max(1, int(list->width() / item_width));
  visible_rows_ = std::max(1, int(list->height() / item_height));
  items = list->PopulateList(achievement_list_.size(), columns_);
  for (size_t i = 0; i < items.size(); ++i) {
    const system::AchievementInfo& a = achievement_list_[i];
    std::string image;
    if (host_.achievements->IsUnlocked(a.id)) {
      image = fmt::format("{}{}", kAchievementScheme, a.id);
    } else if (a.flags & kAchievementShowUnachieved) {
      image = "sharedres://unearnedAchievement.png";
    } else {
      image = "sharedres://secretAchievement.png";
    }
    items[i]->Set("ImagePath", xui::Value{std::move(image)});
  }
  selected_ = 0;
  first_row_ = 0;
  focus_ = nullptr;
  ScrollAchievements();
  SetFocus(items[0], true);
  ShowAchievement(0);
  SetLegends(achievements_->GetString("LegendA"), achievements_->GetString("LegendB"), "");
}

void XboxGuide::ScrollAchievements() {
  xui::Element* list = achievements_ ? achievements_->FindById("AchievementList") : nullptr;
  if (!list) {
    return;
  }
  const size_t row = selected_ / size_t(columns_);
  if (row < first_row_) {
    first_row_ = row;
  } else if (row >= first_row_ + size_t(visible_rows_)) {
    first_row_ = row - size_t(visible_rows_) + 1;
  }
  const auto& items = list->list_items();
  for (size_t i = 0; i < items.size(); ++i) {
    const size_t item_row = i / size_t(columns_);
    const float height = items[i]->height();
    const float width = items[i]->width();
    items[i]->Set("Position",
                  xui::Value{xui::Vec3{float(i % size_t(columns_)) * width,
                                       (float(item_row) - float(first_row_)) * height, 0.0f}});
    items[i]->SetVisible(item_row >= first_row_ && item_row < first_row_ + size_t(visible_rows_));
  }
}

void XboxGuide::ShowAchievement(size_t index) {
  if (index >= achievement_list_.size()) {
    return;
  }
  const system::AchievementInfo& a = achievement_list_[index];
  const bool unlocked = host_.achievements->IsUnlocked(a.id);
  const bool secret = !unlocked && !(a.flags & kAchievementShowUnachieved);
  auto set_text = [&](std::string_view id, std::string text) {
    if (xui::Element* e = achievements_->FindById(id)) {
      e->SetText(std::move(text));
    }
  };
  set_text("labAchievementName", secret ? "Secret Achievement" : a.label);
  set_text("labAchievementDescription",
           secret ? "Continue playing to unlock this secret achievement."
                  : (unlocked || a.unachieved_description.empty() ? a.description
                                                                  : a.unachieved_description));
  set_text("labPoints", fmt::format("{} {}", a.gamerscore, xui::kGamerscoreGlyph));
  std::string date;
  if (unlocked) {
    const uint64_t filetime = host_.achievements->GetUnlockTime(a.id);
    const std::time_t unix_time = std::time_t(filetime / 10000000ull - 11644473600ull);
    std::tm local = {};
    if (filetime && localtime_s(&local, &unix_time) == 0) {
      date = fmt::format("{}/{}/{}", local.tm_mon + 1, local.tm_mday, local.tm_year + 1900);
    }
  }
  set_text("labAcquiredDate", date);
}

void XboxGuide::OpenAchievementDetail() {
  const system::AchievementInfo& a = achievement_list_[selected_];
  const bool unlocked = host_.achievements->IsUnlocked(a.id);
  const bool secret = !unlocked && !(a.flags & kAchievementShowUnachieved);
  achievements_->SetVisible(false);
  details_ = app_host_->AttachScene(SceneNode(assets_->achievement_details), profile_context_);
  screen_ = Screen::kAchievementDetail;
  auto set_text = [&](std::string_view id, std::string text) {
    if (xui::Element* e = details_->FindById(id)) {
      e->SetText(std::move(text));
    }
  };
  set_text("headerText", host_.title_name);
  set_text("achievementTitleText", secret ? "Secret Achievement" : a.label);
  set_text("credText", fmt::format("{} {}", a.gamerscore, xui::kGamerscoreGlyph));
  set_text("achievementDescriptionText",
           secret ? "Continue playing to unlock this secret achievement."
                  : (unlocked || a.unachieved_description.empty() ? a.description
                                                                  : a.unachieved_description));
  details_->Set("ImagePath",
                xui::Value{unlocked ? fmt::format("{}{}", kAchievementScheme, a.id)
                                    : std::string(secret ? "sharedres://secretAchievement.png"
                                                         : "sharedres://unearnedAchievement.png")});
  SetLegends("", details_->GetString("LegendB"), "");
  media_->PlaySound("btn_selectG.xma", "xam/skin");
}

void XboxGuide::CloseAchievements() {
  if (screen_ == Screen::kAchievementDetail) {
    app_host_->RemoveChild(details_);
    details_ = nullptr;
    achievements_->SetVisible(true);
    screen_ = Screen::kAchievements;
    SetLegends(achievements_->GetString("LegendA"), achievements_->GetString("LegendB"), "");
    media_->PlaySound("sharedres://btn_Back.xma", "");
    return;
  }
  hud_root_->Play("FullToHalf");
  app_host_->RemoveChild(achievements_);
  achievements_ = nullptr;
  focus_ = nullptr;
  main_->SetVisible(true);
  main_->Play(fmt::format("{}Close", tab_));
  tabs_->Play(fmt::format("{}Close", tab_));
  screen_ = Screen::kMain;
  SetFocus(return_focus_, true);
  SetLegends(main_->GetString("LegendA"), main_->GetString("LegendB"), main_->GetString("LegendY"));
  media_->PlaySound("sharedres://btn_Back.xma", "");
}

void XboxGuide::HandleAchievements(GuideAction action) {
  if (screen_ == Screen::kAchievementDetail) {
    if (action == GuideAction::kB) {
      CloseAchievements();
    }
    return;
  }
  xui::Element* list = achievements_->FindById("AchievementList");
  const auto& items = list ? list->list_items() : std::vector<xui::Element*>();
  size_t next = selected_;
  switch (action) {
    case GuideAction::kLeft:
      next = selected_ > 0 ? selected_ - 1 : selected_;
      break;
    case GuideAction::kRight:
      next = selected_ + 1 < items.size() ? selected_ + 1 : selected_;
      break;
    case GuideAction::kUp:
      next = selected_ >= size_t(columns_) ? selected_ - size_t(columns_) : selected_;
      break;
    case GuideAction::kDown:
      next = std::min(selected_ + size_t(columns_), items.empty() ? 0 : items.size() - 1);
      break;
    case GuideAction::kA:
      if (!items.empty()) {
        items[selected_]->Press();
        OpenAchievementDetail();
      }
      return;
    case GuideAction::kB:
      CloseAchievements();
      return;
    default:
      return;
  }
  if (next != selected_ && next < items.size()) {
    selected_ = next;
    ScrollAchievements();
    SetFocus(items[selected_]);
    ShowAchievement(selected_);
  }
}

void XboxGuide::OpenConfirm(Confirm confirm) {
  const xui::Node* visual = FindVisual(assets_->skin, "XuiMessageBox3");
  if (!visual || visual->children.empty()) {
    return;
  }
  confirm_ = confirm;
  return_focus_ = focus_;
  hud_root_->Play(screen_ == Screen::kMain ? "HalfToError" : "FullToError");
  message_ = error_host_->AttachScene(visual->children.front(), skin_context_);
  HideButtonLetters(message_);
  screen_ = Screen::kConfirm;

  std::string title(main_->GetString("LegendY"));
  if (focus_ && focus_->enabled() && !focus_->text().empty() &&
      (confirm == Confirm::kTurnOff || focus_->id() == "btnDashboard")) {
    title = focus_->text();
  }
  std::string body =
      confirm == Confirm::kXboxHome
          ? std::string(kLeaveGameWarning)
          : FindString(assets_->hud_strings, "Turning off the console",
                       "Turning off the console will also turn off all controllers.");
  if (confirm == Confirm::kDeleteSave && confirm_save_ < saves_.size()) {
    title = "Delete";
    body = fmt::format("Delete {}? It can't be recovered.", saves_[confirm_save_].name);
  }
  if (confirm == Confirm::kTitleUpdate) {
    title = confirm_title_update_ ? fmt::format("Turn On Title Update {}", confirm_title_update_)
                                  : "Turn Off Title Update";
    body = confirm_title_update_
               ? "The game restarts to run the update. Mods made for the original version are "
                 "left out while it's on. Any unsaved progress will be lost."
               : "The game restarts with the original version. Any unsaved progress will be "
                 "lost.";
  }
  if (confirm == Confirm::kGameUpdate) {
    title = fmt::format("Install Version {}", GetAppUpdateState().latest_version);
    body =
        "The game closes, installs the update and starts again. Saves and settings stay. Any "
        "unsaved progress will be lost.";
  } else if (confirm == Confirm::kGameRollBack) {
    title = "Use Previous Version";
    body =
        "The game closes, goes back to the version before the last update and starts again. "
        "Saves and settings stay. Any unsaved progress will be lost.";
  }
  message_->SetText(title);
  auto set_text = [&](std::string_view id, std::string text) {
    if (xui::Element* e = message_->FindById(id)) {
      e->SetText(std::move(text));
    }
  };
  set_text("MessageText", body);
  set_text("Button0", FindString(assets_->xam_strings, "Yes", "Yes"));
  set_text("Button1", FindString(assets_->xam_strings, "No", "No"));
  set_text("btnA", std::string(main_->GetString("LegendA")));
  set_text("btnB", std::string(main_->GetString("LegendB")));
  for (std::string_view hidden : {"Icon", "ProgressAnimation"}) {
    if (xui::Element* e = message_->FindById(hidden)) {
      e->SetVisible(false);
    }
  }
  SetLegends("", "", "");
  focus_ = nullptr;

  SetFocus(message_->FindById("Button1"), true);
}

void XboxGuide::CloseConfirm() {
  hud_root_->Play(achievements_ || !pages_.empty() ? "ErrorToFull" : "ErrorToHalf");
  pending_removal_.push_back(message_);
  message_ = nullptr;
  screen_ = achievements_    ? (details_ ? Screen::kAchievementDetail : Screen::kAchievements)
            : pages_.empty() ? Screen::kMain
                             : Screen::kSettings;
  focus_ = nullptr;
  SetFocus(return_focus_, true);
  if (screen_ == Screen::kMain) {
    SetLegends(main_->GetString("LegendA"), main_->GetString("LegendB"),
               main_->GetString("LegendY"));
  } else if (screen_ == Screen::kSettings && pages_.back().on_focus) {
    pages_.back().on_focus();
  }
  media_->PlaySound("sharedres://btn_Back.xma", "");
}

void XboxGuide::HandleConfirm(GuideAction action) {
  switch (action) {
    case GuideAction::kUp:
    case GuideAction::kDown:
      if (focus_) {
        SetFocus(focus_->Navigate(action == GuideAction::kUp ? xui::NavDirection::kUp
                                                             : xui::NavDirection::kDown));
      }
      break;
    case GuideAction::kA:
      if (focus_) {
        focus_->Press();
        if (focus_->id() == "Button0") {
          if (confirm_ == Confirm::kTitleUpdate) {
            ApplyTitleUpdateChoice();
          } else if (confirm_ == Confirm::kGameUpdate || confirm_ == Confirm::kGameRollBack) {
            ApplyGameUpdateChoice();
          } else if (confirm_ == Confirm::kDeleteSave) {
            CloseConfirm();
            DeleteChosenSave();
          } else {
            BeginClose(true);
          }
        } else {
          CloseConfirm();
        }
      }
      break;
    case GuideAction::kB:
      CloseConfirm();
      break;
    default:
      break;
  }
}

}
