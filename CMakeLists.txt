# Re360 - Xbox 360 recompilation toolkit (the ReXGlue SDK with ReGPU)
cmake_minimum_required(VERSION 3.25)

# Windows is the only supported host (RG-GDK-024).
if(NOT CMAKE_HOST_WIN32)
    message(FATAL_ERROR "ReXGlue supports Windows hosts only. Linux and macOS support was retired; see RG-GDK-024.")
endif()

project(ReXGlue
    VERSION 0.10.0
    LANGUAGES C CXX
    DESCRIPTION "Xbox 360 Recompilation SDK"
)
# Floor only: bump MAJOR.MINOR for API changes. Patch is derived from git tags
# in cmake/rex_version.cmake. See docs/RELEASING.md.

#==========================================================
# Build Options
#==========================================================
option(REXGLUE_BUILD_TESTS       "Build PPC instruction and unit tests" OFF)
option(REXGLUE_PPC_CORPUS        "Build the hardware-captured PPC test corpus (170,000 cases, label ppc_corpus)" OFF)
option(REXGLUE_ENABLE_SANITIZERS "Enable UndefinedBehaviorSanitizer" OFF)
option(REXGLUE_ENABLE_ASAN       "Enable AddressSanitizer (Release and RelWithDebInfo only)" OFF)
option(REXGLUE_BUILD_FUZZERS     "Build libFuzzer targets for the file parsers (enables AddressSanitizer)" OFF)
option(REXGLUE_ENABLE_FIDELITYFX "Enable optional AMD FidelityFX integration (experimental)" OFF)
option(REXGLUE_ENABLE_TRACY                "Enable Tracy profiler integration" ON)
option(REXGLUE_ENABLE_PERF_COUNTERS        "Enable lightweight perf counter registry and overlay" ON)
# PC GDK is required (ADR-016). Keep the option name only to diagnose old
# configure commands instead of silently producing a different SDK.
option(REXGLUE_USE_GDK "Required Microsoft GDK (PC) integration" ON)
if(NOT REXGLUE_USE_GDK)
    message(FATAL_ERROR
        "GDK-free builds are retired. ReXGlue requires the April 2026 PC GDK "
        "(260404); use the win-amd64-gdk preset. See docs/gdk-toolchain.md.")
endif()
option(REXGLUE_SHADER_DXIL "Opt-in DXIL shader toolchain: Mesa spirv_to_dxil, D3D12 Agility SDK and DXC (RG-GDK-032 stage 1)" OFF)
set(REXGLUE_GDK_EDITION "260404" CACHE STRING "Required GDK edition (_GRDK_EDITION)")
set(REXGLUE_GDK_ROOT "$ENV{GameDKCoreLatest}" CACHE PATH "Installed GDK edition directory")
# Propagate profiling defines globally (compiled out in Release)
if(REXGLUE_ENABLE_TRACY)
    add_compile_definitions($<$<NOT:$<CONFIG:Release>>:REXGLUE_ENABLE_PROFILING>)
endif()
if(REXGLUE_ENABLE_PERF_COUNTERS)
    add_compile_definitions($<$<NOT:$<CONFIG:Release>>:REXGLUE_ENABLE_PERF_COUNTERS>)
endif()

# Graphics backend selection
# D3D12 is the only graphics backend (Vulkan was removed by RG-GDK-023).
# REXGLUE_USE_D3D12 stays defined for existing build logic and consumers.
set(REXGLUE_USE_D3D12 ON)
if(DEFINED REXGLUE_USE_VULKAN AND REXGLUE_USE_VULKAN)
    message(FATAL_ERROR "The Vulkan backend was removed (RG-GDK-023); D3D12 is the only backend.")
endif()

# Root of the SDK source tree
set(REXGLUE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

# SDK install support
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/rexglue_helpers.cmake)

# Enforce an LLVM Clang toolchain
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "ReXGlue requires the Clang compiler. Please set CMAKE_CXX_COMPILER to clang++")
endif()
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "18.0")
    message(FATAL_ERROR "ReXGlue requires Clang 18 or newer (found ${CMAKE_CXX_COMPILER_VERSION})")
endif()

# Enforce x64 architecture
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "ReXGlue only supports 64‑bit architectures (x86_64 or ARM64)")
endif()

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_RELWITHDEBINFO_POSTFIX rd)

# CMAKE_<CONFIG>_POSTFIX seeds libraries only, so without this every config of an
# executable installs over the last one.
function(rex_config_postfix target)
    set_target_properties(${target} PROPERTIES
        DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
        RELWITHDEBINFO_POSTFIX "${CMAKE_RELWITHDEBINFO_POSTFIX}")
endfunction()

# Global compile options
add_compile_options(
    -fno-strict-aliasing
    -ffp-model=strict
    $<$<CONFIG:DEBUG>:-g>
    $<$<CONFIG:DEBUG>:-O0>
    $<$<CONFIG:RELEASE>:-O3>
    $<$<CONFIG:RELEASE>:-DNDEBUG>
    -fno-char8_t
)

# Platform detection (supports AMD64 and ARM64)
set(REX_TARGET_PROCESSOR "${CMAKE_SYSTEM_PROCESSOR}")
if(WIN32)
    if(REX_TARGET_PROCESSOR MATCHES "AMD64|x86_64")
        set(REX_PLATFORM "win-amd64")
    elseif(REX_TARGET_PROCESSOR MATCHES "aarch64|ARM64|arm64")
        set(REX_PLATFORM "win-arm64")
    else()
        message(FATAL_ERROR "Unsupported Windows architecture: ${REX_TARGET_PROCESSOR}")
    endif()
else()
    message(FATAL_ERROR "Unsupported platform. ReXGlue supports Windows only.")
endif()

# Build config as compile definition (multi-config generators don't know config at configure time)
add_compile_definitions(REXGLUE_BUILD_CONFIG="$<CONFIG>")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/rex_pch.cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/rex_version.cmake)
rex_resolve_version(REXGLUE_FULL_VERSION
    FLOOR_MAJOR ${PROJECT_VERSION_MAJOR}
    FLOOR_MINOR ${PROJECT_VERSION_MINOR})

# Strip the -id trailer (if any) for the numeric-only version.
string(REGEX REPLACE "-.*$" "" REXGLUE_NUMERIC_VERSION "${REXGLUE_FULL_VERSION}")

# Parse up to 4 numeric components for consumers that want integer access.
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)(\\.([0-9]+))?(\\.([0-9]+))?" _ "${REXGLUE_NUMERIC_VERSION}")
set(REXGLUE_DERIVED_MAJOR "${CMAKE_MATCH_1}")
set(REXGLUE_DERIVED_MINOR "${CMAKE_MATCH_2}")
if(CMAKE_MATCH_4)
    set(REXGLUE_DERIVED_PATCH "${CMAKE_MATCH_4}")
else()
    set(REXGLUE_DERIVED_PATCH "0")
endif()
if(CMAKE_MATCH_6)
    set(REXGLUE_DERIVED_TWEAK "${CMAKE_MATCH_6}")
else()
    set(REXGLUE_DERIVED_TWEAK "0")
endif()

set(REXGLUE_FLOOR_VERSION
    "${REXGLUE_DERIVED_MAJOR}.${REXGLUE_DERIVED_MINOR}.${REXGLUE_DERIVED_PATCH}")

rex_version_channel(REXGLUE_VERSION_CHANNEL VERSION "${REXGLUE_FULL_VERSION}")

# Generate version header -- must run after REX_PLATFORM is set
# Timestamp is set at configure time -- intentionally not updated on every rebuild
string(TIMESTAMP REXGLUE_BUILD_TIMESTAMP "%Y%m%d_%H%M")
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/rex/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/rex/version.h
    @ONLY
)

# Output directories (multi-config appends /<Config>/ automatically). A GDK
# build gets its own so it never overwrites the standard build's binaries.
set(REXGLUE_OUTPUT_DIR "${REXGLUE_ROOT}/out/${REX_PLATFORM}")
if(REXGLUE_USE_GDK)
    string(APPEND REXGLUE_OUTPUT_DIR "-gdk")
endif()
if(REXGLUE_SHADER_DXIL)
    string(APPEND REXGLUE_OUTPUT_DIR "-dxil")
endif()
if(REXGLUE_BUILD_FUZZERS)
    string(APPEND REXGLUE_OUTPUT_DIR "-fuzz")
elseif(REXGLUE_ENABLE_ASAN)
    string(APPEND REXGLUE_OUTPUT_DIR "-asan")
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${REXGLUE_OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${REXGLUE_OUTPUT_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${REXGLUE_OUTPUT_DIR}")

# Sanitizer support for debugging memory corruption and undefined behavior
if(REXGLUE_ENABLE_SANITIZERS)
    message(STATUS "Enabling UndefinedBehaviorSanitizer (UBSan only - ASan conflicts with custom memory mapping)")
    # Note: ASan doesn't work well with the custom mmap at 0x100000000, so we only use UBSan
    add_compile_options(-fsanitize=undefined -fno-omit-frame-pointer -g)
    add_link_options(-fsanitize=undefined)
endif()

if(REXGLUE_BUILD_FUZZERS)
    set(REXGLUE_ENABLE_ASAN ON)
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
    add_compile_options(-fsanitize=fuzzer-no-link)
endif()
if(REXGLUE_ENABLE_ASAN)
    message(STATUS "Enabling AddressSanitizer; build the Release or RelWithDebInfo configuration")
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_compile_definitions(_DISABLE_STRING_ANNOTATION _DISABLE_VECTOR_ANNOTATION)
    add_link_options(-fsanitize=address)
    execute_process(COMMAND "${CMAKE_CXX_COMPILER}" -print-resource-dir
        OUTPUT_VARIABLE REXGLUE_CLANG_RESOURCE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(REXGLUE_ASAN_RUNTIME "${REXGLUE_CLANG_RESOURCE_DIR}/lib/windows/clang_rt.asan_dynamic-x86_64.dll")
    if(NOT EXISTS "${REXGLUE_ASAN_RUNTIME}")
        message(FATAL_ERROR "AddressSanitizer runtime not found: ${REXGLUE_ASAN_RUNTIME}")
    endif()
    foreach(_config Release RelWithDebInfo)
        file(COPY "${REXGLUE_ASAN_RUNTIME}" DESTINATION "${REXGLUE_OUTPUT_DIR}/${_config}")
    endforeach()
endif()

if(REXGLUE_USE_GDK)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/rexglue_gdk.cmake)
    rexglue_find_gdk(ROOT "${REXGLUE_GDK_ROOT}" EDITION "${REXGLUE_GDK_EDITION}")
endif()

if(REXGLUE_SHADER_DXIL)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/rexglue_shader_dxil.cmake)
endif()

if(REXGLUE_BUILD_TESTS)
    list(APPEND CMAKE_MODULE_PATH "${REXGLUE_ROOT}/cmake")
    list(APPEND CMAKE_MODULE_PATH "${REXGLUE_ROOT}/thirdparty/catch2/extras")
    # Catch's POST_BUILD mode writes one discovery file for all configurations.
    # PRE_TEST keeps Debug/Release command paths separate under multi-config
    # generators, even after alternating builds in the same output directory.
    set(CMAKE_CATCH_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
endif()

# Third-party dependencies (vendored sources only)
add_subdirectory(thirdparty)

# Enable warnings after thirdparty
add_compile_options(
    -Wall
    -Wextra
)

include_directories(${REXGLUE_ROOT}/include)

# The Xbox interface (XUI runtime and Guide), compiled into rexruntime.
include("${REXGLUE_ROOT}/src/xbox/cmake/rexglue.cmake")
include_directories(${XBOX_INCLUDE_DIRS})

add_subdirectory(src/core)
add_subdirectory(src/filesystem)
add_subdirectory(src/ui)
add_subdirectory(src/input)
add_subdirectory(src/audio)
add_subdirectory(src/graphics)
add_subdirectory(src/regpu)
add_subdirectory(src/system)
add_subdirectory(src/updater)
add_subdirectory(src/kernel)
add_subdirectory(src/codegen)
add_subdirectory(src/rexglue)

if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(tools)
endif()

# Add tests subdirectory if tests are enabled
if(REXGLUE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(REXGLUE_BUILD_FUZZERS)
    enable_testing()
    add_subdirectory(tests/fuzz)
endif()

include(cmake/rexglue_link_audit.cmake)

# Print configuration summary
message(STATUS "===========================================")
message(STATUS "Re360 (ReXGlue) Configuration Summary:")
message(STATUS "  Version: ${REXGLUE_FULL_VERSION} (${REXGLUE_VERSION_CHANNEL})")
message(STATUS "  Platform: ${REX_PLATFORM}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Configurations: ${CMAKE_CONFIGURATION_TYPES}")
message(STATUS "  Output Dir: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "  C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Graphics: D3D12 (GPU plugins: xenos, regpu)")
message(STATUS "  FidelityFX: ${REXGLUE_ENABLE_FIDELITYFX}")
message(STATUS "  Tracy: ${REXGLUE_ENABLE_TRACY}")
message(STATUS "  Perf Counters: ${REXGLUE_ENABLE_PERF_COUNTERS}")
if(REXGLUE_USE_GDK)
    message(STATUS "  GDK: ${REXGLUE_GDK_EDITION_FOUND} (${REXGLUE_GDK_ROOT_RESOLVED})")
else()
    message(STATUS "  GDK: OFF")
endif()
message(STATUS "===========================================")

# SDK install/export rules (skipped when consumed as a subdirectory)
if(PROJECT_IS_TOP_LEVEL)
    include(cmake/rexglue_devenv.cmake)
    if(NOT REXGLUE_BUILD_FUZZERS)
        include(cmake/rexglue_install.cmake)
    endif()
else()
    # When consumed via add_subdirectory(), provide the same interface as find_package().
    # This sets REXGLUE_SHARE_DIR and makes rexglue_configure_target() available.
    set(REXGLUE_SHARE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/ui" PARENT_SCOPE)
    set(REXGLUE_SHARE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/ui")
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/rexglue_helpers.cmake)
endif()
