# rexgpu-regpu through the plugin ABI, like a title, on a real D3D12 device.
# gpu.regpu_plugin_smoke: guest threads, MMIO registers, a PM4 stream and
# shaded draws resolved to guest memory, with no window (REGPU_HEADLESS).
# gpu.regpu_present: a frame swapped into a window the host owns, read back
# through ReGPU's debug server. ReGPU keeps one renderer per process and its threads run
# until the process ends, so this is its own executable with its own main.
add_executable(regpu_tests plugin_smoke_test.cpp ${PROJECT_SOURCE_DIR}/tests/gpu/gpu_fixture.cpp)
target_include_directories(regpu_tests PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/tests/gpu)
target_link_libraries(regpu_tests PRIVATE rexruntime fmt::fmt ws2_32)
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
add_test(NAME gpu.regpu_present COMMAND regpu_tests --present)
set_tests_properties(gpu.regpu_plugin_smoke gpu.regpu_present PROPERTIES
    LABELS "gpu;regpu"
    TIMEOUT 120
    SKIP_RETURN_CODE 4
    RUN_SERIAL TRUE
)
