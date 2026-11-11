/**
 * @file        rexcodegen/builders/system.cpp
 * @brief       PPC system instruction code generation
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

bool BuildNop(BuilderContext& ctx) {
  (void)ctx;
  return true;
}

bool BuildAttn(BuilderContext& ctx) {
  (void)ctx;
  return true;
}

bool BuildSync(BuilderContext& ctx) {
  ctx.println("	std::atomic_thread_fence(std::memory_order_seq_cst);");
  return true;
}

bool BuildIsync(BuilderContext& ctx) {
  (void)ctx;
  return true;
}

bool BuildIcbi(BuilderContext& ctx) {
  (void)ctx;
  return true;
}

bool BuildLwsync(BuilderContext& ctx) {
  ctx.println("	std::atomic_thread_fence(std::memory_order_acq_rel);");
  return true;
}

bool BuildEieio(BuilderContext& ctx) {
  ctx.println("	std::atomic_thread_fence(std::memory_order_seq_cst);");
  return true;
}

bool BuildDb16cyc(BuilderContext& ctx) {
  (void)ctx;
  return true;
}

bool BuildCctpl(BuilderContext& ctx) {
  (void)ctx;
  return true;
}

bool BuildCctpm(BuilderContext& ctx) {
  (void)ctx;
  return true;
}

bool BuildCctph(BuilderContext& ctx) {
  (void)ctx;
  return true;
}

bool BuildTdi(BuilderContext& ctx) {
  uint32_t to = (ctx.insn.instruction >> 21) & 0x1F;
  uint32_t ra = (ctx.insn.instruction >> 16) & 0x1F;
  int64_t simm = static_cast<int16_t>(ctx.insn.instruction & 0xFFFF);
  emitTrap(ctx, to, fmt::format("{}.s64", ctx.r(ra)), fmt::format("{}.u64", ctx.r(ra)),
           fmt::format("{}ll", simm), fmt::format("{}ull", static_cast<uint64_t>(simm)));
  return true;
}

bool BuildTwi(BuilderContext& ctx) {
  uint32_t to = (ctx.insn.instruction >> 21) & 0x1F;
  uint32_t ra = (ctx.insn.instruction >> 16) & 0x1F;
  int32_t simm = static_cast<int16_t>(ctx.insn.instruction & 0xFFFF);

  if (to == 0x1F && ra == 0) {
    uint16_t trap_type = static_cast<uint16_t>(simm);
    ctx.println("\tppc_trap(ctx, base, {});", trap_type);
    return true;
  }

  emitTrap(ctx, to, fmt::format("{}.s32", ctx.r(ra)), fmt::format("{}.u32", ctx.r(ra)),
           fmt::format("{}", simm), fmt::format("{}u", static_cast<uint32_t>(simm)));
  return true;
}

bool BuildTd(BuilderContext& ctx) {
  uint32_t to = (ctx.insn.instruction >> 21) & 0x1F;
  uint32_t ra = (ctx.insn.instruction >> 16) & 0x1F;
  uint32_t rb = (ctx.insn.instruction >> 11) & 0x1F;
  emitTrap(ctx, to, fmt::format("{}.s64", ctx.r(ra)), fmt::format("{}.u64", ctx.r(ra)),
           fmt::format("{}.s64", ctx.r(rb)), fmt::format("{}.u64", ctx.r(rb)));
  return true;
}

bool BuildTw(BuilderContext& ctx) {
  uint32_t to = (ctx.insn.instruction >> 21) & 0x1F;
  uint32_t ra = (ctx.insn.instruction >> 16) & 0x1F;
  uint32_t rb = (ctx.insn.instruction >> 11) & 0x1F;
  emitTrap(ctx, to, fmt::format("{}.s32", ctx.r(ra)), fmt::format("{}.u32", ctx.r(ra)),
           fmt::format("{}.s32", ctx.r(rb)), fmt::format("{}.u32", ctx.r(rb)));
  return true;
}

bool BuildDcbf(BuilderContext& ctx) {
  (void)ctx;
  return true;
}

bool BuildDcbt(BuilderContext& ctx) {
  (void)ctx;
  return true;
}

bool BuildDcbtst(BuilderContext& ctx) {
  (void)ctx;
  return true;
}

bool BuildDcbz(BuilderContext& ctx) {
  ctx.print("\t{} = (", ctx.ea());
  if (ctx.insn.operands[0] != 0)
    ctx.print("{}.u32 + ", ctx.r(ctx.insn.operands[0]));
  ctx.println("{}.u32) & ~127;", ctx.r(ctx.insn.operands[1]));
  ctx.println("\tmemset((void*)REX_RAW_ADDR({}), 0, 128);", ctx.ea());
  return true;
}

bool BuildDcbzl(BuilderContext& ctx) {
  ctx.print("\t{} = (", ctx.ea());
  if (ctx.insn.operands[0] != 0)
    ctx.print("{}.u32 + ", ctx.r(ctx.insn.operands[0]));
  ctx.println("{}.u32) & ~127;", ctx.r(ctx.insn.operands[1]));
  ctx.println("\tmemset((void*)REX_RAW_ADDR({}), 0, 128);", ctx.ea());
  return true;
}

bool BuildDcbst(BuilderContext& ctx) {
  (void)ctx;
  return true;
}

bool BuildMr(BuilderContext& ctx) {
  ctx.println("\t{}.u64 = {}.u64;", ctx.r(ctx.insn.operands[0]), ctx.r(ctx.insn.operands[1]));
  emitRecordFormCompare(ctx);

  if (ctx.locals.is_mmio_base(ctx.insn.operands[1]))
    ctx.locals.set_mmio_base(ctx.insn.operands[0]);
  else
    ctx.locals.clear_mmio_base(ctx.insn.operands[0]);

  return true;
}

bool BuildMcrf(BuilderContext& ctx) {
  ctx.println("\t{0} = {1};", ctx.cr(ctx.insn.operands[0]), ctx.cr(ctx.insn.operands[1]));
  return true;
}

bool BuildMcrfs(BuilderContext& ctx) {
  const uint32_t shift = 4 * (7 - ctx.insn.operands[1]);

  constexpr uint32_t kExceptionFlags = 0x9FF80700;
  const uint32_t clear_mask = (0xFu << shift) & kExceptionFlags;
  ctx.println("\t{{");
  ctx.println("\t\tconst uint32_t fpscr = ctx.fpscr.loadFromHost();");
  ctx.println("\t\t{}.set_raw((fpscr >> {}) & 0xF);", ctx.cr(ctx.insn.operands[0]), shift);
  if (clear_mask)
    ctx.println("\t\tctx.fpscr.storeFromGuest(fpscr & 0x{:08X});", ~clear_mask);
  ctx.println("\t}}");
  return true;
}

bool BuildMfxer(BuilderContext& ctx) {
  ctx.println("\t{}.u64 = ({}.so << 31) | ({}.ov << 30) | ({}.ca << 29);",
              ctx.r(ctx.insn.operands[0]), ctx.xer(), ctx.xer(), ctx.xer());
  return true;
}

bool BuildMfctr(BuilderContext& ctx) {
  ctx.println("\t{}.u64 = {}.u64;", ctx.r(ctx.insn.operands[0]), ctx.ctr());
  return true;
}

bool BuildMfcr(BuilderContext& ctx) {
  for (size_t i = 0; i < 32; i++) {
    constexpr std::string_view fields[] = {"lt", "gt", "eq", "so"};
    ctx.println("\t{}.u64 {}= {}.{} ? 0x{:X} : 0;", ctx.r(ctx.insn.operands[0]), i == 0 ? "" : "|",
                ctx.cr(i / 4), fields[i % 4], 1u << (31 - i));
  }
  return true;
}

bool BuildMfocrf(BuilderContext& ctx) {
  uint32_t fxm = ctx.insn.operands[1];
  uint32_t crField = 0;
  for (uint32_t i = 0; i < 8; i++) {
    if (fxm & (0x80u >> i)) {
      crField = i;
      break;
    }
  }
  uint32_t baseShift = 28 - 4 * crField;
  ctx.println("\t{}.u64 = ({}.lt << {}) | ({}.gt << {}) | ({}.eq << {}) | ({}.so << {});",
              ctx.r(ctx.insn.operands[0]), ctx.cr(crField), baseShift + 3, ctx.cr(crField),
              baseShift + 2, ctx.cr(crField), baseShift + 1, ctx.cr(crField), baseShift);
  return true;
}

bool BuildMflr(BuilderContext& ctx) {
  if (!ctx.config().skipLr)
    ctx.println("\t{}.u64 = ctx.lr;", ctx.r(ctx.insn.operands[0]));
  return true;
}

bool BuildMfmsr(BuilderContext& ctx) {
  if (!ctx.config().skipMsr) {
    ctx.println("\tstd::atomic_thread_fence(std::memory_order_seq_cst);");

    ctx.println("\t{}.u64 = 0x1030 | REX_CHECK_GLOBAL_LOCK();", ctx.r(ctx.insn.operands[0]));
  }
  return true;
}

bool BuildMffs(BuilderContext& ctx) {
  ctx.println("\t{}.u64 = ctx.fpscr.loadFromHost();", ctx.f(ctx.insn.operands[0]));
  return true;
}

bool BuildMftb(BuilderContext& ctx) {
  ctx.println("\t{}.u64 = REX_QUERY_TIMEBASE();", ctx.r(ctx.insn.operands[0]));
  return true;
}

bool BuildMftbu(BuilderContext& ctx) {
  ctx.println("\t{}.u64 = REX_QUERY_TIMEBASE() >> 32;", ctx.r(ctx.insn.operands[0]));
  return true;
}

bool BuildMtcr(BuilderContext& ctx) {
  for (size_t i = 0; i < 32; i++) {
    constexpr std::string_view fields[] = {"lt", "gt", "eq", "so"};
    ctx.println("\t{}.{} = ({}.u32 & 0x{:X}) != 0;", ctx.cr(i / 4), fields[i % 4],
                ctx.r(ctx.insn.operands[0]), 1u << (31 - i));
  }
  return true;
}

bool BuildMtcrf(BuilderContext& ctx) {
  uint32_t fxm = ctx.insn.operands[0];
  constexpr std::string_view names[] = {"lt", "gt", "eq", "so"};
  for (uint32_t field = 0; field < 8; field++) {
    if (fxm & (0x80u >> field)) {
      uint32_t base_bit = 28 - 4 * field;
      for (int b = 0; b < 4; b++) {
        ctx.println("\t{}.{} = ({}.u32 & 0x{:X}) != 0;", ctx.cr(field), names[b],
                    ctx.r(ctx.insn.operands[1]), 1u << (base_bit + 3 - b));
      }
    }
  }
  return true;
}

bool BuildMtctr(BuilderContext& ctx) {
  ctx.println("\t{}.u64 = {}.u64;", ctx.ctr(), ctx.r(ctx.insn.operands[0]));
  return true;
}

bool BuildMtlr(BuilderContext& ctx) {
  if (!ctx.config().skipLr)
    ctx.println("\tctx.lr = {}.u64;", ctx.r(ctx.insn.operands[0]));
  return true;
}

bool BuildMtmsrd(BuilderContext& ctx) {
  if (!ctx.config().skipMsr) {
    ctx.println("\tstd::atomic_thread_fence(std::memory_order_seq_cst);");

    ctx.println("\t{{");
    ctx.println("\t\tconst uint32_t next_msr = ({}.u32 & 0x8020) | (ctx.msr & ~0x8020);",
                ctx.r(ctx.insn.operands[0]));
    ctx.println("\t\tif ((ctx.msr ^ next_msr) & 0x8000) {{");
    ctx.println("\t\t\tif (next_msr & 0x8000) {{ REX_LEAVE_GLOBAL_LOCK(); }}");
    ctx.println("\t\t\telse {{ REX_ENTER_GLOBAL_LOCK(); }}");
    ctx.println("\t\t}}");
    ctx.println("\t\tctx.msr = next_msr;");
    ctx.println("\t}}");
  }
  return true;
}

bool BuildMtfsf(BuilderContext& ctx) {
  uint32_t fm = ctx.insn.operands[0];
  uint32_t mask = 0;
  for (int j = 0; j < 8; j++) {
    if (fm & (1 << (7 - j)))
      mask |= 0xFu << (4 * (7 - j));
  }
  if (mask == 0xFFFFFFFF) {
    ctx.println("\tctx.fpscr.storeFromGuest({}.u32);", ctx.f(ctx.insn.operands[1]));
  } else {
    ctx.println(
        "\tctx.fpscr.storeFromGuest((ctx.fpscr.loadFromHost() & 0x{:08X}) | ({}.u32 & 0x{:08X}));",
        ~mask, ctx.f(ctx.insn.operands[1]), mask);
  }
  return true;
}

bool BuildMtxer(BuilderContext& ctx) {
  ctx.println("\t{}.so = ({}.u64 & 0x80000000) != 0;", ctx.xer(), ctx.r(ctx.insn.operands[0]));
  ctx.println("\t{}.ov = ({}.u64 & 0x40000000) != 0;", ctx.xer(), ctx.r(ctx.insn.operands[0]));
  ctx.println("\t{}.ca = ({}.u64 & 0x20000000) != 0;", ctx.xer(), ctx.r(ctx.insn.operands[0]));
  return true;
}

bool BuildClrldi(BuilderContext& ctx) {
  ctx.println("\t{}.u64 = {}.u64 & 0x{:X};", ctx.r(ctx.insn.operands[0]),
              ctx.r(ctx.insn.operands[1]), (1ull << (64 - ctx.insn.operands[2])) - 1);
  emitRecordFormCompare(ctx);
  return true;
}

}
