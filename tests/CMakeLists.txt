cmake_minimum_required(VERSION 3.16)

set(CLEED_TEST_INCLUDE_DIRS
    "${PROJECT_SOURCE_DIR}/src/include"
)

add_library(cleed_test_support OBJECT
    test_support.c
)
target_include_directories(cleed_test_support PRIVATE ${CLEED_TEST_INCLUDE_DIRS})

add_executable(test_lattice_read
    test_lattice_read.c
    $<TARGET_OBJECTS:latt_core>
    $<TARGET_OBJECTS:cleed_test_support>
)
target_include_directories(test_lattice_read PRIVATE ${CLEED_TEST_INCLUDE_DIRS})
target_link_libraries(test_lattice_read PRIVATE m)
add_test(NAME latt.read COMMAND test_lattice_read)

if (WITH_GSL STREQUAL "OFF")
    add_executable(test_search_amoeba
        test_search_amoeba.c
        $<TARGET_OBJECTS:cleed_test_support>
    )
    target_include_directories(test_search_amoeba PRIVATE ${CLEED_TEST_INCLUDE_DIRS})
    target_link_libraries(test_search_amoeba PRIVATE search m)
    add_test(NAME search.amoeba COMMAND test_search_amoeba)
endif()

add_test(
    NAME latt.minimal_fixture
    COMMAND $<TARGET_FILE:latt>
        -h 1 -k 0 -l 0
        --input "${PROJECT_SOURCE_DIR}/tests/fixtures/minimal.latt"
        --max-layers 1
        --max-cells 1
        --max-atoms 8
)
set_tests_properties(latt.minimal_fixture PROPERTIES
    PASS_REGULAR_EXPRESSION "Ca"
)
