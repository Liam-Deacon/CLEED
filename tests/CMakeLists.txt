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

add_executable(test_lattice_read_edge_cases
    test_lattice_read_edge_cases.c
    $<TARGET_OBJECTS:latt_core>
    $<TARGET_OBJECTS:cleed_test_support>
)
target_include_directories(test_lattice_read_edge_cases PRIVATE ${CLEED_TEST_INCLUDE_DIRS})
target_link_libraries(test_lattice_read_edge_cases PRIVATE m)
add_test(NAME latt.read_edge_cases COMMAND test_lattice_read_edge_cases)

add_executable(test_lattice_setup
    test_lattice_setup.c
    $<TARGET_OBJECTS:latt_core>
    $<TARGET_OBJECTS:cleed_test_support>
)
target_include_directories(test_lattice_setup PRIVATE ${CLEED_TEST_INCLUDE_DIRS})
target_link_libraries(test_lattice_setup PRIVATE m)
add_test(NAME latt.setup COMMAND test_lattice_setup)

add_executable(test_search_amoeba
    test_search_amoeba.c
    $<TARGET_OBJECTS:cleed_test_support>
)
target_include_directories(test_search_amoeba PRIVATE ${CLEED_TEST_INCLUDE_DIRS})
if (WIN32)
    target_link_libraries(test_search_amoeba PRIVATE searchStatic m)
else()
    target_link_libraries(test_search_amoeba PRIVATE search m)
endif()
add_test(NAME search.amoeba COMMAND test_search_amoeba)

add_executable(test_search_powell
    test_search_powell.c
    $<TARGET_OBJECTS:cleed_test_support>
)
target_include_directories(test_search_powell PRIVATE ${CLEED_TEST_INCLUDE_DIRS})
if (WIN32)
    target_link_libraries(test_search_powell PRIVATE searchStatic m)
else()
    target_link_libraries(test_search_powell PRIVATE search m)
endif()
add_test(NAME search.powell COMMAND test_search_powell)

add_executable(test_search_amebsa
    test_search_amebsa.c
    $<TARGET_OBJECTS:cleed_test_support>
)
target_include_directories(test_search_amebsa PRIVATE ${CLEED_TEST_INCLUDE_DIRS})
if (WIN32)
    target_link_libraries(test_search_amebsa PRIVATE searchStatic m)
else()
    target_link_libraries(test_search_amebsa PRIVATE search m)
endif()
add_test(NAME search.amebsa COMMAND test_search_amebsa)

add_executable(test_search_parse
    test_search_parse.c
    $<TARGET_OBJECTS:cleed_test_support>
)
target_include_directories(test_search_parse PRIVATE ${CLEED_TEST_INCLUDE_DIRS})
if (WIN32)
    target_link_libraries(test_search_parse PRIVATE searchStatic m)
else()
    target_link_libraries(test_search_parse PRIVATE search m)
endif()
add_test(NAME search.parse COMMAND test_search_parse)

add_executable(test_search_vertex_stats
    test_search_vertex_stats.c
    $<TARGET_OBJECTS:cleed_test_support>
)
target_include_directories(test_search_vertex_stats PRIVATE ${CLEED_TEST_INCLUDE_DIRS})
if (WIN32)
    target_link_libraries(test_search_vertex_stats PRIVATE searchStatic m)
else()
    target_link_libraries(test_search_vertex_stats PRIVATE search m)
endif()
add_test(NAME search.vertex_stats COMMAND test_search_vertex_stats)

add_executable(test_rfac_spline
    test_rfac_spline.c
)
target_include_directories(test_rfac_spline PRIVATE ${CLEED_TEST_INCLUDE_DIRS})
if (WIN32)
    target_link_libraries(test_rfac_spline PRIVATE rfacStatic m)
else()
    target_link_libraries(test_rfac_spline PRIVATE rfac m)
endif()
add_test(NAME rfac.spline COMMAND test_rfac_spline)

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

add_test(
    NAME mkiv.help
    COMMAND $<TARGET_FILE:mkiv> -h
)
set_tests_properties(mkiv.help PROPERTIES
    PASS_REGULAR_EXPRESSION "Syntax:"
)

add_test(
    NAME mkiv.version
    COMMAND $<TARGET_FILE:mkiv> -V
)
set_tests_properties(mkiv.version PROPERTIES
    PASS_REGULAR_EXPRESSION "version"
)
