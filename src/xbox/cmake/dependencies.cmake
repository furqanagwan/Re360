include(FetchContent)

if(NOT TARGET fmt::fmt)
    if(XBOX_FETCH_DEPENDENCIES)
        FetchContent_Declare(fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG 407c905e45ad75fc29bf0f9bb7c5c2fd3475976f)
        FetchContent_MakeAvailable(fmt)
    else()
        find_package(fmt CONFIG REQUIRED)
    endif()
endif()

if(NOT TARGET imgui::imgui)
    if(NOT XBOX_FETCH_DEPENDENCIES)
        message(FATAL_ERROR "Supply imgui::imgui before add_subdirectory(xbox), or enable XBOX_FETCH_DEPENDENCIES")
    endif()
    FetchContent_Declare(imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG 6d910d5487d11ca567b61c7824b0c78c569d62f0)
    FetchContent_MakeAvailable(imgui)
    add_library(xbox_imgui STATIC
        "${imgui_SOURCE_DIR}/imgui.cpp"
        "${imgui_SOURCE_DIR}/imgui_draw.cpp"
        "${imgui_SOURCE_DIR}/imgui_tables.cpp"
        "${imgui_SOURCE_DIR}/imgui_widgets.cpp")
    target_include_directories(xbox_imgui PUBLIC "${imgui_SOURCE_DIR}")
    add_library(imgui::imgui ALIAS xbox_imgui)
endif()

if(XBOX_BUILD_TESTS AND NOT TARGET Catch2::Catch2WithMain)
    if(XBOX_FETCH_DEPENDENCIES)
        FetchContent_Declare(Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG 88abf9bf325c798c33f54f6b9220ef885b267f4f)
        FetchContent_MakeAvailable(Catch2)
    else()
        find_package(Catch2 3 CONFIG REQUIRED)
    endif()
endif()
