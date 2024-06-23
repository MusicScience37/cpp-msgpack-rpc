add_executable(durability_serve_methods serve_methods.cpp)
target_link_libraries(durability_serve_methods PRIVATE ${PROJECT_NAME})

add_executable(durability_request_continuously request_continuously.cpp)
target_link_libraries(durability_request_continuously PRIVATE ${PROJECT_NAME})

option(${UPPER_PROJECT_NAME}_EXECUTE_DURABILITY_TESTS
       "execute durability tests" ON)
if(${UPPER_PROJECT_NAME}_EXECUTE_DURABILITY_TESTS)
    add_test(
        NAME durability_tests
        COMMAND
            ${POETRY_EXECUTABLE} run pytest ${CMAKE_CURRENT_SOURCE_DIR}
            "--build=${CMAKE_BINARY_DIR}"
            "--junit-xml=${${UPPER_PROJECT_NAME}_JUNIT_DIR}/msgpack_rpc_durability_tests.xml"
            "--seconds=1"
        WORKING_DIRECTORY ${${UPPER_PROJECT_NAME}_SOURCE_DIR})
endif()
