cmake_minimum_required(VERSION 3.20)

project(foo)

include(CheckCXXCompilerFlag)
include(CheckIncludeFile)
check_cxx_compiler_flag(-std=c++20 COMPILER_SUPPORTS_CXX20)
check_include_file("openssl/evp.h" EXIST_OPENSSL_EVP_HEADER)
check_include_file("mpfr.h" EXIST_MPFR_HEADER)
check_include_file("readline/readline.h" EXIST_READLINE_READLINE_HEADER)
check_include_file("readline/history.h" EXIST_READLINE_HISTORY_HEADER)

if(DEFINED ENV{FOO_ENV})
    message(STATUS "The project env: FOO_ENV=$ENV{FOO_ENV}")
endif()
include(${CMAKE_SOURCE_DIR}/cmake/common.cmake)
if((DEFINED ENV{FOO_BLD_SCA_COV}) AND ("$ENV{FOO_BLD_SCA_COV}" STREQUAL "on"))
    message(STATUS "Checking coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
endif()
include(${CMAKE_SOURCE_DIR}/cmake/toolchain.cmake)
set(RUNTIME_PRINTING ON CACHE BOOL "runtime printing" FORCE)
include(${CMAKE_SOURCE_DIR}/cmake/condition.cmake)

set(BIN_NAME foo)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${BUILD_DIR}/lib)

aux_source_directory(${CMAKE_SOURCE_DIR}/application/core/source APPLICATION_LIST)
aux_source_directory(${CMAKE_SOURCE_DIR}/application/example/source APPLICATION_LIST)
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/application/core/include
    ${CMAKE_SOURCE_DIR}/application/example/include)

add_subdirectory(${CMAKE_SOURCE_DIR}/utility)
add_subdirectory(${CMAKE_SOURCE_DIR}/algorithm)
add_subdirectory(${CMAKE_SOURCE_DIR}/design_pattern)
add_subdirectory(${CMAKE_SOURCE_DIR}/data_structure)
add_subdirectory(${CMAKE_SOURCE_DIR}/numeric)

add_executable(${BIN_NAME} ${APPLICATION_LIST})
target_include_directories(
    ${BIN_NAME}
    PUBLIC ${CMAKE_SOURCE_DIR}/application/core/include ${CMAKE_SOURCE_DIR}/application/example/include
    PRIVATE ${CMAKE_SOURCE_DIR}/application/core/source ${CMAKE_SOURCE_DIR}/application/example/source)
target_link_libraries(
    ${BIN_NAME}
    PUBLIC crypto mpfr
    PRIVATE foo::foo_util foo::foo_algo foo::foo_dp foo::foo_ds foo::foo_num)

if(PRECOMPILED_HEADER)
    target_precompile_headers(${BIN_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/application/pch/precompiled_header.hpp)
endif()

set(CMAKE_INSTALL_PREFIX "/opt/${PROJECT_NAME}")
install(
    TARGETS ${BIN_NAME} foo_util foo_algo foo_dp foo_ds foo_num
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX})
set_target_properties(${BIN_NAME} PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
