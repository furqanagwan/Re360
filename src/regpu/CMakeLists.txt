# rexgpu-regpu: ReGPU, the lean Xenos renderer, as a GPU plugin (gpu_plugin=regpu).
#
# Runtime-loaded via rex::system::LoadGpuPlugin, like rexgpu-xenos; consumers
# never link it. The renderer itself is the static library `regpu`
# (include/regpu); plugin/ adapts it to IGraphicsSystem.

option(REGPU_FSR3 "ReGPU: AMD FSR 3 upscaling (needs REGPU_FFX with the FidelityFX SDK's signed DLLs)" OFF)
option(REGPU_DLSS "ReGPU: NVIDIA DLSS (needs REGPU_DLSS_DIR)" OFF)
set(REGPU_FFX "" CACHE PATH
    "AMD FidelityFX SDK's Kits/FidelityFX folder. Only needed for REGPU_FSR3 or to rebuild gpu/bytecode/fsr1_cs.h")
set(REGPU_DLSS_DIR "" CACHE PATH "NVIDIA DLSS SDK folder (with REGPU_DLSS)")

set(REGPU_SOURCES
    debug.cpp
    live_switch.cpp
    log.cpp
    png.cpp
    settings.cpp
    gpu/d3dcompiler.cpp
    gpu/frame.cpp
    gpu/gpu.cpp
    gpu/hlsl.cpp
    gpu/renderer.cpp
    gpu/resolve_pack.cpp
    gpu/shader_scan.cpp
    gpu/texture.cpp
    gpu/ucode.cpp
    gpu/upscaler_stubs.cpp
    render/presenter.cpp
)
if(REGPU_FSR3)
    list(APPEND REGPU_SOURCES gpu/fsr3.cpp)
endif()
if(REGPU_DLSS)
    list(APPEND REGPU_SOURCES gpu/dlss.cpp)
endif()

add_library(regpu STATIC ${REGPU_SOURCES})
target_include_directories(regpu
    PUBLIC ${REXGLUE_ROOT}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(regpu PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS)
# ReGPU was written and measured with clang's default floating-point model and
# without the SDK's warning set; keep both as they were.
target_compile_options(regpu PRIVATE -msse4.1 -ffp-model=precise -w)
target_link_libraries(regpu
    PUBLIC d3d12 dxgi ws2_32 winmm synchronization onecore
    PRIVATE simde)

if(REGPU_FSR3)
    if(NOT EXISTS "${REGPU_FFX}/signedbin/amd_fidelityfx_loader_dx12.lib")
        message(FATAL_ERROR "REGPU_FSR3 needs REGPU_FFX set to the FidelityFX SDK's Kits/FidelityFX folder")
    endif()
    target_compile_definitions(regpu PRIVATE REGPU_FSR3=1)
    target_include_directories(regpu PRIVATE "${REGPU_FFX}/api/include" "${REGPU_FFX}/upscalers/include")
    target_link_libraries(regpu PUBLIC "${REGPU_FFX}/signedbin/amd_fidelityfx_loader_dx12.lib")
endif()
if(REGPU_DLSS)
    if(NOT EXISTS "${REGPU_DLSS_DIR}/include")
        message(FATAL_ERROR "REGPU_DLSS needs REGPU_DLSS_DIR set to the NVIDIA DLSS SDK folder")
    endif()
    target_compile_definitions(regpu PRIVATE REGPU_DLSS=1)
    target_include_directories(regpu PRIVATE "${REGPU_DLSS_DIR}/include")
    target_link_libraries(regpu PUBLIC "${REGPU_DLSS_DIR}/lib/Windows_x86_64/x64/nvsdk_ngx_s.lib")
endif()

# gpu/bytecode/fsr1_cs.h is checked in, like src/graphics/shaders/bytecode.
# Rebuild it after editing gpu/fsr1.hlsl: cmake --build <dir> --target regpu-fsr1-bytecode
if(REGPU_FFX)
    file(GLOB _regpu_fxc "$ENV{ProgramFiles\(x86\)}/Windows Kits/10/bin/*/x64/fxc.exe")
    list(SORT _regpu_fxc COMPARE NATURAL ORDER DESCENDING)
    list(GET _regpu_fxc 0 _regpu_fxc)
    get_filename_component(_regpu_fxc_dir "${_regpu_fxc}" DIRECTORY)
    add_custom_target(regpu-fsr1-bytecode
        COMMAND ${CMAKE_COMMAND} -E env "PATH=${_regpu_fxc_dir};$ENV{PATH}"
            py -3 -I "${REXGLUE_ROOT}/tools/regpu/embed_hlsl.py"
            "${CMAKE_CURRENT_SOURCE_DIR}/gpu/fsr1.hlsl"
            "${CMAKE_CURRENT_SOURCE_DIR}/gpu/bytecode/fsr1_cs.h"
            EasuMain,RcasMain,DepthCopyMain,GammaMain,EnhanceMain
            "${REGPU_FFX}/api/internal/gpu" "${REGPU_FFX}/upscalers/fsr3/include/gpu/fsr1"
        VERBATIM)
endif()

add_library(rexgpu-regpu SHARED plugin/graphics_system.cpp)
add_library(rex::gpu-regpu ALIAS rexgpu-regpu)
set_target_properties(rexgpu-regpu PROPERTIES
    EXPORT_NAME gpu-regpu
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)
# Everything PRIVATE: the plugin has no consumers (see src/graphics/CMakeLists.txt).
target_link_libraries(rexgpu-regpu PRIVATE rexruntime regpu)

if(PROJECT_IS_TOP_LEVEL)
    # Build check: ReGPU linked with nothing else, so a symbol it still expects
    # from a host shows up as an unresolved external.
    add_executable(regpu_linkcheck EXCLUDE_FROM_ALL ${REXGLUE_ROOT}/tests/regpu/linkcheck.cpp)
    target_link_libraries(regpu_linkcheck PRIVATE regpu)

    # Xenos shader disassembler and translator check (regpu_dis --hlsl <ucode files>).
    add_executable(regpu_dis EXCLUDE_FROM_ALL ${REXGLUE_ROOT}/tools/regpu/xenos_dis.cpp)
    target_include_directories(regpu_dis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_options(regpu_dis PRIVATE -w)
    target_link_libraries(regpu_dis PRIVATE regpu d3dcompiler)
endif()
