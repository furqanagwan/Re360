/**
 * @file        ui/guide/src/dlc_catalog.cpp
 * @brief       A title's downloadable content catalogue, built into it (RG-GDK-050)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/guide/dlc_catalog.h>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <string_view>

namespace rex::ui::guide {
namespace {

constexpr std::string_view kMagic = "RXDLC001";

constexpr uint32_t kMaxEntries = 4096;

void PutU32(std::vector<uint8_t>& out, uint32_t value) {
  for (int i = 0; i < 4; ++i) {
    out.push_back(uint8_t(value >> (8 * i)));
  }
}

template <typename Bytes>
void PutField(std::vector<uint8_t>& out, const Bytes& bytes) {
  PutU32(out, uint32_t(bytes.size()));
  out.insert(out.end(), bytes.begin(), bytes.end());
}

struct Reader {
  std::span<const uint8_t> data;
  size_t at = 0;

  bool U32(uint32_t* value) {
    if (data.size() - at < 4) {
      return false;
    }
    *value = 0;
    for (int i = 0; i < 4; ++i) {
      *value |= uint32_t(data[at + i]) << (8 * i);
    }
    at += 4;
    return true;
  }
  template <typename Bytes>
  bool Field(Bytes* out) {
    uint32_t size = 0;
    if (!U32(&size) || data.size() - at < size) {
      return false;
    }
    out->assign(data.begin() + at, data.begin() + at + size);
    at += size;
    return true;
  }
};

std::span<const uint8_t> g_embedded;

}

std::vector<uint8_t> WriteDlcCatalog(const std::vector<DlcCatalogEntry>& entries) {
  std::vector<uint8_t> out(kMagic.begin(), kMagic.end());
  PutU32(out, uint32_t(entries.size()));
  for (const DlcCatalogEntry& e : entries) {
    PutField(out, e.id);
    PutField(out, e.title);
    PutField(out, e.publisher);
    PutField(out, e.developer);
    PutField(out, e.description);
    PutField(out, e.release_date);
    PutField(out, e.banner);
    PutField(out, e.tile);
  }
  return out;
}

std::optional<std::vector<DlcCatalogEntry>> ReadDlcCatalog(std::span<const uint8_t> data) {
  if (data.size() < kMagic.size() || std::memcmp(data.data(), kMagic.data(), kMagic.size()) != 0) {
    return std::nullopt;
  }
  Reader r{data, kMagic.size()};
  uint32_t count = 0;
  if (!r.U32(&count) || count > kMaxEntries) {
    return std::nullopt;
  }
  std::vector<DlcCatalogEntry> entries(count);
  for (DlcCatalogEntry& e : entries) {
    if (!r.Field(&e.id) || !r.Field(&e.title) || !r.Field(&e.publisher) || !r.Field(&e.developer) ||
        !r.Field(&e.description) || !r.Field(&e.release_date) || !r.Field(&e.banner) ||
        !r.Field(&e.tile)) {
      return std::nullopt;
    }
  }
  return entries;
}

bool RegisterEmbeddedDlcCatalog(const uint8_t* data, size_t size) {
  g_embedded = {data, size};
  return true;
}

std::span<const uint8_t> EmbeddedDlcCatalog() {
  return g_embedded;
}

const std::vector<DlcCatalogEntry>& EmbeddedDlcEntries() {
  static const std::vector<DlcCatalogEntry> entries =
      ReadDlcCatalog(g_embedded).value_or(std::vector<DlcCatalogEntry>{});
  return entries;
}

const DlcCatalogEntry* FindEmbeddedDlc(std::string_view id) {
  for (const DlcCatalogEntry& entry : EmbeddedDlcEntries()) {
    if (entry.id.size() == id.size() &&
        std::equal(entry.id.begin(), entry.id.end(), id.begin(), [](char a, char b) {
          return std::toupper(uint8_t(a)) == std::toupper(uint8_t(b));
        })) {
      return &entry;
    }
  }
  return nullptr;
}

}
