add_executable(integ_inter_process_server server.cpp)
target_link_libraries(integ_inter_process_server PRIVATE ${PROJECT_NAME})

add_executable(integ_inter_process_client client.cpp)
target_link_libraries(integ_inter_process_client PRIVATE ${PROJECT_NAME})

add_test(
    NAME integ_inter_process
    COMMAND
        ${POETRY_EXECUTABLE} run pytest ${CMAKE_CURRENT_SOURCE_DIR}
        "--build=${CMAKE_BINARY_DIR}"
        "--junit-xml=${${UPPER_PROJECT_NAME}_JUNIT_DIR}/msgpack_rpc_integ_inter_process.xml"
    WORKING_DIRECTORY ${${UPPER_PROJECT_NAME}_SOURCE_DIR})
