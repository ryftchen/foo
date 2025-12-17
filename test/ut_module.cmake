include_guard()

set(MODULE_NAME_ALGO foo_test_algo)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../algorithm/source ALGORITHM_LIST)
add_library(${MODULE_NAME_ALGO} STATIC ${ALGORITHM_LIST})
add_library(${PROJECT_NAME}::${MODULE_NAME_ALGO} ALIAS ${MODULE_NAME_ALGO})
target_include_directories(
    ${MODULE_NAME_ALGO}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../algorithm/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../algorithm/source)
set(MODULE_TARGETS ${MODULE_TARGETS} ${MODULE_NAME_ALGO})
unset(MODULE_NAME_ALGO)

set(MODULE_NAME_DP foo_test_dp)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../design_pattern/source DESIGN_PATTERN_LIST)
add_library(${MODULE_NAME_DP} STATIC ${DESIGN_PATTERN_LIST})
add_library(${PROJECT_NAME}::${MODULE_NAME_DP} ALIAS ${MODULE_NAME_DP})
target_include_directories(
    ${MODULE_NAME_DP}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../design_pattern/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../design_pattern/source)
set(MODULE_TARGETS ${MODULE_TARGETS} ${MODULE_NAME_DP})
unset(MODULE_NAME_DP)

set(MODULE_NAME_DS foo_test_ds)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../data_structure/source DATA_STRUCTURE_LIST)
add_library(${MODULE_NAME_DS} STATIC ${DATA_STRUCTURE_LIST})
add_library(${PROJECT_NAME}::${MODULE_NAME_DS} ALIAS ${MODULE_NAME_DS})
target_include_directories(
    ${MODULE_NAME_DS}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../data_structure/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../data_structure/source)
set(MODULE_TARGETS ${MODULE_TARGETS} ${MODULE_NAME_DS})
unset(MODULE_NAME_DS)

set(MODULE_NAME_NUM foo_test_num)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../numeric/source NUMERIC_LIST)
add_library(${MODULE_NAME_NUM} STATIC ${NUMERIC_LIST})
add_library(${PROJECT_NAME}::${MODULE_NAME_NUM} ALIAS ${MODULE_NAME_NUM})
target_include_directories(
    ${MODULE_NAME_NUM}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../numeric/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../numeric/source)
set(MODULE_TARGETS ${MODULE_TARGETS} ${MODULE_NAME_NUM})
unset(MODULE_NAME_NUM)
