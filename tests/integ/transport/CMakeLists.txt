include(${CMAKE_CURRENT_SOURCE_DIR}/source_list.cmake)
add_executable(integ_transport ${SOURCE_FILES})
target_add_catch2(integ_transport)
target_add_ausan(integ_transport)

add_executable(integ_transport_unity EXCLUDE_FROM_ALL unity_source.cpp)
target_link_libraries(integ_transport_unity PRIVATE Catch2::Catch2WithMain
                                                    ${PROJECT_NAME})
