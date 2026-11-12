/**
 * @file        rex/codegen/function_types.h
 * @brief       Types and structures used by FunctionNode and FunctionGraph
 *
 * @copyright   Copyright (c) 2026 Tom Clay <tomc@tctechstuff.com>
 *              All rights reserved.
 *
 * @license     BSD 3-Clause License
 *              See LICENSE file in the project root for full license text.
 */

#pragma once

#include <bitset>
#include <map>
#include <optional>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <span>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include <rex/types.h>

namespace rex::codegen::ppc {
struct Instruction;
}

namespace rex::runtime {
class ExportResolver;
}

namespace rex::codegen {

class FunctionGraph;
class FunctionNode;
class BinaryView;
struct RecompilerConfig;

struct SwitchedWord {
  uint32_t original = 0;
  uint32_t patched = 0;
  uint32_t patch_index = 0;
  std::string patch_name;
};

// A switchable set's condition: a register is non-zero, or (with an offset) so is the guest
// word at register + offset (read only when the register is non-zero).
struct PatchNonzero {
  uint32_t reg = 0;
  std::optional<int32_t> offset;
};

struct SwitchedSet {
  uint32_t reg = 0;
  uint64_t value = 0;
  std::optional<uint32_t> lr;
  std::vector<PatchNonzero> when_nonzero;
  uint32_t patch_index = 0;
  std::string patch_name;
};

struct EmitContext {
  const BinaryView& binary;
  const RecompilerConfig& config;
  const FunctionGraph& graph;
  uint32_t entryPoint = 0;
  runtime::ExportResolver* resolver = nullptr;

  std::unordered_set<std::string>* referenced = nullptr;

  const std::map<uint32_t, SwitchedWord>* switched = nullptr;
  const std::multimap<uint32_t, SwitchedSet>* switched_sets = nullptr;

  void reference(std::string_view name) const {
    if (referenced)
      referenced->emplace(name);
  }
};

enum class FunctionAuthority : uint8_t {
  GAP_FILL = 0,
  DISCOVERED = 1,
  VTABLE = 2,
  HELPER = 3,
  PDATA = 4,
  CONFIG = 5,
  IMPORT = 6,
};

enum class TargetKind {
  InternalLabel,
  Function,
  Import,
  Unknown,
};

const char* AuthorityName(FunctionAuthority auth);

enum class FunctionState : uint8_t {
  kRegistered,
  kDiscovered,
  kSealed,
};

constexpr FunctionState PENDING = FunctionState::kRegistered;
constexpr FunctionState SEALED = FunctionState::kSealed;

struct SehScope {
  uint32_t tryStart;
  uint32_t tryEnd;
  uint32_t handler;
  uint32_t filter;
};

struct SehExceptionInfo {
  uint32_t handlerThunk;
  uint32_t scopeTableAddr;
  std::vector<SehScope> scopes;
  uint32_t frameSize = 0;
  uint32_t restoreHelper = 0;
};

constexpr uint32_t CXX_EH_MAGIC = 0x19930522;

struct CxxUnwindEntry {
  int32_t toState;
  uint32_t action;
};

struct CxxIPStateEntry {
  uint32_t ip;
  int32_t state;
};

struct CxxCatchHandler {
  uint32_t adjectives;
  uint32_t typeDescriptor;
  int32_t catchObjDisplacement;
  uint32_t handlerAddress;
};

struct CxxTryBlock {
  int32_t tryLow;
  int32_t tryHigh;
  int32_t catchHigh;
  std::vector<CxxCatchHandler> handlers;
};

struct CxxExceptionInfo {
  uint32_t handlerThunk;
  uint32_t funcInfoAddr;
  uint32_t maxState;
  std::vector<CxxUnwindEntry> unwindMap;
  std::vector<CxxTryBlock> tryBlocks;
  std::vector<CxxIPStateEntry> ipToStateMap;
};

struct ExceptionInfo {
  std::variant<std::monostate, SehExceptionInfo, CxxExceptionInfo> data;

  bool hasInfo() const { return !std::holds_alternative<std::monostate>(data); }
  bool isSeh() const { return std::holds_alternative<SehExceptionInfo>(data); }
  bool isCxx() const { return std::holds_alternative<CxxExceptionInfo>(data); }

  const SehExceptionInfo* asSeh() const { return std::get_if<SehExceptionInfo>(&data); }
  const CxxExceptionInfo* asCxx() const { return std::get_if<CxxExceptionInfo>(&data); }

  uint32_t handlerThunk() const {
    if (auto* seh = asSeh())
      return seh->handlerThunk;
    if (auto* cxx = asCxx())
      return cxx->handlerThunk;
    return 0;
  }
};

struct CallTarget {
  struct ToFunction {
    FunctionNode* node;
  };
  struct ToImport {
    uint32_t address;
    std::string name;
  };
  struct Unresolved {
    uint32_t address;
  };

  std::variant<ToFunction, ToImport, Unresolved> value;

  bool isResolved() const { return !std::holds_alternative<Unresolved>(value); }
  bool isFunction() const { return std::holds_alternative<ToFunction>(value); }
  bool isImport() const { return std::holds_alternative<ToImport>(value); }

  FunctionNode* asFunction() const {
    if (auto* f = std::get_if<ToFunction>(&value))
      return f->node;
    return nullptr;
  }

  static CallTarget function(FunctionNode* fn) { return {ToFunction{fn}}; }
  static CallTarget import(uint32_t addr, std::string name) {
    return {ToImport{addr, std::move(name)}};
  }
  static CallTarget unresolved(uint32_t addr) { return {Unresolved{addr}}; }
};

struct CallEdge {
  uint32_t site;
  CallTarget target;
};

struct Block {
  uint32_t base;
  uint32_t size;

  uint32_t end() const { return base + size; }
  bool contains(uint32_t addr) const { return addr >= base && addr < end(); }
};

struct JumpTable {
  uint32_t bctrAddress;
  uint32_t tableAddress;
  uint8_t indexRegister;
  std::vector<uint32_t> targets;
};

struct FunctionAnalysis {
  enum class CsrRequirement : uint8_t { None, Fpu, Vmx };

  bool usesCtr = false;
  bool usesXer = false;
  bool usesCr = false;
  bool usesFpscr = false;

  CsrRequirement csrRequirement = CsrRequirement::None;
};

struct UnresolvedJump {
  uint32_t site;
  uint32_t target;
  bool isCall;
  bool isConditional;
};

struct CodeBuffer {
  std::vector<uint8_t> data;
  uint32_t baseAddress = 0;

  uint32_t size() const { return static_cast<uint32_t>(data.size()); }
  uint32_t endAddress() const { return baseAddress + size(); }

  bool contains(uint32_t addr) const { return addr >= baseAddress && addr < endAddress(); }

  const uint8_t* translate(uint32_t addr) const {
    if (!contains(addr))
      return nullptr;
    return data.data() + (addr - baseAddress);
  }
};

}
