add_executable(integ_logging_write_log write_log.cpp)
target_link_libraries(integ_logging_write_log PRIVATE ${PROJECT_NAME} bfg::lyra)
add_test(
    NAME integ_logging_write_log
    COMMAND integ_logging_write_log
    WORKING_DIRECTORY ${${UPPER_PROJECT_NAME}_TEMP_TEST_DIR})
