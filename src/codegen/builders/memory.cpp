/**
 * @file        rexcodegen/builders/memory.cpp
 * @brief       PPC memory instruction code generation
 *
 * @copyright   Copyright (c) 2026 Tom Clay <tomc@tctechstuff.com>
 *              All rights reserved.
 *
 * @license     BSD 3-Clause License
 *              See LICENSE file in the project root for full license text.
 */

#include "builder_context.h"
#include "helpers.h"

namespace rex::codegen {

bool BuildLi(BuilderContext& ctx) {
  ctx.println("\t{}.s64 = {};", ctx.r(ctx.insn.operands[0]),
              static_cast<int32_t>(ctx.insn.operands[1]));
  return true;
}

bool BuildLis(BuilderContext& ctx) {
  uint32_t imm = static_cast<uint32_t>(ctx.insn.operands[1]);
  size_t dest_reg = ctx.insn.operands[0];

  ctx.println("\t{}.s64 = {};", ctx.r(dest_reg), static_cast<int32_t>(imm << 16));

  if (isMMIOUpperBits(imm)) {
    ctx.locals.set_mmio_base(dest_reg);
  } else {
    ctx.locals.clear_mmio_base(dest_reg);
  }

  return true;
}

bool BuildLbz(BuilderContext& ctx) {
  ctx.emit_load_d_form("REX_LOAD_U8", "u64");
  return true;
}

bool BuildLbzu(BuilderContext& ctx) {
  emitLoadWithUpdate(ctx, "REX_LOAD_U8");
  return true;
}

bool BuildLbzx(BuilderContext& ctx) {
  ctx.emit_load_x_form("REX_LOAD_U8", "u64");
  return true;
}

bool BuildLbzux(BuilderContext& ctx) {
  emitLoadXFormWithUpdate(ctx, "REX_LOAD_U8");
  return true;
}

bool BuildLha(BuilderContext& ctx) {
  emitSignExtendLoadDForm(ctx, "int16_t", "REX_LOAD_U16");
  return true;
}

bool BuildLhax(BuilderContext& ctx) {
  emitSignExtendLoadXForm(ctx, "int16_t", "REX_LOAD_U16");
  return true;
}

bool BuildLhz(BuilderContext& ctx) {
  ctx.emit_load_d_form("REX_LOAD_U16", "u64");
  return true;
}

bool BuildLhzx(BuilderContext& ctx) {
  ctx.emit_load_x_form("REX_LOAD_U16", "u64");
  return true;
}

bool BuildLhzu(BuilderContext& ctx) {
  emitLoadWithUpdate(ctx, "REX_LOAD_U16");
  return true;
}

bool BuildLhzux(BuilderContext& ctx) {
  emitLoadXFormWithUpdate(ctx, "REX_LOAD_U16");
  return true;
}

bool BuildLhau(BuilderContext& ctx) {
  ctx.println("\t{} = {} + {}.u32;", ctx.ea(), static_cast<int32_t>(ctx.insn.operands[1]),
              ctx.r(ctx.insn.operands[2]));
  ctx.println("\t{}.s64 = int16_t(REX_LOAD_U16({}));", ctx.r(ctx.insn.operands[0]), ctx.ea());
  ctx.println("\t{}.u32 = {};", ctx.r(ctx.insn.operands[2]), ctx.ea());
  return true;
}

bool BuildLhaux(BuilderContext& ctx) {
  ctx.println("\t{} = {}.u32 + {}.u32;", ctx.ea(), ctx.r(ctx.insn.operands[1]),
              ctx.r(ctx.insn.operands[2]));
  ctx.println("\t{}.s64 = int16_t(REX_LOAD_U16({}));", ctx.r(ctx.insn.operands[0]), ctx.ea());
  ctx.println("\t{}.u32 = {};", ctx.r(ctx.insn.operands[1]), ctx.ea());
  return true;
}

bool BuildLhbrx(BuilderContext& ctx) {
  ctx.print("\t{}.u64 = __builtin_bswap16({}(", ctx.r(ctx.insn.operands[0]),
            ctx.mmio_check_x_form() ? "REX_MM_LOAD_U16" : "REX_LOAD_U16");
  if (ctx.insn.operands[1] != 0)
    ctx.print("{}.u32 + ", ctx.r(ctx.insn.operands[1]));
  ctx.println("{}.u32));", ctx.r(ctx.insn.operands[2]));
  return true;
}

bool BuildLwa(BuilderContext& ctx) {
  emitSignExtendLoadDForm(ctx, "int32_t", "REX_LOAD_U32");
  return true;
}

bool BuildLwaux(BuilderContext& ctx) {
  ctx.println("\t{} = {}.u32 + {}.u32;", ctx.ea(), ctx.r(ctx.insn.operands[1]),
              ctx.r(ctx.insn.operands[2]));
  ctx.println("\t{}.s64 = int32_t(REX_LOAD_U32({}));", ctx.r(ctx.insn.operands[0]), ctx.ea());
  ctx.println("\t{}.u32 = {};", ctx.r(ctx.insn.operands[1]), ctx.ea());
  return true;
}

bool BuildLwax(BuilderContext& ctx) {
  emitSignExtendLoadXForm(ctx, "int32_t", "REX_LOAD_U32");
  return true;
}

bool BuildLwz(BuilderContext& ctx) {
  ctx.emit_load_d_form("REX_LOAD_U32", "u64");
  return true;
}

bool BuildLmw(BuilderContext& ctx) {
  const auto first = ctx.insn.operands[0];
  const auto ra = ctx.insn.operands[2];
  const auto displacement = static_cast<int16_t>(ctx.insn.operands[1]);
  if (ra)
    ctx.println("\t{} = {}.u32 + int32_t({});", ctx.ea(), ctx.r(ra), displacement);
  else
    ctx.println("\t{} = uint32_t(int32_t({}));", ctx.ea(), displacement);
  for (uint32_t reg = first; reg < 32; ++reg)
    ctx.println("\t{}.u64 = REX_LOAD_U32({} + {});", ctx.r(reg), ctx.ea(), (reg - first) * 4);
  return true;
}

bool BuildLwzu(BuilderContext& ctx) {
  emitLoadWithUpdate(ctx, "REX_LOAD_U32");
  return true;
}

bool BuildLwzx(BuilderContext& ctx) {
  ctx.emit_load_x_form("REX_LOAD_U32", "u64");
  return true;
}

bool BuildLwzux(BuilderContext& ctx) {
  emitLoadXFormWithUpdate(ctx, "REX_LOAD_U32");
  return true;
}

bool BuildLwbrx(BuilderContext& ctx) {
  ctx.print("\t{}.u64 = __builtin_bswap32({}(", ctx.r(ctx.insn.operands[0]),
            ctx.mmio_check_x_form() ? "REX_MM_LOAD_U32" : "REX_LOAD_U32");
  if (ctx.insn.operands[1] != 0)
    ctx.print("{}.u32 + ", ctx.r(ctx.insn.operands[1]));
  ctx.println("{}.u32));", ctx.r(ctx.insn.operands[2]));
  return true;
}

bool BuildLd(BuilderContext& ctx) {
  ctx.emit_load_d_form("REX_LOAD_U64", "u64");
  return true;
}

bool BuildLdu(BuilderContext& ctx) {
  emitLoadWithUpdate(ctx, "REX_LOAD_U64");
  return true;
}

bool BuildLdbrx(BuilderContext& ctx) {
  ctx.print("	{}.u64 = __builtin_bswap64(REX_LOAD_U64(", ctx.r(ctx.insn.operands[0]));
  if (ctx.insn.operands[1] != 0)
    ctx.print("{}.u32 + ", ctx.r(ctx.insn.operands[1]));
  ctx.println("{}.u32));", ctx.r(ctx.insn.operands[2]));
  return true;
}

bool BuildLdx(BuilderContext& ctx) {
  ctx.emit_load_x_form("REX_LOAD_U64", "u64");
  return true;
}

bool BuildLdux(BuilderContext& ctx) {
  emitLoadXFormWithUpdate(ctx, "REX_LOAD_U64");
  return true;
}

bool BuildLwarx(BuilderContext& ctx) {
  emitAtomicLoadReserve(ctx, "uint32_t", "__builtin_bswap32", "u32");
  return true;
}

bool BuildLdarx(BuilderContext& ctx) {
  emitAtomicLoadReserve(ctx, "uint64_t", "__builtin_bswap64", "u64");
  return true;
}

bool BuildLfd(BuilderContext& ctx) {
  ctx.emit_set_flush_mode(false);
  ctx.print("\t{}.u64 = REX_LOAD_U64(", ctx.f(ctx.insn.operands[0]));
  if (ctx.insn.operands[2] != 0)
    ctx.print("{}.u32 + ", ctx.r(ctx.insn.operands[2]));
  ctx.println("{});", static_cast<int32_t>(ctx.insn.operands[1]));
  return true;
}

bool BuildLfdx(BuilderContext& ctx) {
  ctx.emit_set_flush_mode(false);
  ctx.print("\t{}.u64 = REX_LOAD_U64(", ctx.f(ctx.insn.operands[0]));
  if (ctx.insn.operands[1] != 0)
    ctx.print("{}.u32 + ", ctx.r(ctx.insn.operands[1]));
  ctx.println("{}.u32);", ctx.r(ctx.insn.operands[2]));
  return true;
}

bool BuildLfs(BuilderContext& ctx) {
  ctx.emit_set_flush_mode(false);
  ctx.print("\t{}.u32 = REX_LOAD_U32(", ctx.temp());
  if (ctx.insn.operands[2] != 0)
    ctx.print("{}.u32 + ", ctx.r(ctx.insn.operands[2]));
  ctx.println("{});", static_cast<int32_t>(ctx.insn.operands[1]));
  ctx.println("\t{}.f64 = rex::ppc::fp::load_single({}.u32);", ctx.f(ctx.insn.operands[0]),
              ctx.temp());
  return true;
}

bool BuildLfsx(BuilderContext& ctx) {
  ctx.emit_set_flush_mode(false);
  ctx.print("\t{}.u32 = REX_LOAD_U32(", ctx.temp());
  if (ctx.insn.operands[1] != 0)
    ctx.print("{}.u32 + ", ctx.r(ctx.insn.operands[1]));
  ctx.println("{}.u32);", ctx.r(ctx.insn.operands[2]));
  ctx.println("\t{}.f64 = rex::ppc::fp::load_single({}.u32);", ctx.f(ctx.insn.operands[0]),
              ctx.temp());
  return true;
}

bool BuildLfdu(BuilderContext& ctx) {
  ctx.emit_set_flush_mode(false);
  ctx.println("\t{} = {} + {}.u32;", ctx.ea(), static_cast<int32_t>(ctx.insn.operands[1]),
              ctx.r(ctx.insn.operands[2]));
  ctx.println("\t{}.u64 = REX_LOAD_U64({});", ctx.f(ctx.insn.operands[0]), ctx.ea());
  ctx.println("\t{}.u32 = {};", ctx.r(ctx.insn.operands[2]), ctx.ea());
  return true;
}

bool BuildLfdux(BuilderContext& ctx) {
  ctx.emit_set_flush_mode(false);
  ctx.println("\t{} = {}.u32 + {}.u32;", ctx.ea(), ctx.r(ctx.insn.operands[1]),
              ctx.r(ctx.insn.operands[2]));
  ctx.println("\t{}.u64 = REX_LOAD_U64({});", ctx.f(ctx.insn.operands[0]), ctx.ea());
  ctx.println("\t{}.u32 = {};", ctx.r(ctx.insn.operands[1]), ctx.ea());
  return true;
}

bool BuildLfsu(BuilderContext& ctx) {
  ctx.emit_set_flush_mode(false);
  ctx.println("\t{} = {} + {}.u32;", ctx.ea(), static_cast<int32_t>(ctx.insn.operands[1]),
              ctx.r(ctx.insn.operands[2]));
  ctx.println("\t{}.u32 = REX_LOAD_U32({});", ctx.temp(), ctx.ea());
  ctx.println("\t{}.f64 = rex::ppc::fp::load_single({}.u32);", ctx.f(ctx.insn.operands[0]),
              ctx.temp());
  ctx.println("\t{}.u32 = {};", ctx.r(ctx.insn.operands[2]), ctx.ea());
  return true;
}

bool BuildLfsux(BuilderContext& ctx) {
  ctx.emit_set_flush_mode(false);
  ctx.println("\t{} = {}.u32 + {}.u32;", ctx.ea(), ctx.r(ctx.insn.operands[1]),
              ctx.r(ctx.insn.operands[2]));
  ctx.println("\t{}.u32 = REX_LOAD_U32({});", ctx.temp(), ctx.ea());
  ctx.println("\t{}.f64 = rex::ppc::fp::load_single({}.u32);", ctx.f(ctx.insn.operands[0]),
              ctx.temp());
  ctx.println("\t{}.u32 = {};", ctx.r(ctx.insn.operands[1]), ctx.ea());
  return true;
}

bool BuildStb(BuilderContext& ctx) {
  ctx.emit_store_d_form("REX_STORE_U8", "u8", true);
  return true;
}

bool BuildStbu(BuilderContext& ctx) {
  emitStoreWithUpdate(ctx, "REX_STORE_U8", "u8");
  return true;
}

bool BuildStbx(BuilderContext& ctx) {
  ctx.emit_store_x_form("REX_STORE_U8", "u8", true);
  return true;
}

bool BuildStbux(BuilderContext& ctx) {
  emitStoreXFormWithUpdate(ctx, "REX_STORE_U8", "REX_MM_STORE_U8", "u8");
  return true;
}

bool BuildSth(BuilderContext& ctx) {
  ctx.emit_store_d_form("REX_STORE_U16", "u16", true);
  return true;
}

bool BuildSthbrx(BuilderContext& ctx) {
  ctx.print("{}", ctx.mmio_check_x_form() ? "\tREX_MM_STORE_U16(" : "\tREX_STORE_U16(");
  if (ctx.insn.operands[1] != 0)
    ctx.print("{}.u32 + ", ctx.r(ctx.insn.operands[1]));
  ctx.println("{}.u32, __builtin_bswap16({}.u16));", ctx.r(ctx.insn.operands[2]),
              ctx.r(ctx.insn.operands[0]));
  return true;
}

bool BuildSthx(BuilderContext& ctx) {
  ctx.emit_store_x_form("REX_STORE_U16", "u16", true);
  return true;
}

bool BuildSthu(BuilderContext& ctx) {
  emitStoreWithUpdate(ctx, "REX_STORE_U16", "u16");
  return true;
}

bool BuildSthux(BuilderContext& ctx) {
  emitStoreXFormWithUpdate(ctx, "REX_STORE_U16", "REX_MM_STORE_U16", "u16");
  return true;
}

bool BuildStw(BuilderContext& ctx) {
  ctx.emit_store_d_form("REX_STORE_U32", "u32", true);
  return true;
}

bool BuildStwu(BuilderContext& ctx) {
  emitStoreWithUpdate(ctx, "REX_STORE_U32", "u32");
  return true;
}

bool BuildStwux(BuilderContext& ctx) {
  emitStoreXFormWithUpdate(ctx, "REX_STORE_U32", "REX_MM_STORE_U32", "u32");
  return true;
}

bool BuildStwx(BuilderContext& ctx) {
  ctx.emit_store_x_form("REX_STORE_U32", "u32", true);
  return true;
}

bool BuildStmw(BuilderContext& ctx) {
  uint32_t rS = ctx.insn.operands[0];
  int32_t offset = static_cast<int32_t>(ctx.insn.operands[1]);
  uint32_t rA = ctx.insn.operands[2];
  if (rA != 0)
    ctx.println("\t{} = {}.u32 + {};", ctx.ea(), ctx.r(rA), offset);
  else
    ctx.println("\t{} = {};", ctx.ea(), offset);
  for (uint32_t i = rS; i <= 31; ++i) {
    ctx.println("\tREX_STORE_U32({} + {}, {}.u32);", ctx.ea(), (i - rS) * 4, ctx.r(i));
  }
  return true;
}

bool BuildStwbrx(BuilderContext& ctx) {
  ctx.print("{}", ctx.mmio_check_x_form() ? "\tREX_MM_STORE_U32(" : "\tREX_STORE_U32(");
  if (ctx.insn.operands[1] != 0)
    ctx.print("{}.u32 + ", ctx.r(ctx.insn.operands[1]));
  ctx.println("{}.u32, __builtin_bswap32({}.u32));", ctx.r(ctx.insn.operands[2]),
              ctx.r(ctx.insn.operands[0]));
  return true;
}

bool BuildStwcx(BuilderContext& ctx) {
  emitAtomicStoreConditional(ctx, "uint32_t", "__builtin_bswap32", "u32");
  return true;
}

bool BuildStdcx(BuilderContext& ctx) {
  emitAtomicStoreConditional(ctx, "uint64_t", "__builtin_bswap64", "u64");
  return true;
}

bool BuildStd(BuilderContext& ctx) {
  ctx.emit_store_d_form("REX_STORE_U64", "u64", true);
  return true;
}

bool BuildStdu(BuilderContext& ctx) {
  emitStoreWithUpdate(ctx, "REX_STORE_U64", "u64");
  return true;
}

bool BuildStdbrx(BuilderContext& ctx) {
  ctx.print("{}", ctx.mmio_check_x_form() ? "	REX_MM_STORE_U64(" : "	REX_STORE_U64(");
  if (ctx.insn.operands[1] != 0)
    ctx.print("{}.u32 + ", ctx.r(ctx.insn.operands[1]));
  ctx.println("{}.u32, __builtin_bswap64({}.u64));", ctx.r(ctx.insn.operands[2]),
              ctx.r(ctx.insn.operands[0]));
  return true;
}

bool BuildStdx(BuilderContext& ctx) {
  ctx.emit_store_x_form("REX_STORE_U64", "u64", true);
  return true;
}

bool BuildStdux(BuilderContext& ctx) {
  emitStoreXFormWithUpdate(ctx, "REX_STORE_U64", "REX_MM_STORE_U64", "u64");
  return true;
}

bool BuildStfd(BuilderContext& ctx) {
  ctx.emit_set_flush_mode(false);
  ctx.print("{}", ctx.mmio_check_d_form() ? "\tREX_MM_STORE_U64(" : "\tREX_STORE_U64(");
  if (ctx.insn.operands[2] != 0)
    ctx.print("{}.u32 + ", ctx.r(ctx.insn.operands[2]));
  ctx.println("{}, {}.u64);", static_cast<int32_t>(ctx.insn.operands[1]),
              ctx.f(ctx.insn.operands[0]));
  return true;
}

bool BuildStfdx(BuilderContext& ctx) {
  ctx.emit_set_flush_mode(false);
  ctx.print("{}", ctx.mmio_check_x_form() ? "\tREX_MM_STORE_U64(" : "\tREX_STORE_U64(");
  if (ctx.insn.operands[1] != 0)
    ctx.print("{}.u32 + ", ctx.r(ctx.insn.operands[1]));
  ctx.println("{}.u32, {}.u64);", ctx.r(ctx.insn.operands[2]), ctx.f(ctx.insn.operands[0]));
  return true;
}

bool BuildStfdux(BuilderContext& ctx) {
  ctx.emit_set_flush_mode(false);
  ctx.println("\t{} = {}.u32 + {}.u32;", ctx.ea(), ctx.r(ctx.insn.operands[1]),
              ctx.r(ctx.insn.operands[2]));
  ctx.println("\tREX_STORE_U64({}, {}.u64);", ctx.ea(), ctx.f(ctx.insn.operands[0]));
  ctx.println("\t{}.u32 = {};", ctx.r(ctx.insn.operands[1]), ctx.ea());
  return true;
}

bool BuildStfiwx(BuilderContext& ctx) {
  ctx.emit_set_flush_mode(false);
  ctx.print("{}", ctx.mmio_check_x_form() ? "\tREX_MM_STORE_U32(" : "\tREX_STORE_U32(");
  if (ctx.insn.operands[1] != 0)
    ctx.print("{}.u32 + ", ctx.r(ctx.insn.operands[1]));
  ctx.println("{}.u32, {}.u32);", ctx.r(ctx.insn.operands[2]), ctx.f(ctx.insn.operands[0]));
  return true;
}

bool BuildStfs(BuilderContext& ctx) {
  ctx.emit_set_flush_mode(false);
  ctx.println("\t{}.u32 = rex::ppc::fp::store_single({}.f64);", ctx.temp(),
              ctx.f(ctx.insn.operands[0]));
  ctx.print("{}", ctx.mmio_check_d_form() ? "\tREX_MM_STORE_U32(" : "\tREX_STORE_U32(");
  if (ctx.insn.operands[2] != 0)
    ctx.print("{}.u32 + ", ctx.r(ctx.insn.operands[2]));
  ctx.println("{}, {}.u32);", static_cast<int32_t>(ctx.insn.operands[1]), ctx.temp());
  return true;
}

bool BuildStfsx(BuilderContext& ctx) {
  ctx.emit_set_flush_mode(false);
  ctx.println("\t{}.u32 = rex::ppc::fp::store_single({}.f64);", ctx.temp(),
              ctx.f(ctx.insn.operands[0]));
  ctx.print("{}", ctx.mmio_check_x_form() ? "\tREX_MM_STORE_U32(" : "\tREX_STORE_U32(");
  if (ctx.insn.operands[1] != 0)
    ctx.print("{}.u32 + ", ctx.r(ctx.insn.operands[1]));
  ctx.println("{}.u32, {}.u32);", ctx.r(ctx.insn.operands[2]), ctx.temp());
  return true;
}

bool BuildStfdu(BuilderContext& ctx) {
  ctx.emit_set_flush_mode(false);
  ctx.println("\t{} = {} + {}.u32;", ctx.ea(), static_cast<int32_t>(ctx.insn.operands[1]),
              ctx.r(ctx.insn.operands[2]));
  ctx.println("\tREX_STORE_U64({}, {}.u64);", ctx.ea(), ctx.f(ctx.insn.operands[0]));
  ctx.println("\t{}.u32 = {};", ctx.r(ctx.insn.operands[2]), ctx.ea());
  return true;
}

bool BuildStfsu(BuilderContext& ctx) {
  ctx.emit_set_flush_mode(false);
  ctx.println("\t{} = {} + {}.u32;", ctx.ea(), static_cast<int32_t>(ctx.insn.operands[1]),
              ctx.r(ctx.insn.operands[2]));
  ctx.println("\t{}.u32 = rex::ppc::fp::store_single({}.f64);", ctx.temp(),
              ctx.f(ctx.insn.operands[0]));
  ctx.println("\tREX_STORE_U32({}, {}.u32);", ctx.ea(), ctx.temp());
  ctx.println("\t{}.u32 = {};", ctx.r(ctx.insn.operands[2]), ctx.ea());
  return true;
}

bool BuildStfsux(BuilderContext& ctx) {
  ctx.emit_set_flush_mode(false);
  ctx.println("\t{}.u32 = rex::ppc::fp::store_single({}.f64);", ctx.temp(),
              ctx.f(ctx.insn.operands[0]));
  ctx.println("\t{} = {}.u32 + {}.u32;", ctx.ea(), ctx.r(ctx.insn.operands[1]),
              ctx.r(ctx.insn.operands[2]));
  ctx.println("\t{}({}, {}.u32);", ctx.mmio_check_x_form() ? "REX_MM_STORE_U32" : "REX_STORE_U32",
              ctx.ea(), ctx.temp());
  ctx.println("\t{}.u32 = {};", ctx.r(ctx.insn.operands[1]), ctx.ea());
  return true;
}

bool BuildLvx(BuilderContext& ctx) {
  emitVectorEA(ctx, "0xF");
  ctx.println(
      "\tsimde_mm_store_si128((simde__m128i*){}.u8, "
      "simde_mm_shuffle_epi8(simde_mm_load_si128((simde__m128i*)REX_RAW_ADDR({})), "
      "simde_mm_load_si128((simde__m128i*)VectorMaskL)));",
      ctx.v(ctx.insn.operands[0]), ctx.ea());
  return true;
}

bool BuildLvlx(BuilderContext& ctx) {
  emitVectorTempEA(ctx);
  ctx.println(
      "\tsimde_mm_store_si128((simde__m128i*){}.u8, "
      "simde_mm_shuffle_epi8(simde_mm_load_si128((simde__m128i*)REX_RAW_ADDR({}.u32 & ~0xF)), "
      "simde_mm_load_si128((simde__m128i*)&VectorMaskL[({}.u32 & 0xF) * 16])));",
      ctx.v(ctx.insn.operands[0]), ctx.temp(), ctx.temp());
  return true;
}

bool BuildLvrx(BuilderContext& ctx) {
  emitVectorTempEA(ctx);
  ctx.println(
      "\tsimde_mm_store_si128((simde__m128i*){}.u8, {}.u32 & 0xF ? "
      "simde_mm_shuffle_epi8(simde_mm_load_si128((simde__m128i*)REX_RAW_ADDR({}.u32 & ~0xF)), "
      "simde_mm_load_si128((simde__m128i*)&VectorMaskR[({}.u32 & 0xF) * 16])) : "
      "simde_mm_setzero_si128());",
      ctx.v(ctx.insn.operands[0]), ctx.temp(), ctx.temp(), ctx.temp());
  return true;
}

bool BuildLvsl(BuilderContext& ctx) {
  emitVectorTempEA(ctx);
  ctx.println(
      "\tsimde_mm_store_si128((simde__m128i*){}.u8, "
      "simde_mm_load_si128((simde__m128i*)&VectorShiftTableL[({}.u32 & 0xF) * 16]));",
      ctx.v(ctx.insn.operands[0]), ctx.temp());
  return true;
}

bool BuildLvsr(BuilderContext& ctx) {
  emitVectorTempEA(ctx);
  ctx.println(
      "\tsimde_mm_store_si128((simde__m128i*){}.u8, "
      "simde_mm_load_si128((simde__m128i*)&VectorShiftTableR[({}.u32 & 0xF) * 16]));",
      ctx.v(ctx.insn.operands[0]), ctx.temp());
  return true;
}

bool BuildStvebx(BuilderContext& ctx) {
  emitVectorEA(ctx);
  ctx.println("\tREX_STORE_U8({}, {}.u8[15 - ({} & 0xF)]);", ctx.ea(), ctx.v(ctx.insn.operands[0]),
              ctx.ea());
  return true;
}

bool BuildStvehx(BuilderContext& ctx) {
  emitVectorEA(ctx, "0x1");
  ctx.println("\tREX_STORE_U16(ea, {}.u16[7 - (({} & 0xF) >> 1)]);", ctx.v(ctx.insn.operands[0]),
              ctx.ea());
  return true;
}

bool BuildStvewx(BuilderContext& ctx) {
  emitVectorEA(ctx, "0x3");
  ctx.println("\tREX_STORE_U32(ea, {}.u32[3 - (({} & 0xF) >> 2)]);", ctx.v(ctx.insn.operands[0]),
              ctx.ea());
  return true;
}

bool BuildStvlx(BuilderContext& ctx) {
  emitVectorEA(ctx);

  ctx.println("\tfor (size_t i = 0; i < (16 - ({} & 0xF)); i++)", ctx.ea());
  ctx.println("\t\tREX_STORE_U8({} + i, {}.u8[15 - i]);", ctx.ea(), ctx.v(ctx.insn.operands[0]));
  return true;
}

bool BuildStvrx(BuilderContext& ctx) {
  emitVectorEA(ctx);

  ctx.println("\tfor (size_t i = 0; i < ({} & 0xF); i++)", ctx.ea());
  ctx.println("\t\tREX_STORE_U8({} - i - 1, {}.u8[i]);", ctx.ea(), ctx.v(ctx.insn.operands[0]));
  return true;
}

bool BuildStvx(BuilderContext& ctx) {
  emitVectorEA(ctx, "0xF");
  ctx.println(
      "\tsimde_mm_store_si128((simde__m128i*)REX_RAW_ADDR({}), "
      "simde_mm_shuffle_epi8(simde_mm_load_si128((simde__m128i*){}.u8), "
      "simde_mm_load_si128((simde__m128i*)VectorMaskL)));",
      ctx.ea(), ctx.v(ctx.insn.operands[0]));
  return true;
}

}
