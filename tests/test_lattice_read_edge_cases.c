// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdio.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>
// cppcheck-suppress missingIncludeSystem
#include <string.h>

#include "lattice.h"
#include "test_support.h"

FILE *inf_stream = NULL;
FILE *ctr_stream = NULL;

static lattice_t *create_minimal_lattice(void)
{
    lattice_t *lat = lattice_init(1);
    if (!lat) {
        return NULL;
    }
    lat->vec_h = 0.0;
    lat->vec_k = 0.0;
    lat->vec_l = 1.0;
    return lat;
}

static void free_read(lattice_t *lat, coord_t *bas, char *bas_name)
{
    free(bas);
    free(bas_name);
    lattice_free(lat);
}

static int test_lattice_read_missing_file_returns_error(void)
{
    lattice_t *lat = create_minimal_lattice();
    CLEED_TEST_ASSERT(lat != NULL);
    CLEED_TEST_ASSERT(lattice_set_input_filename(lat, "does-not-exist.latt") == 0);

    coord_t a1 = {0}, a2 = {0}, a3 = {0}, nor = {0};
    coord_t *bas = (coord_t *)calloc(1, sizeof(coord_t));
    char *bas_name = (char *)calloc(NAMSZ, sizeof(char));
    size_t n_bas = 0;
    CLEED_TEST_ASSERT(bas != NULL);
    CLEED_TEST_ASSERT(bas_name != NULL);

    const int rc = lattice_read(lat, &a1, &a2, &a3, &nor, &bas, &bas_name, &n_bas);
    CLEED_TEST_ASSERT(rc != 0);

    free_read(lat, bas, bas_name);
    return 0;
}

static int test_lattice_read_truncates_long_element_name(void)
{
    const char *path = "test_lattice_read_long_name.latt";
    const char *contents =
        "a1: 1 0 0\n"
        "a2: 0 1 0\n"
        "a3: 0 0 1\n"
        "pb: AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAA 0 0 0\n";

    CLEED_TEST_ASSERT(cleed_test_write_text_file(path, contents) == 0);

    lattice_t *lat = create_minimal_lattice();
    CLEED_TEST_ASSERT(lat != NULL);
    CLEED_TEST_ASSERT(lattice_set_input_filename(lat, path) == 0);

    coord_t a1 = {0}, a2 = {0}, a3 = {0}, nor = {0};
    coord_t *bas = (coord_t *)calloc(1, sizeof(coord_t));
    char *bas_name = (char *)calloc(NAMSZ, sizeof(char));
    size_t n_bas = 0;
    CLEED_TEST_ASSERT(bas != NULL);
    CLEED_TEST_ASSERT(bas_name != NULL);

    CLEED_TEST_ASSERT(lattice_read(lat, &a1, &a2, &a3, &nor, &bas, &bas_name, &n_bas) == 0);
    CLEED_TEST_ASSERT(n_bas == 1);
    CLEED_TEST_ASSERT(strnlen(bas_name, NAMSZ) == 127);

    free_read(lat, bas, bas_name);
    cleed_test_remove_file(path);
    return 0;
}

static int write_many_basis_entries(const char *path, size_t count)
{
    FILE *file = fopen(path, "w");
    if (!file) {
        return -1;
    }
    fputs("a1: 1 0 0\n", file);
    fputs("a2: 0 1 0\n", file);
    fputs("a3: 0 0 1\n", file);
    for (size_t i = 0; i < count; ++i) {
        fprintf(file, "pb: Ca 0 0 %zu\n", i);
    }
    fclose(file);
    return 0;
}

static int test_lattice_read_caps_basis_entries(void)
{
    const char *path = "test_lattice_read_many_basis.latt";
    const size_t requested = (size_t)MAX_INP_ATOMS + 8;
    CLEED_TEST_ASSERT(write_many_basis_entries(path, requested) == 0);

    lattice_t *lat = create_minimal_lattice();
    CLEED_TEST_ASSERT(lat != NULL);
    CLEED_TEST_ASSERT(lattice_set_input_filename(lat, path) == 0);

    coord_t a1 = {0}, a2 = {0}, a3 = {0}, nor = {0};
    coord_t *bas = (coord_t *)calloc(1, sizeof(coord_t));
    char *bas_name = (char *)calloc(NAMSZ, sizeof(char));
    size_t n_bas = 0;
    CLEED_TEST_ASSERT(bas != NULL);
    CLEED_TEST_ASSERT(bas_name != NULL);

    CLEED_TEST_ASSERT(lattice_read(lat, &a1, &a2, &a3, &nor, &bas, &bas_name, &n_bas) == 0);
    CLEED_TEST_ASSERT(n_bas == (size_t)MAX_INP_ATOMS);

    free_read(lat, bas, bas_name);
    cleed_test_remove_file(path);
    return 0;
}

int main(void)
{
    inf_stream = stderr;
    ctr_stream = stderr;

    if (test_lattice_read_missing_file_returns_error() != 0) {
        return 1;
    }
    if (test_lattice_read_truncates_long_element_name() != 0) {
        return 1;
    }
    if (test_lattice_read_caps_basis_entries() != 0) {
        return 1;
    }

    return 0;
}
