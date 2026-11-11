# rexglue_shader_dxil.cmake — opt-in DXIL shader toolchain (RG-GDK-032 stage 1)
#
# Expects REXGLUE_SHADER_DXIL to be set before inclusion. Nothing here runs in
# the default build.
#
# Pins and builds what the future SPIR-V -> DXIL guest shader path needs, the
# way has207/xenia-edge does, with the D3D12 redistributables Microsoft's own
# PC backward compatibility ships (Fuzion Frenzy, package 2608.3123.1.0:
# D3D12Core.dll 1.618.5, dxcompiler.dll 1.8.2502.8):
#
#   * Mesa spirv_to_dxil, from Edge's Mesa fork at the commit Edge pins, built
#     out of tree by meson and linked statically (rex::spirv_to_dxil).
#   * The D3D12 Agility SDK 1.618.5 (D3D12Core.dll, D3D12SDKLayers.dll):
#     Shader Model 6.6 on any supported Windows build.
#   * DXC 1.8.2502.8: dxil.dll signs the DXIL spirv_to_dxil emits unsigned.
#
# rexglue_deploy_d3d12_redist(<target>) copies the redistributables beside an
# executable, which must export D3D12SDKVersion = REXGLUE_D3D12_SDK_VERSION
# and D3D12SDKPath = ".\\D3D12\\".
#
# Prerequisites: an x64 Visual Studio developer shell (meson compiles Mesa
# with cl), meson >= 1.4, ninja and the Python mako module.

if(NOT REXGLUE_SHADER_DXIL)
    return()
endif()
if(NOT WIN32)
    message(FATAL_ERROR "REXGLUE_SHADER_DXIL: the DXIL toolchain is Windows only")
endif()

set(REXGLUE_MESA_COMMIT 7a1fc756809f3bdc9771b54e6036b156389dfc85)
set(REXGLUE_MESA_SHA256 8700a4aa5d2e87817c064c54c9285fb389973c1100573e4a2549ef01b7298f6d)
set(REXGLUE_D3D12_AGILITY_VERSION 1.618.5)
set(REXGLUE_D3D12_AGILITY_SHA256 0027fc24f947c48dbded13ada7d280be221eb651644e23a8a476f0f1f0a079dd)
set(REXGLUE_D3D12_SDK_VERSION 618)
include(${CMAKE_CURRENT_LIST_DIR}/rexglue_dxc.cmake)

set(REXGLUE_MESA_SOURCE_DIR "" CACHE PATH
    "Mesa tree to build spirv_to_dxil from (default: Edge's fork, downloaded at the pinned commit)")

set(_rex_dxil_deps "${CMAKE_BINARY_DIR}/_deps/shader_dxil")

# Downloads <url> once, checks its SHA-256 and extracts it to <dir>.
function(_rexglue_dxil_fetch name url sha256 dir)
    set(_archive "${_rex_dxil_deps}/${name}")
    set(_stamp "${dir}.sha256")
    if(EXISTS "${_stamp}")
        file(READ "${_stamp}" _done)
        if(_done STREQUAL sha256)
            return()
        endif()
    endif()
    message(STATUS "Shader DXIL: downloading ${name}")
    file(DOWNLOAD "${url}" "${_archive}" EXPECTED_HASH SHA256=${sha256} STATUS _status)
    list(GET _status 0 _code)
    if(NOT _code EQUAL 0)
        message(FATAL_ERROR "Shader DXIL: downloading ${url} failed: ${_status}")
    endif()
    file(REMOVE_RECURSE "${dir}")
    file(ARCHIVE_EXTRACT INPUT "${_archive}" DESTINATION "${dir}")
    file(WRITE "${_stamp}" "${sha256}")
endfunction()

# ── D3D12 Agility SDK and DXC (NuGet packages are zip archives) ──────────────
set(REXGLUE_D3D12_AGILITY_DIR "${_rex_dxil_deps}/d3d12-${REXGLUE_D3D12_AGILITY_VERSION}")
_rexglue_dxil_fetch("microsoft.direct3d.d3d12.${REXGLUE_D3D12_AGILITY_VERSION}.zip"
    "https://api.nuget.org/v3-flatcontainer/microsoft.direct3d.d3d12/${REXGLUE_D3D12_AGILITY_VERSION}/microsoft.direct3d.d3d12.${REXGLUE_D3D12_AGILITY_VERSION}.nupkg"
    ${REXGLUE_D3D12_AGILITY_SHA256} "${REXGLUE_D3D12_AGILITY_DIR}")
rexglue_fetch_dxc()
set(REXGLUE_D3D12_REDIST_FILES
    "${REXGLUE_D3D12_AGILITY_DIR}/build/native/bin/x64/D3D12Core.dll"
    "${REXGLUE_D3D12_AGILITY_DIR}/build/native/bin/x64/d3d12SDKLayers.dll"
    "${REXGLUE_DXC_DIR}/build/native/bin/x64/dxil.dll")
set(REXGLUE_DXC_INCLUDE_DIR "${REXGLUE_DXC_DIR}/build/native/include")

# Copies the Agility SDK and dxil.dll into <exe dir>/D3D12/ after each build.
function(rexglue_deploy_d3d12_redist target)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target}>/D3D12"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${REXGLUE_D3D12_REDIST_FILES}
            "$<TARGET_FILE_DIR:${target}>/D3D12"
        VERBATIM)
endfunction()

# ── Mesa spirv_to_dxil ───────────────────────────────────────────────────────
if(REXGLUE_MESA_SOURCE_DIR)
    set(_rex_mesa_src "${REXGLUE_MESA_SOURCE_DIR}")
else()
    set(_rex_mesa_root "${_rex_dxil_deps}/mesa")
    _rexglue_dxil_fetch("mesa-${REXGLUE_MESA_COMMIT}.tar.gz"
        "https://gitlab.freedesktop.org/has207/mesa/-/archive/${REXGLUE_MESA_COMMIT}/mesa-${REXGLUE_MESA_COMMIT}.tar.gz"
        ${REXGLUE_MESA_SHA256} "${_rex_mesa_root}")
    set(_rex_mesa_src "${_rex_mesa_root}/mesa-${REXGLUE_MESA_COMMIT}")
endif()
if(NOT EXISTS "${_rex_mesa_src}/meson.build")
    message(FATAL_ERROR "Shader DXIL: no Mesa tree at ${_rex_mesa_src}")
endif()

file(GLOB _rex_python_scripts "$ENV{APPDATA}/Python/Python3*/Scripts")
find_program(REXGLUE_MESON_EXECUTABLE meson HINTS ${_rex_python_scripts})
find_program(REXGLUE_NINJA_EXECUTABLE ninja)
find_program(REXGLUE_MSVC_CL cl)
find_package(Python3 COMPONENTS Interpreter)
if(NOT REXGLUE_MESON_EXECUTABLE OR NOT REXGLUE_NINJA_EXECUTABLE OR NOT REXGLUE_MSVC_CL)
    message(FATAL_ERROR
        "Shader DXIL: Mesa needs meson >= 1.4, ninja and MSVC cl (an x64 Visual Studio "
        "developer shell): meson=${REXGLUE_MESON_EXECUTABLE} ninja=${REXGLUE_NINJA_EXECUTABLE} "
        "cl=${REXGLUE_MSVC_CL}. Install meson with `python -m pip install --user meson mako`.")
endif()
if(Python3_FOUND)
    execute_process(COMMAND "${Python3_EXECUTABLE}" -c "import mako"
        RESULT_VARIABLE _rex_mako OUTPUT_QUIET ERROR_QUIET)
    if(NOT _rex_mako EQUAL 0)
        message(FATAL_ERROR "Shader DXIL: Mesa needs the Python mako module "
            "(`python -m pip install --user mako`)")
    endif()
endif()

set(_rex_mesa_build "${CMAKE_BINARY_DIR}/mesa-spirv_to_dxil")
set(_rex_s2d_main "${_rex_mesa_build}/src/microsoft/spirv_to_dxil/libspirv_to_dxil.a")
set(_rex_s2d_deps
    "${_rex_mesa_build}/src/microsoft/compiler/libdxil_compiler.a"
    "${_rex_mesa_build}/src/compiler/spirv/libvtn.a"
    "${_rex_mesa_build}/src/compiler/nir/libnir.a"
    "${_rex_mesa_build}/src/compiler/libcompiler.a"
    "${_rex_mesa_build}/src/util/libmesa_util.a"
    "${_rex_mesa_build}/src/util/libmesa_util_simd.a"
    "${_rex_mesa_build}/src/c11/impl/libmesa_util_c11.a"
    "${_rex_mesa_build}/src/util/blake3/libblake3.a"
    "${_rex_mesa_build}/subprojects/zlib-1.3.1/libz.a")
set(_rex_s2d_targets "")
foreach(_ar IN LISTS _rex_s2d_main _rex_s2d_deps)
    file(RELATIVE_PATH _rel "${_rex_mesa_build}" "${_ar}")
    list(APPEND _rex_s2d_targets "${_rel}")
endforeach()

# Release with /MD as Edge builds it: the archives are only linked statically.
# /wd4189 overrides Mesa's /we4189 (assert-only locals in release).
set(_rex_mesa_native "${_rex_dxil_deps}/mesa-native.ini")
file(WRITE "${_rex_mesa_native}" "[binaries]\nc = 'cl'\ncpp = 'cl'\n")
include(ExternalProject)
ExternalProject_Add(mesa_spirv_to_dxil
    SOURCE_DIR "${_rex_mesa_src}"
    CONFIGURE_COMMAND
        "${CMAKE_COMMAND}" -E rm -rf "${_rex_mesa_build}"
        COMMAND "${REXGLUE_MESON_EXECUTABLE}" setup "${_rex_mesa_build}" "${_rex_mesa_src}"
            --native-file "${_rex_mesa_native}"
            --default-library=static --buildtype=release
            -Dspirv-to-dxil=true -Dgallium-drivers= -Dvulkan-drivers= -Dplatforms=
            -Dglx=disabled -Degl=disabled -Dgbm=disabled -Dopengl=false
            -Dgles1=disabled -Dgles2=disabled -Dllvm=disabled -Dvideo-codecs=
            -Db_vscrt=md -Dc_args=/wd4189 -Dcpp_args=/wd4189
    BUILD_COMMAND "${REXGLUE_NINJA_EXECUTABLE}" -C "${_rex_mesa_build}" ${_rex_s2d_targets}
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS "${_rex_s2d_main}" ${_rex_s2d_deps}
    USES_TERMINAL_CONFIGURE ON
    USES_TERMINAL_BUILD ON)

add_library(rex_spirv_to_dxil STATIC IMPORTED GLOBAL)
add_library(rex::spirv_to_dxil ALIAS rex_spirv_to_dxil)
add_dependencies(rex_spirv_to_dxil mesa_spirv_to_dxil)
set_target_properties(rex_spirv_to_dxil PROPERTIES
    IMPORTED_LOCATION "${_rex_s2d_main}"
    INTERFACE_INCLUDE_DIRECTORIES
        "${_rex_mesa_src}/src/microsoft/spirv_to_dxil;${_rex_mesa_src}/src/microsoft/compiler"
    INTERFACE_LINK_LIBRARIES "${_rex_s2d_deps};synchronization;version")
foreach(_cfg IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER "${_cfg}" _cfg_u)
    set_target_properties(rex_spirv_to_dxil PROPERTIES IMPORTED_LOCATION_${_cfg_u} "${_rex_s2d_main}")
endforeach()

# ── glslang (SPIR-V builder for the guest shader translator) ─────────────────
# The commit xenia-edge pins (16.0.0): its SPIRV library, and the standalone
# compiler for the host tessellation shaders (src/graphics/shaders/spirv).
set(REXGLUE_GLSLANG_COMMIT a57276bf558f5cf94d3a9854ebdf5a2236849a5a)
set(REXGLUE_GLSLANG_SHA256 02f4321a3ed01b9a9a9874f76e2d8ad078825818fcc9aae12dbf2c01674fb8af)
set(_rex_glslang_root "${_rex_dxil_deps}/glslang")
_rexglue_dxil_fetch("glslang-${REXGLUE_GLSLANG_COMMIT}.tar.gz"
    "https://github.com/KhronosGroup/glslang/archive/${REXGLUE_GLSLANG_COMMIT}.tar.gz"
    ${REXGLUE_GLSLANG_SHA256} "${_rex_glslang_root}")
set(ENABLE_GLSLANG_BINARIES ON CACHE BOOL "" FORCE)
set(ENABLE_SPVREMAPPER OFF CACHE BOOL "" FORCE)
set(ENABLE_HLSL OFF CACHE BOOL "" FORCE)
set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
set(GLSLANG_TESTS OFF CACHE BOOL "" FORCE)
set(GLSLANG_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
# spv::Builder is subclassed by SpirvBuilder.
set(ENABLE_RTTI ON CACHE BOOL "" FORCE)
add_subdirectory("${_rex_glslang_root}/glslang-${REXGLUE_GLSLANG_COMMIT}"
    "${CMAKE_BINARY_DIR}/glslang" EXCLUDE_FROM_ALL)
set(REXGLUE_GLSLANG_INCLUDE_DIR "${_rex_glslang_root}/glslang-${REXGLUE_GLSLANG_COMMIT}")

message(STATUS "Shader DXIL: glslang ${REXGLUE_GLSLANG_COMMIT}")
message(STATUS "Shader DXIL: Mesa ${REXGLUE_MESA_COMMIT}, D3D12 Agility SDK "
    "${REXGLUE_D3D12_AGILITY_VERSION} (D3D12SDKVersion ${REXGLUE_D3D12_SDK_VERSION}), "
    "DXC ${REXGLUE_DXC_VERSION}")
