include_guard()

set(MODULE_NAME_1 foo_test_algo)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../algorithm/source ALGORITHM_LIST)
add_library(${MODULE_NAME_1} STATIC ${ALGORITHM_LIST})
add_library(foo_test::${MODULE_NAME_1} ALIAS ${MODULE_NAME_1})
target_include_directories(
    ${MODULE_NAME_1}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../algorithm/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../algorithm/source
)

set(MODULE_NAME_2 foo_test_dp)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../design_pattern/source DESIGN_PATTERN_LIST)
add_library(${MODULE_NAME_2} STATIC ${DESIGN_PATTERN_LIST})
add_library(foo_test::${MODULE_NAME_2} ALIAS ${MODULE_NAME_2})
target_include_directories(
    ${MODULE_NAME_2}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../design_pattern/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../design_pattern/source
)

set(MODULE_NAME_3 foo_test_ds)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../data_structure/source DATA_STRUCTURE_LIST)
add_library(${MODULE_NAME_3} STATIC ${DATA_STRUCTURE_LIST})
add_library(foo_test::${MODULE_NAME_3} ALIAS ${MODULE_NAME_3})
target_include_directories(
    ${MODULE_NAME_3}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../data_structure/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../data_structure/source
)

set(MODULE_NAME_4 foo_test_num)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../numeric/source NUMERIC_LIST)
add_library(${MODULE_NAME_4} STATIC ${NUMERIC_LIST})
add_library(foo_test::${MODULE_NAME_4} ALIAS ${MODULE_NAME_4})
target_include_directories(
    ${MODULE_NAME_4}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../numeric/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../numeric/source
)
