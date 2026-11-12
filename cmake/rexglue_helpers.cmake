#==========================================================
# rexglue_helpers.cmake
#
# Three helpers, each with a single responsibility:
#   rexglue_apply_target_settings(<target>)        - common compile/platform flags
#   rexglue_configure_target(<target>)             - host application
#   rexglue_configure_module_target(<target> ...)  - guest DLL module
#==========================================================
include_guard(GLOBAL)

#==========================================================
# rexglue_apply_target_settings(<target>) - Common flags
#
# Applied to both host apps and guest DLL modules. Compile/link flags only;
# runtime DLL staging is the host's job (see rexglue_configure_target).
#==========================================================
function(rexglue_apply_target_settings target_name)
    if(NOT MSVC)
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
            target_compile_options(${target_name} PRIVATE -msse4.1)
        endif()
    endif()
endfunction()

#==========================================================
# rexglue_configure_target(<target>) - Host application
#
# Adds:
#   - Win32 entry point source (windowed_app_main.cpp)
#   - ReXApp base class source (rex_app.cpp)
#   - Build-config define for the version stamp
#   - Windows POST_BUILD copy of TARGET_RUNTIME_DLLS and the FidelityFX DLLs.
#     Guest modules colocate with the host (see rexglue_configure_module_target),
#     so this single copy handles them transitively.
#   - The shipped shader cache (SHADER_CACHE <dir>, or ./shader_cache).
#   - GUIDE_PRESENTATION xbox360 (default) or original-xbox selects its Guide scenes.
#   - The title's cvar defaults (CVAR_DEFAULTS "name=value" ...), plus any
#     the build passes in REXGLUE_CVAR_DEFAULTS.
#   - The title's replacement shaders (SHADER_REPLACEMENTS <dir>): HLSL
#     compiled with FXC, or DXBC, staged in shader_replacements beside the
#     executable, with shader_replacements=true as a title default
#     (docs/shader-replacements.md).
#==========================================================
function(rexglue_configure_target target_name)
    cmake_parse_arguments(ARG "" "SHADER_CACHE;SHADER_REPLACEMENTS;ICON;GUIDE_PRESENTATION"
        "GPU_PLUGINS;CVAR_DEFAULTS" ${ARGN})
    if(NOT ARG_GUIDE_PRESENTATION)
        set(ARG_GUIDE_PRESENTATION xbox360)
    endif()
    if(NOT ARG_GUIDE_PRESENTATION MATCHES "^(xbox360|original-xbox)$")
        message(FATAL_ERROR "GUIDE_PRESENTATION must be xbox360 or original-xbox")
    endif()
    if(ARG_GUIDE_PRESENTATION STREQUAL "original-xbox")
        target_compile_definitions(${target_name} PRIVATE REXGLUE_GUIDE_ORIGINAL_XBOX=1)
    endif()
    if(ARG_ICON)
        rexglue_embed_title_icon(${target_name} "${ARG_ICON}")
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gdk/Title.ico")
        rexglue_embed_title_icon(${target_name} "${CMAKE_CURRENT_SOURCE_DIR}/gdk/Title.ico")
    endif()
    if(ARG_SHADER_REPLACEMENTS)
        list(APPEND ARG_CVAR_DEFAULTS "shader_replacements=true")
    endif()

    target_sources(${target_name} PRIVATE
        ${REXGLUE_SHARE_DIR}/windowed_app_main.cpp
        ${REXGLUE_SHARE_DIR}/rex_app.cpp)

    target_compile_definitions(${target_name} PRIVATE
        REXGLUE_BUILD_CONFIG="$<CONFIG>")
    if(REXGLUE_TITLE_VERSION)
        target_compile_definitions(${target_name} PRIVATE
            REXGLUE_TITLE_VERSION="${REXGLUE_TITLE_VERSION}")
    endif()
    if(REXGLUE_UPDATE_REPOSITORY)
        list(APPEND ARG_CVAR_DEFAULTS "update_repository=${REXGLUE_UPDATE_REPOSITORY}")
    endif()
    if(REXGLUE_UPDATE_ASSET)
        list(APPEND ARG_CVAR_DEFAULTS "update_asset=${REXGLUE_UPDATE_ASSET}")
    endif()
    # Defaults a build adds without editing the title's CMakeLists, such as a
    # release choosing its GPU plugin (-DREXGLUE_CVAR_DEFAULTS=gpu_plugin=regpu).
    if(REXGLUE_CVAR_DEFAULTS)
        list(APPEND ARG_CVAR_DEFAULTS ${REXGLUE_CVAR_DEFAULTS})
    endif()
    # The title's own cvar defaults ("name=value" each), under its config
    # file and the command line (ADR-009: titles opt in by name).
    if(ARG_CVAR_DEFAULTS)
        list(JOIN ARG_CVAR_DEFAULTS "|" _cvar_defaults)
        target_compile_definitions(${target_name} PRIVATE
            REXGLUE_TITLE_CVAR_DEFAULTS="${_cvar_defaults}")
    endif()

    rexglue_apply_target_settings(${target_name})
    _rexglue_embed_xbox_guide(${target_name} "${ARG_GUIDE_PRESENTATION}")
    _rexglue_embed_dlc_catalog(${target_name})
    _rexglue_stage_shader_cache(${target_name} "${ARG_SHADER_CACHE}")
    if(ARG_SHADER_REPLACEMENTS)
        _rexglue_stage_shader_replacements(${target_name} "${ARG_SHADER_REPLACEMENTS}")
    endif()
    _rexglue_stage_d3d12_agility(${target_name})

    if(WIN32)
        # Stage runtime DLLs (rexruntime, TracyClient, etc.) next to the host
        # binary on every link. copy_if_different is a no-op when up to date.
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND "$<$<BOOL:$<TARGET_RUNTIME_DLLS:${target_name}>>:${CMAKE_COMMAND};-E;copy_if_different;$<TARGET_RUNTIME_DLLS:${target_name}>;$<TARGET_FILE_DIR:${target_name}>>"
            COMMAND_EXPAND_LISTS
            VERBATIM
        )
    endif()

    if(WIN32)
        # FidelityFX is linked PRIVATE by rexui (to avoid propagating DLL
        # requirements to tool-mode targets), so copy its DLLs explicitly.
        foreach(_fx amd_fidelityfx_dx12)
            if(TARGET ${_fx})
                add_custom_command(TARGET ${target_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        $<TARGET_FILE:${_fx}>
                        $<TARGET_FILE_DIR:${target_name}>
                    VERBATIM
                )
            endif()
        endforeach()
    endif()

    if(TARGET rexglue-updater)
        add_dependencies(${target_name} rexglue-updater)
        set(_updater_target rexglue-updater)
    elseif(TARGET rex::updater)
        set(_updater_target rex::updater)
    endif()
    if(_updater_target)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${_updater_target}>
                $<TARGET_FILE_DIR:${target_name}>
            VERBATIM
        )
        unset(_updater_target)
    endif()

    # Stage requested GPU emulation plugins next to the executable. Plugins
    # are runtime-loaded (never linked), so TARGET_RUNTIME_DLLS misses them.
    foreach(_plugin IN LISTS ARG_GPU_PLUGINS)
        if(TARGET rexgpu-${_plugin})
            # In-tree build: depend on it so it gets built.
            set(_plugin_target rexgpu-${_plugin})
            add_dependencies(${target_name} ${_plugin_target})
        elseif(TARGET rex::gpu-${_plugin})
            # Installed SDK import.
            set(_plugin_target rex::gpu-${_plugin})
        else()
            message(FATAL_ERROR
                "rexglue_configure_target: unknown GPU plugin '${_plugin}' "
                "(no target rexgpu-${_plugin} or rex::gpu-${_plugin})")
        endif()
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${_plugin_target}>
                $<TARGET_FILE_DIR:${target_name}>
            VERBATIM
        )
        if(_plugin STREQUAL "regpu")
            _rexglue_stage_regpu_dxil(${target_name})
            _rexglue_stage_regpu_ffx(${target_name})
        endif()
        unset(_plugin_target)
    endforeach()

endfunction()

#==========================================================
# ReGPU's DXIL shader pack (REGPU_DXIL_PACK, docs/regpu.md)
#
# The pinned dxcompiler.dll and dxil.dll go beside the executable, and
# <target>_regpu_shaders builds regpu_shaders\ there from the shader cache a
# run left (REGPU_SHADER_CACHE, else %LOCALAPPDATA%\<target>\cache\regpu\
# shadercache). The pack holds the game's shaders: keep it out of the
# repository, like the game.
#==========================================================
function(_rexglue_stage_regpu_dxil target_name)
    if(NOT REGPU_DXIL_PACK)
        return()
    endif()
    if(TARGET regpu_dis)
        set(_dis $<TARGET_FILE:regpu_dis>)
        set(_files ${REGPU_DXC_FILES})
    else()
        set(_dis "${REXGLUE_BIN_DIR}/regpu_dis.exe")
        set(_files "${REXGLUE_BIN_DIR}/dxcompiler.dll" "${REXGLUE_BIN_DIR}/dxil.dll")
    endif()
    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_files} $<TARGET_FILE_DIR:${target_name}>
        VERBATIM
    )
    if(REGPU_SHADER_CACHE)
        set(_cache "${REGPU_SHADER_CACHE}")
    else()
        file(TO_CMAKE_PATH "$ENV{LOCALAPPDATA}/${target_name}/cache/regpu/shadercache" _cache)
    endif()
    add_custom_target(${target_name}_regpu_shaders
        COMMAND "${_dis}" --pack "$<TARGET_FILE_DIR:${target_name}>/regpu_shaders" "${_cache}"
        COMMENT "Building ${target_name}'s ReGPU shader pack from ${_cache}"
        VERBATIM
    )
    if(TARGET regpu_dis)
        add_dependencies(${target_name}_regpu_shaders regpu_dis)
    endif()
endfunction()

# The upscalers' runtime DLLs go beside the executable when the SDK was built with them:
# FidelityFX (FSR 3/4, REGPU_FSR3), DLSS (REGPU_DLSS) and XeSS (REGPU_XESS, with its licence),
# from the build tree or from an installed SDK's bin folder.
function(_rexglue_stage_regpu_ffx target_name)
    if(REGPU_FFX_FILES OR REGPU_DLSS_FILES OR REGPU_XESS_FILES)
        set(_files ${REGPU_FFX_FILES} ${REGPU_DLSS_FILES} ${REGPU_XESS_FILES})
        if(REGPU_XESS_FILES)
            add_custom_command(TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${REGPU_XESS_DIR}/LICENSE.txt"
                    "$<TARGET_FILE_DIR:${target_name}>/XeSS-LICENSE.txt"
                VERBATIM
            )
        endif()
    else()
        file(GLOB _files "${REXGLUE_BIN_DIR}/amd_fidelityfx_*_dx12.dll"
            "${REXGLUE_BIN_DIR}/nvngx_dlss.dll" "${REXGLUE_BIN_DIR}/libxess.dll"
            "${REXGLUE_BIN_DIR}/XeSS-LICENSE.txt")
    endif()
    if(NOT _files)
        return()
    endif()
    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_files} $<TARGET_FILE_DIR:${target_name}>
        VERBATIM
    )
endfunction()

# Embeds native Windows resources independently of the runtime window icon
# and MicrosoftGame.config. Call for each host/TU EXE; guest DLLs do not need it.
function(rexglue_embed_title_icon target_name icon_path)
    if(NOT WIN32)
        message(FATAL_ERROR "rexglue_embed_title_icon requires Windows")
    endif()
    get_filename_component(_icon "${icon_path}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    if(NOT EXISTS "${_icon}" OR IS_DIRECTORY "${_icon}")
        message(FATAL_ERROR "Title icon does not exist: ${_icon}")
    endif()
    get_target_property(_existing ${target_name} REXGLUE_EMBEDDED_ICON)
    if(_existing)
        message(FATAL_ERROR "Title icon already configured for ${target_name}")
    endif()
    enable_language(RC)
    set(_resource "${CMAKE_CURRENT_BINARY_DIR}/${target_name}_title_icon.rc")
    # Forward slashes avoid RC interpreting Windows path escape sequences.
    file(TO_CMAKE_PATH "${_icon}" _icon)
    file(WRITE "${_resource}" "// Generated title icon; the linker retains its manifest.\n1 ICON \"${_icon}\"\n")
    set_source_files_properties("${_resource}" PROPERTIES OBJECT_DEPENDS "${_icon}")
    target_sources(${target_name} PRIVATE "${_resource}")
    set_property(TARGET ${target_name} PROPERTY REXGLUE_EMBEDDED_ICON "${_icon}")
endfunction()

#==========================================================
# The D3D12 Agility SDK and dxil.dll (RG-GDK-032)
#
# With an SDK built with REXGLUE_SHADER_DXIL, a title exports the Agility SDK
# version (d3d12_agility.cpp) and ships D3D12Core.dll, d3d12SDKLayers.dll and
# dxil.dll in D3D12\ beside the executable, as Microsoft's PC backward
# compatibility packages do; gpu_shader_path=dxil needs them.
#==========================================================
function(_rexglue_stage_d3d12_agility target_name)
    if(NOT REXGLUE_SHADER_DXIL)
        return()
    endif()
    if(REXGLUE_D3D12_REDIST_FILES)
        set(_files ${REXGLUE_D3D12_REDIST_FILES})
    else()
        file(GLOB _files "${REXGLUE_SHARE_DIR}/d3d12/*.dll")
    endif()
    if(NOT _files)
        message(FATAL_ERROR "rexglue_configure_target: the SDK was built with REXGLUE_SHADER_DXIL "
            "but its D3D12 redistributables are missing")
    endif()
    target_sources(${target_name} PRIVATE ${REXGLUE_SHARE_DIR}/d3d12_agility.cpp)
    set_source_files_properties(${REXGLUE_SHARE_DIR}/d3d12_agility.cpp PROPERTIES
        COMPILE_DEFINITIONS "REXGLUE_D3D12_SDK_VERSION=${REXGLUE_D3D12_SDK_VERSION}"
        SKIP_PRECOMPILE_HEADERS ON)
    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target_name}>/D3D12"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_files}
            "$<TARGET_FILE_DIR:${target_name}>/D3D12"
        VERBATIM
    )
endfunction()

#==========================================================
# The shipped shader cache (RG-GDK-064)
#
# A title's recorded shader and pipeline storage files (<title ID>.xsh and
# <title ID>.*.d3d12.xpso, from a playthrough's cache\shaders\shareable)
# go beside the executable in shader_cache\; at startup they seed the
# player's cache, so pipelines are built before play instead of mid-frame.
# From SHADER_CACHE, else the project's own shader_cache folder. They hold
# the game's shaders: keep them out of the repository, like the game.
#==========================================================
function(_rexglue_stage_shader_cache target_name dir)
    if(NOT dir)
        set(dir "${CMAKE_CURRENT_SOURCE_DIR}/shader_cache")
        if(NOT IS_DIRECTORY "${dir}")
            return()
        endif()
    elseif(NOT IS_DIRECTORY "${dir}")
        message(FATAL_ERROR "rexglue_configure_target: SHADER_CACHE '${dir}' is not a folder")
    endif()
    file(GLOB _files CONFIGURE_DEPENDS "${dir}/*.xsh" "${dir}/*.xpso")
    if(NOT _files)
        message(WARNING "rexglue_configure_target: no .xsh or .xpso files in '${dir}'")
        return()
    endif()
    list(LENGTH _files _count)
    message(STATUS "${target_name}: shipping ${_count} shader cache files from ${dir}")
    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target_name}>/shader_cache"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_files}
            "$<TARGET_FILE_DIR:${target_name}>/shader_cache"
        VERBATIM
    )
endfunction()

# Replacement shaders (RG-GDK-067): <HASH>[_<MODIFICATION>].<vs|ps_rtv|ps_rov>
# .hlsl is compiled by FXC (entry point main, shader model 5.1, as
# scripts/build_shaders.py builds the SDK's own); a .dxbc of that name is
# shipped as it is.
function(_rexglue_stage_shader_replacements target_name dir)
    if(NOT IS_ABSOLUTE "${dir}")
        set(dir "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
    endif()
    if(NOT IS_DIRECTORY "${dir}")
        message(FATAL_ERROR "rexglue_configure_target: SHADER_REPLACEMENTS '${dir}' is not a folder")
    endif()
    file(GLOB _hlsl CONFIGURE_DEPENDS "${dir}/*.hlsl")
    file(GLOB _dxbc CONFIGURE_DEPENDS "${dir}/*.dxbc")
    set(_outputs ${_dxbc})
    if(_hlsl)
        file(GLOB _fxc_candidates
            "$ENV{ProgramFiles\(x86\)}/Windows Kits/10/bin/*/x64/fxc.exe")
        list(SORT _fxc_candidates COMPARE NATURAL ORDER DESCENDING)
        list(GET _fxc_candidates 0 _fxc)
        if(NOT _fxc)
            message(FATAL_ERROR "rexglue_configure_target: SHADER_REPLACEMENTS needs FXC (Windows SDK)")
        endif()
        set(_out_dir "${CMAKE_CURRENT_BINARY_DIR}/${target_name}_shader_replacements")
        file(MAKE_DIRECTORY "${_out_dir}")
        foreach(_source IN LISTS _hlsl)
            get_filename_component(_name "${_source}" NAME_WLE)
            if(NOT _name MATCHES "^[0-9A-Fa-f]+(_[0-9A-Fa-f]+)?[.](vs|ps_rtv|ps_rov)$")
                message(FATAL_ERROR
                    "rexglue_configure_target: replacement shader '${_name}.hlsl' is not "
                    "<HASH>[_<MODIFICATION>].<vs|ps_rtv|ps_rov>.hlsl")
            endif()
            if(_name MATCHES "[.]vs$")
                set(_profile vs_5_1)
            else()
                set(_profile ps_5_1)
            endif()
            set(_output "${_out_dir}/${_name}.dxbc")
            add_custom_command(OUTPUT "${_output}"
                COMMAND "${_fxc}" /nologo /T ${_profile} /E main /O3 /Fo "${_output}" "${_source}"
                DEPENDS "${_source}"
                COMMENT "Compiling replacement shader ${_name}"
                VERBATIM)
            list(APPEND _outputs "${_output}")
        endforeach()
        add_custom_target(${target_name}_shader_replacements DEPENDS ${_outputs})
        add_dependencies(${target_name} ${target_name}_shader_replacements)
    endif()
    if(NOT _outputs)
        message(WARNING "rexglue_configure_target: no .hlsl or .dxbc files in '${dir}'")
        return()
    endif()
    list(LENGTH _outputs _count)
    message(STATUS "${target_name}: shipping ${_count} replacement shaders from ${dir}")
    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:${target_name}>/shader_replacements"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_outputs}
            "$<TARGET_FILE_DIR:${target_name}>/shader_replacements"
        VERBATIM
    )
endfunction()

#==========================================================
# The Xbox guide (RG-GDK-041), built into every title
#
# The guide runs the console's own scenes. They come from the builder's own
# console system update (like the game files, never shipped with the SDK),
# named by REXGLUE_SYSTEM_UPDATE or the environment variable of that name.
# REXGLUE_GUIDE_FLASH optionally names an Xbox PC backward-compatibility
# game's Content/Flash folder, used only by GUIDE_PRESENTATION original-xbox.
# Xbox 360 titles retain the console system-update bundle even when Flash is set.
# `rexglue guide-bundle` takes the modules the guide reads and the console fonts, and the executable embeds them, so
# players need nothing for the guide.
#==========================================================
set(REXGLUE_SYSTEM_UPDATE "$ENV{REXGLUE_SYSTEM_UPDATE}" CACHE PATH
    "Console $SystemUpdate (dashboard 2.0.17559) built into each title for the Xbox guide")
set(REXGLUE_TITLE_VERSION "" CACHE STRING
    "The game's release version (for example 0.1.0-alpha.2); empty turns its updates off")
set(REXGLUE_UPDATE_REPOSITORY "" CACHE STRING
    "GitHub repository (owner/name) whose releases update the game")
set(REXGLUE_UPDATE_ASSET "" CACHE STRING
    "Release file pattern for the game's update, with * for the version")
set(REXGLUE_GUIDE_FLASH "$ENV{REXGLUE_GUIDE_FLASH}" CACHE PATH
    "Optional Xbox PC backward-compatibility Content/Flash folder for the Xbox guide")

function(_rexglue_embed_xbox_guide target_name presentation)
    if(presentation STREQUAL "original-xbox" AND NOT REXGLUE_GUIDE_FLASH)
        message(FATAL_ERROR "Original Xbox Guide requires REXGLUE_GUIDE_FLASH")
    endif()
    if(NOT REXGLUE_SYSTEM_UPDATE AND NOT presentation STREQUAL "original-xbox")
        message(STATUS "${target_name}: Xbox guide not built in; set REXGLUE_SYSTEM_UPDATE "
                       "to the console's $SystemUpdate folder")
        return()
    endif()
    if(REXGLUE_SYSTEM_UPDATE AND NOT EXISTS "${REXGLUE_SYSTEM_UPDATE}")
        message(FATAL_ERROR "REXGLUE_SYSTEM_UPDATE: '${REXGLUE_SYSTEM_UPDATE}' does not exist")
    endif()
    if(TARGET rexglue)
        set(_rexglue_cli rexglue)
    else()
        set(_rexglue_cli rex::rexglue)
    endif()

    string(MAKE_C_IDENTIFIER "${target_name}" _id)
    set(_dir "${CMAKE_CURRENT_BINARY_DIR}/rexglue_guide")
    set(_bundle "${_dir}/${_id}_xbox_guide.bin")
    set(_source "${_dir}/${_id}_xbox_guide.cpp")
    set(_sources "${REXGLUE_SYSTEM_UPDATE}")
    if(presentation STREQUAL "original-xbox")
        if(NOT IS_DIRECTORY "${REXGLUE_GUIDE_FLASH}")
            message(FATAL_ERROR "REXGLUE_GUIDE_FLASH: '${REXGLUE_GUIDE_FLASH}' is not a folder")
        endif()
        list(PREPEND _sources "${REXGLUE_GUIDE_FLASH}")
    elseif(REXGLUE_GUIDE_FLASH)
        message(STATUS "${target_name}: ignoring BC Flash override for Xbox 360 Guide")
    endif()
    set(_inputs "")
    foreach(_src IN LISTS _sources)
        if(IS_DIRECTORY "${_src}")
            file(GLOB_RECURSE _src_inputs CONFIGURE_DEPENDS "${_src}/*")
            list(APPEND _inputs ${_src_inputs})
        else()
            list(APPEND _inputs "${_src}")
        endif()
    endforeach()
    add_custom_command(
        OUTPUT "${_bundle}"
        COMMAND $<TARGET_FILE:${_rexglue_cli}> guide-bundle ${_sources}
                -o "${_bundle}"
        # The CLI too: a newer SDK may take more from the same update.
        DEPENDS ${_inputs} ${_rexglue_cli}
        COMMENT "Building the Xbox guide into ${target_name}"
        VERBATIM)

    # The bundle goes into read-only data as it is; .incbin needs Clang.
    file(TO_CMAKE_PATH "${_bundle}" _bundle_path)
    file(WRITE "${_source}.in"
"// Generated by rexglue_configure_target - DO NOT EDIT
#include <cstddef>
#include <cstdint>

namespace rex::ui::guide {
bool RegisterEmbeddedGuide(const std::uint8_t* data, std::size_t size);
}  // namespace rex::ui::guide

extern \"C\" const std::uint8_t rexglue_xbox_guide_${_id}[];
extern \"C\" const std::uint8_t rexglue_xbox_guide_${_id}_end[];
__asm__(\".section .rdata,\\\"dr\\\"\\n\"
        \".p2align 4\\n\"
        \".globl rexglue_xbox_guide_${_id}\\n\"
        \"rexglue_xbox_guide_${_id}:\\n\"
        \".incbin \\\"${_bundle_path}\\\"\\n\"
        \".globl rexglue_xbox_guide_${_id}_end\\n\"
        \"rexglue_xbox_guide_${_id}_end:\\n\"
        \".text\\n\");

namespace {
const bool kRegistered = rex::ui::guide::RegisterEmbeddedGuide(
    rexglue_xbox_guide_${_id},
    std::size_t(rexglue_xbox_guide_${_id}_end - rexglue_xbox_guide_${_id}));
}  // namespace
")
    configure_file("${_source}.in" "${_source}" COPYONLY)
    set_source_files_properties("${_source}" PROPERTIES OBJECT_DEPENDS "${_bundle}")
    target_sources(${target_name} PRIVATE "${_source}")
    message(STATUS "${target_name}: Xbox guide built in from ${REXGLUE_SYSTEM_UPDATE}")
endfunction()

#==========================================================
# The title's add-on catalogue (RG-GDK-050), built into it
#
# The title's config lists its add-ons as [[dlc]] entries, which codegen
# passes on as REXGLUE_TITLE_DLC_IDS. `rexglue dlc-catalog` fetches their
# names, descriptions and art from the Xbox 360 marketplace catalogue in the
# builder's language, once (the file is kept until the list changes), and the
# executable embeds it, so the guide's Manage Game page works offline. A build
# without internet embeds the IDs only and says so.
#==========================================================
set(REXGLUE_DLC_LOCALE "" CACHE STRING
    "Marketplace language and market for the add-on catalogue, such as en-GB (default: this PC's)")

function(_rexglue_embed_dlc_catalog target_name)
    if(NOT REXGLUE_TITLE_DLC_IDS)
        return()
    endif()
    if(TARGET rexglue)
        set(_rexglue_cli rexglue)
    else()
        set(_rexglue_cli rex::rexglue)
    endif()
    string(MAKE_C_IDENTIFIER "${target_name}" _id)
    set(_dir "${CMAKE_CURRENT_BINARY_DIR}/rexglue_dlc")
    # Named by its inputs, so a changed list or locale fetches it again.
    string(MD5 _key "${REXGLUE_TITLE_DLC_IDS}|${REXGLUE_DLC_LOCALE}")
    string(SUBSTRING "${_key}" 0 12 _key)
    set(_catalog "${_dir}/${_id}_dlc_${_key}.bin")
    set(_source "${_dir}/${_id}_dlc.cpp")
    string(REPLACE ";" "," _ids "${REXGLUE_TITLE_DLC_IDS}")
    set(_locale_args)
    if(REXGLUE_DLC_LOCALE)
        set(_locale_args --locale "${REXGLUE_DLC_LOCALE}")
    endif()
    add_custom_command(
        OUTPUT "${_catalog}"
        COMMAND $<TARGET_FILE:${_rexglue_cli}> dlc-catalog --ids "${_ids}" ${_locale_args}
                -o "${_catalog}"
        COMMENT "Fetching the add-on catalogue for ${target_name}"
        VERBATIM)

    file(TO_CMAKE_PATH "${_catalog}" _catalog_path)
    file(WRITE "${_source}.in"
"// Generated by rexglue_configure_target - DO NOT EDIT
#include <cstddef>
#include <cstdint>

namespace rex::ui::guide {
bool RegisterEmbeddedDlcCatalog(const std::uint8_t* data, std::size_t size);
}  // namespace rex::ui::guide

extern \"C\" const std::uint8_t rexglue_dlc_catalog_${_id}[];
extern \"C\" const std::uint8_t rexglue_dlc_catalog_${_id}_end[];
__asm__(\".section .rdata,\\\"dr\\\"\\n\"
        \".p2align 4\\n\"
        \".globl rexglue_dlc_catalog_${_id}\\n\"
        \"rexglue_dlc_catalog_${_id}:\\n\"
        \".incbin \\\"${_catalog_path}\\\"\\n\"
        \".globl rexglue_dlc_catalog_${_id}_end\\n\"
        \"rexglue_dlc_catalog_${_id}_end:\\n\"
        \".text\\n\");

namespace {
const bool kRegistered = rex::ui::guide::RegisterEmbeddedDlcCatalog(
    rexglue_dlc_catalog_${_id},
    std::size_t(rexglue_dlc_catalog_${_id}_end - rexglue_dlc_catalog_${_id}));
}  // namespace
")
    configure_file("${_source}.in" "${_source}" COPYONLY)
    set_source_files_properties("${_source}" PROPERTIES OBJECT_DEPENDS "${_catalog}")
    target_sources(${target_name} PRIVATE "${_source}")
    message(STATUS "${target_name}: add-on catalogue of ${REXGLUE_TITLE_DLC_IDS} built in")
endfunction()

#==========================================================
# rexglue_configure_module_target(<target> [HOST <host_target>])
#   - Guest DLL module
#
# Output is colocated with HOST (or CMAKE_RUNTIME_OUTPUT_DIRECTORY) so the
# host's LoadUserModule finds it; the host is wired to depend on the module
# so a top-level build of the host pulls in all guest DLLs. No runtime DLL
# staging here; the host's POST_BUILD copy covers shared dependencies.
#==========================================================
function(rexglue_configure_module_target target_name)
    cmake_parse_arguments(ARG "" "HOST" "" ${ARGN})

    if(ARG_HOST)
        set_target_properties(${target_name} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:${ARG_HOST}>
            RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:${ARG_HOST}>
            ARCHIVE_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:${ARG_HOST}>
        )
        # Defer add_dependencies so the host target may be declared after
        # this module call. Wrapping in EVAL CODE expands the variables now,
        # which DEFER CALL otherwise treats as literal argument text.
        cmake_language(EVAL CODE
            "cmake_language(DEFER CALL add_dependencies ${ARG_HOST} ${target_name})")
    elseif(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        set_target_properties(${target_name} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
    endif()

    rexglue_apply_target_settings(${target_name})
endfunction()

#==========================================================
# rexglue_embed_metadata(<target> DIRECTORY <dir> [PREFIX <relative-prefix>])
#
# Embeds files into the host executable as metadata assets. Runtime lookups use
# paths relative to the metadata root, so embedding metadata/icons with PREFIX
# icons makes "icons/foo.png" resolve without a loose sidecar file.
#==========================================================
function(rexglue_embed_metadata target_name)
    cmake_parse_arguments(ARG "" "DIRECTORY;PREFIX" "" ${ARGN})

    if(NOT TARGET ${target_name})
        message(FATAL_ERROR "rexglue_embed_metadata: target '${target_name}' does not exist")
    endif()
    if(NOT ARG_DIRECTORY)
        message(FATAL_ERROR "rexglue_embed_metadata: DIRECTORY is required")
    endif()
    if(NOT IS_DIRECTORY "${ARG_DIRECTORY}")
        message(FATAL_ERROR "rexglue_embed_metadata: '${ARG_DIRECTORY}' is not a directory")
    endif()

    file(GLOB_RECURSE _rexglue_metadata_files
        CONFIGURE_DEPENDS
        "${ARG_DIRECTORY}/*")

    set(_rexglue_regular_files)
    foreach(_rexglue_file IN LISTS _rexglue_metadata_files)
        if(NOT IS_DIRECTORY "${_rexglue_file}")
            list(APPEND _rexglue_regular_files "${_rexglue_file}")
        endif()
    endforeach()

    if(NOT _rexglue_regular_files)
        message(STATUS "rexglue_embed_metadata: no files found in ${ARG_DIRECTORY}")
        return()
    endif()

    string(MAKE_C_IDENTIFIER "${target_name}" _rexglue_target_id)
    set(_rexglue_output_dir "${CMAKE_CURRENT_BINARY_DIR}/rexglue_embedded_metadata")
    set(_rexglue_output "${_rexglue_output_dir}/${_rexglue_target_id}_embedded_metadata.cpp")
    file(MAKE_DIRECTORY "${_rexglue_output_dir}")

    set(_rexglue_content "// Auto-generated by rexglue_embed_metadata - DO NOT EDIT\n")
    string(APPEND _rexglue_content
        "#include <cstddef>\n"
        "#include <cstdint>\n"
        "#include <rex/embedded_metadata.h>\n\n"
        "namespace {\n\n")

    set(_rexglue_count 0)
    foreach(_rexglue_file IN LISTS _rexglue_regular_files)
        file(RELATIVE_PATH _rexglue_rel "${ARG_DIRECTORY}" "${_rexglue_file}")
        string(REPLACE "\\" "/" _rexglue_rel "${_rexglue_rel}")
        if(ARG_PREFIX)
            string(REPLACE "\\" "/" _rexglue_prefix "${ARG_PREFIX}")
            string(REGEX REPLACE "/$" "" _rexglue_prefix "${_rexglue_prefix}")
            set(_rexglue_asset_path "${_rexglue_prefix}/${_rexglue_rel}")
        else()
            set(_rexglue_asset_path "${_rexglue_rel}")
        endif()
        string(REPLACE "\"" "\\\"" _rexglue_asset_path "${_rexglue_asset_path}")

        string(MD5 _rexglue_asset_id "${_rexglue_asset_path}")
        file(READ "${_rexglue_file}" _rexglue_hex HEX)
        string(REGEX REPLACE "([0-9A-Fa-f][0-9A-Fa-f])" "0x\\1," _rexglue_bytes "${_rexglue_hex}")

        string(APPEND _rexglue_content
            "const std::uint8_t kAsset_${_rexglue_asset_id}[] = {${_rexglue_bytes}};\n"
            "const bool kRegistered_${_rexglue_asset_id} = "
            "::rex::RegisterEmbeddedMetadataAsset(\"${_rexglue_asset_path}\", "
            "kAsset_${_rexglue_asset_id}, sizeof(kAsset_${_rexglue_asset_id}));\n\n")
        math(EXPR _rexglue_count "${_rexglue_count} + 1")
    endforeach()

    string(APPEND _rexglue_content "}  // namespace\n")
    file(WRITE "${_rexglue_output}" "${_rexglue_content}")
    target_sources(${target_name} PRIVATE "${_rexglue_output}")
    message(STATUS "Embedded ${_rexglue_count} metadata asset(s) into ${target_name}")
endfunction()

#==========================================================
# rexglue_add_game_config(<target> DIRECTORY <dir>)
#
# Stages the MicrosoftGame.config and ShellVisuals images that
# `rexglue init gameconfig` wrote into <dir> next to the target's executable,
# so the build output is a loose PC layout that `wdapp register` and
# `makepkg genmap/pack /pc` accept (RG-GDK-022, docs/gdk-packaging.md).
#==========================================================
function(rexglue_add_game_config target_name)
    cmake_parse_arguments(ARG "" "DIRECTORY" "" ${ARGN})
    if(NOT ARG_DIRECTORY OR NOT EXISTS "${ARG_DIRECTORY}/MicrosoftGame.config")
        message(FATAL_ERROR
            "rexglue_add_game_config: no MicrosoftGame.config in '${ARG_DIRECTORY}'. "
            "Generate one with 'rexglue init gameconfig'.")
    endif()
    file(GLOB _images CONFIGURE_DEPENDS "${ARG_DIRECTORY}/*.png")
    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${ARG_DIRECTORY}/MicrosoftGame.config" ${_images}
            $<TARGET_FILE_DIR:${target_name}>
        VERBATIM
    )
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
        "${ARG_DIRECTORY}/MicrosoftGame.config")
endfunction()
