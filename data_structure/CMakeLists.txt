set(MODULE_NAME foo_ds)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source DATA_STRUCTURE_LIST)
include_directories(${CMAKE_SOURCE_DIR})

add_library(${MODULE_NAME} SHARED ${DATA_STRUCTURE_LIST})
add_library(foo::${MODULE_NAME} ALIAS ${MODULE_NAME})

target_include_directories(
    ${MODULE_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source)
