include(${CMAKE_CURRENT_SOURCE_DIR}/source_list.cmake)
add_executable(integ_server ${SOURCE_FILES})
target_include_directories(integ_server
                           PRIVATE ${${UPPER_PROJECT_NAME}_SOURCE_DIR}/src)
target_add_catch2(integ_server)

add_executable(integ_server_unity EXCLUDE_FROM_ALL unity_source.cpp)
target_link_libraries(integ_server_unity PRIVATE Catch2::Catch2WithMain
                                                 ${PROJECT_NAME})
target_include_directories(integ_server_unity
                           PRIVATE ${${UPPER_PROJECT_NAME}_SOURCE_DIR}/src)
