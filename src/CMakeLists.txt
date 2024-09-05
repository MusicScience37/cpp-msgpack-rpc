include(${CMAKE_CURRENT_SOURCE_DIR}/source_list.cmake)
add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${${UPPER_PROJECT_NAME}_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
           $<BUILD_INTERFACE:${${UPPER_PROJECT_NAME}_GENERATED_HEADER_DIR}>
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC asio::asio
           msgpack-cxx
           spdlog::spdlog
           fmt::fmt
           re2::re2
           tomlplusplus::tomlplusplus
           Boost::atomic
           Threads::Threads
           $<BUILD_INTERFACE:${PROJECT_NAME}_cpp_warnings>)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Configurations to use as libraries.
set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES C_VISIBILITY_PRESET hidden
               CXX_VISIBILITY_PRESET hidden
               VISIBILITY_INLINES_HIDDEN 1
               DEBUG_POSTFIX d
               VERSION ${PROJECT_VERSION})
include(GenerateExportHeader)
generate_export_header(
    ${PROJECT_NAME}
    EXPORT_FILE_NAME
        "${${UPPER_PROJECT_NAME}_GENERATED_HEADER_DIR}/${PROJECT_NAME}/impl/${PROJECT_NAME}_export.h"
)

# Platform-specific configurations.
if(UNIX)
    set(MSGPACK_RPC_ENABLE_UNIX_SOCKETS
        1
        CACHE STRING "enable Unix sockets (1: enable, 0: disable)")
    set(MSGPACK_RPC_ENABLE_POSIX_SHM
        0 # TODO enable this
        CACHE STRING "enable POSIX shared memory (1: enable, 0: disable)")
else()
    set(MSGPACK_RPC_ENABLE_UNIX_SOCKETS
        0
        CACHE STRING "enable Unix sockets (1: enable, 0: disable)" FORCE)
    set(MSGPACK_RPC_ENABLE_POSIX_SHM
        0
        CACHE STRING "enable POSIX shared memory (1: enable, 0: disable)" FORCE)
endif()
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/impl/config.h.in
    ${${UPPER_PROJECT_NAME}_GENERATED_HEADER_DIR}/${PROJECT_NAME}/impl/config.h)

# For clang-tidy.
add_library(${PROJECT_NAME}_unity EXCLUDE_FROM_ALL unity_source.cpp)
target_include_directories(
    ${PROJECT_NAME}_unity
    PUBLIC $<BUILD_INTERFACE:${${UPPER_PROJECT_NAME}_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
           $<BUILD_INTERFACE:${${UPPER_PROJECT_NAME}_GENERATED_HEADER_DIR}>
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(
    ${PROJECT_NAME}_unity
    PUBLIC asio::asio
           msgpack-cxx
           spdlog::spdlog
           fmt::fmt
           re2::re2
           tomlplusplus::tomlplusplus
           Boost::atomic
           Threads::Threads
           $<BUILD_INTERFACE:${PROJECT_NAME}_cpp_warnings>)
target_compile_features(${PROJECT_NAME}_unity PUBLIC cxx_std_17)
