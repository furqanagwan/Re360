/**
 * @file        ui/xui/src/schema.cpp
 * @brief       XUI class and property order used to decode XUR v8 (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/xui/schema.h>

namespace rex::ui::xui {
namespace {

using T = PropType;

constexpr PropDef kXuiElement[] = {
    {"Id", T::kString},
    {"Width", T::kFloat},
    {"Height", T::kFloat},
    {"Position", T::kVector},
    {"Scale", T::kVector},
    {"Rotation", T::kQuaternion},
    {"Opacity", T::kFloat},
    {"Anchor", T::kUnsigned},
    {"Pivot", T::kVector},
    {"Show", T::kBool},
    {"BlendMode", T::kUnsigned},
    {"DisableTimelineRecursion", T::kBool},
    {"DesignTime", T::kBool},
    {"ColorWriteFlags", T::kUnsigned},
    {"ClipChildren", T::kBool},
    {"EnableEffects", T::kBool},
    {"DisableFocusRecursion", T::kBool},
    {"GripTarget", T::kBool},
    {"Hittable", T::kBool},
    {"LayoutLineBreak", T::kBool},
    {"LayoutFloat", T::kBool},
    {"Column", T::kUnsigned},
    {"Row", T::kUnsigned},
    {"ColumnSpan", T::kUnsigned},
    {"RowSpan", T::kUnsigned},
    {"ColorFactor", T::kColor},
    {"CenterPivot", T::kBool},
};

constexpr PropDef kFigureFill[] = {
    {"FillType", T::kUnsigned},
    {"FillColor", T::kColor},
    {"TextureFileName", T::kString},
    {"Gradient", T::kObject},
    {"Translation", T::kVector},
    {"Scale", T::kVector},
    {"Rotation", T::kFloat},
    {"WrapX", T::kUnsigned},
    {"WrapY", T::kUnsigned},
    {"BrushFlags", T::kUnsigned},
    {"TransformVersion", T::kUnsigned},
};

constexpr PropDef kFigureFillGradient[] = {
    {"Radial", T::kBool},
    {"NumStops", T::kInteger},
    {"StopColor", T::kColor, true},
    {"StopPos", T::kFloat, true},
};

constexpr PropDef kFigureStroke[] = {
    {"StrokeWidth", T::kFloat},
    {"StrokeColor", T::kColor},
};

constexpr PropDef kXuiControl[] = {
    {"ClassOverride", T::kString},  {"Visual", T::kString},       {"Enabled", T::kBool},
    {"UnfocusedInput", T::kBool},   {"NavLeft", T::kString},      {"NavRight", T::kString},
    {"NavUp", T::kString},          {"NavDown", T::kString},      {"NavTabForward", T::kString},
    {"NavTabBackward", T::kString}, {"Text", T::kString},         {"PointSize", T::kFloat},
    {"ImagePath", T::kString},      {"HasContextMenu", T::kBool}, {"SizeToText", T::kBool},
    {"UseNuiAsMouse", T::kBool},    {"AutoId", T::kString},       {"HoverSelectTimer", T::kFloat},
    {"QuickInput", T::kBool},
};

constexpr PropDef kXuiFigure[] = {
    {"Stroke", T::kObject},
    {"Fill", T::kObject},
    {"Closed", T::kBool},
    {"Points", T::kCustom},
};

constexpr PropDef kXuiGridPanel[] = {
    {"Columns", T::kString}, {"Rows", T::kString},  {"CellSpacing", T::kFloat},
    {"Param0", T::kFloat},   {"Param1", T::kFloat}, {"Param2", T::kFloat},
    {"Param3", T::kFloat},
};

constexpr PropDef kXuiImage[] = {
    {"SizeMode", T::kUnsigned},   {"ImagePath", T::kString},
    {"BrushFlags", T::kUnsigned}, {"TextureSurfaceElement", T::kString},
    {"LoadType", T::kUnsigned},
};

constexpr PropDef kXuiImagePresenter[] = {
    {"SizeMode", T::kUnsigned},
    {"DataAssociation", T::kUnsigned},
    {"BrushFlags", T::kUnsigned},
    {"LoadType", T::kUnsigned},
};

constexpr PropDef kXuiNineGrid[] = {
    {"TextureFileName", T::kString}, {"LeftOffset", T::kUnsigned},   {"TopOffset", T::kUnsigned},
    {"RightOffset", T::kUnsigned},   {"BottomOffset", T::kUnsigned}, {"NoCenter", T::kBool},
};

constexpr PropDef kXuiSound[] = {
    {"State", T::kUnsigned},
    {"Loop", T::kBool},
    {"Finish", T::kBool},
    {"Volume", T::kFloat},
};

constexpr PropDef kXuiText[] = {
    {"Text", T::kString},
    {"TextColor", T::kColor},
    {"DropShadowColor", T::kColor},
    {"PointSize", T::kFloat},
    {"Font", T::kString},
    {"TextStyle", T::kUnsigned},
    {"LineSpacingAdjust", T::kInteger},
    {"TextScale", T::kFloat},
};

constexpr PropDef kXuiTextPresenter[] = {
    {"TextColor", T::kColor},          {"DropShadowColor", T::kColor},
    {"PointSize", T::kFloat},          {"Font", T::kString},
    {"TextStyle", T::kUnsigned},       {"LineSpacingAdjust", T::kInteger},
    {"DataAssociation", T::kUnsigned}, {"TextScale", T::kFloat},
};

constexpr PropDef kXuiButton[] = {
    {"PressKey", T::kUnsigned},          {"PressAnimObject", T::kString},
    {"PressAnimStartFrame", T::kString}, {"PressAnimEndFrame", T::kString},
    {"FocusAnimObject", T::kString},     {"FocusAnimStartFrame", T::kString},
    {"FocusAnimEndFrame", T::kString},
};

constexpr PropDef kPressKeyOnly[] = {
    {"PressKey", T::kUnsigned},
};

constexpr PropDef kXuiEdit[] = {
    {"TextLimit", T::kInteger}, {"AllowedChars", T::kString}, {"PasswordChar", T::kString},
    {"ReadOnly", T::kBool},     {"Multiline", T::kBool},      {"SmoothScroll", T::kBool},
};

constexpr PropDef kXuiGamerCard[] = {
    {"Format", T::kString},
    {"ShowExtendedPanel", T::kBool},
};

constexpr PropDef kXuiLabel[] = {
    {"MaxFlowLines", T::kUnsigned},
};

constexpr PropDef kXuiList[] = {
    {"Wrap", T::kBool},
    {"WrapBump", T::kBool},
};

constexpr PropDef kXuiCommonList[] = {
    {"ItemsText", T::kString},
    {"ItemsImage", T::kString},
    {"ItemsNavPath", T::kString},
    {"ComboBoxStyle", T::kUnsigned},
};

constexpr PropDef kXuiProgressBar[] = {
    {"RangeMin", T::kInteger},
    {"RangeMax", T::kInteger},
    {"Value", T::kInteger},
};

constexpr PropDef kXuiScene[] = {
    {"DefaultFocus", T::kString}, {"TransFrom", T::kString},
    {"TransTo", T::kString},      {"TransBackFrom", T::kString},
    {"TransBackTo", T::kString},  {"InterruptTransitions", T::kUnsigned},
    {"IgnorePresses", T::kBool},  {"RecurseTransitions", T::kBool},
};

constexpr PropDef kXuiScrollEnd[] = {
    {"Direction", T::kUnsigned},
};

constexpr PropDef kXuiSlider[] = {
    {"RangeMin", T::kInteger},   {"RangeMax", T::kInteger}, {"Value", T::kInteger},
    {"Step", T::kInteger},       {"Vertical", T::kBool},    {"AccelInc", T::kInteger},
    {"AccelTime", T::kUnsigned},
};

constexpr PropDef kXuiSoundXAudio[] = {
    {"File", T::kString},
};

constexpr PropDef kHUDScene[] = {
    {"OpenType", T::kUnsigned}, {"LegendA", T::kString}, {"LegendB", T::kString},
    {"LegendX", T::kString},    {"LegendY", T::kString}, {"ShowGamerInfo", T::kBool},
};

constexpr PropDef kXuiListItem[] = {
    {"Layout", T::kUnsigned},
    {"Checkable", T::kBool},
    {"SelectedSize", T::kVector},
    {"KeepSizeUnfocused", T::kBool},
    {"InterItemSpacing", T::kVector},
    {"SmoothScroll", T::kBool},
    {"SmoothScrollBaseSpeed", T::kFloat},
    {"SmoothScrollMaxSpeed", T::kFloat},
    {"SmoothScrollAcceleration", T::kFloat},
};

constexpr PropDef kXuiNavButton[] = {
    {"PressPath", T::kString},
    {"StayVisible", T::kBool},
    {"SrcTransIndex", T::kUnsigned},
    {"DestTransIndex", T::kUnsigned},
};

constexpr PropDef kXuiTabScene[] = {
    {"TabCount", T::kUnsigned}, {"Wrap", T::kBool},       {"UserInterrupt", T::kBool},
    {"VerticalTabs", T::kBool}, {"NoAutoHide", T::kBool}, {"DefaultTab", T::kUnsigned},
};

constexpr PropDef kGuideDashCommandNavButton[] = {
    {"DashCommand", T::kInteger},
};

constexpr PropDef kGuideMainSceneNavButton[] = {
    {"MainScenePressPath", T::kString},
};

constexpr ClassDef kClasses[] = {
    {"XuiElement", "", kXuiElement},
    {"XuiCanvas", "XuiElement", {}},
    {"XuiGroup", "XuiElement", {}},
    {"XuiVisual", "XuiElement", {}},
    {"XuiFigure", "XuiElement", kXuiFigure},
    {"XuiGridPanel", "XuiElement", kXuiGridPanel},
    {"XuiImage", "XuiElement", kXuiImage},
    {"XuiImagePresenter", "XuiElement", kXuiImagePresenter},
    {"XuiNineGrid", "XuiElement", kXuiNineGrid},
    {"XuiSound", "XuiElement", kXuiSound},
    {"XuiSoundXAudio", "XuiSound", kXuiSoundXAudio},
    {"XuiText", "XuiElement", kXuiText},
    {"XuiTextPresenter", "XuiElement", kXuiTextPresenter},
    {"XuiControl", "XuiElement", kXuiControl},
    {"XuiButton", "XuiControl", kXuiButton},
    {"XuiBackButton", "XuiButton", {}},
    {"XuiNavButton", "XuiButton", kXuiNavButton},
    {"GuideDashCommandNavButton", "XuiNavButton", kGuideDashCommandNavButton},
    {"GuideMainSceneNavButton", "XuiNavButton", kGuideMainSceneNavButton},

    {"AccountManagementNavButton", "GuideDashCommandNavButton", {}},
    {"XuiCaret", "XuiControl", {}},
    {"XuiCheckbox", "XuiControl", kPressKeyOnly},
    {"XuiListItem", "XuiCheckbox", kXuiListItem},
    {"XuiRadioButton", "XuiControl", kPressKeyOnly},
    {"XuiRadioGroup", "XuiControl", {}},
    {"XuiEdit", "XuiControl", kXuiEdit},
    {"XuiGamerCard", "XuiControl", kXuiGamerCard},
    {"XuiLabel", "XuiControl", kXuiLabel},
    {"XuiList", "XuiControl", kXuiList},
    {"XuiCommonList", "XuiList", kXuiCommonList},
    {"XuiProgressBar", "XuiControl", kXuiProgressBar},
    {"XuiScrollEnd", "XuiControl", kXuiScrollEnd},
    {"XuiSlider", "XuiControl", kXuiSlider},
    {"XuiScene", "XuiControl", kXuiScene},
    {"HUDScene", "XuiScene", kHUDScene},
    {"XuiTabScene", "XuiScene", kXuiTabScene},

    {"XuiFigureFill", "", kFigureFill},
    {"XuiFigureFillGradient", "", kFigureFillGradient},
    {"XuiFigureStroke", "", kFigureStroke},
};

}

const ClassDef* FindClass(std::string_view name) {
  for (const ClassDef& cls : kClasses) {
    if (cls.name == name) {
      return &cls;
    }
  }
  return nullptr;
}

std::vector<const ClassDef*> ClassChain(const ClassDef* derived) {
  std::vector<const ClassDef*> chain;
  for (const ClassDef* cls = derived; cls;
       cls = cls->base.empty() ? nullptr : FindClass(cls->base)) {
    chain.insert(chain.begin(), cls);
  }
  return chain;
}

const ClassDef* CompoundClass(std::string_view property_name) {
  if (property_name == "Fill") {
    return FindClass("XuiFigureFill");
  }
  if (property_name == "Gradient") {
    return FindClass("XuiFigureFillGradient");
  }
  if (property_name == "Stroke") {
    return FindClass("XuiFigureStroke");
  }
  return nullptr;
}

}
