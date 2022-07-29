set(MODULE_NAME algorithm)

aux_source_directory(source ALGORITHM_LIST)

add_library(${MODULE_NAME} SHARED ${ALGORITHM_LIST})

include_directories(../utility/include)

target_include_directories(${MODULE_NAME} PUBLIC include PRIVATE source)
