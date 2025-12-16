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

static double max_basis_z(const coord_t *bas, size_t n_bas)
{
    if (!bas || n_bas == 0) {
        return 0.0;
    }
    double max_z = bas[0].z;
    for (size_t i = 1; i < n_bas; ++i) {
        if (bas[i].z > max_z) {
            max_z = bas[i].z;
        }
    }
    return max_z;
}

typedef struct lattice_read_fixture {
    const char *path;
    lattice_t *lat;
    coord_t a1;
    coord_t a2;
    coord_t a3;
    coord_t nor;
    coord_t *bas;
    char *bas_name;
    size_t n_bas;
} lattice_read_fixture_t;

static int lattice_read_fixture_run(lattice_read_fixture_t *fixture,
                                    const char *contents)
{
    CLEED_TEST_ASSERT(fixture != NULL);
    CLEED_TEST_ASSERT(cleed_test_write_text_file(fixture->path, contents) == 0);

    fixture->lat = create_minimal_lattice();
    CLEED_TEST_ASSERT(fixture->lat != NULL);
    CLEED_TEST_ASSERT(lattice_set_input_filename(fixture->lat, fixture->path) == 0);

    fixture->a1 = (coord_t){.x = 0., .y = 0., .z = 0.};
    fixture->a2 = (coord_t){.x = 0., .y = 0., .z = 0.};
    fixture->a3 = (coord_t){.x = 0., .y = 0., .z = 0.};
    fixture->nor = (coord_t){.x = 0., .y = 0., .z = 0.};

    fixture->bas = (coord_t *)calloc(1, sizeof(coord_t));
    fixture->bas_name = (char *)calloc(NAMSZ, sizeof(char));
    fixture->n_bas = 0;
    CLEED_TEST_ASSERT(fixture->bas != NULL);
    CLEED_TEST_ASSERT(fixture->bas_name != NULL);

    const int rc = lattice_read(fixture->lat, &fixture->a1, &fixture->a2, &fixture->a3,
                                &fixture->nor, &fixture->bas, &fixture->bas_name,
                                &fixture->n_bas);
    CLEED_TEST_ASSERT(rc == 0);
    return 0;
}

static void lattice_read_fixture_free(lattice_read_fixture_t *fixture)
{
    if (!fixture) {
        return;
    }
    free(fixture->bas);
    free(fixture->bas_name);
    if (fixture->lat) {
        lattice_free(fixture->lat);
    }
    if (fixture->path) {
        cleed_test_remove_file(fixture->path);
    }
}

static int test_lattice_read_parses_commands(void)
{
    const char *contents =
        "a1: 1 0 0\n"
        "a2: 0 1 0\n"
        "a3: 0 0 5\n"
        "m1: 2 0\n"
        "m2: 0 3\n"
        "pb: Ca 0 0 0\n"
        "po: Ca 0 0 2\n"
        "il: 12.5\n"
        "nl: 4\n"
        "uc: 7\n"
        "q\n";

    lattice_read_fixture_t fixture = {.path = "test_lattice_read_full.latt"};
    CLEED_TEST_ASSERT(lattice_read_fixture_run(&fixture, contents) == 0);

    CLEED_TEST_ASSERT(fixture.n_bas == 2);
    CLEED_TEST_ASSERT_NEAR(fixture.a1.x, 2.0, 1e-12);
    CLEED_TEST_ASSERT_NEAR(fixture.a1.y, 0.0, 1e-12);
    CLEED_TEST_ASSERT_NEAR(fixture.a2.x, 0.0, 1e-12);
    CLEED_TEST_ASSERT_NEAR(fixture.a2.y, 3.0, 1e-12);
    CLEED_TEST_ASSERT_NEAR(fixture.a3.z, 5.0, 1e-12);

    CLEED_TEST_ASSERT_NEAR(fixture.lat->image_len, 12.5, 1e-12);
    CLEED_TEST_ASSERT(fixture.lat->max_layers == 4);
    CLEED_TEST_ASSERT(fixture.lat->max_cells == 7);

    CLEED_TEST_ASSERT(strcmp(fixture.bas_name, "Ca") == 0);
    CLEED_TEST_ASSERT(strcmp(fixture.bas_name + NAMSZ, "Ca") == 0);
    CLEED_TEST_ASSERT_NEAR(max_basis_z(fixture.bas, fixture.n_bas), 0.0, 1e-12);

    CLEED_TEST_ASSERT_NEAR(fixture.nor.x, 0.0, 1e-12);
    CLEED_TEST_ASSERT_NEAR(fixture.nor.y, 0.0, 1e-12);
    CLEED_TEST_ASSERT_NEAR(fixture.nor.z, 6.0, 1e-12);

    lattice_read_fixture_free(&fixture);
    return 0;
}

static int test_lattice_read_sets_max_layers_from_overlayer(void)
{
    const char *path = "test_lattice_read_overlayer.latt";
    const char *contents =
        "a1: 1 0 0\n"
        "a2: 0 1 0\n"
        "a3: 0 0 1\n"
        "po: Ca 0 0 0\n"
        "q\n";

    CLEED_TEST_ASSERT(cleed_test_write_text_file(path, contents) == 0);

    lattice_t *lat = create_minimal_lattice();
    CLEED_TEST_ASSERT(lat != NULL);
    CLEED_TEST_ASSERT(lattice_set_input_filename(lat, path) == 0);
    lat->max_layers = 0;

    coord_t a1 = {.x = 0., .y = 0., .z = 0.};
    coord_t a2 = {.x = 0., .y = 0., .z = 0.};
    coord_t a3 = {.x = 0., .y = 0., .z = 0.};
    coord_t nor = {.x = 0., .y = 0., .z = 0.};

    coord_t *bas = (coord_t *)calloc(1, sizeof(coord_t));
    char *bas_name = (char *)calloc(NAMSZ, sizeof(char));
    size_t n_bas = 0;
    CLEED_TEST_ASSERT(bas != NULL);
    CLEED_TEST_ASSERT(bas_name != NULL);

    const int rc = lattice_read(lat, &a1, &a2, &a3, &nor, &bas, &bas_name, &n_bas);
    CLEED_TEST_ASSERT(rc == 0);
    CLEED_TEST_ASSERT(lat->max_layers == 1);

    free(bas);
    free(bas_name);
    lattice_free(lat);
    cleed_test_remove_file(path);
    return 0;
}

int main(void)
{
    inf_stream = stderr;
    ctr_stream = stderr;

    if (test_lattice_read_parses_commands() != 0) {
        return 1;
    }
    if (test_lattice_read_sets_max_layers_from_overlayer() != 0) {
        return 1;
    }

    return 0;
}
