add_executable(bench_shared_objects shared_objects.cpp)
target_link_libraries(bench_shared_objects PRIVATE ${PROJECT_NAME}
                                                   cpp_stat_bench::stat_bench)

if(${${UPPER_PROJECT_NAME}_TEST_BENCHMARKS})
    add_test(
        NAME bench_shared_objects
        COMMAND bench_shared_objects --json shared_objects/result.json
                --compressed-msgpack shared_objects/result.data
        WORKING_DIRECTORY ${${UPPER_PROJECT_NAME}_BENCH_DIR})
endif()
