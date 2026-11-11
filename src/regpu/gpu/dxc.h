#pragma once
// Translated shaders as DXIL, compiled with DXC (dxcompiler.dll and dxil.dll, which signs the output,
// beside the game executable, else the system's). REGPU_DXIL=1 builds every translated shader this
// way instead of with fxc: a pipeline takes either DXBC or DXIL for all its stages, never a mix.
// A shader pack (ShaderPackDir) holds DXIL built ahead of time, by tools/regpu/xenos_dis --pack, under
// the same key the shader cache uses.
#include <cstdint>
#include <filesystem>
#include <string>

#include <d3dcommon.h>

// The key of a translated shader's source, which names its bytecode in the shader cache and pack.
uint64_t ShaderSourceKey(const std::string& hlsl, bool pixel);

// REGPU_DXIL=1 and DXC loads.
bool UseDxil();
bool DxcAvailable();

// vs_6_0 / ps_6_0 with HLSL 2018 rules (the translator uses vector && || and ?:, which 2021 rejects).
// Fails, with a message in errors, when DXC is missing or the output is unsigned (no dxil.dll):
// D3D12 rejects unsigned DXIL.
bool CompileDxil(const std::string& hlsl, bool pixel, bool optimize, ID3DBlob** code, std::string* errors);

// REGPU_SHADER_PACK, else regpu_shaders beside the game executable.
const std::filesystem::path& ShaderPackDir();
std::filesystem::path ShaderPackPath(uint64_t key);
