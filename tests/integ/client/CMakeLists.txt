include(${CMAKE_CURRENT_SOURCE_DIR}/source_list.cmake)
add_executable(integ_client ${SOURCE_FILES})
target_add_catch2(integ_client)

add_executable(integ_client_unity EXCLUDE_FROM_ALL unity_source.cpp)
target_link_libraries(integ_client_unity PRIVATE Catch2::Catch2WithMain
                                                 ${PROJECT_NAME})
