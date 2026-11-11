/**
 * @file        guest_draw.h
 * @brief       Guest draw helpers for GPU fixtures: hand-assembled shaders and
 *              constant-color rectangles (RG-GDK-009, RG-GDK-010)
 *
 * @copyright   Copyright (c) 2026 Tom Clay
 * @license     BSD 3-Clause License
 */

#pragma once

#include <bit>
#include <cstdint>
#include <vector>

#include <rex/graphics/format/ucode.h>
#include <rex/graphics/pipeline/texture/util.h>
#include <rex/graphics/registers.h>
#include <rex/graphics/xenos.h>

#include "gpu_fixture.h"

namespace rex::testing::guest_draw {

using namespace rex::graphics;  // NOLINT: XE_GPU_REG_* register indices.
namespace reg = rex::graphics::reg;
namespace xenos = rex::graphics::xenos;
namespace ucode = rex::graphics::ucode;

inline uint32_t PackXY(uint32_t x, uint32_t y) {
  return x | (y << 16);
}

inline const std::vector<uint32_t> kVertexShader = {
    0x00011002, 0x00001000, 0xC2000000, 0x00001003, 0x00002000, 0x00000000,
    0x00081000, 0x00260688, 0x00000004, 0xC80F803E, 0x00000000, 0xC2010100,
};

inline const std::vector<uint32_t> kPixelShader = {
    0x00000000, 0x1001C400, 0x20000000, 0xC80F8000, 0x00000000, 0x02000000,
};

inline std::vector<uint32_t> LoadShader(xenos::ShaderType type,
                                        const std::vector<uint32_t>& ucode) {
  std::vector<uint32_t> packet = {
      xenos::MakePacketType3(xenos::PM4_IM_LOAD_IMMEDIATE, uint32_t(2 + ucode.size())),
      uint32_t(type), uint32_t(ucode.size())};
  packet.insert(packet.end(), ucode.begin(), ucode.end());
  return packet;
}

struct Surface {
  xenos::MsaaSamples msaa;

  uint32_t pitch;
};

struct DrawOptions {
  float z = 0.0f;
  float z_right = -1.0f;
  uint32_t color_base_tiles = 0;
  xenos::ColorRenderTargetFormat color_format = xenos::ColorRenderTargetFormat::k_8_8_8_8;
  xenos::DepthRenderTargetFormat depth_format = xenos::DepthRenderTargetFormat::kD24S8;
  reg::RB_DEPTHCONTROL depth_control = {};

  uint32_t color_mask = 0xF;

  xenos::FetchConstantType fetch_type = xenos::FetchConstantType::kVertex;
};

inline void SetupDraw(GpuFixture& fixture, const Surface& surface, uint32_t width, uint32_t height,
                      const DrawOptions& options = {}) {
  uint32_t vertices = fixture.AllocPhysical(0x100);

  float l = -8.0f, r = float(width + 8), t = -8.0f, b = float(height + 8);

  float z_right = options.z_right < 0.0f ? options.z : options.z_right;
  float dz = (z_right - options.z) / float(width);
  float zl = options.z + dz * l, zr = options.z + dz * r;
  std::vector<uint32_t> vertex_data;
  for (float v : {l, t, zl, 1.0f, r, t, zr, 1.0f, l, b, zl, 1.0f, r, b, zr, 1.0f}) {
    vertex_data.push_back(std::bit_cast<uint32_t>(v));
  }
  fixture.WriteDwords(vertices, vertex_data);

  reg::SQ_PROGRAM_CNTL program_cntl = {};
  program_cntl.vs_num_reg = 1;
  program_cntl.ps_num_reg = 0;
  reg::RB_SURFACE_INFO surface_info = {};
  surface_info.surface_pitch = surface.pitch;
  surface_info.msaa_samples = surface.msaa;
  reg::RB_COLOR_INFO color_info = {};
  color_info.color_format = options.color_format;
  color_info.color_base = options.color_base_tiles;
  reg::RB_DEPTH_INFO depth_info = {};
  depth_info.depth_format = options.depth_format;
  reg::RB_MODECONTROL mode_control = {};
  mode_control.edram_mode = xenos::EdramMode::kColorDepth;
  reg::PA_CL_CLIP_CNTL clip_cntl = {};
  clip_cntl.clip_disable = 1;
  xenos::xe_gpu_vertex_fetch_t fetch = {};
  fetch.type = options.fetch_type;
  fetch.address = vertices >> 2;
  fetch.endian = xenos::Endian::k8in32;
  fetch.size = 4 * 4;

  fixture.Submit(LoadShader(xenos::ShaderType::kVertex, kVertexShader));
  fixture.Submit(LoadShader(xenos::ShaderType::kPixel, kPixelShader));
  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_SQ_PROGRAM_CNTL, {program_cntl.value, 0}));
  fixture.Submit(GpuFixture::SetRegisters(
      XE_GPU_REG_RB_SURFACE_INFO, {surface_info.value, color_info.value, depth_info.value}));
  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_RB_MODECONTROL, {mode_control.value}));
  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_RB_COLOR_MASK, {options.color_mask}));

  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_RB_BLENDCONTROL0, {0x00010001}));
  fixture.Submit(
      GpuFixture::SetRegisters(XE_GPU_REG_RB_DEPTHCONTROL, {options.depth_control.value}));
  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_PA_CL_CLIP_CNTL, {clip_cntl.value}));
  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_PA_CL_VTE_CNTL, {0}));
  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_PA_SU_SC_MODE_CNTL, {0}));
  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_PA_SU_VTX_CNTL, {0}));
  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_PA_SC_AA_MASK, {0xFFFF}));
  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_PA_SC_SCREEN_SCISSOR_TL,
                                          {PackXY(0, 0), PackXY(width, height)}));
  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_PA_SC_WINDOW_OFFSET, {0}));
  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_VGT_MAX_VTX_INDX, {0xFFFFFF, 0, 0}));
  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_SHADER_CONSTANT_FETCH_00_0,
                                          {fetch.dword_0, fetch.dword_1}));
}

inline void DrawRectFloat(GpuFixture& fixture, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1,
                          float r, float g, float b, float a) {
  reg::PA_SC_WINDOW_SCISSOR_TL window_tl = {};
  window_tl.window_offset_disable = 1;
  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_PA_SC_WINDOW_SCISSOR_TL,
                                          {window_tl.value | PackXY(x0, y0), PackXY(x1, y1)}));
  fixture.Submit(GpuFixture::SetRegisters(
      XE_GPU_REG_SHADER_CONSTANT_256_X, {std::bit_cast<uint32_t>(r), std::bit_cast<uint32_t>(g),
                                         std::bit_cast<uint32_t>(b), std::bit_cast<uint32_t>(a)}));
  reg::VGT_DRAW_INITIATOR initiator = {};
  initiator.prim_type = xenos::PrimitiveType::kTriangleStrip;
  initiator.source_select = xenos::SourceSelect::kAutoIndex;
  initiator.num_indices = 4;
  fixture.Submit({xenos::MakePacketType3(xenos::PM4_DRAW_INDX_2, 1), initiator.value});
}

inline void DrawRect(GpuFixture& fixture, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1,
                     uint32_t color) {
  DrawRectFloat(fixture, x0, y0, x1, y1, float(color & 0xFF) / 255.0f,
                float((color >> 8) & 0xFF) / 255.0f, float((color >> 16) & 0xFF) / 255.0f,
                float(color >> 24) / 255.0f);
}

struct ResolveFormat {
  xenos::ColorRenderTargetFormat source = xenos::ColorRenderTargetFormat::k_8_8_8_8;
  xenos::ColorFormat dest = xenos::ColorFormat::k_8_8_8_8;
  xenos::SurfaceNumberFormat dest_number = xenos::SurfaceNumberFormat::kUnsignedRepeatingFraction;
};

inline void Resolve(GpuFixture& fixture, const Surface& surface, uint32_t width, uint32_t height,
                    xenos::CopySampleSelect sample, uint32_t dest, uint32_t dest_pitch = 32,
                    uint32_t color_base_tiles = 0, const ResolveFormat& format = {},
                    uint32_t dest_height = 32) {
  uint32_t vertices = fixture.AllocPhysical(0x100);
  fixture.WriteDwords(
      vertices, {0, 0, std::bit_cast<uint32_t>(float(width)), 0,
                 std::bit_cast<uint32_t>(float(width)), std::bit_cast<uint32_t>(float(height))});
  reg::RB_SURFACE_INFO surface_info = {};
  surface_info.surface_pitch = surface.pitch;
  surface_info.msaa_samples = surface.msaa;
  reg::RB_COLOR_INFO color_info = {};
  color_info.color_format = format.source;
  color_info.color_base = color_base_tiles;
  reg::PA_SC_WINDOW_SCISSOR_TL window_tl = {};
  window_tl.window_offset_disable = 1;
  reg::RB_MODECONTROL mode_control = {};
  mode_control.edram_mode = xenos::EdramMode::kCopy;
  reg::RB_COPY_CONTROL copy_control = {};
  copy_control.copy_sample_select = sample;
  copy_control.copy_command = xenos::CopyCommand::kRaw;
  reg::RB_COPY_DEST_PITCH copy_dest_pitch = {};
  copy_dest_pitch.copy_dest_pitch = dest_pitch;
  copy_dest_pitch.copy_dest_height = dest_height;
  reg::RB_COPY_DEST_INFO dest_info = {};
  dest_info.copy_dest_endian = xenos::Endian128::k8in32;
  dest_info.copy_dest_format = format.dest;
  dest_info.copy_dest_number = format.dest_number;
  xenos::xe_gpu_vertex_fetch_t fetch = {};
  fetch.type = xenos::FetchConstantType::kVertex;
  fetch.address = vertices >> 2;
  fetch.endian = xenos::Endian::k8in32;
  fetch.size = 3 * 2;

  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_RB_SURFACE_INFO,
                                          {surface_info.value, color_info.value, 0}));
  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_PA_SC_SCREEN_SCISSOR_TL,
                                          {PackXY(0, 0), PackXY(width, height)}));
  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_PA_SC_WINDOW_OFFSET,
                                          {0, window_tl.value, PackXY(width, height)}));
  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_RB_MODECONTROL, {mode_control.value}));
  fixture.Submit(
      GpuFixture::SetRegisters(XE_GPU_REG_RB_COPY_CONTROL,
                               {copy_control.value, dest, copy_dest_pitch.value, dest_info.value}));
  fixture.Submit(GpuFixture::SetRegisters(XE_GPU_REG_SHADER_CONSTANT_FETCH_00_0,
                                          {fetch.dword_0, fetch.dword_1}));
  reg::VGT_DRAW_INITIATOR initiator = {};
  initiator.prim_type = xenos::PrimitiveType::kRectangleList;
  initiator.source_select = xenos::SourceSelect::kAutoIndex;
  initiator.num_indices = 3;
  fixture.Submit({xenos::MakePacketType3(xenos::PM4_DRAW_INDX_2, 1), initiator.value});
}

inline uint32_t ReadTexel(const GpuFixture& fixture, uint32_t texture, uint32_t x, uint32_t y,
                          uint32_t pitch = 32) {
  return fixture.ReadDword(
      texture + uint32_t(texture_util::GetTiledOffset2D(int32_t(x), int32_t(y), pitch, 2)));
}

struct Cf {
  uint32_t dword_0, dword_1;
};
inline Cf Exec(uint32_t address, uint32_t count, uint32_t fetch_sequence, bool end) {
  return {address | (count << 12) | (fetch_sequence << 16), end ? 0x2000u : 0x1000u};
}
inline Cf Alloc(ucode::AllocType type) {
  return {0, 0xC000u | (uint32_t(type) << 9)};
}
inline void PackCf(std::vector<uint32_t>& out, Cf a, Cf b) {
  out.insert(out.end(), {a.dword_0, (a.dword_1 & 0xFFFF) | (b.dword_0 << 16),
                         (b.dword_0 >> 16) | (b.dword_1 << 16)});
}

inline std::vector<uint32_t> AluExport(uint32_t dest, uint32_t opcode, uint32_t src1, uint32_t src2,
                                       uint32_t src3, uint32_t src1_swizzle, bool src1_temp,
                                       bool src2_temp, bool src3_temp) {
  return {0xC80F8000u | dest, src1_swizzle << 16,
          src3 | (src2 << 8) | (src1 << 16) | (opcode << 24) | (uint32_t(src3_temp) << 29) |
              (uint32_t(src2_temp) << 30) | (uint32_t(src1_temp) << 31)};
}

constexpr uint32_t kAluMax = 2;
constexpr uint32_t kAluMad = 11;

constexpr uint32_t kSwizzleXXXX = 0 | (3 << 2) | (2 << 4) | (1 << 6);

}
