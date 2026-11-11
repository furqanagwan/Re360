set(XBOX_FUZZ_SECONDS 60 CACHE STRING "Seconds the xbox.fuzz.xui CTest test runs")

add_executable(xui_fuzz_seeds xui_fuzz_seeds.cpp)
target_include_directories(xui_fuzz_seeds PRIVATE ../tests)
target_compile_features(xui_fuzz_seeds PRIVATE cxx_std_23)

add_executable(xui_fuzzer xui_fuzzer.cpp)
target_link_libraries(xui_fuzzer PRIVATE xbox::ui_core)
target_link_options(xui_fuzzer PRIVATE -fsanitize=fuzzer)

foreach(_target xui_fuzz_seeds xui_fuzzer)
    add_custom_command(TARGET ${_target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${XBOX_ASAN_RUNTIME}" $<TARGET_FILE_DIR:${_target}>)
endforeach()

set(_seeds ${CMAKE_CURRENT_BINARY_DIR}/seeds)
set(_corpus ${CMAKE_CURRENT_BINARY_DIR}/corpus)
set(_artifacts ${CMAKE_CURRENT_BINARY_DIR}/artifacts)
file(MAKE_DIRECTORY ${_corpus} ${_artifacts})
add_test(NAME xbox.fuzz.seeds COMMAND xui_fuzz_seeds ${_seeds})
set_tests_properties(xbox.fuzz.seeds PROPERTIES FIXTURES_SETUP xui_seeds LABELS "fuzz")
add_test(NAME xbox.fuzz.xui
    COMMAND xui_fuzzer -max_total_time=${XBOX_FUZZ_SECONDS} -rss_limit_mb=4096
        -print_final_stats=1 -artifact_prefix=${_artifacts}/ ${_corpus} ${_seeds})
set_tests_properties(xbox.fuzz.xui PROPERTIES
    FIXTURES_REQUIRED xui_seeds LABELS "fuzz" TIMEOUT 0)
