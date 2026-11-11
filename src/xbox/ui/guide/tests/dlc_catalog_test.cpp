/**
 * @file        ui/guide/tests/dlc_catalog_test.cpp
 * @brief       The add-on catalogue built into a title (RG-GDK-050)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <vector>

#include <rex/ui/guide/dlc_catalog.h>

using rex::ui::guide::DlcCatalogEntry;
using rex::ui::guide::ReadDlcCatalog;
using rex::ui::guide::WriteDlcCatalog;

TEST_CASE("The add-on catalogue reads back what was written", "[guide][dlc]") {
  DlcCatalogEntry skyfall;
  skyfall.id = "D4C83E1F-243B-4A68-AD70-ABF3C0FBF372";
  skyfall.title = "SKYFALL Content Pack";
  skyfall.publisher = "Activision";
  skyfall.developer = "Eurocom";
  skyfall.description = "Take on the role of James Bond as played by Daniel Craig.";
  skyfall.release_date = "2012-11-20";
  skyfall.banner = {0x89, 'P', 'N', 'G', 0, 1, 2};
  skyfall.tile = {0x89, 'P', 'N', 'G'};
  DlcCatalogEntry ids_only;
  ids_only.id = "A492C3E9-105C-41DD-9CD8-4977A615655A";

  const std::vector<uint8_t> bytes = WriteDlcCatalog({skyfall, ids_only});
  const auto read = ReadDlcCatalog(bytes);
  REQUIRE(read);
  REQUIRE(read->size() == 2);
  const DlcCatalogEntry& a = (*read)[0];
  CHECK(a.id == skyfall.id);
  CHECK(a.title == skyfall.title);
  CHECK(a.publisher == skyfall.publisher);
  CHECK(a.developer == skyfall.developer);
  CHECK(a.description == skyfall.description);
  CHECK(a.release_date == skyfall.release_date);
  CHECK(a.banner == skyfall.banner);
  CHECK(a.tile == skyfall.tile);
  CHECK((*read)[1].id == ids_only.id);
  CHECK((*read)[1].title.empty());
  CHECK((*read)[1].banner.empty());

  CHECK(ReadDlcCatalog(WriteDlcCatalog({}))->empty());
}

TEST_CASE("A damaged add-on catalogue is refused", "[guide][dlc]") {
  DlcCatalogEntry entry;
  entry.id = "30621358-F254-48C1-B607-B6AA7DAACC33";
  entry.title = "Patrice Character Skin";
  std::vector<uint8_t> bytes = WriteDlcCatalog({entry});

  CHECK_FALSE(ReadDlcCatalog({}));
  std::vector<uint8_t> truncated(bytes.begin(), bytes.end() - 3);
  CHECK_FALSE(ReadDlcCatalog(truncated));
  std::vector<uint8_t> wrong_magic = bytes;
  wrong_magic[0] = 'X';
  CHECK_FALSE(ReadDlcCatalog(wrong_magic));
  std::vector<uint8_t> huge_count = bytes;
  huge_count[8] = 0xFF;
  huge_count[9] = 0xFF;
  huge_count[10] = 0xFF;
  huge_count[11] = 0x7F;
  CHECK_FALSE(ReadDlcCatalog(huge_count));
}
