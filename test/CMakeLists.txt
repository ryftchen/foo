cmake_minimum_required(VERSION 3.0)

project(foo_test)

include(CheckCXXCompilerFlag)
include(CheckIncludeFile)
check_cxx_compiler_flag(-std=c++20 COMPILER_SUPPORTS_CXX20)
check_include_file("mpfr.h" HAVE_MPFR_HEADER)
check_include_file("sys/time.h" HAVE_SYS_TIME_HEADER)
find_package(GTest REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)
set(CMAKE_C_COMPILER /usr/bin/clang-12)
set(CMAKE_CXX_COMPILER /usr/bin/clang++-12)
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
add_compile_options(-std=c++20 -stdlib=libstdc++ -W -fcolor-diagnostics)
option(RUNTIME_PRINTING "runtime printing" OFF)
if(RUNTIME_PRINTING)
    add_compile_options(-D__RUNTIME_PRINTING__)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,-export-dynamic")
endif()
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time") 
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CMAKE_COMMAND} -E time")

set(BIN_NAME foo_test)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR}/bin)

file(GLOB TEST_LIST)
aux_source_directory(${CMAKE_SOURCE_DIR}/unit TEST_LIST)
aux_source_directory(${CMAKE_SOURCE_DIR}/../algorithm/source TEST_LIST)
aux_source_directory(${CMAKE_SOURCE_DIR}/../data_structure/source TEST_LIST)
aux_source_directory(${CMAKE_SOURCE_DIR}/../design_pattern/source TEST_LIST)
aux_source_directory(${CMAKE_SOURCE_DIR}/../numeric/source TEST_LIST)
include_directories(
    ${CMAKE_SOURCE_DIR}/../
    ${CMAKE_SOURCE_DIR}/../algorithm/include
    ${CMAKE_SOURCE_DIR}/../data_structure/include
    ${CMAKE_SOURCE_DIR}/../design_pattern/include
    ${CMAKE_SOURCE_DIR}/../numeric/include)

add_executable(${BIN_NAME} ${TEST_LIST})
target_link_libraries(
    ${BIN_NAME}
    PUBLIC
        mpfr
        GTest::Main)
