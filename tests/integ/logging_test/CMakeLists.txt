add_executable(integ_logging_write_log write_log.cpp)
target_link_libraries(integ_logging_write_log PRIVATE ${PROJECT_NAME} bfg::lyra)
add_test(
    NAME integ_logging_write_log
    COMMAND
        ${POETRY_EXECUTABLE} run pytest ${CMAKE_CURRENT_SOURCE_DIR}
        "--build=${CMAKE_BINARY_DIR}"
        "--junit-xml=${${UPPER_PROJECT_NAME}_JUNIT_DIR}/msgpack_rpc_integ_logging_write_log.xml"
    WORKING_DIRECTORY ${${UPPER_PROJECT_NAME}_SOURCE_DIR})
