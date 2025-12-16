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

static lattice_t *create_lattice_for_setup(int latt_type)
{
    lattice_t *lat = lattice_init(1);
    if (!lat) {
        return NULL;
    }
    lat->latt_type = latt_type;
    lat->a_latt = 2.0;
    lat->c_latt = 0.0;
    lat->vec_h = 0.0;
    lat->vec_k = 0.0;
    lat->vec_l = 1.0;
    return lat;
}

static int run_setup(lattice_t *lat, coord_t *a1, coord_t *a2, coord_t *a3,
                     coord_t *nor, coord_t **bas, char **bas_name, size_t *n_bas)
{
    *bas = (coord_t *)calloc(1, sizeof(coord_t));
    *bas_name = (char *)calloc(NAMSZ, sizeof(char));
    CLEED_TEST_ASSERT(*bas != NULL);
    CLEED_TEST_ASSERT(*bas_name != NULL);
    *n_bas = 0;
    return lattice_setup(lat, a1, a2, a3, nor, bas, bas_name, n_bas);
}

static void free_setup(lattice_t *lat, coord_t *bas, char *bas_name)
{
    free(bas);
    free(bas_name);
    lattice_free(lat);
}

static int test_lattice_setup_fcc_basic(void)
{
    lattice_t *lat = create_lattice_for_setup(LAT_FCC);
    CLEED_TEST_ASSERT(lat != NULL);

    coord_t a1 = {0}, a2 = {0}, a3 = {0}, nor = {0};
    coord_t *bas = NULL;
    char *bas_name = NULL;
    size_t n_bas = 0;
    CLEED_TEST_ASSERT(run_setup(lat, &a1, &a2, &a3, &nor, &bas, &bas_name, &n_bas) == 0);

    CLEED_TEST_ASSERT(n_bas == 1);
    CLEED_TEST_ASSERT_NEAR(lat->a_nn, lat->a_latt / sqrt(2.0), 1e-12);
    CLEED_TEST_ASSERT_NEAR(nor.x, 0.0, 1e-12);
    CLEED_TEST_ASSERT_NEAR(nor.y, 0.0, 1e-12);
    CLEED_TEST_ASSERT_NEAR(nor.z, 2.0, 1e-12);

    free_setup(lat, bas, bas_name);
    return 0;
}

static int test_lattice_setup_invalid_miller_indices(void)
{
    lattice_t *lat = create_lattice_for_setup(LAT_FCC);
    CLEED_TEST_ASSERT(lat != NULL);
    lat->vec_h = 0.0;
    lat->vec_k = 0.0;
    lat->vec_l = 0.0;

    coord_t a1 = {0}, a2 = {0}, a3 = {0}, nor = {0};
    coord_t *bas = NULL;
    char *bas_name = NULL;
    size_t n_bas = 0;
    const int rc = run_setup(lat, &a1, &a2, &a3, &nor, &bas, &bas_name, &n_bas);
    CLEED_TEST_ASSERT(rc != 0);

    free_setup(lat, bas, bas_name);
    return 0;
}

int main(void)
{
    inf_stream = stderr;
    ctr_stream = stderr;

    if (test_lattice_setup_fcc_basic() != 0) {
        return 1;
    }
    if (test_lattice_setup_invalid_miller_indices() != 0) {
        return 1;
    }

    return 0;
}

