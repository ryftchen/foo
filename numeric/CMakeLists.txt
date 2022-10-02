set(MODULE_NAME numeric)

aux_source_directory(source NUMERIC_LIST)

add_library(${MODULE_NAME} SHARED ${NUMERIC_LIST})

include_directories(../utility/include)

target_include_directories(${MODULE_NAME} PUBLIC include PRIVATE source)
