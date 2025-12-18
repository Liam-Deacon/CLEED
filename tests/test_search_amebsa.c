#include "search.h"

// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdio.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>

#include "test_support.h"

extern long sa_idum;

static real quadratic_2d(real *x)
{
    const real dx = x[1] - 1.0;
    const real dy = x[2] + 2.0;
    return (dx * dx) + (dy * dy);
}

static void configure_simplex(real **p, real *y)
{
    p[1][1] = 0.0;
    p[1][2] = 0.0;
    p[2][1] = 1.0;
    p[2][2] = 0.0;
    p[3][1] = 0.0;
    p[3][2] = 1.0;

    const int mpts = 3;
    for (int i = 1; i <= mpts; i++) {
        y[i] = quadratic_2d(p[i]);
    }
}

typedef struct amebsa_result {
    real yb;
    real pb1;
    real pb2;
    int used;
} amebsa_result;

static int run_amebsa_once(real temptr, long seed, amebsa_result *out)
{
    const int ndim = 2;
    const int mpts = ndim + 1;

    real **p = cleed_test_alloc_matrix_1based(mpts, ndim);
    real *y = cleed_test_alloc_vector_1based(mpts);
    real *pb = cleed_test_alloc_vector_1based(ndim);
    if (!p || !y || !pb) {
        fprintf(stderr, "allocation failed\n");
        cleed_test_free_matrix_1based(p);
        cleed_test_free_vector_1based(y);
        cleed_test_free_vector_1based(pb);
        return 1;
    }

    configure_simplex(p, y);

    sa_idum = seed;
    real yb = 1e30;
    int budget = 500;

    sr_amebsa_cfg cfg;
    cfg.ftol = 1e-6;
    cfg.funk = quadratic_2d;
    cfg.temptr = temptr;

    const int rc = sr_amebsa(p, y, ndim, pb, &yb, &cfg, &budget);
    if (rc != 0) {
        fprintf(stderr, "sr_amebsa failed: %d\n", rc);
        cleed_test_free_matrix_1based(p);
        cleed_test_free_vector_1based(y);
        cleed_test_free_vector_1based(pb);
        return 1;
    }

    CLEED_TEST_ASSERT(budget >= 0);
    CLEED_TEST_ASSERT(budget <= 500);

    /* Should improve from the initial best value (which is 4.0 at (1,0)). */
    CLEED_TEST_ASSERT(yb < 1.0);

    CLEED_TEST_ASSERT_NEAR(pb[1], 1.0, 2e-1);
    CLEED_TEST_ASSERT_NEAR(pb[2], -2.0, 2e-1);

    if (out) {
        out->yb = yb;
        out->pb1 = pb[1];
        out->pb2 = pb[2];
        out->used = budget;
    }

    cleed_test_free_matrix_1based(p);
    cleed_test_free_vector_1based(y);
    cleed_test_free_vector_1based(pb);

    return 0;
}

#ifdef REAL_IS_FLOAT
#define AMEBSA_DETERMINISM_TOL ((real)5e-4)
#else
#define AMEBSA_DETERMINISM_TOL ((real)1e-10)
#endif

static int test_reproducible_with_fixed_seed(void)
{
    amebsa_result r1, r2;
    if (run_amebsa_once((real)1.0, -12345, &r1) != 0) return 1;
    if (run_amebsa_once((real)1.0, -12345, &r2) != 0) return 1;

    CLEED_TEST_ASSERT_NEAR(r1.yb, r2.yb, AMEBSA_DETERMINISM_TOL);
    CLEED_TEST_ASSERT_NEAR(r1.pb1, r2.pb1, AMEBSA_DETERMINISM_TOL);
    CLEED_TEST_ASSERT_NEAR(r1.pb2, r2.pb2, AMEBSA_DETERMINISM_TOL);
    CLEED_TEST_ASSERT(r1.used == r2.used);

    return 0;
}

static int test_zero_temperature_seed_independent(void)
{
    amebsa_result r1, r2;
    if (run_amebsa_once((real)0.0, -1, &r1) != 0) return 1;
    if (run_amebsa_once((real)0.0, -999, &r2) != 0) return 1;

    CLEED_TEST_ASSERT_NEAR(r1.yb, r2.yb, AMEBSA_DETERMINISM_TOL);
    CLEED_TEST_ASSERT_NEAR(r1.pb1, r2.pb1, AMEBSA_DETERMINISM_TOL);
    CLEED_TEST_ASSERT_NEAR(r1.pb2, r2.pb2, AMEBSA_DETERMINISM_TOL);
    CLEED_TEST_ASSERT(r1.used == r2.used);

    return 0;
}

int main(void)
{
    if (test_reproducible_with_fixed_seed() != 0) return 1;
    if (test_zero_temperature_seed_independent() != 0) return 1;
    return 0;
}
