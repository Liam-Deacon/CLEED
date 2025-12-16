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

#define ASSERT_TRUE(condition)                                                 \
    do {                                                                       \
        if (!(condition)) {                                                    \
            fprintf(stderr, "assert failed: %s (%s:%d)\n", #condition,         \
                    __FILE__, __LINE__);                                       \
            return 1;                                                          \
        }                                                                      \
    } while (0)

#define ASSERT_NEAR(actual, expected, tol)                                     \
    do {                                                                       \
        const double diff = fabs((double)(actual) - (double)(expected));       \
        if (diff > (tol)) {                                                    \
            fprintf(stderr,                                                    \
                    "assert failed: %s ~= %s (got=%g expected=%g tol=%g) "     \
                    "(%s:%d)\n",                                               \
                    #actual, #expected, (double)(actual), (double)(expected),  \
                    (double)(tol), __FILE__, __LINE__);                        \
            return 1;                                                          \
        }                                                                      \
    } while (0)

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

static int test_lattice_read_parses_commands(void)
{
    const char *path = "test_lattice_read_full.latt";
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

    ASSERT_TRUE(cleed_test_write_text_file(path, contents) == 0);

    lattice_t *lat = create_minimal_lattice();
    ASSERT_TRUE(lat != NULL);
    ASSERT_TRUE(lattice_set_input_filename(lat, path) == 0);

    coord_t a1 = {.x = 0., .y = 0., .z = 0.};
    coord_t a2 = {.x = 0., .y = 0., .z = 0.};
    coord_t a3 = {.x = 0., .y = 0., .z = 0.};
    coord_t nor = {.x = 0., .y = 0., .z = 0.};

    coord_t *bas = (coord_t *)calloc(1, sizeof(coord_t));
    char *bas_name = (char *)calloc(NAMSZ, sizeof(char));
    size_t n_bas = 0;
    ASSERT_TRUE(bas != NULL);
    ASSERT_TRUE(bas_name != NULL);

    const int rc = lattice_read(lat, &a1, &a2, &a3, &nor, &bas, &bas_name, &n_bas);
    ASSERT_TRUE(rc == 0);
    ASSERT_TRUE(n_bas == 2);

    ASSERT_NEAR(a1.x, 2.0, 1e-12);
    ASSERT_NEAR(a1.y, 0.0, 1e-12);
    ASSERT_NEAR(a2.x, 0.0, 1e-12);
    ASSERT_NEAR(a2.y, 3.0, 1e-12);
    ASSERT_NEAR(a3.z, 5.0, 1e-12);

    ASSERT_NEAR(lat->image_len, 12.5, 1e-12);
    ASSERT_TRUE(lat->max_layers == 4);
    ASSERT_TRUE(lat->max_cells == 7);

    ASSERT_TRUE(strcmp(bas_name, "Ca") == 0);
    ASSERT_TRUE(strcmp(bas_name + NAMSZ, "Ca") == 0);

    double max_z = bas[0].z;
    for (size_t i = 1; i < n_bas; ++i) {
        if (bas[i].z > max_z) {
            max_z = bas[i].z;
        }
    }
    ASSERT_NEAR(max_z, 0.0, 1e-12);

    ASSERT_NEAR(nor.x, 0.0, 1e-12);
    ASSERT_NEAR(nor.y, 0.0, 1e-12);
    ASSERT_NEAR(nor.z, 6.0, 1e-12);

    free(bas);
    free(bas_name);
    lattice_free(lat);
    cleed_test_remove_file(path);
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

    ASSERT_TRUE(cleed_test_write_text_file(path, contents) == 0);

    lattice_t *lat = create_minimal_lattice();
    ASSERT_TRUE(lat != NULL);
    ASSERT_TRUE(lattice_set_input_filename(lat, path) == 0);
    lat->max_layers = 0;

    coord_t a1 = {.x = 0., .y = 0., .z = 0.};
    coord_t a2 = {.x = 0., .y = 0., .z = 0.};
    coord_t a3 = {.x = 0., .y = 0., .z = 0.};
    coord_t nor = {.x = 0., .y = 0., .z = 0.};

    coord_t *bas = (coord_t *)calloc(1, sizeof(coord_t));
    char *bas_name = (char *)calloc(NAMSZ, sizeof(char));
    size_t n_bas = 0;
    ASSERT_TRUE(bas != NULL);
    ASSERT_TRUE(bas_name != NULL);

    const int rc = lattice_read(lat, &a1, &a2, &a3, &nor, &bas, &bas_name, &n_bas);
    ASSERT_TRUE(rc == 0);
    ASSERT_TRUE(lat->max_layers == 1);

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

