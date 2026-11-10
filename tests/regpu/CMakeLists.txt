# rexgpu-regpu through the plugin ABI, like a title: runtime, guest threads,
# MMIO registers and a PM4 stream on a real D3D12 device, with no window
# (REGPU_HEADLESS). ReGPU keeps one renderer per process and its threads run
# until the process ends, so this is its own executable with its own main.
add_executable(regpu_tests plugin_smoke_test.cpp)
target_include_directories(regpu_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(regpu_tests PRIVATE rexruntime)
target_compile_options(regpu_tests PRIVATE
    -UTRACY_ENABLE -UREXGLUE_ENABLE_PROFILING
    -UREXGLUE_ENABLE_PERF_COUNTERS -UREXGLUE_PROFILE_GUEST_FUNCTIONS
)
set_target_properties(regpu_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:rexruntime>
)
add_dependencies(regpu_tests rexgpu-regpu)
add_custom_command(TARGET regpu_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:rexgpu-regpu>
        $<TARGET_FILE_DIR:regpu_tests>
    VERBATIM
)

add_test(NAME gpu.regpu_plugin_smoke COMMAND regpu_tests)
set_tests_properties(gpu.regpu_plugin_smoke PROPERTIES
    LABELS "gpu;regpu"
    TIMEOUT 120
    SKIP_RETURN_CODE 4
)
