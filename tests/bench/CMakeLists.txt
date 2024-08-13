option(${UPPER_PROJECT_NAME}_TEST_BENCHMARKS "execute benchmarks in tests" OFF)

set(${UPPER_PROJECT_NAME}_BENCH_DIR
    "${CMAKE_BINARY_DIR}/bench"
    CACHE PATH "directory to which benchmark results are written" FORCE)
file(MAKE_DIRECTORY ${${UPPER_PROJECT_NAME}_BENCH_DIR})

add_subdirectory(memory)
add_subdirectory(echo)
