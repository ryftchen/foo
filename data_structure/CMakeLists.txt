set(MODULE_NAME data_structure)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source DATA_STRUCTURE_LIST)
include_directories(${CMAKE_SOURCE_DIR})

add_library(${MODULE_NAME} SHARED ${DATA_STRUCTURE_LIST})
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
    target_precompile_headers(${MODULE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/pch_data_structure.hpp)
endif()
