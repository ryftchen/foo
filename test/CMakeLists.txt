cmake_minimum_required(VERSION 3.0)

project(foo_test)

include(CheckCXXCompilerFlag)
include(CheckIncludeFile)
check_cxx_compiler_flag(-std=c++20 COMPILER_SUPPORTS_CXX20)
check_include_file("gtest/gtest.h" EXIST_GTEST_GTEST_HEADER)
check_include_file("mpfr.h" EXIST_MPFR_HEADER)
find_package(GTest REQUIRED)

if(DEFINED ENV{FOO_ENV})
    message(STATUS "The test project env: FOO_ENV=$ENV{FOO_ENV}")
endif()
include(${CMAKE_SOURCE_DIR}/../cmake/common.cmake)
include(${CMAKE_SOURCE_DIR}/../cmake/toolchain.cmake)
set(RUNTIME_PRINTING OFF CACHE BOOL "runtime printing" FORCE)
include(${CMAKE_SOURCE_DIR}/../cmake/condition.cmake)

set(BIN_NAME foo_test)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${BUILD_DIR}/lib)
include(${CMAKE_SOURCE_DIR}/ut_module.cmake)

aux_source_directory(${CMAKE_SOURCE_DIR}/unit TEST_LIST)
include_directories(
    ${CMAKE_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/../algorithm/include
    ${CMAKE_SOURCE_DIR}/../data_structure/include
    ${CMAKE_SOURCE_DIR}/../design_pattern/include
    ${CMAKE_SOURCE_DIR}/../numeric/include
)

add_executable(${BIN_NAME} ${TEST_LIST})
target_link_libraries(
    ${BIN_NAME}
    PUBLIC mpfr GTest::Main
    PRIVATE foo_test::algorithm foo_test::data_structure foo_test::design_pattern foo_test::numeric
)

if(PRECOMPILED_HEADER)
    target_precompile_headers(
        ${BIN_NAME}
        PRIVATE ${CMAKE_SOURCE_DIR}/../application/pch/precompiled_header.hpp
    )
endif()
