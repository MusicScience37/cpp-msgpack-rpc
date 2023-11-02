include(${CMAKE_CURRENT_SOURCE_DIR}/source_list.cmake)
add_executable(${PROJECT_NAME}_test_units ${SOURCE_FILES})
target_add_catch2(${PROJECT_NAME}_test_units)
target_include_directories(${PROJECT_NAME}_test_units
                           PRIVATE ${${UPPER_PROJECT_NAME}_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}_test_units_unity EXCLUDE_FROM_ALL
               unity_source.cpp)
target_link_libraries(
    ${PROJECT_NAME}_test_units_unity
    PRIVATE Catch2::Catch2WithMain trompeloeil::trompeloeil
            ApprovalTests::ApprovalTests ${PROJECT_NAME})
target_include_directories(
    ${PROJECT_NAME}_test_units_unity
    PRIVATE ${${UPPER_PROJECT_NAME}_TEST_INCLUDE_DIR}
            ${${UPPER_PROJECT_NAME}_SOURCE_DIR}/src)

file(
    GLOB CONFIG_PARSER_TEST_SAMPLES
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/config"
    "${CMAKE_CURRENT_SOURCE_DIR}/config/config_parser_test_samples/*")
foreach(CONFIG_PARSER_TEST_SAMPLE ${CONFIG_PARSER_TEST_SAMPLES})
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/config/${CONFIG_PARSER_TEST_SAMPLE}"
        "${${UPPER_PROJECT_NAME}_TEMP_TEST_DIR}/${CONFIG_PARSER_TEST_SAMPLE}"
        COPYONLY)
endforeach()
