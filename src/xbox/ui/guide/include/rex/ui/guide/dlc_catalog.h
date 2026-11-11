/**
 * @file        ui/guide/include/rex/ui/guide/dlc_catalog.h
 * @brief       A title's downloadable content catalogue, built into it (RG-GDK-050)
 *
 * `rexglue dlc-catalog` fetches the entries a title's config lists from the
 * Xbox 360 marketplace catalogue at build time, in the builder's language,
 * with their banner and tile art. rexglue_configure_target embeds the result,
 * so Games & Apps > Manage Game lists the title's add-ons offline.
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace rex::ui::guide {

struct DlcCatalogEntry {
  std::string id;
  std::string title;
  std::string publisher;
  std::string developer;
  std::string description;
  std::string release_date;
  std::vector<uint8_t> banner;
  std::vector<uint8_t> tile;
};

std::vector<uint8_t> WriteDlcCatalog(const std::vector<DlcCatalogEntry>& entries);
std::optional<std::vector<DlcCatalogEntry>> ReadDlcCatalog(std::span<const uint8_t> data);

bool RegisterEmbeddedDlcCatalog(const uint8_t* data, size_t size);

std::span<const uint8_t> EmbeddedDlcCatalog();

const std::vector<DlcCatalogEntry>& EmbeddedDlcEntries();

const DlcCatalogEntry* FindEmbeddedDlc(std::string_view id);

}
