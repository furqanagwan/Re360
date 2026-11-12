/**
 * @file        rexcodegen/config.cpp
 *
 * @copyright   Copyright (c) 2026 Tom Clay <tomc@tctechstuff.com>
 *              All rights reserved.
 *
 * @license     BSD 3-Clause License
 *              See LICENSE file in the project root for full license text.
 *
 * @remarks     Based on XenonRecomp/UnleashedRecomp toml config
 */

#include <algorithm>
#include <cctype>
#include <map>
#include <set>

#include <fmt/format.h>

#include <rex/codegen/config.h>
#include <rex/logging.h>

#include "codegen_logging.h"

#include <toml++/toml.hpp>

namespace rex::codegen {

namespace {

constexpr uint32_t kMaxIncludeDepth = 32;

std::optional<uint32_t> ParseHexAddress(const std::string& keyStr) {
  try {
    if (keyStr.starts_with("0x") || keyStr.starts_with("0X")) {
      return static_cast<uint32_t>(std::stoul(keyStr.substr(2), nullptr, 16));
    } else {
      return static_cast<uint32_t>(std::stoul(keyStr, nullptr, 16));
    }
  } catch (...) {
    return std::nullopt;
  }
}

template <typename T>
void MergeScalar(T& dst, const T& src, const char* name) {
  if (src != T{} && src != dst) {
    if (dst != T{}) {
      if constexpr (std::is_same_v<T, bool>) {
        REXCODEGEN_DEBUG("[config]   {}: {} -> {} (overridden)", name, dst, src);
      } else if constexpr (std::is_same_v<T, std::string>) {
        REXCODEGEN_DEBUG("[config]   {}: \"{}\" -> \"{}\" (overridden)", name, dst, src);
      } else {
        REXCODEGEN_DEBUG("[config]   {}: {} -> {} (overridden)", name, dst, src);
      }
    }
    dst = src;
  }
}

void MergeBool(bool& dst, bool src, bool present, const char* name) {
  if (!present)
    return;
  if (src != dst) {
    REXCODEGEN_DEBUG("[config]   {}: {} -> {} (overridden)", name, dst ? "true" : "false",
                     src ? "true" : "false");
  }
  dst = src;
}

void ApplyToml(const toml::table& toml, RecompilerConfig& cfg, const std::string& filePath) {
  if (auto v = toml["project_name"].value<std::string>()) {
    MergeScalar(cfg.projectName, *v, "project_name");
  }
  if (auto v = toml["file_path"].value<std::string>()) {
    MergeScalar(cfg.filePath, *v, "file_path");
  }
  if (auto v = toml["out_directory_path"].value<std::string>()) {
    MergeScalar(cfg.outDirectoryPath, *v, "out_directory_path");
  }
  if (auto v = toml["template_dir"].value<std::string>()) {
    MergeScalar(cfg.templateDir, *v, "template_dir");
  }

  auto hasBool = [&](const char* key) -> bool { return toml[key].is_boolean(); };
  MergeBool(cfg.skipLr, toml["skip_lr"].value_or(false), hasBool("skip_lr"), "skip_lr");
  MergeBool(cfg.skipMsr, toml["skip_msr"].value_or(false), hasBool("skip_msr"), "skip_msr");
  MergeBool(cfg.ctrAsLocalVariable, toml["ctr_as_local"].value_or(false), hasBool("ctr_as_local"),
            "ctr_as_local");
  MergeBool(cfg.xerAsLocalVariable, toml["xer_as_local"].value_or(false), hasBool("xer_as_local"),
            "xer_as_local");
  MergeBool(cfg.reservedRegisterAsLocalVariable, toml["reserved_as_local"].value_or(false),
            hasBool("reserved_as_local"), "reserved_as_local");
  MergeBool(cfg.crRegistersAsLocalVariables, toml["cr_as_local"].value_or(false),
            hasBool("cr_as_local"), "cr_as_local");
  MergeBool(cfg.nonArgumentRegistersAsLocalVariables, toml["non_argument_as_local"].value_or(false),
            hasBool("non_argument_as_local"), "non_argument_as_local");
  MergeBool(cfg.nonVolatileRegistersAsLocalVariables, toml["non_volatile_as_local"].value_or(false),
            hasBool("non_volatile_as_local"), "non_volatile_as_local");
  MergeBool(cfg.generateExceptionHandlers, toml["generate_exception_handlers"].value_or(false),
            hasBool("generate_exception_handlers"), "generate_exception_handlers");
  if (hasBool("is_dll")) {
    cfg.isDll = toml["is_dll"].value_or(false);
  }

  if (auto v = toml["longjmp_address"].value<int64_t>()) {
    uint32_t addr = static_cast<uint32_t>(*v);
    MergeScalar(cfg.longJmpAddress, addr, "longjmp_address");
  }
  if (auto v = toml["setjmp_address"].value<int64_t>()) {
    uint32_t addr = static_cast<uint32_t>(*v);
    MergeScalar(cfg.setJmpAddress, addr, "setjmp_address");
  }

  if (auto* analysisTable = toml["analysis"].as_table()) {
    if (auto v = (*analysisTable)["max_jump_extension"].value<uint32_t>()) {
      MergeScalar(cfg.maxJumpExtension, *v, "analysis.max_jump_extension");
    }
    if (auto v = (*analysisTable)["data_region_threshold"].value<uint32_t>()) {
      MergeScalar(cfg.dataRegionThreshold, *v, "analysis.data_region_threshold");
    }
    if (auto v = (*analysisTable)["large_function_threshold"].value<uint32_t>()) {
      MergeScalar(cfg.largeFunctionThreshold, *v, "analysis.large_function_threshold");
    }

    if (auto handlers = (*analysisTable)["exception_handler_funcs"].as_array()) {
      for (const auto& elem : *handlers) {
        if (auto val = elem.value<int64_t>()) {
          cfg.exceptionHandlerFuncHints.push_back(static_cast<uint32_t>(*val));
        }
      }
    }
  }

  if (auto* rexcrt = toml["rexcrt"].as_table()) {
    size_t added = 0;
    for (const auto& [name, val] : *rexcrt) {
      if (auto addr = val.value<int64_t>()) {
        auto key = std::string(name);
        auto it = cfg.rexcrtFunctions.find(key);
        if (it != cfg.rexcrtFunctions.end()) {
          REXCODEGEN_DEBUG("[config]   [rexcrt] {} overridden: 0x{:08X} -> 0x{:08X}", key,
                           it->second, static_cast<uint32_t>(*addr));
        }
        cfg.rexcrtFunctions[key] = static_cast<uint32_t>(*addr);
        ++added;
      }
    }
    if (added > 0) {
      REXCODEGEN_DEBUG("[config]   [rexcrt] added/updated {} entries from {}", added, filePath);
    }
  }

  if (auto functionsTable = toml["functions"].as_table()) {
    size_t added = 0;
    for (auto& [key, value] : *functionsTable) {
      std::string keyStr(key.str());
      auto addrOpt = ParseHexAddress(keyStr);
      if (!addrOpt) {
        REXCODEGEN_ERROR("Invalid function address key: {}", keyStr);
        continue;
      }
      uint32_t address = *addrOpt;

      auto* table = value.as_table();
      if (!table) {
        REXCODEGEN_ERROR("Invalid [functions] entry at 0x{:08X}: expected table", address);
        continue;
      }

      FunctionConfig fcfg;
      fcfg.size = (*table)["size"].value_or(0u);
      fcfg.end = (*table)["end"].value_or(0u);
      fcfg.name = (*table)["name"].value_or(std::string{});
      fcfg.parent = (*table)["parent"].value_or(0u);
      fcfg.shareRegisters = (*table)["share_registers"].value_or(false);

      if (fcfg.size && fcfg.end) {
        REXCODEGEN_ERROR("Function 0x{:08X}: cannot specify both 'size' and 'end'", address);
        continue;
      }
      if (fcfg.end && fcfg.end <= address) {
        REXCODEGEN_ERROR("Function 0x{:08X}: 'end' (0x{:08X}) must be greater than address",
                         address, fcfg.end);
        continue;
      }

      auto it = cfg.functions.find(address);
      if (it != cfg.functions.end()) {
        REXCODEGEN_DEBUG("[config]   [functions] 0x{:08X} overridden from {}", address, filePath);
      }
      cfg.functions.insert_or_assign(address, std::move(fcfg));
      ++added;
    }
    if (added > 0) {
      REXCODEGEN_DEBUG("[config]   [functions] added/updated {} entries from {}", added, filePath);
    }
  }

  if (auto patchArray = toml["patch"].as_array()) {
    for (auto& entry : *patchArray) {
      auto* table = entry.as_table();
      if (!table) {
        REXCODEGEN_ERROR("Invalid [[patch]] entry in {}: expected table", filePath);
        continue;
      }
      auto name = (*table)["name"].value<std::string>();
      if (!name || name->empty()) {
        REXCODEGEN_ERROR("[[patch]] entry in {} has no name", filePath);
        continue;
      }

      CodePatch parsed;
      parsed.name = *name;
      bool hasWrites = false;
      constexpr std::pair<const char*, size_t> kWidths[] = {
          {"be8", 1}, {"be16", 2}, {"be32", 4}, {"be64", 8}};
      for (const auto& [key, width] : kWidths) {
        auto* writes = (*table)[key].as_array();
        if (!writes) {
          continue;
        }
        hasWrites = true;
        for (auto& w : *writes) {
          auto* wt = w.as_table();
          auto address = wt ? (*wt)["address"].value<int64_t>() : std::nullopt;
          auto value = wt ? (*wt)["value"].value<int64_t>() : std::nullopt;
          if (!address || !value || *address < 0 || *address > 0xFFFFFFFFll) {
            parsed.error = fmt::format("a {} entry needs an address and a value", key);
            continue;
          }
          const uint64_t v = static_cast<uint64_t>(*value);
          if (width < 8 && (v >> (width * 8)) != 0) {
            parsed.error = fmt::format("{} value 0x{:X} at 0x{:08X} does not fit in {} bytes", key,
                                       v, static_cast<uint32_t>(*address), width);
            continue;
          }
          PatchWrite write{static_cast<uint32_t>(*address), {}};
          for (size_t i = 0; i < width; ++i) {
            write.bytes.push_back(uint8_t(v >> ((width - 1 - i) * 8)));
          }
          parsed.writes.push_back(std::move(write));
        }
      }
      bool hasSets = false;
      if (auto* sets = (*table)["set"].as_array()) {
        hasSets = true;
        for (auto& s : *sets) {
          auto* st = s.as_table();
          auto address = st ? (*st)["address"].value<int64_t>() : std::nullopt;
          auto reg = st ? (*st)["register"].value<std::string>() : std::nullopt;
          auto value = st ? (*st)["value"].value<int64_t>() : std::nullopt;
          auto lr = st ? (*st)["lr"].value<int64_t>() : std::nullopt;
          uint32_t reg_index = 32;
          if (reg && reg->size() >= 2 && (*reg)[0] == 'r') {
            reg_index = uint32_t(std::strtoul(reg->c_str() + 1, nullptr, 10));
            if (reg->find_first_not_of("0123456789", 1) != std::string::npos) {
              reg_index = 32;
            }
          }
          if (!address || !value || *address < 0 || *address > 0xFFFFFFFFll || (*address & 3) ||
              reg_index > 31 || (lr && (*lr < 0 || *lr > 0xFFFFFFFFll))) {
            parsed.error =
                "a set entry needs a word address, a register r0 to r31, a value and an "
                "optional lr";
            continue;
          }
          // when_nonzero = ["r26", "r26+316"]: each a register, or the word at register +
          // offset; the set applies only when all are non-zero.
          std::vector<PatchNonzero> when_nonzero;
          bool conditions_ok = true;
          if (auto* conditions = (*st)["when_nonzero"].as_array()) {
            for (auto& c : *conditions) {
              auto text = c.value<std::string>();
              PatchNonzero condition;
              size_t plus = text ? text->find('+') : std::string::npos;
              const std::string reg_text = text ? text->substr(0, plus) : std::string();
              if (reg_text.size() < 2 || reg_text[0] != 'r' ||
                  reg_text.find_first_not_of("0123456789", 1) != std::string::npos ||
                  std::strtoul(reg_text.c_str() + 1, nullptr, 10) > 31) {
                conditions_ok = false;
                break;
              }
              condition.reg = uint32_t(std::strtoul(reg_text.c_str() + 1, nullptr, 10));
              if (plus != std::string::npos) {
                const std::string offset_text = text->substr(plus + 1);
                char* end = nullptr;
                const long long offset = std::strtoll(offset_text.c_str(), &end, 0);
                if (offset_text.empty() || *end || offset < -32768 || offset > 32767) {
                  conditions_ok = false;
                  break;
                }
                condition.offset = int32_t(offset);
              }
              when_nonzero.push_back(condition);
            }
          }
          if (!conditions_ok) {
            parsed.error =
                "when_nonzero entries are registers (\"r26\") or a register and an offset "
                "(\"r26+316\")";
            continue;
          }
          parsed.sets.push_back({static_cast<uint32_t>(*address), reg_index,
                                 static_cast<uint64_t>(*value),
                                 lr ? std::optional<uint32_t>(uint32_t(*lr)) : std::nullopt,
                                 std::move(when_nonzero)});
        }
      }
      auto switchable = (*table)["switchable"].value<bool>();
      auto category = (*table)["category"].value<std::string>();

      auto enabled = (*table)["enabled"].value<bool>();
      if (!enabled) {
        enabled = (*table)["is_enabled"].value<bool>();
      }

      auto it = std::find_if(cfg.patches.begin(), cfg.patches.end(),
                             [&](const CodePatch& p) { return p.name == parsed.name; });
      if (it == cfg.patches.end()) {
        parsed.enabled = enabled.value_or(true);
        parsed.switchable = switchable.value_or(false);
        parsed.category = category.value_or("patch");
        if (parsed.category == "cheat") {
          parsed.category = "mod";
        }
        parsed.source = filePath;
        if (!hasWrites && !hasSets && !parsed.switchable && parsed.error.empty()) {
          parsed.error = "it has no be8, be16, be32 or be64 writes";
        }
        if (hasSets && !parsed.switchable && parsed.error.empty()) {
          parsed.error = "set entries need switchable = true";
        }
        cfg.patches.push_back(std::move(parsed));
        continue;
      }
      REXCODEGEN_DEBUG("[config]   [[patch]] \"{}\" updated from {}", parsed.name, filePath);
      if (hasWrites || hasSets) {
        if (hasWrites) {
          it->writes = std::move(parsed.writes);
        }
        if (hasSets) {
          it->sets = std::move(parsed.sets);
        }
        it->error = std::move(parsed.error);
        it->source = filePath;
      }
      if (enabled) {
        it->enabled = *enabled;
      }
      if (switchable) {
        it->switchable = *switchable;
      }
      if (category) {
        it->category = *category == "cheat" ? "mod" : *category;
      }
    }
  }

  if (auto cheatArray = toml["cheat"].as_array()) {
    for (auto& entry : *cheatArray) {
      auto* table = entry.as_table();
      auto name = table ? (*table)["name"].value<std::string>() : std::nullopt;
      auto code = table ? (*table)["code"].value<std::string>() : std::nullopt;
      if (!name || name->empty() || !code || code->empty()) {
        REXCODEGEN_ERROR("[[cheat]] entry in {} needs a name and a code", filePath);
        continue;
      }
      TitleCheat parsed{*name, *code, (*table)["description"].value_or(std::string()),
                        (*table)["where"].value_or(std::string())};
      auto it = std::find_if(cfg.cheats.begin(), cfg.cheats.end(),
                             [&](const TitleCheat& c) { return c.name == parsed.name; });
      if (it == cfg.cheats.end()) {
        cfg.cheats.push_back(std::move(parsed));
      } else {
        *it = std::move(parsed);
      }
    }
  }

  if (auto dlcArray = toml["dlc"].as_array()) {
    for (auto& entry : *dlcArray) {
      auto* table = entry.as_table();
      auto id = table ? (*table)["id"].value<std::string>() : std::nullopt;
      if (!id || id->empty()) {
        REXCODEGEN_ERROR("[[dlc]] entry in {} needs an id", filePath);
        continue;
      }
      std::string upper = *id;
      std::transform(upper.begin(), upper.end(), upper.begin(),
                     [](unsigned char c) { return char(std::toupper(c)); });
      TitleDlc parsed{upper, (*table)["package_name"].value_or(std::string()),
                      uint32_t((*table)["requires_title_update"].value_or(int64_t(0)))};
      auto it = std::find_if(cfg.dlc.begin(), cfg.dlc.end(),
                             [&](const TitleDlc& d) { return d.id == parsed.id; });
      if (it == cfg.dlc.end()) {
        cfg.dlc.push_back(std::move(parsed));
      } else {
        *it = std::move(parsed);
      }
    }
  }

  if (auto updateArray = toml["title_update"].as_array()) {
    for (auto& entry : *updateArray) {
      auto* table = entry.as_table();
      const int64_t version = table ? (*table)["version"].value_or(int64_t(0)) : 0;
      auto media = table ? (*table)["media_id"].value<std::string>() : std::nullopt;
      auto content = table ? (*table)["content_id"].value<std::string>() : std::nullopt;
      if (version <= 0 || !media || media->size() != 8 || !content || content->size() != 40) {
        REXCODEGEN_ERROR(
            "[[title_update]] entry in {} needs version, media_id (8 hex digits) and "
            "content_id (40 hex digits)",
            filePath);
        continue;
      }
      auto upper = [](std::string text) {
        std::transform(text.begin(), text.end(), text.begin(),
                       [](unsigned char c) { return char(std::toupper(c)); });
        return text;
      };
      TitleUpdateInfo parsed{uint32_t(version),
                             upper(*media),
                             uint32_t((*table)["base_version"].value_or(int64_t(0))),
                             upper(*content),
                             uint32_t((*table)["size_kb"].value_or(int64_t(0))),
                             (*table)["date"].value_or(std::string()),
                             (*table)["changelog"].value_or(std::string())};
      auto it = std::find_if(cfg.titleUpdates.begin(), cfg.titleUpdates.end(),
                             [&](const TitleUpdateInfo& u) { return u.version == parsed.version; });
      if (it == cfg.titleUpdates.end()) {
        cfg.titleUpdates.push_back(std::move(parsed));
      } else {
        *it = std::move(parsed);
      }
    }
  }

  if (auto invalidArray = toml["invalid_instructions"].as_array()) {
    for (auto& entry : *invalidArray) {
      auto* table = entry.as_table();
      if (!table) {
        REXCODEGEN_ERROR("Invalid [[invalid_instructions]] entry: expected table");
        continue;
      }

      auto data_opt = (*table)["data"].value<uint32_t>();
      auto size_opt = (*table)["size"].value<uint32_t>();

      if (!data_opt) {
        REXCODEGEN_ERROR("Missing 'data' in [[invalid_instructions]] entry");
        continue;
      }
      if (!size_opt) {
        REXCODEGEN_ERROR("Missing 'size' in [[invalid_instructions]] entry");
        continue;
      }

      auto it = cfg.invalidInstructionHints.find(*data_opt);
      if (it != cfg.invalidInstructionHints.end()) {
        REXCODEGEN_DEBUG("[config]   [[invalid_instructions]] 0x{:08X} overridden from {}",
                         *data_opt, filePath);
      }
      cfg.invalidInstructionHints.insert_or_assign(*data_opt, *size_opt);
    }
  }

  if (auto switchTableArray = toml["switch_tables"].as_array()) {
    for (auto& entry : *switchTableArray) {
      auto* table = entry.as_table();
      if (!table) {
        REXCODEGEN_ERROR("Invalid [[switch_tables]] entry: expected table");
        continue;
      }

      auto address_opt = (*table)["address"].value<uint32_t>();
      auto register_opt = (*table)["register"].value<uint32_t>();
      auto labels_array = (*table)["labels"].as_array();

      if (!address_opt) {
        REXCODEGEN_ERROR("Missing 'address' in [[switch_tables]] entry");
        continue;
      }
      if (!register_opt) {
        REXCODEGEN_ERROR("Missing 'register' in [[switch_tables]] entry");
        continue;
      }
      if (!labels_array) {
        REXCODEGEN_ERROR("Missing 'labels' in [[switch_tables]] entry");
        continue;
      }

      JumpTable jt;
      jt.bctrAddress = *address_opt;
      jt.tableAddress = 0;
      jt.indexRegister = static_cast<uint8_t>(*register_opt);

      for (auto& label : *labels_array) {
        if (auto label_val = label.value<int64_t>()) {
          jt.targets.push_back(static_cast<uint32_t>(*label_val));
        }
      }

      if (jt.targets.empty()) {
        REXCODEGEN_ERROR("Empty 'labels' array in [[switch_tables]] at 0x{:08X}", *address_opt);
        continue;
      }

      auto it = cfg.switchTables.find(*address_opt);
      if (it != cfg.switchTables.end()) {
        REXCODEGEN_DEBUG("[config]   [[switch_tables]] 0x{:08X} overridden from {}", *address_opt,
                         filePath);
      }
      size_t label_count = jt.targets.size();
      cfg.switchTables.insert_or_assign(*address_opt, std::move(jt));
      REXCODEGEN_DEBUG("Loaded manual switch table at 0x{:08X} with {} labels", *address_opt,
                       label_count);
    }
  }

  if (auto midAsmHookArray = toml["midasm_hook"].as_array()) {
    for (auto& entry : *midAsmHookArray) {
      auto* table = entry.as_table();
      if (!table) {
        REXCODEGEN_ERROR("Invalid [[midasm_hook]] entry: expected table");
        continue;
      }

      auto address_opt = (*table)["address"].value<uint32_t>();
      auto name_opt = (*table)["name"].value<std::string>();

      if (!address_opt) {
        REXCODEGEN_ERROR("Missing 'address' in [[midasm_hook]] entry");
        continue;
      }
      if (!name_opt) {
        REXCODEGEN_ERROR("Missing 'name' in [[midasm_hook]] entry");
        continue;
      }

      MidAsmHook midAsmHook;
      midAsmHook.name = *name_opt;

      if (auto registerArray = (*table)["registers"].as_array()) {
        for (auto& reg : *registerArray) {
          if (auto reg_str = reg.value<std::string>()) {
            midAsmHook.registers.push_back(*reg_str);
          }
        }
      }

      midAsmHook.ret = (*table)["return"].value_or(false);
      midAsmHook.returnOnTrue = (*table)["return_on_true"].value_or(false);
      midAsmHook.returnOnFalse = (*table)["return_on_false"].value_or(false);

      midAsmHook.jumpAddress = (*table)["jump_address"].value_or(0u);
      midAsmHook.jumpAddressOnTrue = (*table)["jump_address_on_true"].value_or(0u);
      midAsmHook.jumpAddressOnFalse = (*table)["jump_address_on_false"].value_or(0u);

      if ((midAsmHook.ret && midAsmHook.jumpAddress != 0) ||
          (midAsmHook.returnOnTrue && midAsmHook.jumpAddressOnTrue != 0) ||
          (midAsmHook.returnOnFalse && midAsmHook.jumpAddressOnFalse != 0)) {
        REXCODEGEN_ERROR("{}: can't return and jump at the same time", midAsmHook.name);
      }

      if ((midAsmHook.ret || midAsmHook.jumpAddress != 0) &&
          (midAsmHook.returnOnFalse || midAsmHook.returnOnTrue ||
           midAsmHook.jumpAddressOnFalse != 0 || midAsmHook.jumpAddressOnTrue != 0)) {
        REXCODEGEN_ERROR("{}: can't mix direct and conditional return/jump", midAsmHook.name);
      }

      midAsmHook.afterInstruction = (*table)["after_instruction"].value_or(false);

      auto it = cfg.midAsmHooks.find(*address_opt);
      if (it != cfg.midAsmHooks.end()) {
        REXCODEGEN_DEBUG("[config]   [[midasm_hook]] 0x{:08X} overridden from {}", *address_opt,
                         filePath);
      }
      cfg.midAsmHooks.insert_or_assign(*address_opt, std::move(midAsmHook));
    }
  }

  if (auto indirectCallArray = toml["indirect_calls"].as_array()) {
    for (auto& entry : *indirectCallArray) {
      if (auto addr = entry.value<int64_t>()) {
        cfg.knownIndirectCallHints.insert(static_cast<uint32_t>(*addr));
        REXCODEGEN_DEBUG("Loaded known indirect call hint at 0x{:08X}", *addr);
      }
    }
  }
}

bool ApplyTableWithIncludes(const toml::table& tbl, const std::filesystem::path& base_dir,
                            RecompilerConfig& cfg, std::set<std::string>& visited, uint32_t depth,
                            const std::string& description);

bool LoadRecursive(const std::filesystem::path& filePath, RecompilerConfig& cfg,
                   std::set<std::string>& visited, uint32_t depth) {
  if (depth > kMaxIncludeDepth) {
    REXCODEGEN_ERROR("[config] include depth exceeds maximum ({}) at: {}", kMaxIncludeDepth,
                     filePath.string());
    return false;
  }

  std::error_code ec;
  auto canonical = std::filesystem::canonical(filePath, ec);
  if (ec) {
    REXCODEGEN_ERROR("[config] cannot resolve path '{}': {}", filePath.string(), ec.message());
    return false;
  }
  std::string canonicalStr = canonical.string();
  if (visited.contains(canonicalStr)) {
    REXCODEGEN_ERROR("[config] circular include detected: {}", canonicalStr);
    return false;
  }
  visited.insert(canonicalStr);
  cfg.loadedFiles.push_back(canonicalStr);

  toml::table toml;
  try {
    toml = toml::parse_file(canonicalStr);
  } catch (const toml::parse_error& e) {
    REXCODEGEN_ERROR("Failed to parse config file '{}': {}", canonicalStr, e.what());
    return false;
  }
  REXCODEGEN_DEBUG("[config] loaded: {}", filePath.filename().string());
  return ApplyTableWithIncludes(toml, canonical.parent_path(), cfg, visited, depth,
                                filePath.filename().string());
}

bool ApplyTableWithIncludes(const toml::table& tbl, const std::filesystem::path& base_dir,
                            RecompilerConfig& cfg, std::set<std::string>& visited, uint32_t depth,
                            const std::string& description) {
  if (auto* includesArray = tbl["includes"].as_array()) {
    for (const auto& elem : *includesArray) {
      if (auto includePath = elem.value<std::string>()) {
        auto resolved = base_dir / *includePath;
        if (!std::filesystem::exists(resolved)) {
          REXCODEGEN_ERROR("[config] included file not found: {} (resolved from {})", *includePath,
                           description);
          return false;
        }
        if (!LoadRecursive(resolved, cfg, visited, depth + 1)) {
          return false;
        }
      }
    }
  }
  ApplyToml(tbl, cfg, description);
  return true;
}

}

namespace {

bool FinalizeConfig(RecompilerConfig& cfg) {
  if (!cfg.functions.empty()) {
    size_t chunks_count = 0;
    for (const auto& [addr, fc] : cfg.functions) {
      if (fc.isChunk())
        chunks_count++;
    }
    REXCODEGEN_TRACE("Loaded {} function configs ({} standalone, {} chunks)", cfg.functions.size(),
                     cfg.functions.size() - chunks_count, chunks_count);
  }
  if (!cfg.exceptionHandlerFuncHints.empty()) {
    std::sort(cfg.exceptionHandlerFuncHints.begin(), cfg.exceptionHandlerFuncHints.end());
    cfg.exceptionHandlerFuncHints.erase(
        std::unique(cfg.exceptionHandlerFuncHints.begin(), cfg.exceptionHandlerFuncHints.end()),
        cfg.exceptionHandlerFuncHints.end());
  }

  bool ok = true;
  if (cfg.filePath.empty()) {
    REXCODEGEN_ERROR("Missing required field: file_path");
    ok = false;
  }

  auto result = cfg.Validate();
  for (const auto& warning : result.warnings) {
    REXCODEGEN_WARN("[config] {}", warning);
  }
  for (const auto& error : result.errors) {
    REXCODEGEN_ERROR("[config] {}", error);
  }
  if (!result.valid) {
    ok = false;
  }
  return ok;
}

}

bool RecompilerConfig::Load(const std::string_view& configFilePath) {
  std::set<std::string> visited;
  std::filesystem::path path(configFilePath);
  if (!LoadRecursive(path, *this, visited, 0)) {
    return false;
  }
  return FinalizeConfig(*this);
}

bool RecompilerConfig::LoadFromTable(const toml::table& tbl,
                                     const std::filesystem::path& base_dir) {
  std::set<std::string> visited;
  if (!ApplyTableWithIncludes(tbl, base_dir, *this, visited, 0, "<inline>")) {
    return false;
  }
  return FinalizeConfig(*this);
}

RecompilerConfig::ValidationResult RecompilerConfig::Validate() const {
  ValidationResult result;

  auto checkAlignment = [&](uint32_t addr, const char* name) {
    if (addr != 0 && (addr & 0x3) != 0) {
      result.errors.push_back(fmt::format("{} address 0x{:08X} is not 4-byte aligned", name, addr));
      result.valid = false;
    }
  };

  checkAlignment(longJmpAddress, "longjmp");
  checkAlignment(setJmpAddress, "setjmp");

  for (const auto& [name, addr] : rexcrtFunctions) {
    if (addr & 0x3) {
      result.errors.push_back(
          fmt::format("rexcrt function '{}' address 0x{:08X} is not 4-byte aligned", name, addr));
      result.valid = false;
    }
  }

  for (const auto& [addr, size] : functions) {
    if (addr & 0x3) {
      result.errors.push_back(fmt::format("Function address 0x{:08X} is not 4-byte aligned", addr));
      result.valid = false;
    }
  }

  {
    std::map<uint32_t, uint32_t> seen;
    for (const auto& [addr, cfg] : functions) {
      uint32_t funcSize = cfg.getSize(addr);
      auto it = seen.find(addr);
      if (it != seen.end()) {
        if (it->second == funcSize) {
          result.errors.push_back(
              fmt::format("Duplicate function boundary: 0x{:08X} size 0x{:X}", addr, funcSize));
        } else {
          result.errors.push_back(fmt::format("Conflicting sizes at 0x{:08X}: 0x{:X} vs 0x{:X}",
                                              addr, it->second, funcSize));
        }
        result.valid = false;
      }
      seen[addr] = funcSize;
    }
  }

  {
    std::vector<std::pair<uint32_t, uint32_t>> sorted;
    for (const auto& [addr, cfg] : functions) {
      if (!cfg.isChunk()) {
        sorted.emplace_back(addr, cfg.getSize(addr));
      }
    }
    std::sort(sorted.begin(), sorted.end());
    for (size_t i = 1; i < sorted.size(); ++i) {
      uint32_t prev_end = sorted[i - 1].first + sorted[i - 1].second;
      if (sorted[i].first < prev_end) {
        result.errors.push_back(fmt::format(
            "Overlapping boundaries: 0x{:08X}+0x{:X} overlaps 0x{:08X}+0x{:X}", sorted[i - 1].first,
            sorted[i - 1].second, sorted[i].first, sorted[i].second));
        result.valid = false;
      }
    }
  }

  if (!rexcrtFunctions.empty()) {
    auto checkGroup = [&](const char* groupName, std::initializer_list<const char*> required) {
      size_t found = 0;
      for (const char* name : required) {
        if (rexcrtFunctions.contains(name))
          ++found;
      }
      if (found > 0 && found < required.size()) {
        std::string missing;
        for (const char* name : required) {
          if (!rexcrtFunctions.contains(name)) {
            if (!missing.empty())
              missing += ", ";
            missing += name;
          }
        }
        result.errors.push_back(
            fmt::format("[rexcrt] {} group is incomplete ({}/{} specified), missing: {}", groupName,
                        found, required.size(), missing));
        result.valid = false;
      }
    };

    checkGroup("heap", {"RtlAllocateHeap", "RtlFreeHeap", "RtlSizeHeap", "RtlReAllocateHeap"});
  }

  if (filePath.empty()) {
    result.warnings.push_back("file_path is empty");
  }
  if (outDirectoryPath.empty()) {
    result.warnings.push_back("out_directory_path is empty");
  }

  return result;
}

}
