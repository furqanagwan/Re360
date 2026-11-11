/**
 * @file        ui/guide/src/guide_settings.cpp
 * @brief       The guide's settings pages on the console's Options scenes (RG-GDK-041)
 *
 * Preferences and the pages it opens, plus Patches, Mods and Cheats, are each one
 * of the dashboard's own Options scenes, so they look and sound like the
 * rest of the guide. Entries a recompiled title has no use for are taken
 * out of a scene and the list closed up; new entries are copies of the
 * scene's own controls.
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/guide/xbox_guide.h>

#include <algorithm>
#include <vector>

#include <fmt/format.h>

#include <rex/cvar.h>
#include <rex/logging.h>

#include <rex/ui/guide/device_details.h>
#include <rex/ui/guide/guide_layout.h>

REXCVAR_DECLARE(bool, vibration);
REXCVAR_DECLARE(int32_t, audio_volume);
REXCVAR_DECLARE(bool, audio_mute_minimized);
REXCVAR_DECLARE(std::string, audio_output_device);
REXCVAR_DECLARE(int32_t, monitor);

namespace rex::ui::guide {
namespace {

constexpr int kVolumeStep = 10;
constexpr float kRowHeight = 28.0f;
constexpr size_t kMaxChoiceRows = 8;

constexpr double kSliderFocusFrame = 101.0;

void SetText(xui::Element* scene, std::string_view id, std::string text) {
  if (xui::Element* e = scene->FindById(id)) {
    e->SetText(std::move(text));
  }
}

void Hide(xui::Element* scene, std::initializer_list<std::string_view> ids) {
  for (std::string_view id : ids) {
    if (xui::Element* e = scene->FindById(id)) {
      e->Suppress();
    }
  }
}

void MoveTo(xui::Element* scene, std::string_view id, float y) {
  if (xui::Element* e = scene->FindById(id)) {
    xui::Vec3 p = e->GetVector("Position");
    p.y = y;
    e->Set("Position", xui::Value{p});
  }
}

void SetNav(xui::Element* control, std::string up, std::string down) {
  control->Set("NavUp", xui::Value{std::move(up)});
  control->Set("NavDown", xui::Value{std::move(down)});
}

std::vector<std::string> OutputRowNames(const std::vector<audio::AudioOutput>& outputs) {
  std::vector<std::string> names;
  for (const audio::AudioOutput& output : outputs) {
    const size_t adapter = output.name.rfind(" (");
    names.push_back(adapter != std::string::npos && adapter > 0 && output.name.ends_with(')')
                        ? output.name.substr(0, adapter)
                        : output.name);
    if (names.back().empty()) {
      names.back() = "Audio Output";
    }
  }
  const std::vector<std::string> short_names = names;
  for (size_t i = 0; i < names.size(); ++i) {
    if (std::count(short_names.begin(), short_names.end(), short_names[i]) > 1 &&
        !outputs[i].name.empty()) {
      names[i] = outputs[i].name;
    }
  }
  return names;
}

std::vector<xui::Element*> FillPlayThroughList(xui::Element* scene,
                                               const std::vector<std::string>& texts,
                                               std::string heading) {
  Hide(scene, {"sliderVolume", "sliderDucking", "chkMuteKinect"});
  xui::Element* group = scene->FindById("radgrpOutputLocation");
  xui::Element* last = scene->FindById("radbtnPlayBoth");
  if (!group || !last || texts.empty()) {
    return {};
  }
  constexpr std::string_view kSceneRows[] = {"radbtnPlayHeadset", "radbtnPlayTV", "radbtnPlayBoth"};
  std::vector<xui::Element*> rows;
  for (size_t i = 0; i < texts.size(); ++i) {
    xui::Element* row = i < std::size(kSceneRows)
                            ? group->FindById(kSceneRows[i])
                            : group->CloneChild(*last, fmt::format("radbtnChoice{}", i));
    if (!row) {
      return {};
    }
    row->Set("Position", xui::Value{xui::Vec3{0.0f, float(i) * kRowHeight, 0.0f}});
    row->SetText(texts[i]);
    rows.push_back(row);
  }
  for (size_t i = texts.size(); i < std::size(kSceneRows); ++i) {
    Hide(group, {kSceneRows[i]});
  }
  for (size_t i = 0; i < rows.size(); ++i) {
    SetNav(rows[i], i > 0 ? std::string(rows[i - 1]->id()) : "",
           i + 1 < rows.size() ? std::string(rows[i + 1]->id()) : "");
  }
  group->Set("Height", xui::Value{float(rows.size()) * kRowHeight});
  xui::Element* first = scene->FindById("sliderVolume");
  const xui::Vec3 at = first ? first->GetVector("Position") : xui::Vec3{0.0f, 72.0f, 0.0f};
  MoveTo(scene, "LabelSubHeader2", at.y - 6.0f);
  MoveTo(scene, "radgrpOutputLocation", at.y + 22.0f);
  SetText(scene, "LabelSubHeader2", std::move(heading));
  return rows;
}

void CheckOnly(xui::Element* group, xui::Element* chosen) {
  for (const auto& button : group->children()) {
    if (button->IsA("XuiRadioButton")) {
      button->SetChecked(button.get() == chosen);
    }
  }
}

}

XboxGuide::SettingsPage& XboxGuide::PushPage(const xui::Document& scene, std::string heading) {
  if (pages_.empty()) {
    hud_root_->Play("HalfToFull");
    tabs_->Play(fmt::format("{}Open", tab_));
    main_->SetVisible(false);
    screen_ = Screen::kSettings;
  } else {
    pages_.back().scene->SetVisible(false);
  }
  SettingsPage page;
  page.return_focus = focus_;
  page.scene = app_host_->AttachScene(SceneNode(scene), hud_context_);
  focus_ = nullptr;
  if (!heading.empty()) {
    SetText(page.scene, "labelHeading", std::move(heading));
  }
  SetLegends(page.scene->GetString("LegendA"), page.scene->GetString("LegendB"), "");
  pages_.push_back(std::move(page));
  return pages_.back();
}

void XboxGuide::PopPage() {
  SettingsPage page = std::move(pages_.back());
  pages_.pop_back();
  if (page.scene == manage_scene_) {
    manage_scene_ = nullptr;
    manage_rows_.clear();
  }
  app_host_->RemoveChild(page.scene);
  focus_ = nullptr;
  if (pages_.empty()) {
    hud_root_->Play("FullToHalf");
    main_->SetVisible(true);
    main_->Play(fmt::format("{}Close", tab_));
    tabs_->Play(fmt::format("{}Close", tab_));
    screen_ = Screen::kMain;
    SetLegends(main_->GetString("LegendA"), main_->GetString("LegendB"),
               main_->GetString("LegendY"));
  } else {
    xui::Element* below = pages_.back().scene;
    below->SetVisible(true);
    SetLegends(below->GetString("LegendA"), below->GetString("LegendB"), "");
  }
  SetFocus(page.return_focus, true);
  if (!pages_.empty() && pages_.back().on_focus) {
    pages_.back().on_focus();
  }
  media_->PlaySound("sharedres://btn_Back.xma", "");
}

void XboxGuide::HandleSettings(GuideAction action) {
  const SettingsPage page = pages_.back();
  switch (action) {
    case GuideAction::kUp:
    case GuideAction::kDown:
      if (focus_) {
        SetFocus(focus_->Navigate(action == GuideAction::kUp ? xui::NavDirection::kUp
                                                             : xui::NavDirection::kDown));
        if (page.on_focus) {
          page.on_focus();
        }
      }
      break;
    case GuideAction::kLeft:
    case GuideAction::kRight:
      if (focus_ && page.on_adjust) {
        page.on_adjust(focus_, action == GuideAction::kLeft ? -1 : 1);
      }
      break;
    case GuideAction::kA:
      if (focus_) {
        focus_->Press();
        if (focus_->enabled() && page.on_select) {
          page.on_select(focus_);
        }
      }
      break;
    case GuideAction::kB:
      PopPage();
      break;
    case GuideAction::kX:
      if (focus_ && page.on_x) {
        page.on_x(focus_);
      }
      break;
    case GuideAction::kY:
      if (page.on_y) {
        page.on_y();
      }
      break;
    default:
      break;
  }
}

void XboxGuide::OpenPreferences() {
  xui::Element* scene = PushPage(assets_->options, "").scene;

  for (std::string_view id : {"btnOnlineStatus", "btnPlayTimer", "btnWordRegister"}) {
    RemoveEntry(scene, id);
  }
  SetText(scene, "btnVoice", "Volume");
  if (host_.audio_outputs) {
    AddEntry(scene, "btnVoice", "btnVoice", "btnAudioOutput", "Audio Output");
  }
  if (host_.displays) {
    AddEntry(scene, "btnVoice", host_.audio_outputs ? "btnAudioOutput" : "btnVoice", "btnDisplay",
             "Display");
  }

  if (!assets_->has_xbox_settings) {
    AddEntry(scene, "btnController", "btnController", "btnResolution", "Resolution");
  }
  pages_.back().on_select = [this](xui::Element* control) {
    const std::string_view id = control->id();
    if (id == "btnNotifications") {
      OpenNotifications();
    } else if (id == "btnVoice") {
      OpenVolume();
    } else if (id == "btnAudioOutput") {
      OpenAudioOutput();
    } else if (id == "btnDisplay") {
      OpenDisplay();
    } else if (id == "btnController") {
      OpenVibration();
    } else if (id == "btnResolution") {
      OpenResolution();
    }
  };

  xui::Element* top = scene->FindById("btnNotifications");
  SetFocus(top ? top : FirstFocusable(scene), true);
}

void XboxGuide::OpenVibration() {
  xui::Element* scene = PushPage(assets_->options_vibration, "").scene;
  xui::Element* check = scene->FindById("chkVibration");
  if (!check) {
    return;
  }
  check->SetChecked(REXCVAR_GET(vibration));
  pages_.back().on_select = [this](xui::Element* control) {
    const bool on = !control->checked();
    control->SetChecked(on);
    rex::cvar::SetFlagByName("vibration", on ? "true" : "false");
    if (host_.save_settings) {
      host_.save_settings();
    }
  };
  SetFocus(check, true);
}

void XboxGuide::SetSlider(xui::Element* slider, int value) {
  value = std::clamp(value, 0, 100);
  slider->SetSecondaryText(std::to_string(value));
  if (xui::Element* body = slider->FindById("SliderBody")) {
    body->Seek(double(value) + (slider->focused() ? kSliderFocusFrame : 0.0));
  }
}

void XboxGuide::OpenVolume() {
  xui::Element* scene = PushPage(assets_->options_voice, "Volume").scene;

  Hide(scene, {"sliderVolume", "radgrpOutputLocation", "LabelSubHeader2"});
  xui::Element* slider = scene->FindById("sliderDucking");
  xui::Element* mute = scene->FindById("chkMuteKinect");
  if (!slider) {
    return;
  }
  MoveTo(scene, "sliderDucking", 72.0f);
  SetNav(slider, "", mute ? "chkMuteKinect" : "");
  if (mute) {
    MoveTo(scene, "chkMuteKinect", 72.0f + slider->height() + 14.0f);
    mute->SetVisible(true);
    mute->SetText("Mute When Minimized");
    mute->SetChecked(REXCVAR_GET(audio_mute_minimized));
    SetNav(mute, "sliderDucking", "");
  }
  SetText(scene, "LabelSubHeader1",
          "Game Volume sets how loud the game is. Press left or right to change it.\r\n\r\n"
          "Mute When Minimized silences the game while its window is minimized.");
  pages_.back().on_adjust = [this](xui::Element* control, int direction) {
    if (control->id() != "sliderDucking") {
      return;
    }
    const int volume = std::clamp(REXCVAR_GET(audio_volume) + direction * kVolumeStep, 0, 100);
    if (volume == REXCVAR_GET(audio_volume)) {
      return;
    }
    rex::cvar::SetFlagByName("audio_volume", std::to_string(volume));
    SetSlider(control, volume);
    media_->PlaySound("sharedres://btn_Focus.xma", "");
    if (host_.save_settings) {
      host_.save_settings();
    }
  };
  pages_.back().on_select = [this](xui::Element* control) {
    if (control->id() != "chkMuteKinect") {
      return;
    }
    const bool on = !control->checked();
    control->SetChecked(on);
    rex::cvar::SetFlagByName("audio_mute_minimized", on ? "true" : "false");
    if (host_.save_settings) {
      host_.save_settings();
    }
  };
  pages_.back().on_focus = [this, slider] { SetSlider(slider, REXCVAR_GET(audio_volume)); };
  SetFocus(slider, true);
  SetSlider(slider, REXCVAR_GET(audio_volume));
}

void XboxGuide::OpenNotifications() {
  xui::Element* scene = PushPage(assets_->options_notifications, "").scene;

  Hide(scene, {"chkShowMovies", "chkShowIPTV", "XuiLabel2", "labelSoundDisabled"});
  xui::Element* show = scene->FindById("chkShow");
  xui::Element* sound = scene->FindById("chkSound");
  if (!show || !sound) {
    return;
  }
  auto refresh = [show, sound] {
    show->SetChecked(REXCVAR_GET(notifications_show));

    sound->Set("Enabled", xui::Value{REXCVAR_GET(notifications_show)});
    sound->SetChecked(REXCVAR_GET(notifications_sound));
  };
  refresh();
  pages_.back().on_select = [this, show, refresh](xui::Element* control) {
    const bool on = !control->checked();
    rex::cvar::SetFlagByName(control == show ? "notifications_show" : "notifications_sound",
                             on ? "true" : "false");
    refresh();
    if (host_.save_settings) {
      host_.save_settings();
    }
  };
  SetFocus(show, true);
}

void XboxGuide::OpenResolution() {
  xui::Element* scene = PushPage(assets_->options_voice, "Resolution").scene;

  Hide(scene, {"sliderVolume", "sliderDucking", "chkMuteKinect"});
  xui::Element* group = scene->FindById("radgrpOutputLocation");
  xui::Element* last = scene->FindById("radbtnPlayBoth");
  if (!group || !last) {
    return;
  }
  const int display = std::max(1, host_.display_scale);
  struct Choice {
    std::string id;
    std::string text;
    int scale;
  };
  const std::vector<Choice> choices = {
      {"radbtnPlayHeadset", "Original", 1},
      {"radbtnPlayTV", "2x (Experimental)", 2},
      {"radbtnPlayBoth", "3x (Experimental)", 3},
      {"radbtn4K", fmt::format("Match Display, {}x (Experimental)", display), 0},
  };
  xui::Element* clone = group->CloneChild(*last, choices.back().id);
  clone->Set("Position", xui::Value{xui::Vec3{0.0f, 3 * kRowHeight, 0.0f}});
  group->Set("Height", xui::Value{4 * kRowHeight});
  xui::Element* first = scene->FindById("sliderVolume");
  const xui::Vec3 at = first ? first->GetVector("Position") : xui::Vec3{0.0f, 72.0f, 0.0f};
  MoveTo(scene, "LabelSubHeader2", at.y - 6.0f);
  MoveTo(scene, "radgrpOutputLocation", at.y + 22.0f);
  SetText(scene, "LabelSubHeader2", "Render Resolution");
  SetText(scene, "LabelSubHeader1",
          "Original draws the game at its own resolution, as the console does, and scales it "
          "to your screen. 2x and 3x are sharper but much slower; Match Display picks the "
          "multiple for your screen.\r\n\r\nThe new resolution is used the next time you start "
          "the game.");
  xui::Element* chosen = nullptr;
  for (size_t i = 0; i < choices.size(); ++i) {
    xui::Element* button = group->FindById(choices[i].id);
    button->SetText(choices[i].text);
    SetNav(button, i > 0 ? choices[i - 1].id : "", i + 1 < choices.size() ? choices[i + 1].id : "");
    const bool current =
        choices[i].scale == 0
            ? REXCVAR_GET(resolution_match_display)
            : !REXCVAR_GET(resolution_match_display) &&
                  rex::cvar::Query<int32_t>("resolution_scale") == choices[i].scale;
    if (current) {
      chosen = button;
    }
  }
  CheckOnly(group, chosen);
  pages_.back().on_select = [this, group, choices](xui::Element* control) {
    for (const Choice& choice : choices) {
      if (control->id() != choice.id) {
        continue;
      }
      rex::cvar::SetFlagByName("resolution_match_display", choice.scale == 0 ? "true" : "false");
      if (choice.scale != 0) {
        rex::cvar::SetFlagByName("resolution_scale", std::to_string(choice.scale));
      }
      CheckOnly(group, control);
      if (host_.save_settings) {
        host_.save_settings();
      }
    }
  };
  SetFocus(chosen ? chosen : group->FindById(choices.front().id), true);
}

void XboxGuide::OpenAudioOutput() {
  xui::Element* scene = PushPage(assets_->options_voice, "Audio Output").scene;
  struct Choice {
    std::string text;
    std::string device;
    std::string details;
  };
  const std::string current = REXCVAR_GET(audio_output_device);
  const std::vector<audio::AudioOutput> outputs = host_.audio_outputs();
  std::string default_name;
  for (const audio::AudioOutput& output : outputs) {
    if (output.is_default) {
      default_name = output.name;
    }
  }
  const audio::AudioOutput* default_output = nullptr;
  for (const audio::AudioOutput& output : outputs) {
    if (output.is_default) {
      default_output = &output;
    }
  }
  std::vector<Choice> choices = {
      {"Windows Default", "",
       fmt::format("Windows Default follows the output chosen in Windows' sound settings{}.{}",
                   default_name.empty() ? "" : ", now " + default_name,
                   default_output ? "\r\n\r\n" + DescribeAudioOutput(*default_output) : "")}};
  const std::vector<std::string> names = OutputRowNames(outputs);
  bool current_connected = current.empty();
  for (size_t i = 0; i < outputs.size(); ++i) {
    current_connected = current_connected || outputs[i].id == current;
    if (choices.size() < kMaxChoiceRows) {
      choices.push_back(
          {names[i], outputs[i].id,
           fmt::format("{}\r\n\r\n{}", outputs[i].name.empty() ? names[i] : outputs[i].name,
                       DescribeAudioOutput(outputs[i]))});
    }
  }
  if (!current_connected) {
    choices.push_back({"Last Chosen (Not Connected)", current,
                       "The output chosen last time isn't connected, so the game plays through "
                       "the Windows default until it is."});
  }

  std::vector<std::string> texts;
  for (const Choice& choice : choices) {
    texts.push_back(choice.text);
  }
  const std::vector<xui::Element*> rows = FillPlayThroughList(scene, texts, "Play Through");
  if (rows.empty()) {
    return;
  }
  xui::Element* group = rows.front()->parent();

  auto chosen = std::find_if(choices.begin(), choices.end(),
                             [&](const Choice& choice) { return choice.device == current; });
  xui::Element* checked = rows[size_t(chosen - choices.begin())];
  CheckOnly(group, checked);
  pages_.back().on_focus = [this, scene, rows, choices] {
    const auto row = std::find(rows.begin(), rows.end(), focus_);
    if (row != rows.end()) {
      SetText(scene, "LabelSubHeader1", choices[size_t(row - rows.begin())].details);
    }
  };
  pages_.back().on_select = [this, group, rows, choices](xui::Element* control) {
    const auto row = std::find(rows.begin(), rows.end(), control);
    if (row == rows.end()) {
      return;
    }
    const Choice& choice = choices[size_t(row - rows.begin())];
    REXLOG_INFO("Xbox guide: audio output {}",
                choice.device.empty() ? "Windows default" : choice.text);
    rex::cvar::SetFlagByName("audio_output_device", choice.device);
    CheckOnly(group, control);
    if (host_.save_settings) {
      host_.save_settings();
    }
  };
  SetFocus(checked, true);
  pages_.back().on_focus();
}

void XboxGuide::OpenDisplay() {
  xui::Element* scene = PushPage(assets_->options_voice, "Display").scene;
  const std::vector<DisplayInfo> displays = host_.displays();
  std::vector<std::string> texts;
  for (const DisplayInfo& display : displays) {
    if (texts.size() < kMaxChoiceRows) {
      texts.push_back(display.name);
    }
  }
  const std::vector<xui::Element*> rows = FillPlayThroughList(scene, texts, "Play On");
  if (rows.empty()) {
    SetText(scene, "LabelSubHeader1", "Windows lists no displays.");
    return;
  }
  xui::Element* group = rows.front()->parent();
  const int32_t chosen_index = REXCVAR_GET(monitor);
  size_t chosen = 0;
  for (size_t i = 0; i < rows.size(); ++i) {
    if (chosen_index > 0 ? displays[i].monitor_index == chosen_index : displays[i].shows_game) {
      chosen = i;
    }
  }
  CheckOnly(group, rows[chosen]);
  pages_.back().on_focus = [this, scene, rows, displays] {
    const auto row = std::find(rows.begin(), rows.end(), focus_);
    if (row == rows.end()) {
      return;
    }
    const DisplayInfo& display = displays[size_t(row - rows.begin())];
    SetText(scene, "LabelSubHeader1",
            fmt::format("{}\r\n{}", display.name, DescribeDisplay(display)));
  };
  pages_.back().on_select = [this, group, rows, displays](xui::Element* control) {
    const auto row = std::find(rows.begin(), rows.end(), control);
    if (row == rows.end()) {
      return;
    }
    const DisplayInfo& display = displays[size_t(row - rows.begin())];
    REXLOG_INFO("Xbox guide: playing on display {} ({})", display.monitor_index, display.name);
    rex::cvar::SetFlagByName("monitor", std::to_string(display.monitor_index));
    CheckOnly(group, control);
    if (host_.save_settings) {
      host_.save_settings();
    }
  };
  SetFocus(rows[chosen], true);
  pages_.back().on_focus();
}

void XboxGuide::OpenXboxSettings() {
  xui::Element* scene = PushPage(assets_->xbox_settings, "").scene;
  SetText(scene, "labelHeadingOptions", "Xbox Settings");

  SetText(scene, "XuiLabel1",
          "The new setting is used the next time you start the game.\n"
          "Optimizing for graphics makes this game look better.\n"
          "Optimizing for performance can make play feel smoother.");
  xui::Element* group = scene->FindById("radgrpSettings");
  xui::Element* graphics = scene->FindById("radbtnGraphics");
  xui::Element* performance = scene->FindById("radbtnPerformance");
  if (!group || !graphics || !performance) {
    return;
  }
  SetNav(graphics, "", "radbtnPerformance");
  SetNav(performance, "radbtnGraphics", "");
  const bool match = REXCVAR_GET(resolution_match_display);
  CheckOnly(group, match                                                ? graphics
                   : rex::cvar::Query<int32_t>("resolution_scale") == 1 ? performance
                                                                        : nullptr);
  pages_.back().on_select = [this, group, graphics](xui::Element* control) {
    if (!control->IsA("XuiRadioButton")) {
      return;
    }
    rex::cvar::SetFlagByName("resolution_match_display", control == graphics ? "true" : "false");
    if (control != graphics) {
      rex::cvar::SetFlagByName("resolution_scale", "1");
    }
    CheckOnly(group, control);
    if (host_.save_settings) {
      host_.save_settings();
    }
  };
  SetFocus(match ? graphics : performance, true);
}

void XboxGuide::OpenPatches(std::string_view category) {
  const bool mods = category == "mod";
  xui::Element* scene = PushPage(assets_->options_notifications, mods ? "Mods" : "Patches").scene;
  xui::Element* model = scene->FindById("chkShow");
  std::vector<const PPCSwitchablePatch*> patches;
  for (const PPCSwitchablePatch* p = host_.patches; p && p->name; ++p) {
    if (p->category && category == p->category) {
      patches.push_back(p);
    }
  }

  std::vector<xui::Element*> boxes;
  const float top = model ? model->GetVector("Position").y : 62.0f;
  const size_t rows = std::min(patches.size(), size_t(12));
  for (size_t i = 0; model && i < rows; ++i) {
    xui::Element* box = scene->CloneChild(*model, fmt::format("chkPatch{}", i));
    xui::Vec3 p = model->GetVector("Position");
    p.y = top + float(i) * kRowHeight;
    box->Set("Position", xui::Value{p});
    box->SetText(patches[i]->name);
    SetNav(box, i > 0 ? fmt::format("chkPatch{}", i - 1) : "",
           i + 1 < rows ? fmt::format("chkPatch{}", i + 1) : "");
    box->SetChecked(__atomic_load_n(patches[i]->active, __ATOMIC_RELAXED) != 0);
    boxes.push_back(box);
  }
  Hide(scene,
       {"chkShow", "chkSound", "chkShowMovies", "chkShowIPTV", "XuiLabel2", "labelSoundDisabled"});
  if (boxes.empty()) {
    SetText(
        scene, "XuiLabel1",
        mods ? "No mods are available for this game." : "No patches are available for this game.");
    SetLegends("", scene->GetString("LegendB"), "");
    return;
  }
  SetText(scene, "XuiLabel1",
          mods ? "Turn mods on or off. They take effect straight away."
               : "Turn patches on or off, such as the frame rate. They take effect straight "
                 "away.");
  pages_.back().on_select = [this, boxes, patches](xui::Element* control) {
    for (size_t i = 0; i < boxes.size(); ++i) {
      if (boxes[i] != control) {
        continue;
      }
      const bool on = !control->checked();
      __atomic_store_n(patches[i]->active, uint8_t(on ? 1 : 0), __ATOMIC_RELAXED);
      control->SetChecked(on);
      rex::cvar::SetFlagByName("code_patch_states", SaveCodePatchStates(host_.patches));
      if (host_.save_settings) {
        host_.save_settings();
      }
    }
  };
  SetFocus(boxes.front(), true);
}

void XboxGuide::OpenCheats() {
  xui::Element* scene = PushPage(assets_->options_notifications, "Cheats").scene;
  xui::Element* model = scene->FindById("chkShow");
  std::vector<const PPCTitleCheat*> cheats;
  for (const PPCTitleCheat* c = host_.cheats; c && c->name; ++c) {
    cheats.push_back(c);
  }
  std::vector<xui::Element*> rows;
  const float top = model ? model->GetVector("Position").y : 62.0f;
  const size_t count = std::min(cheats.size(), size_t(12));
  for (size_t i = 0; model && i < count; ++i) {
    xui::Element* row = scene->CloneChild(*model, fmt::format("btnCheat{}", i));
    xui::Vec3 p = model->GetVector("Position");
    p.y = top + float(i) * kRowHeight;
    row->Set("Position", xui::Value{p});
    row->SetText(cheats[i]->name);
    SetNav(row, i > 0 ? fmt::format("btnCheat{}", i - 1) : "",
           i + 1 < count ? fmt::format("btnCheat{}", i + 1) : "");
    Hide(row, {"CheckboxRule", "Checkbox", "XuiImage"});
    rows.push_back(row);
  }
  Hide(scene,
       {"chkShow", "chkSound", "chkShowMovies", "chkShowIPTV", "XuiLabel2", "labelSoundDisabled"});
  SetLegends("", scene->GetString("LegendB"), "");
  if (rows.empty()) {
    SetText(scene, "XuiLabel1", "This game has no cheat codes.");
    return;
  }
  auto show = [this, scene, rows, cheats] {
    for (size_t i = 0; i < rows.size(); ++i) {
      if (rows[i] != focus_) {
        continue;
      }
      const PPCTitleCheat& cheat = *cheats[i];
      std::string text = fmt::format("Code: {}", cheat.code);
      if (cheat.description && *cheat.description) {
        text += fmt::format("\r\n\r\n{}", cheat.description);
      }
      if (cheat.where && *cheat.where) {
        text += fmt::format("\r\n\r\nEnter it in the game at {}.", cheat.where);
      }
      SetText(scene, "XuiLabel1", std::move(text));
    }
  };
  pages_.back().on_focus = show;
  SetFocus(rows.front(), true);
  show();
}

}
