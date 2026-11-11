// Copyright (c) 2026 Xbox Guide contributors. BSD 3-Clause License; see LICENSE.
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <rex/ui/guide/guide_list_page.h>
#include <rex/ui/guide/message_box.h>
#include <rex/ui/guide/xbox_guide.h>
#include <rex/ui/xui/system_update.h>

TEST_CASE("Both private Guide skins host source and recovery message boxes",
          "[guide][message_box][local]") {
  const bool original_xbox = GENERATE(false, true);
  const auto presentation = original_xbox ? rex::ui::guide::GuidePresentation::OriginalXbox
                                          : rex::ui::guide::GuidePresentation::Xbox360;

  const char* bundle = original_xbox ? nullptr : std::getenv("REXGLUE_GUIDE_BUNDLE");
  const char* asset_variable = original_xbox ? "REXGLUE_GUIDE_FLASH" : "REXGLUE_SYSTEM_UPDATE";
  const char* path = std::getenv(asset_variable);
  INFO(asset_variable);
  if ((!path || !*path) && (!bundle || !*bundle))
    SKIP("Selected private Guide assets are not set");
  std::string error;
  std::unique_ptr<rex::ui::guide::GuideAssets> assets;
  if (bundle && *bundle) {
    std::ifstream file(std::filesystem::path(bundle), std::ios::binary);
    const std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), {});
    assets = rex::ui::guide::GuideAssets::LoadBundle(data, &error, presentation);
  } else {
    auto modules = rex::ui::xui::SystemUpdate::ReadModules(std::filesystem::path(path), &error);
    REQUIRE(modules);
    assets = rex::ui::guide::GuideAssets::FromUpdate(
        rex::ui::xui::SystemUpdate::FromModules(*modules, &error), &error, presentation);
  }
  INFO(error);
  REQUIRE(assets);
  rex::ui::xui::SceneContext context;
  context.skin = &assets->skin;
  context.package = "huduiskin";
  const std::string choices[] = {"Choose a disc image (ISO)", "Read from disc drive",
                                 "Choose an extracted folder"};
  auto source =
      rex::ui::guide::MessageBoxScene::Create(context, "Game files", "Choose your source", choices);
  REQUIRE(source);
  for (int i = 0; i < 3; ++i) {
    CHECK(source->Activate() == size_t(i));
    source->Move(1);
  }
  const std::string retry[] = {"Retry", "Leave Game"};
  auto recovery = rex::ui::guide::MessageBoxScene::Create(context, "Game source unavailable",
                                                          "Reinsert the disc", retry, 1);
  REQUIRE(recovery);
  CHECK(recovery->Activate() == 1);
  CHECK_FALSE(recovery->root().FindById("Button2")->visible());
  auto downloads =
      rex::ui::guide::ActiveDownloadsScene::Create(assets->options_notifications, context);
  REQUIRE(downloads);
  downloads->Update({});
  CHECK(downloads->size() == 0);

  for (const char* id :
       {"chkShow", "chkSound", "chkShowMovies", "chkShowIPTV", "labelSoundDisabled", "XuiLabel2"}) {
    if (auto* control = downloads->root().FindById(id))
      CHECK_FALSE(control->visible());
  }
  CHECK(downloads->root().FindById("XuiLabel1")->text() == "Nothing is downloading.");
  int cancelled = 0;
  downloads->Update({{"Game source extraction", "50%", "Copying", [&] { ++cancelled; }}});
  REQUIRE(downloads->size() == 1);
  CHECK(downloads->row(0)->secondary_text() == "50%");
  downloads->Activate();
  CHECK(cancelled == 1);
  downloads->Update({{"Game source extraction", "Completed", "Copied to PC", {}}});
  downloads->Activate();
  CHECK(cancelled == 1);

  rex::ui::xui::SceneContext frame_context = context, options_context = context;
  frame_context.package = "xam/xam";
  options_context.package = "hud/hud";
  auto page = rex::ui::guide::GuideListPage::Create(assets->backdrop, frame_context,
                                                    assets->options_notifications, options_context);
  REQUIRE(page);
  page->Show("Choose game ISO", {}, 0, "Select", "Back", "No ISO files here.");
  CHECK_FALSE(page->Activate());
  CHECK(page->slot(0) == nullptr);
  CHECK(page->scene().FindById("XuiLabel1")->text() == "No ISO files here.");
  std::vector<rex::ui::guide::GuideListRow> rows;
  for (int i = 0; i < 30; ++i)
    rows.push_back({"Game " + std::to_string(i), "ISO", "D:\\Games\\Game " + std::to_string(i)});
  page->Show("Choose game ISO", rows);
  CHECK(page->slot(0)->text() == "Game 0");
  CHECK(page->slot(rex::ui::guide::GuideListPage::kVisibleRows - 1) != nullptr);
  for (size_t i = 0; i < rex::ui::guide::GuideListPage::kVisibleRows; ++i)
    page->Move(1);
  CHECK(page->focused() == rex::ui::guide::GuideListPage::kVisibleRows);
  CHECK(page->first_visible() == 1);
  CHECK(page->slot(0)->text() == "Game 1");
  CHECK(page->scene().FindById("XuiLabel1")->text() == "D:\\Games\\Game 11");
  page->Move(-1);
  CHECK(page->first_visible() == 1);
  CHECK(page->Activate() == size_t(10));
  page->Show("Choose game ISO", rows, 29);
  CHECK(page->first_visible() == 30 - rex::ui::guide::GuideListPage::kVisibleRows);
  page->Move(1);
  CHECK(page->focused() == 29);
}
