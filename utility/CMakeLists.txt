set(MODULE_NAME utility)

aux_source_directory(source UTILITY_LIST)

add_library(utility SHARED ${UTILITY_LIST})

target_include_directories(${MODULE_NAME} PUBLIC include PRIVATE source)
