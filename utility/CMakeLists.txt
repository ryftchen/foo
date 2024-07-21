set(MODULE_NAME foo_util)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source UTILITY_LIST)

add_library(${MODULE_NAME} SHARED ${UTILITY_LIST})
add_library(foo::${MODULE_NAME} ALIAS ${MODULE_NAME})

target_include_directories(
    ${MODULE_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source)
target_link_libraries(${MODULE_NAME} PUBLIC readline)
