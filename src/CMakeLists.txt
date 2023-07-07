include(${CMAKE_CURRENT_SOURCE_DIR}/source_list.cmake)
add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC ${${UPPER_PROJECT_NAME}_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_cpp_warnings)
target_add_ausan(${PROJECT_NAME})

add_executable(${PROJECT_NAME}_unity EXCLUDE_FROM_ALL unity_source.cpp)
