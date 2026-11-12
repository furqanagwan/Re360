/**
 * @file        rexglue/commands/guide_bundle_command.cpp
 * @brief       rexglue guide-bundle: the Xbox guide's console files for embedding (RG-GDK-041)
 *
 * Reads the system modules the Xbox guide needs (hud, huduiskin, xam,
 * gamerprofile) and the console's fonts from the builder's own console system
 * update, or an Xbox PC backward-compatibility game's Flash folder ahead of
 * it, and writes them as one bundle. rexglue_configure_target runs this at build time and embeds
 * the bundle into the title, so players need nothing for the guide.
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include "guide_bundle_command.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <rex/logging.h>
#include <rex/ui/xui/system_update.h>

namespace rexglue::cli {

using rex::Err;
using rex::ErrorCategory;
using rex::Result;

namespace {

struct GuideBundleArgs {
  std::vector<std::string> sources;
  std::string output;
};

Result<void> WriteGuideBundle(const GuideBundleArgs& args) {
  using rex::ui::xui::SystemUpdate;
  std::string error;
  const std::vector<std::filesystem::path> sources(args.sources.begin(), args.sources.end());
  auto modules = SystemUpdate::ReadModules(sources, &error);

  std::unique_ptr<SystemUpdate> update;
  if (!modules || !(update = SystemUpdate::FromModules(*modules, &error))) {
    return Err<void>(ErrorCategory::Config,
                     fmt::format("{}: {}", fmt::join(args.sources, ", "), error));
  }
  for (std::string_view font : SystemUpdate::kFonts) {
    if (modules->contains(fmt::format("font/{}", font)) && update->Font(font).empty()) {
      REXLOG_WARN("XboxUI bundle: font {} does not convert; the guide falls back", font);
    }
  }
  const auto bundle = SystemUpdate::WriteBundle(*modules);
  const std::filesystem::path out(args.output);
  std::error_code ec;
  if (out.has_parent_path()) {
    std::filesystem::create_directories(out.parent_path(), ec);
  }

  const std::filesystem::path temp = out.string() + ".tmp";
  {
    std::ofstream file(temp, std::ios::binary | std::ios::trunc);
    file.write(reinterpret_cast<const char*>(bundle.data()), std::streamsize(bundle.size()));
    if (!file) {
      return Err<void>(ErrorCategory::IO, fmt::format("could not write {}", temp.string()));
    }
  }
  std::filesystem::rename(temp, out, ec);
  if (ec) {
    return Err<void>(ErrorCategory::IO,
                     fmt::format("could not write {}: {}", out.string(), ec.message()));
  }
  REXLOG_INFO("XboxUI bundle: {} modules, {} KiB -> {}", modules->size(), bundle.size() / 1024,
              out.string());
  return rex::Ok();
}

struct XuiDumpArgs {
  std::vector<std::string> sources;
  std::string output;
};

Result<void> DumpXui(const XuiDumpArgs& args) {
  using rex::ui::xui::SystemUpdate;
  std::string error;
  const std::vector<std::filesystem::path> sources(args.sources.begin(), args.sources.end());
  auto modules = SystemUpdate::ReadModules(sources, &error);
  std::unique_ptr<SystemUpdate> update;
  if (!modules || !(update = SystemUpdate::FromModules(*modules, &error))) {
    return Err<void>(ErrorCategory::Config,
                     fmt::format("{}: {}", fmt::join(args.sources, ", "), error));
  }
  size_t files = 0;
  for (const auto& [name, package] : update->packages()) {
    for (const auto& entry : package.entries()) {
      std::filesystem::path out = std::filesystem::path(args.output) / name;
      std::string relative = entry.name;
      std::replace(relative.begin(), relative.end(), '\\', '/');
      out /= relative;
      std::error_code ec;
      std::filesystem::create_directories(out.parent_path(), ec);
      const auto bytes = package.Find(entry.name);
      std::ofstream file(out, std::ios::binary | std::ios::trunc);
      file.write(reinterpret_cast<const char*>(bytes.data()), std::streamsize(bytes.size()));
      ++files;
    }
  }
  REXLOG_INFO("XUI dump: {} files from {} packages -> {}", files, update->packages().size(),
              args.output);
  return rex::Ok();
}

}

void RegisterGuideBundle(CLI::App& parent, const CliContext& ctx, DeferredAction& pending) {
  (void)ctx;
  auto args = std::make_shared<GuideBundleArgs>();
  auto* sub = parent.add_subcommand(
      "guide-bundle", "Bundle XboxUI's console files (the Xbox guide) for embedding in a title");
  sub->add_option("sources", args->sources,
                  "$SystemUpdate folder (dashboard 2.0.17559), its su*_00000000 package, or a "
                  "folder of $flash_<module>.xex or <module>.xex files (an Xbox PC "
                  "backward-compatibility game's Content/Flash); a module or font comes from "
                  "the first source that has it")
      ->required()
      ->type_name("PATH");
  sub->add_option("-o,--output", args->output, "Bundle file to write")
      ->required()
      ->type_name("PATH");
  sub->callback([args, &pending]() {
    pending = [args]() -> Result<void> { return WriteGuideBundle(*args); };
  });

  auto dump = std::make_shared<XuiDumpArgs>();
  auto* dump_sub = parent.add_subcommand(
      "xui-dump", "Write out every file of the system modules' XUI packages, for research");
  dump_sub->add_option("sources", dump->sources, "As for guide-bundle")
      ->required()
      ->type_name("PATH");
  dump_sub->add_option("-o,--output", dump->output, "Folder to write the files to")
      ->required()
      ->type_name("PATH");
  dump_sub->callback(
      [dump, &pending]() { pending = [dump]() -> Result<void> { return DumpXui(*dump); }; });
}

}
