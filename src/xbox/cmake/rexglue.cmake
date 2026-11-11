# These functions attach the existing ReXGlue host adapter without duplicating
# rexcore globals or linking rexruntime back into its own object libraries.
set(XBOX_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
get_filename_component(XBOX_SOURCE_DIR "${XBOX_SOURCE_DIR}" ABSOLUTE)
set(XBOX_INCLUDE_DIRS
    "${XBOX_SOURCE_DIR}/ui/xui/include"
    "${XBOX_SOURCE_DIR}/ui/guide/include")
set(XBOX_FUZZ_DIR "${XBOX_SOURCE_DIR}/ui/xui/fuzz")
set(XBOX_FUZZ_TEST_DATA_DIR "${XBOX_SOURCE_DIR}/ui/xui/tests")

function(xbox_attach_rexglue target)
    if(NOT TARGET ${target} OR NOT DEFINED REXGLUE_ROOT)
        message(FATAL_ERROR "xbox_attach_rexglue requires a ReXGlue source build")
    endif()
    file(GLOB sources CONFIGURE_DEPENDS
        "${XBOX_SOURCE_DIR}/ui/xui/src/*.cpp"
        "${XBOX_SOURCE_DIR}/ui/guide/src/*.cpp")
    target_sources(${target} PRIVATE ${sources})
    foreach(include_dir IN LISTS XBOX_INCLUDE_DIRS)
        target_include_directories(${target} PUBLIC $<BUILD_INTERFACE:${include_dir}>)
    endforeach()
    target_link_libraries(${target} PRIVATE winhttp)
endfunction()

function(xbox_attach_tests target)
    file(GLOB sources CONFIGURE_DEPENDS
        "${XBOX_SOURCE_DIR}/ui/xui/tests/*.cpp"
        "${XBOX_SOURCE_DIR}/ui/guide/tests/*.cpp")
    target_sources(${target} PRIVATE ${sources})
    target_include_directories(${target} PRIVATE "${XBOX_SOURCE_DIR}/ui/xui/tests")
endfunction()
