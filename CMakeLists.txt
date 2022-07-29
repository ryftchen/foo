cmake_minimum_required(VERSION 3.0)

project(foo)

set(BUILD_DIR ${PROJECT_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${BUILD_DIR}/lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++-12")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
if("$ENV{FOO_ENV}" STREQUAL "CODE_COVERAGE")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
endif()
add_compile_options(-std=c++20 -stdlib=libstdc++ -W)

add_subdirectory(application)
add_subdirectory(utility)
add_subdirectory(algorithm)
