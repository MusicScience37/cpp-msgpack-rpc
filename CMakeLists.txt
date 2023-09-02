cmake_minimum_required(VERSION 3.12)

set(SHORT_PROJECT_NAME "msgpack_rpc")
set(FULL_PROJECT_NAME "cpp-msgpack-rpc")
string(TOUPPER ${SHORT_PROJECT_NAME} UPPER_PROJECT_NAME)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ParseVersion)
parse_version(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${SHORT_PROJECT_NAME}/version.h
    ${UPPER_PROJECT_NAME})

project(
    ${SHORT_PROJECT_NAME}
    VERSION ${${UPPER_PROJECT_NAME}_VERSION}
    DESCRIPTION "An RPC library implementing MessagePack RPC."
    LANGUAGES CXX)
message(STATUS "${FULL_PROJECT_NAME} version ${PROJECT_VERSION}")
message(STATUS "build type: ${CMAKE_BUILD_TYPE}")

string(TOUPPER ${PROJECT_NAME} UPPER_PROJECT_NAME)

set(BUILDNAME
    "${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}-${CMAKE_BUILD_TYPE}"
)
include(CTest)

option(${UPPER_PROJECT_NAME}_BUILD_DOC
       "build documentation of ${FULL_PROJECT_NAME}" OFF)
option(${UPPER_PROJECT_NAME}_TESTING "enable tests of ${FULL_PROJECT_NAME}" OFF)
option(${UPPER_PROJECT_NAME}_ENABLE_INSTALL
       "enable CMake target to install ${FULL_PROJECT_NAME}" ON)

set(BUILD_SHARED_LIBS
    ON
    CACHE BOOL "Build shared libraries.")

set(CMAKE_CXX_STANDARD
    "17"
    CACHE STRING "C++ standard")

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

find_package(asio REQUIRED)
find_package(msgpack-cxx REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(re2 REQUIRED)
find_package(Threads REQUIRED)
if(${UPPER_PROJECT_NAME}_TESTING)
    find_package(Catch2 REQUIRED)
    find_package(trompeloeil REQUIRED)
    find_package(lyra REQUIRED)
    find_package(cpp-stat-bench REQUIRED)

    # ApprovalTests.
    find_path(APPROVAL_TESTS_CPP_INCLUDE_DIRS "ApprovalTests.hpp")
    add_library(ApprovalTests_ApprovalTests INTERFACE)
    target_include_directories(ApprovalTests_ApprovalTests
                               INTERFACE ${APPROVAL_TESTS_CPP_INCLUDE_DIRS})
    add_library(ApprovalTests::ApprovalTests ALIAS ApprovalTests_ApprovalTests)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE BOOL "export compile commands" FORCE)

include(CppWarningFlags)
include(ConfigureSanitizer)
include(ConfigureCcache)
include(poetry)

set(${UPPER_PROJECT_NAME}_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(${UPPER_PROJECT_NAME}_GENERATED_HEADER_DIR ${CMAKE_BINARY_DIR}/include)

add_subdirectory(src)

if(${UPPER_PROJECT_NAME}_BUILD_DOC)
    add_subdirectory(doc)
endif()

if(${UPPER_PROJECT_NAME}_TESTING)
    add_subdirectory(tests)
endif()

if(${UPPER_PROJECT_NAME}_ENABLE_INSTALL)
    install(
        TARGETS ${SHORT_PROJECT_NAME}
        EXPORT ${FULL_PROJECT_NAME}-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        FRAMEWORK DESTINATION ${CMAKE_INSTALL_PREFIX})

    install(
        DIRECTORY ${${UPPER_PROJECT_NAME}_SOURCE_DIR}/include/msgpack_rpc
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING
        PATTERN "*.h*")

    install(
        EXPORT ${FULL_PROJECT_NAME}-targets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${FULL_PROJECT_NAME}
        NAMESPACE cpp_msgpack_rpc::)

    include(CMakePackageConfigHelpers)
    configure_package_config_file(
        cmake/${FULL_PROJECT_NAME}-config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/${FULL_PROJECT_NAME}-config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${FULL_PROJECT_NAME}"
    )

    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/${FULL_PROJECT_NAME}-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${FULL_PROJECT_NAME}")
endif()
