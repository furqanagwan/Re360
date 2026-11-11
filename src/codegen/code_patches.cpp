/**
 * @file        codegen/code_patches.cpp
 * @brief       Guest code patches applied to the image before analysis
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#include <map>

#include <fmt/format.h>

#include <rex/codegen/binary_view.h>
#include <rex/codegen/code_patches.h>
#include <rex/logging.h>

#include "codegen_logging.h"

namespace rex::codegen {

Result<std::vector<std::string>> ApplyCodePatches(BinaryView& binary,
                                                  const std::vector<CodePatch>& patches) {
  std::map<uint32_t, std::pair<uint32_t, const CodePatch*>> claimed;
  for (const auto& patch : patches) {
    if (!patch.enabled || patch.switchable) {
      continue;
    }
    if (!patch.error.empty()) {
      return Err<std::vector<std::string>>(
          ErrorCategory::Config,
          fmt::format("Patch \"{}\" ({}): {}", patch.name, patch.source, patch.error));
    }
    for (const auto& write : patch.writes) {
      const uint32_t start = write.address;
      const uint32_t end = start + static_cast<uint32_t>(write.bytes.size());
      const auto* section = binary.findSection(start);
      if (!section || !section->executable || end > section->end()) {
        return Err<std::vector<std::string>>(
            ErrorCategory::Config,
            fmt::format("Patch \"{}\" writes 0x{:08X}, outside the code sections. Only guest code "
                        "can be patched: the running title loads the original image.",
                        patch.name, start));
      }
      auto next = claimed.lower_bound(start);
      const bool overlapsNext = next != claimed.end() && next->first < end;
      const bool overlapsPrev = next != claimed.begin() && std::prev(next)->second.first > start;
      if (overlapsNext || overlapsPrev) {
        const auto* other = overlapsNext ? next->second.second : std::prev(next)->second.second;
        return Err<std::vector<std::string>>(
            ErrorCategory::Config, fmt::format("Patches \"{}\" and \"{}\" both write 0x{:08X}",
                                               other->name, patch.name, start));
      }
      claimed.emplace(start, std::make_pair(end, &patch));
    }
  }

  std::vector<std::string> applied;
  for (const auto& patch : patches) {
    if (patch.switchable) {
      continue;
    }
    if (!patch.enabled) {
      REXCODEGEN_INFO("Patch \"{}\" is disabled", patch.name);
      continue;
    }
    for (const auto& write : patch.writes) {
      binary.writeCode(write.address, write.bytes);
    }
    REXCODEGEN_INFO("Applied patch \"{}\" ({} writes)", patch.name, patch.writes.size());
    applied.push_back(patch.name);
  }
  return applied;
}

namespace {

bool ChangesControlFlow(uint32_t word) {
  const uint32_t primary = word >> 26;
  if (primary == 3 || primary == 16 || primary == 17 || primary == 18 || primary == 19) {
    return true;
  }
  return primary == 31 && ((word >> 1) & 0x3FF) == 4;
}

}

Result<SwitchablePatches> PrepareSwitchablePatches(const BinaryView& binary,
                                                   const std::vector<CodePatch>& patches,
                                                   bool keeps_lr) {
  SwitchablePatches out;
  for (const auto& patch : patches) {
    if (!patch.switchable) {
      continue;
    }
    if (!patch.error.empty()) {
      return Err<SwitchablePatches>(
          ErrorCategory::Config,
          fmt::format("Patch \"{}\" ({}): {}", patch.name, patch.source, patch.error));
    }
    const uint32_t index = static_cast<uint32_t>(out.patches.size());
    out.patches.push_back({patch.name, patch.enabled, patch.category});
    for (const auto& write : patch.writes) {
      const uint32_t start = write.address;
      const auto* section = binary.findSection(start);
      if (!section || !section->executable ||
          start + static_cast<uint32_t>(write.bytes.size()) > section->end()) {
        return Err<SwitchablePatches>(
            ErrorCategory::Config,
            fmt::format("Patch \"{}\" writes 0x{:08X}, outside the code sections.", patch.name,
                        start));
      }
      for (size_t i = 0; i < write.bytes.size(); ++i) {
        const uint32_t address = start + static_cast<uint32_t>(i);
        const uint32_t word_address = address & ~3u;
        auto it = out.words.find(word_address);
        if (it == out.words.end()) {
          const auto* host = reinterpret_cast<const uint8_t*>(binary.translate(word_address));
          if (!host) {
            return Err<SwitchablePatches>(
                ErrorCategory::Config,
                fmt::format("Patch \"{}\" writes 0x{:08X}, which is not mapped.", patch.name,
                            address));
          }
          const uint32_t original =
              uint32_t(host[0]) << 24 | uint32_t(host[1]) << 16 | uint32_t(host[2]) << 8 | host[3];
          SwitchedWord word;
          word.original = original;
          word.patched = original;
          word.patch_index = index;
          word.patch_name = patch.name;
          it = out.words.emplace(word_address, std::move(word)).first;
        } else if (it->second.patch_index != index) {
          return Err<SwitchablePatches>(ErrorCategory::Config,
                                        fmt::format("Patches \"{}\" and \"{}\" both write 0x{:08X}",
                                                    it->second.patch_name, patch.name, address));
        }
        const uint32_t shift = (3 - (address & 3)) * 8;
        it->second.patched =
            (it->second.patched & ~(0xFFu << shift)) | (uint32_t(write.bytes[i]) << shift);
      }
    }
    for (const auto& set : patch.sets) {
      const auto* section = binary.findSection(set.address);
      if (!section || !section->executable || (set.address & 3)) {
        return Err<SwitchablePatches>(
            ErrorCategory::Config,
            fmt::format("Patch \"{}\" sets r{} at 0x{:08X}, which is not an instruction in code.",
                        patch.name, set.reg, set.address));
      }
      if (set.lr && !keeps_lr) {
        return Err<SwitchablePatches>(
            ErrorCategory::Config,
            fmt::format("Patch \"{}\" tests lr at 0x{:08X}, but skip_lr drops the link register.",
                        patch.name, set.address));
      }
      out.sets.emplace(set.address, SwitchedSet{set.reg, set.value, set.lr, set.when_nonzero, index,
                                                patch.name});
    }
  }

  std::erase_if(out.words,
                [](const auto& entry) { return entry.second.original == entry.second.patched; });
  for (const auto& [address, word] : out.words) {
    if (ChangesControlFlow(word.original) || ChangesControlFlow(word.patched)) {
      return Err<SwitchablePatches>(
          ErrorCategory::Config,
          fmt::format("Patch \"{}\" changes the branch or call at 0x{:08X}; a switchable patch "
                      "may only change other instructions. Make it a fixed patch instead.",
                      word.patch_name, address));
    }
  }
  for (const auto& patch : out.patches) {
    REXCODEGEN_INFO("Patch \"{}\" is switchable ({} by default)", patch.name,
                    patch.enabled ? "on" : "off");
  }
  return out;
}

}
