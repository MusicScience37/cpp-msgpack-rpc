add_test(
    NAME schema
    COMMAND
        ${POETRY_EXECUTABLE} run pytest ${CMAKE_CURRENT_SOURCE_DIR}
        "--junit-xml=${${UPPER_PROJECT_NAME}_JUNIT_DIR}/msgpack_rpc_schema.xml"
    WORKING_DIRECTORY ${${UPPER_PROJECT_NAME}_SOURCE_DIR})
