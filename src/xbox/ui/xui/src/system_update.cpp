/**
 * @file        ui/xui/src/system_update.cpp
 * @brief       The dashboard XUI packages, read from the user's own system update (RG-GDK-041)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <rex/ui/xui/system_update.h>

#include <cstring>
#include <fstream>

#include <fmt/format.h>

#include <rex/filesystem.h>
#include <rex/filesystem/devices/stfs_container_device.h>
#include <rex/filesystem/entry.h>
#include <rex/filesystem/file.h>
#include <rex/system/lzx.h>
#include <rex/ui/xui/xtt_font.h>

namespace rex::ui::xui {
namespace {

constexpr uint32_t kXex2Magic = 0x58455832;
constexpr uint32_t kResourceInfoKey = 0x000002FF;
constexpr uint32_t kFileFormatKey = 0x000003FF;
constexpr uint32_t kXuizMagic = 0x5855495A;

uint32_t Be32(std::span<const uint8_t> b, size_t at) {
  const uint8_t* p = b.data() + at;
  return uint32_t(p[0]) << 24 | uint32_t(p[1]) << 16 | uint32_t(p[2]) << 8 | p[3];
}

uint16_t Be16(std::span<const uint8_t> b, size_t at) {
  return uint16_t(b[at] << 8 | b[at + 1]);
}

std::optional<std::vector<uint8_t>> ReadHostFile(const std::filesystem::path& path) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    return std::nullopt;
  }
  return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});
}

std::optional<std::vector<uint8_t>> ReadStfsFile(rex::filesystem::StfsContainerDevice& device,
                                                 std::string_view name) {
  rex::filesystem::Entry* entry = device.ResolvePath(name);
  if (!entry) {
    return std::nullopt;
  }
  rex::filesystem::File* file = nullptr;
  if (entry->Open(rex::filesystem::FileAccess::kGenericRead, &file) != X_STATUS_SUCCESS || !file) {
    return std::nullopt;
  }
  std::vector<uint8_t> bytes(entry->size());
  size_t read = 0;
  const X_STATUS status = file->ReadSync(bytes, 0, &read);
  file->Destroy();
  if (status != X_STATUS_SUCCESS || read != bytes.size()) {
    return std::nullopt;
  }
  return bytes;
}

}

std::optional<std::vector<XexResource>> ReadXexResources(std::span<const uint8_t> xex,
                                                         std::string* error) {
  auto fail = [&](std::string message) -> std::optional<std::vector<XexResource>> {
    if (error) {
      *error = std::move(message);
    }
    return std::nullopt;
  };
  if (xex.size() < 0x18 || Be32(xex, 0) != kXex2Magic) {
    return fail("not an XEX2 image");
  }
  const uint32_t header_size = Be32(xex, 0x08);
  const uint32_t security_offset = Be32(xex, 0x10);
  const uint32_t header_count = Be32(xex, 0x14);
  if (header_size > xex.size() || uint64_t(security_offset) + 0x114 > xex.size() ||
      0x18 + uint64_t(header_count) * 8 > header_size) {
    return fail("XEX2 header is truncated");
  }
  const uint32_t image_size = Be32(xex, security_offset + 0x04);
  const uint32_t load_address = Be32(xex, security_offset + 0x110);
  uint32_t resource_info = 0, file_format = 0;
  for (uint32_t i = 0; i < header_count; ++i) {
    const uint32_t key = Be32(xex, 0x18 + i * 8);
    const uint32_t value = Be32(xex, 0x1C + i * 8);
    if (key == kResourceInfoKey) {
      resource_info = value;
    } else if (key == kFileFormatKey) {
      file_format = value;
    }
  }
  if (!file_format || uint64_t(file_format) + 8 > header_size) {
    return fail("XEX2 has no file format header");
  }
  const uint32_t format_size = Be32(xex, file_format);
  const uint16_t encryption = Be16(xex, file_format + 4);
  const uint16_t compression = Be16(xex, file_format + 6);
  if (encryption != 0) {
    return fail("XEX2 is encrypted; system update XEXs are not, so this is another file");
  }
  if (uint64_t(file_format) + format_size > header_size || image_size > (256u << 20)) {
    return fail("XEX2 file format header is not valid");
  }

  std::vector<uint8_t> image(image_size);
  std::span<const uint8_t> body = xex.subspan(header_size);
  switch (compression) {
    case 0:
      std::memcpy(image.data(), body.data(), std::min<size_t>(body.size(), image.size()));
      break;
    case 1: {
      size_t in = 0, out = 0;
      for (uint32_t at = file_format + 8; at + 8 <= file_format + format_size; at += 8) {
        const uint32_t data_size = Be32(xex, at);
        const uint32_t zero_size = Be32(xex, at + 4);
        if (in + data_size > body.size() || out + uint64_t(data_size) + zero_size > image.size()) {
          return fail("XEX2 basic compression block runs past the end");
        }
        std::memcpy(image.data() + out, body.data() + in, data_size);
        in += data_size;
        out += size_t(data_size) + zero_size;
      }
      break;
    }
    case 2: {
      if (format_size < 8 + 4 + 20) {
        return fail("XEX2 LZX header is truncated");
      }
      const uint32_t window_size = Be32(xex, file_format + 8);
      uint32_t block_size = Be32(xex, file_format + 12);
      std::vector<uint8_t> compressed;
      compressed.reserve(body.size());
      size_t block = 0;
      while (block_size) {
        if (block + block_size > body.size() || block_size < 24) {
          return fail("XEX2 LZX block runs past the end");
        }
        const uint32_t next_size = Be32(body, block);
        size_t at = block + 24;
        const size_t block_end = block + block_size;
        while (at + 2 <= block_end) {
          const uint16_t chunk = Be16(body, at);
          at += 2;
          if (!chunk) {
            break;
          }
          if (at + chunk > block_end) {
            return fail("XEX2 LZX chunk runs past its block");
          }
          compressed.insert(compressed.end(), body.begin() + at, body.begin() + at + chunk);
          at += chunk;
        }
        block = block_end;
        block_size = next_size;
      }
      if (lzx_decompress(compressed.data(), compressed.size(), image.data(), image.size(),
                         window_size, nullptr, 0) != 0) {
        return fail("XEX2 LZX data did not decompress");
      }
      break;
    }
    default:
      return fail(fmt::format("XEX2 compression {} is not supported", compression));
  }

  std::vector<XexResource> resources;
  if (!resource_info) {
    return resources;
  }
  if (uint64_t(resource_info) + 4 > header_size) {
    return fail("XEX2 resource header is truncated");
  }
  const uint32_t info_size = Be32(xex, resource_info);
  if (info_size < 4 || uint64_t(resource_info) + info_size > header_size) {
    return fail("XEX2 resource header is not valid");
  }

  for (uint32_t at = resource_info + 4; at + 16 <= resource_info + info_size; at += 16) {
    XexResource resource;
    const char* name = reinterpret_cast<const char*>(xex.data() + at);
    resource.name.assign(name, strnlen(name, 8));
    const uint32_t address = Be32(xex, at + 8);
    const uint32_t size = Be32(xex, at + 12);
    if (address < load_address || uint64_t(address - load_address) + size > image.size()) {
      return fail(fmt::format("XEX2 resource {} is outside the image", resource.name));
    }
    resource.bytes.assign(image.begin() + (address - load_address),
                          image.begin() + (address - load_address) + size);
    resources.push_back(std::move(resource));
  }
  return resources;
}

bool SystemUpdate::AddModule(std::string_view module, std::span<const uint8_t> xex,
                             std::string* error) {
  std::string xex_error;
  auto resources = ReadXexResources(xex, &xex_error);
  if (!resources) {
    if (error) {
      *error = fmt::format("{}: {}", module, xex_error);
    }
    return false;
  }
  for (XexResource& resource : *resources) {
    if (resource.bytes.size() < 4 || Be32(resource.bytes, 0) != kXuizMagic) {
      continue;
    }
    std::string key = fmt::format("{}/{}", module, resource.name);
    std::string package_error;
    auto package = Package::Parse(std::move(resource.bytes), &package_error);
    if (!package) {
      if (error) {
        *error = fmt::format("{}: {}", key, package_error);
      }
      return false;
    }
    packages_.insert_or_assign(std::move(key), std::move(*package));
  }
  return true;
}

bool SystemUpdate::AddFont(std::string_view name, std::span<const uint8_t> xtt,
                           std::string* error) {
  std::string font_error;
  auto font = XttToTrueType(xtt, &font_error);
  if (!font) {
    if (error) {
      *error = fmt::format("font/{}: {}", name, font_error);
    }
    return false;
  }
  fonts_.insert_or_assign(std::string(name), std::move(*font));
  return true;
}

std::span<const uint8_t> SystemUpdate::Font(std::string_view name) const {
  for (const auto& [key, font] : fonts_) {
    if (SamePath(key, name)) {
      return font;
    }
  }
  return {};
}

const Package* SystemUpdate::Find(std::string_view module_resource) const {
  for (const auto& [key, package] : packages_) {
    if (SamePath(key, module_resource)) {
      return &package;
    }
  }
  return nullptr;
}

std::unique_ptr<SystemUpdate> SystemUpdate::Load(const std::filesystem::path& path,
                                                 std::string* error) {
  auto modules = ReadModules(path, error);
  return modules ? FromModules(*modules, error) : nullptr;
}

std::unique_ptr<SystemUpdate> SystemUpdate::FromModules(const Modules& modules,
                                                        std::string* error) {
  auto update = std::make_unique<SystemUpdate>();
  std::string first_error;
  for (const auto& [module, bytes] : modules) {
    std::string add_error;

    const bool added = module.starts_with("font/")
                           ? update->AddFont(module.substr(5), bytes, &add_error)
                           : update->AddModule(module, bytes, &add_error);
    if (!added && first_error.empty()) {
      first_error = add_error;
    }
  }
  for (std::string_view required : {"hud/hud", "huduiskin/skin", "xam/skin", "xam/shrdres"}) {
    if (!update->Find(required)) {
      if (error) {
        *error = fmt::format("the system update has no {} package{}", required,
                             first_error.empty() ? "" : fmt::format(" ({})", first_error));
      }
      return nullptr;
    }
  }
  return update;
}

namespace {
constexpr uint32_t kBundleMagic = 0x55475852;
constexpr uint32_t kBundleVersion = 1;

void PutU32(std::vector<uint8_t>& out, uint32_t v) {
  for (int i = 0; i < 4; ++i) {
    out.push_back(uint8_t(v >> (i * 8)));
  }
}
}

std::vector<uint8_t> SystemUpdate::WriteBundle(const Modules& modules) {
  std::vector<uint8_t> out;
  PutU32(out, kBundleMagic);
  PutU32(out, kBundleVersion);
  PutU32(out, uint32_t(modules.size()));
  for (const auto& [module, bytes] : modules) {
    PutU32(out, uint32_t(module.size()));
    out.insert(out.end(), module.begin(), module.end());
    PutU32(out, uint32_t(bytes.size()));
    out.insert(out.end(), bytes.begin(), bytes.end());
  }
  return out;
}

std::optional<SystemUpdate::Modules> SystemUpdate::ReadBundle(std::span<const uint8_t> bundle,
                                                              std::string* error) {
  size_t at = 0;
  bool ok = true;
  auto u32 = [&]() -> uint32_t {
    if (at + 4 > bundle.size()) {
      ok = false;
      return 0;
    }
    uint32_t v = 0;
    for (int i = 0; i < 4; ++i) {
      v |= uint32_t(bundle[at + i]) << (i * 8);
    }
    at += 4;
    return v;
  };
  auto bytes = [&](uint32_t size) -> std::span<const uint8_t> {
    if (!ok || size > bundle.size() - at) {
      ok = false;
      return {};
    }
    auto s = bundle.subspan(at, size);
    at += size;
    return s;
  };
  if (u32() != kBundleMagic || u32() != kBundleVersion) {
    if (error) {
      *error = "not a guide bundle (or from another SDK version)";
    }
    return std::nullopt;
  }
  Modules modules;
  const uint32_t count = u32();
  for (uint32_t i = 0; ok && i < count; ++i) {
    auto name = bytes(u32());
    auto data = bytes(u32());
    if (ok) {
      modules.emplace(std::string(name.begin(), name.end()),
                      std::vector<uint8_t>(data.begin(), data.end()));
    }
  }
  if (!ok) {
    if (error) {
      *error = "the guide bundle is truncated";
    }
    return std::nullopt;
  }
  return modules;
}

std::optional<SystemUpdate::Modules> SystemUpdate::ReadModules(const std::filesystem::path& path,
                                                               std::string* error) {
  auto fail = [&](std::string message) -> std::optional<Modules> {
    if (error) {
      *error = std::move(message);
    }
    return std::nullopt;
  };
  std::error_code ec;
  Modules modules;
  auto add = [&](std::string_view module, std::optional<std::vector<uint8_t>> bytes) {
    if (bytes) {
      modules.emplace(std::string(module), std::move(*bytes));
    }
  };

  auto read_flash = [&](std::string_view name) {
    auto bytes = ReadHostFile(path / fmt::format("$flash_{}", name));
    return bytes ? bytes : ReadHostFile(path / name);
  };
  auto add_fonts = [&](auto&& read) {
    for (std::string_view font : kFonts) {
      add(fmt::format("font/{}", font), read(fmt::format("{}.xtt", font)));
    }
  };

  std::filesystem::path package_path;
  if (std::filesystem::is_directory(path, ec)) {
    if (std::filesystem::is_regular_file(path / "$flash_hud.xex", ec) ||
        std::filesystem::is_regular_file(path / "hud.xex", ec)) {
      for (std::string_view module : kModules) {
        add(module, read_flash(fmt::format("{}.xex", module)));
      }
      add_fonts(read_flash);
    } else {
      add_fonts([&](const std::string& name) { return ReadHostFile(path / name); });

      for (const auto& item : std::filesystem::directory_iterator(path, ec)) {
        const std::string name = item.path().filename().string();
        if (item.is_regular_file(ec) && name.starts_with("su") && name.ends_with("_00000000")) {
          package_path = item.path();
          break;
        }
      }
      if (package_path.empty()) {
        return fail(fmt::format("{} holds no system update package (su*_00000000)", path.string()));
      }
    }
  } else if (std::filesystem::is_regular_file(path, ec)) {
    package_path = path;
  } else {
    return fail(fmt::format("{} does not exist", path.string()));
  }

  if (!package_path.empty()) {
    rex::filesystem::StfsContainerDevice device("\\SystemUpdate", package_path);
    if (!device.Initialize()) {
      return fail(fmt::format("{} is not a readable STFS package", package_path.string()));
    }
    for (std::string_view module : kModules) {
      add(module, ReadStfsFile(device, fmt::format("$flash_{}.xex", module)));
    }
    add_fonts([&](const std::string& name) {
      return ReadStfsFile(device, fmt::format("$flash_{}", name));
    });
  }

  if (modules.empty()) {
    return fail(fmt::format("{} holds none of the system modules", path.string()));
  }
  return modules;
}

std::optional<SystemUpdate::Modules> SystemUpdate::ReadModules(
    std::span<const std::filesystem::path> paths, std::string* error) {
  Modules modules;
  for (const std::filesystem::path& path : paths) {
    auto read = ReadModules(path, error);
    if (!read) {
      return std::nullopt;
    }
    modules.merge(*read);
  }
  if (modules.empty() && error) {
    *error = "no system update given";
  }
  return modules.empty() ? std::nullopt : std::optional<Modules>(std::move(modules));
}

}
