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

static int run_amebsa_regression(void)
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

    sa_idum = -12345;
    real yb = 1e30;
    int budget = 500;

    const int rc = sr_amebsa(p, y, ndim, pb, &yb, 1e-6, quadratic_2d, &budget, 1.0);
    if (rc != 0) {
        fprintf(stderr, "sr_amebsa failed: %d\n", rc);
        cleed_test_free_matrix_1based(p);
        cleed_test_free_vector_1based(y);
        cleed_test_free_vector_1based(pb);
        return 1;
    }

    CLEED_TEST_ASSERT(budget > 0);
    CLEED_TEST_ASSERT(budget <= 500);

    /* Should improve from the initial best value (which is 4.0 at (1,0)). */
    CLEED_TEST_ASSERT(yb < 1.0);

    CLEED_TEST_ASSERT_NEAR(pb[1], 1.0, 2e-1);
    CLEED_TEST_ASSERT_NEAR(pb[2], -2.0, 2e-1);

    cleed_test_free_matrix_1based(p);
    cleed_test_free_vector_1based(y);
    cleed_test_free_vector_1based(pb);

    return 0;
}

int main(void)
{
    return run_amebsa_regression();
}

