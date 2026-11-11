/**
 * @file        system/gpu_plugin.h
 * @brief       GPU emulation plugin ABI and host-side loader
 *
 * @copyright   Copyright (c) 2026 Tom Clay <tomc@tctechstuff.com>
 *              All rights reserved.
 *
 * @license     BSD 3-Clause License
 *              See LICENSE file in the project root for full license text.
 *
 * @remarks     Two plugins exist: rexgpu-xenos (the default and the reference)
 *              and rexgpu-regpu (ReGPU, src/regpu). The factory carries an ABI
 *              version; version 2 added SetHostWindow and FlushCaches, version 3
 *              GetShaderPreparation.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

#include <rex/system/interfaces/graphics.h>

#if defined(_WIN32)
#define REX_GPU_PLUGIN_EXPORT __declspec(dllexport)
#else
#define REX_GPU_PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

namespace rex::system {

inline constexpr uint32_t kGpuPluginAbiVersion = 3;

inline constexpr const char* kGpuCreateSymbol = "rex_gpu_create";
inline constexpr const char* kGpuAbiVersionSymbol = "rex_gpu_abi_version";

struct GpuCreateInfo {
  uint32_t struct_size = 0;
  const char* backend = nullptr;
};

using GpuAbiVersionFn = uint32_t (*)();
using GpuCreateFn = IGraphicsSystem* (*)(uint32_t abi_version, const GpuCreateInfo* info);

inline constexpr std::string_view kDefaultGpuPlugin = "xenos";
inline constexpr std::string_view kNoGpuPlugin = "none";

std::string ResolveGpuPluginName(std::string_view configured, bool default_plugin_staged);
bool IsGpuPluginStaged(std::string_view name);

std::unique_ptr<IGraphicsSystem> LoadGpuPlugin(std::string_view name,
                                               std::string_view backend = "any");

}
