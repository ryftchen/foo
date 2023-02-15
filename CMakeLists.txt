cmake_minimum_required(VERSION 3.0)

project(foo)

include(CheckCXXCompilerFlag)
include(CheckIncludeFile)
include(CheckIncludeFiles)
check_cxx_compiler_flag(-std=c++20 COMPILER_SUPPORTS_CXX20)
check_include_file("ext/stdio_filebuf.h" HAVE_EXT_STDIO_FILEBUF_HEADER)
check_include_file("cxxabi.h" HAVE_CXXABI_HEADER)
check_include_file("dlfcn.h" HAVE_DLFCN_HEADER)
check_include_file("execinfo.h" HAVE_EXECINFO_HEADER)
check_include_file("mpfr.h" HAVE_MPFR_HEADER)
check_include_file("unistd.h" HAVE_UNISTD_HEADER)
check_include_files("readline/history.h;readline/readline.h" HAVE_READLINE_HEADER)
check_include_files("sys/file.h;sys/time.h" HAVE_SYS_HEADER)
find_package(Threads REQUIRED)

include(${CMAKE_SOURCE_DIR}/cmake/common.cmake)
if(DEFINED ENV{FOO_ENV})
    message(STATUS "The project env: $ENV{FOO_ENV}")
    if("$ENV{FOO_ENV}" STREQUAL "foo_cov")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
    endif()
endif()
include(${CMAKE_SOURCE_DIR}/cmake/toolchain.cmake)
set(RUNTIME_PRINTING ON CACHE BOOL "runtime printing" FORCE)
include(${CMAKE_SOURCE_DIR}/cmake/condition.cmake)

set(BIN_NAME foo)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${BUILD_DIR}/lib)

aux_source_directory(${CMAKE_SOURCE_DIR}/application/source APPLICATION_LIST)
include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(${CMAKE_SOURCE_DIR}/utility)
add_subdirectory(${CMAKE_SOURCE_DIR}/algorithm)
add_subdirectory(${CMAKE_SOURCE_DIR}/data_structure)
add_subdirectory(${CMAKE_SOURCE_DIR}/design_pattern)
add_subdirectory(${CMAKE_SOURCE_DIR}/numeric)

add_executable(${BIN_NAME} ${APPLICATION_LIST})
target_include_directories(
    ${BIN_NAME}
    PUBLIC ${CMAKE_SOURCE_DIR}/application/include
    PRIVATE ${CMAKE_SOURCE_DIR}/application/source
)
target_link_libraries(
    ${BIN_NAME}
    PUBLIC
        dl
        pthread
        readline
        mpfr
    PRIVATE
        foo::utility
        foo::algorithm
        foo::data_structure
        foo::design_pattern
        foo::numeric
)
