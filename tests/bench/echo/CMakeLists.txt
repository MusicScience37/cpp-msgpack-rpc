add_executable(bench_echo_server server.cpp)
target_link_libraries(bench_echo_server PRIVATE ${PROJECT_NAME})

add_executable(bench_echo_client client.cpp)
target_link_libraries(bench_echo_client PRIVATE ${PROJECT_NAME}
                                                cpp_stat_bench::stat_bench)

add_executable(bench_echo_client_random client_random.cpp)
target_link_libraries(bench_echo_client_random PRIVATE ${PROJECT_NAME})

if(${${UPPER_PROJECT_NAME}_TEST_BENCHMARKS})
    add_test(
        NAME bench_echo
        COMMAND ${POETRY_EXECUTABLE} run python
                ${CMAKE_CURRENT_SOURCE_DIR}/bench.py -b ${CMAKE_BINARY_DIR}
        WORKING_DIRECTORY ${${UPPER_PROJECT_NAME}_SOURCE_DIR})

    add_test(
        NAME bench_echo
        COMMAND
            ${POETRY_EXECUTABLE} run python
            ${CMAKE_CURRENT_SOURCE_DIR}/bench_random.py -b ${CMAKE_BINARY_DIR}
        WORKING_DIRECTORY ${${UPPER_PROJECT_NAME}_SOURCE_DIR})
endif()
