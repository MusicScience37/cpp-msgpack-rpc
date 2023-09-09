add_executable(integ_config_parse_toml parse_toml.cpp)
target_link_libraries(integ_config_parse_toml PRIVATE ${PROJECT_NAME})
target_include_directories(integ_config_parse_toml
                           PRIVATE ${${UPPER_PROJECT_NAME}_SOURCE_DIR}/src)
add_test(
    NAME integ_config
    COMMAND
        ${POETRY_EXECUTABLE} run pytest ${CMAKE_CURRENT_SOURCE_DIR}
        "--build=${CMAKE_BINARY_DIR}"
        "--junit-xml=${${UPPER_PROJECT_NAME}_JUNIT_DIR}/msgpack_rpc_integ_config.xml"
    WORKING_DIRECTORY ${${UPPER_PROJECT_NAME}_SOURCE_DIR})
