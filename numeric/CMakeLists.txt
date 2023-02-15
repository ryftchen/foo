set(MODULE_NAME numeric)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source NUMERIC_LIST)
include_directories(${CMAKE_SOURCE_DIR})

add_library(${MODULE_NAME} SHARED ${NUMERIC_LIST})
add_library(foo::${MODULE_NAME} ALIAS ${MODULE_NAME})

target_include_directories(
    ${MODULE_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source
)
target_link_libraries(${MODULE_NAME} PUBLIC foo::utility)
if(PRECOMPILED_HEADER)
    target_precompile_headers(
        ${MODULE_NAME}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/pch_numeric.hpp
    )
endif()
