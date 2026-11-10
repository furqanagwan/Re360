# ReXGlue Tests
# Test infrastructure for PPC instruction validation and unit tests

# Unit tests for core components
add_subdirectory(unit)

# Kernel/XAM service tests on an image-less Runtime
if(WIN32)
  add_subdirectory(kernel)
endif()

# D3D12 capability/fixture tests (Windows D3D12 only)
add_subdirectory(gpu)

# ReGPU through the GPU plugin ABI (gpu_plugin=regpu)
add_subdirectory(regpu)

# The opt-in DXIL toolchain (RG-GDK-032 stage 1)
if(REXGLUE_SHADER_DXIL)
  add_subdirectory(dxil)
endif()

# PPC instruction tests (requires PPC toolchain)
add_subdirectory(ppc)

# The precompiled DXBC shaders must rebuild byte for byte from their sources
# (scripts/build_shaders.py, RG-GDK-009). Needs the Windows SDK's FXC.
find_package(Python3 COMPONENTS Interpreter)
if(Python3_FOUND)
  add_test(NAME audio.xma_dump_format
    COMMAND Python3::Interpreter ${CMAKE_SOURCE_DIR}/tests/unit/audio/xma_dump_test.py)
  set_tests_properties(audio.xma_dump_format PROPERTIES LABELS "audio;xma")
endif()
if(WIN32 AND Python3_FOUND)
  add_test(NAME shaders.bytecode_reproducible
    COMMAND Python3::Interpreter ${CMAKE_SOURCE_DIR}/scripts/build_shaders.py --check)
endif()

# `rexglue init gameconfig` output must satisfy the installed GDK's own
# MicrosoftGame.config schema (RG-GDK-022). The schema ships beside the
# edition directories, so only GDK builds can run this.
if(REXGLUE_USE_GDK)
  # Minimal title: unpackaged and registered launch, save data across reinstall.
  add_subdirectory(gdk_smoke)

  get_filename_component(_gameconfig_schema
    "${REXGLUE_GDK_ROOT_RESOLVED}/../bin/GameConfigSchema.xsd" ABSOLUTE)
  if(EXISTS "${_gameconfig_schema}")
    add_test(NAME gdk.gameconfig_schema
      COMMAND powershell -NoProfile -ExecutionPolicy Bypass
        -File ${CMAKE_SOURCE_DIR}/scripts/validate_gameconfig.ps1
        -Rexglue $<TARGET_FILE:rexglue>
        -Schema ${_gameconfig_schema}
        -WorkDir ${CMAKE_CURRENT_BINARY_DIR}/gameconfig_schema/$<CONFIG>)
    set_tests_properties(gdk.gameconfig_schema PROPERTIES LABELS "gdk" TIMEOUT 120)
  else()
    message(STATUS "GameConfigSchema.xsd not found beside the GDK; skipping gdk.gameconfig_schema")
  endif()
endif()
