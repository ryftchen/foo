include_guard()

set(MODULE_NAME_1 test_algorithm)
file(GLOB TEST_ALGORITHM_LIST ${CMAKE_SOURCE_DIR}/unit/test_algorithm.cpp)
aux_source_directory(${CMAKE_SOURCE_DIR}/../algorithm/source TEST_ALGORITHM_LIST)
add_library(${MODULE_NAME_1} STATIC ${TEST_ALGORITHM_LIST})
add_library(foo_test::${MODULE_NAME_1} ALIAS ${MODULE_NAME_1})
target_include_directories(
    ${MODULE_NAME_1}
    PUBLIC ${CMAKE_SOURCE_DIR}/../algorithm/include
    PRIVATE ${CMAKE_SOURCE_DIR}/../algorithm/source
)

set(MODULE_NAME_2 test_data_structure)
file(GLOB TEST_DATA_STRUCTURE_LIST ${CMAKE_SOURCE_DIR}/unit/test_data_structure.cpp)
aux_source_directory(${CMAKE_SOURCE_DIR}/../data_structure/source TEST_DATA_STRUCTURE_LIST)
add_library(${MODULE_NAME_2} STATIC ${TEST_DATA_STRUCTURE_LIST})
add_library(foo_test::${MODULE_NAME_2} ALIAS ${MODULE_NAME_2})
target_include_directories(
    ${MODULE_NAME_2}
    PUBLIC ${CMAKE_SOURCE_DIR}/../data_structure/include
    PRIVATE ${CMAKE_SOURCE_DIR}/../data_structure/source
)

set(MODULE_NAME_3 test_design_pattern)
file(GLOB TEST_DESIGN_PATTERN_LIST ${CMAKE_SOURCE_DIR}/unit/test_design_pattern.cpp)
aux_source_directory(${CMAKE_SOURCE_DIR}/../design_pattern/source TEST_DESIGN_PATTERN_LIST)
add_library(${MODULE_NAME_3} STATIC ${TEST_DESIGN_PATTERN_LIST})
add_library(foo_test::${MODULE_NAME_3} ALIAS ${MODULE_NAME_3})
target_include_directories(
    ${MODULE_NAME_3}
    PUBLIC ${CMAKE_SOURCE_DIR}/../design_pattern/include
    PRIVATE ${CMAKE_SOURCE_DIR}/../design_pattern/source
)

set(MODULE_NAME_4 test_numeric)
file(GLOB TEST_NUMERIC_LIST ${CMAKE_SOURCE_DIR}/unit/test_numeric.cpp)
aux_source_directory(${CMAKE_SOURCE_DIR}/../numeric/source TEST_NUMERIC_LIST)
add_library(${MODULE_NAME_4} STATIC ${TEST_NUMERIC_LIST})
add_library(foo_test::${MODULE_NAME_4} ALIAS ${MODULE_NAME_4})
target_include_directories(
    ${MODULE_NAME_4}
    PUBLIC ${CMAKE_SOURCE_DIR}/../numeric/include
    PRIVATE ${CMAKE_SOURCE_DIR}/../numeric/source
)
