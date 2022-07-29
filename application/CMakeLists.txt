set(MODULE_NAME foo)

aux_source_directory(source APPLICATION_LIST)

add_executable(${MODULE_NAME} ${APPLICATION_LIST})

include_directories(../utility/include ../algorithm/include)

target_include_directories(${MODULE_NAME} PUBLIC include PRIVATE source)

target_link_libraries(${MODULE_NAME} utility algorithm pthread)
