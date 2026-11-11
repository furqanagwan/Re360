/**
 * @file        ui/guide/tests/guide_font_test.cpp
 * @brief       The guide's font covers more than Latin (RG-GDK-061)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <catch2/catch_test_macros.hpp>

#include <imgui.h>

#include <rex/ui/guide/xbox_guide.h>

TEST_CASE("The guide's font takes Greek, Cyrillic and Latin Extended-B", "[guide][font]") {
  ImGuiContext* context = ImGui::CreateContext();
  ImFontAtlas* atlas = ImGui::GetIO().Fonts;
  const rex::ui::guide::GuideFonts fonts = rex::ui::guide::AddGuideFonts(atlas, 1080);
  if (!fonts.regular) {
    ImGui::DestroyContext(context);
    SKIP("No guide font on this PC (no bundle, no Segoe UI)");
  }
  unsigned char* pixels = nullptr;
  int width = 0, height = 0;
  atlas->GetTexDataAsRGBA32(&pixels, &width, &height);
  ImFontBaked* baked = fonts.regular->GetFontBaked(fonts.regular->LegacySize);
  REQUIRE(baked);
  CHECK(baked->IsGlyphLoaded(ImWchar('A')));
  CHECK(baked->IsGlyphLoaded(ImWchar(0x00E9)));
  CHECK(baked->IsGlyphLoaded(ImWchar(0x0218)));
  CHECK(baked->IsGlyphLoaded(ImWchar(0x03A9)));
  CHECK(baked->IsGlyphLoaded(ImWchar(0x0416)));

  CHECK_FALSE(baked->IsGlyphLoaded(ImWchar(0x0531)));
  ImGui::DestroyContext(context);
}
