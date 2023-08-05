set(MODULE_NAME utility)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source UTILITY_LIST)

add_library(${MODULE_NAME} STATIC ${UTILITY_LIST})
add_library(foo::${MODULE_NAME} ALIAS ${MODULE_NAME})

target_include_directories(
    ${MODULE_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source
)
