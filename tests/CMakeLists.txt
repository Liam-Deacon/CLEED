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

add_executable(test_search_optimizer
    test_search_optimizer.c
    $<TARGET_OBJECTS:cleed_test_support>
)
target_include_directories(test_search_optimizer PRIVATE ${CLEED_TEST_INCLUDE_DIRS})
if (WIN32)
    target_link_libraries(test_search_optimizer PRIVATE searchStatic m)
else()
    target_link_libraries(test_search_optimizer PRIVATE search m)
endif()
add_test(NAME search.optimizer COMMAND test_search_optimizer)

add_executable(test_search_pso
    test_search_pso.c
    $<TARGET_OBJECTS:cleed_test_support>
)
target_include_directories(test_search_pso PRIVATE ${CLEED_TEST_INCLUDE_DIRS})
if (WIN32)
    target_link_libraries(test_search_pso PRIVATE searchStatic m)
else()
    target_link_libraries(test_search_pso PRIVATE search m)
endif()
add_test(NAME search.pso COMMAND test_search_pso)

add_executable(test_search_de
    test_search_de.c
    $<TARGET_OBJECTS:cleed_test_support>
)
target_include_directories(test_search_de PRIVATE ${CLEED_TEST_INCLUDE_DIRS})
if (WIN32)
    target_link_libraries(test_search_de PRIVATE searchStatic m)
else()
    target_link_libraries(test_search_de PRIVATE search m)
endif()
add_test(NAME search.de COMMAND test_search_de)

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

add_executable(test_search_simplex
    test_search_simplex.c
    $<TARGET_OBJECTS:cleed_test_support>
)
target_include_directories(test_search_simplex PRIVATE ${CLEED_TEST_INCLUDE_DIRS})
if (WIN32)
    target_link_libraries(test_search_simplex PRIVATE searchStatic m)
else()
    target_link_libraries(test_search_simplex PRIVATE search m)
endif()
add_test(NAME search.simplex_utils COMMAND test_search_simplex)

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

add_executable(fake_csearch_leed
    fakes/fake_csearch_leed.c
)

add_executable(fake_csearch_rfac
    fakes/fake_csearch_rfac.c
)

add_test(
    NAME csearch.e2e_stub_simplex
    COMMAND ${CMAKE_COMMAND}
        -DPROGRAM=$<TARGET_FILE:csearch>
        -DLEED_PROGRAM=$<TARGET_FILE:fake_csearch_leed>
        -DRFAC_PROGRAM=$<TARGET_FILE:fake_csearch_rfac>
        -DINPUT=${PROJECT_SOURCE_DIR}/tests/fixtures/csearch_stub/stub.inp
        -DBULK=${PROJECT_SOURCE_DIR}/tests/fixtures/csearch_stub/stub.bul
        -DCTR=${PROJECT_SOURCE_DIR}/tests/fixtures/csearch_stub/stub.ctr
        -DOUT_BASENAME=stub
        -P ${PROJECT_SOURCE_DIR}/tests/cmake/run_csearch_e2e.cmake
)

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
    NAME latt.example_tio2_110
    COMMAND ${CMAKE_COMMAND}
        -DPROGRAM=$<TARGET_FILE:latt>
        -DINPUT=${PROJECT_SOURCE_DIR}/examples/latt/TiO2_110_1x1.latt
        -DH=1 -DK=1 -DL=0
        -DOUT_BASENAME=tio2_110_1x1
        -DNEEDLES_PIPE=Read lattice input from file|pb: Ti|pb: O|il:
        -P ${PROJECT_SOURCE_DIR}/tests/cmake/run_latt_example.cmake
)

add_test(
    NAME latt.example_re_0001
    COMMAND ${CMAKE_COMMAND}
        -DPROGRAM=$<TARGET_FILE:latt>
        -DINPUT=${PROJECT_SOURCE_DIR}/examples/latt/Re_0001.latt
        -DH=0 -DK=0 -DL=1
        -DOUT_BASENAME=re_0001
        -DNEEDLES_PIPE=Read lattice input from file|pb: Re|il:
        -P ${PROJECT_SOURCE_DIR}/tests/cmake/run_latt_example.cmake
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
