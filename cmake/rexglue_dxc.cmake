# rexglue_dxc.cmake — the pinned DXC redistributable (dxcompiler.dll, dxil.dll)
#
# One pin for everything that ships DXC with a title: the DXIL shader path
# (rexglue_shader_dxil.cmake, which ships dxil.dll to sign Mesa's output) and
# ReGPU's DXIL shader pack (REGPU_DXIL_PACK, which ships both to compile and
# sign translated shaders). 1.8.2502.8 is the dxcompiler.dll Microsoft's PC
# backward compatibility packages ship (Fuzion Frenzy, package 2608.3123.1.0).
#
# rexglue_fetch_dxc() downloads the NuGet package once into
# <build>/_deps/dxc-<version>, checks its SHA-256 and sets REXGLUE_DXC_DIR.

include_guard(GLOBAL)

set(REXGLUE_DXC_VERSION 1.8.2502.8)
set(REXGLUE_DXC_SHA256 44b0b17c972acfe1129d1d35e59f6a620149a9af084eabcc848013e97d9e9518)

function(rexglue_fetch_dxc)
    set(_deps "${CMAKE_BINARY_DIR}/_deps")
    set(_dir "${_deps}/dxc-${REXGLUE_DXC_VERSION}")
    set(_stamp "${_dir}.sha256")
    set(_done "")
    if(EXISTS "${_stamp}")
        file(READ "${_stamp}" _done)
    endif()
    if(NOT _done STREQUAL REXGLUE_DXC_SHA256)
        set(_archive "${_deps}/microsoft.direct3d.dxc.${REXGLUE_DXC_VERSION}.zip")
        set(_url "https://api.nuget.org/v3-flatcontainer/microsoft.direct3d.dxc/${REXGLUE_DXC_VERSION}/microsoft.direct3d.dxc.${REXGLUE_DXC_VERSION}.nupkg")
        message(STATUS "DXC: downloading ${REXGLUE_DXC_VERSION}")
        file(DOWNLOAD "${_url}" "${_archive}" EXPECTED_HASH SHA256=${REXGLUE_DXC_SHA256} STATUS _status)
        list(GET _status 0 _code)
        if(NOT _code EQUAL 0)
            message(FATAL_ERROR "DXC: downloading ${_url} failed: ${_status}")
        endif()
        file(REMOVE_RECURSE "${_dir}")
        file(ARCHIVE_EXTRACT INPUT "${_archive}" DESTINATION "${_dir}")
        file(WRITE "${_stamp}" "${REXGLUE_DXC_SHA256}")
    endif()
    set(REXGLUE_DXC_DIR "${_dir}" PARENT_SCOPE)
endfunction()
