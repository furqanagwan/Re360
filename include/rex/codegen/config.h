/**
 * @file        rexcodegen/internal/config.h
 * @brief       Recompiler configuration types
 *
 * @copyright   Copyright (c) 2026 Tom Clay <tomc@tctechstuff.com>
 *              All rights reserved.
 *
 * @license     BSD 3-Clause License
 *              See LICENSE file in the project root for full license text.
 */

#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <toml++/toml.hpp>

#include <rex/codegen/function_graph.h>

namespace rex::codegen {

struct MidAsmHook {
  std::string name;
  std::vector<std::string> registers;

  bool ret = false;
  bool returnOnTrue = false;
  bool returnOnFalse = false;

  uint32_t jumpAddress = 0;
  uint32_t jumpAddressOnTrue = 0;
  uint32_t jumpAddressOnFalse = 0;

  bool afterInstruction = false;
};

struct FunctionConfig {
  uint32_t size = 0;
  uint32_t end = 0;
  std::string name;
  uint32_t parent = 0;

  bool shareRegisters = false;

  uint32_t getSize(uint32_t address) const {
    return size ? size : (end > address ? end - address : 0);
  }

  bool isChunk() const { return parent != 0; }
};

struct PatchWrite {
  uint32_t address = 0;
  std::vector<uint8_t> bytes;
};

struct PatchRegisterSet {
  uint32_t address = 0;
  uint32_t reg = 0;
  uint64_t value = 0;
  std::optional<uint32_t> lr;
  std::vector<PatchNonzero> when_nonzero;  // all must be non-zero for the set to apply
};

struct CodePatch {
  std::string name;
  bool enabled = true;

  bool switchable = false;

  std::string category = "patch";
  std::vector<PatchWrite> writes;
  std::vector<PatchRegisterSet> sets;
  std::string source;
  std::string error;
};

struct TitleCheat {
  std::string name;
  std::string code;
  std::string description;
  std::string where;
};

struct TitleDlc {
  std::string id;
  std::string package_name;
  uint32_t requires_title_update = 0;
};

struct TitleUpdateInfo {
  uint32_t version = 0;
  std::string media_id;
  uint32_t base_version = 0;
  std::string content_id;
  uint32_t size_kb = 0;
  std::string date;
  std::string changelog;
};

struct SectionInfo {
  std::string name;
  uint64_t address = 0;
  uint64_t size = 0;
  std::string flags;
};

struct FunctionEntry {
  uint64_t address = 0;
  uint64_t size = 0;
  std::string name;
};

struct RecompilerConfig {
  std::string projectName = "rex";
  std::string filePath;
  std::string outDirectoryPath;
  std::string templateDir;

  std::vector<std::string> loadedFiles;

  bool skipLr = false;
  bool ctrAsLocalVariable = false;
  bool xerAsLocalVariable = false;
  bool reservedRegisterAsLocalVariable = false;
  bool skipMsr = false;
  bool crRegistersAsLocalVariables = false;
  bool nonArgumentRegistersAsLocalVariables = false;
  bool nonVolatileRegistersAsLocalVariables = false;
  bool generateExceptionHandlers = false;

  uint32_t maxJumpExtension = 65536;
  uint32_t dataRegionThreshold = 16;
  uint32_t largeFunctionThreshold = 1048576;

  std::optional<bool> isDll;

  uint32_t titleUpdateVersion = 0;

  std::unordered_map<uint32_t, FunctionConfig> functions;
  std::unordered_map<uint32_t, JumpTable> switchTables;
  std::unordered_map<uint32_t, MidAsmHook> midAsmHooks;

  std::vector<CodePatch> patches;

  std::vector<TitleCheat> cheats;

  std::vector<TitleDlc> dlc;

  std::vector<TitleUpdateInfo> titleUpdates;
  uint32_t longJmpAddress = 0;
  uint32_t setJmpAddress = 0;

  uint32_t setJmpHookAddress = 0;

  std::unordered_map<std::string, uint32_t> rexcrtFunctions;

  std::unordered_map<uint32_t, uint32_t> invalidInstructionHints;
  std::unordered_set<uint32_t> knownIndirectCallHints;
  std::vector<uint32_t> exceptionHandlerFuncHints;

  bool Load(const std::string_view& configFilePath);

  bool LoadFromTable(const toml::table& tbl, const std::filesystem::path& base_dir);

  struct ValidationResult {
    bool valid = true;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;

    explicit operator bool() const { return valid; }
  };

  ValidationResult Validate() const;
};

}
