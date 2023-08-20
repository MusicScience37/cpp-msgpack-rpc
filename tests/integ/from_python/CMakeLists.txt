add_executable(integ_from_python_server server.cpp)
target_link_libraries(integ_from_python_server PRIVATE ${PROJECT_NAME})

add_test(
    NAME integ_from_python
    COMMAND
        ${POETRY_EXECUTABLE} run pytest ${CMAKE_CURRENT_SOURCE_DIR}
        "--build=${CMAKE_BINARY_DIR}"
        "--junit-xml=${${UPPER_PROJECT_NAME}_JUNIT_DIR}/msgpack_rpc_integ_from_python.xml"
    WORKING_DIRECTORY ${${UPPER_PROJECT_NAME}_SOURCE_DIR})
