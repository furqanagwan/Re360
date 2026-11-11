cmake_minimum_required(VERSION 3.25)
project(Xbox VERSION 0.2.0 LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "Xbox currently supports Windows hosts only")
endif()

option(XBOX_BUILD_CORE "Build the reusable XUI scene library" ${PROJECT_IS_TOP_LEVEL})
option(XBOX_BUILD_TESTS "Build standalone synthetic tests" ${PROJECT_IS_TOP_LEVEL})
option(XBOX_BUILD_FUZZERS "Build the libFuzzer XUI target with AddressSanitizer (Clang, Release)" OFF)
option(XBOX_FETCH_DEPENDENCIES "Fetch pinned fmt, ImGui and Catch2 when not supplied" OFF)

set(XBOX_UI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ui")

if(XBOX_BUILD_CORE)
    if(XBOX_BUILD_FUZZERS)
        set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
        add_compile_options(-fsanitize=address,fuzzer-no-link -fno-omit-frame-pointer)
        add_compile_definitions(_DISABLE_STRING_ANNOTATION _DISABLE_VECTOR_ANNOTATION)
        add_link_options(-fsanitize=address)
        execute_process(COMMAND "${CMAKE_CXX_COMPILER}" -print-resource-dir
            OUTPUT_VARIABLE XBOX_CLANG_RESOURCE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
        set(XBOX_ASAN_RUNTIME "${XBOX_CLANG_RESOURCE_DIR}/lib/windows/clang_rt.asan_dynamic-x86_64.dll")
        if(NOT EXISTS "${XBOX_ASAN_RUNTIME}")
            message(FATAL_ERROR "AddressSanitizer runtime not found: ${XBOX_ASAN_RUNTIME}")
        endif()
    endif()
    include(cmake/dependencies.cmake)
    add_library(xbox_ui_core STATIC
        ui/xui/src/package.cpp
        ui/xui/src/schema.cpp
        ui/xui/src/xur_reader.cpp
        ui/xui/src/runtime.cpp
        ui/xui/src/renderer.cpp
        ui/guide/src/active_downloads.cpp
        ui/guide/src/file_browser.cpp
        ui/guide/src/guide_list_page.cpp
        ui/guide/src/message_box.cpp
        ui/guide/src/guide_layout.cpp
        ui/guide/src/virtual_keyboard.cpp
    )
    add_library(xbox::ui_core ALIAS xbox_ui_core)
    target_compile_features(xbox_ui_core PUBLIC cxx_std_23)
    target_include_directories(xbox_ui_core PUBLIC
        "${XBOX_UI_DIR}/xui/include"
        "${XBOX_UI_DIR}/guide/include")
    target_link_libraries(xbox_ui_core PUBLIC fmt::fmt imgui::imgui)
    if(XBOX_BUILD_TESTS)
        enable_testing()
        add_subdirectory(tests/standalone)
    endif()
    if(XBOX_BUILD_FUZZERS)
        enable_testing()
        add_subdirectory(ui/xui/fuzz)
    endif()
elseif(XBOX_BUILD_TESTS)
    message(FATAL_ERROR "Standalone tests require XBOX_BUILD_CORE=ON")
endif()

include(cmake/rexglue.cmake)
