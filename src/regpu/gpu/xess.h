#pragma once
#include "gpu/dlss.h"

// Intel XeSS super resolution (xess.cpp, the XeSS SDK): the same inputs and states as DLSS
// (dlss.h). Runs on Intel Arc (XMX) and on other GPUs with Shader Model 6.4 (DP4a).
//
// Whether XeSS can run: built with the XeSS SDK (REGPU_XESS), libxess.dll beside the game (the
// plugin delay-loads it) and a context created on this device.
bool XessInit(ID3D12Device* device);
// Records the upscale into `list` (the real command list). False if XeSS is unavailable or failed.
bool XessEvaluate(ID3D12Device* device, ID3D12GraphicsCommandList* list, const DlssSetup& setup, const DlssFrame& frame);
