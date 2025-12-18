#include "search.h"

// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdio.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>

#include "test_support.h"

static real quadratic_2d(real *x)
{
    const real dx = x[1] - 1.0;
    const real dy = x[2] + 2.0;
    return (dx * dx) + (dy * dy);
}

static int run_powell_regression(void)
{
    const int ndim = 2;

    real *p = cleed_test_alloc_vector_1based(ndim);
    real **xi = cleed_test_alloc_matrix_1based(ndim, ndim);
    if (!p || !xi) {
        fprintf(stderr, "allocation failed\n");
        cleed_test_free_vector_1based(p);
        cleed_test_free_matrix_1based(xi);
        return 1;
    }

    p[1] = 0.0;
    p[2] = 0.0;

    for (int i = 1; i <= ndim; i++) {
        for (int j = 1; j <= ndim; j++) {
            xi[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }

    int iter = 0;
    real fret = 0.0;
    const int rc = sr_powell(p, xi, ndim, 1e-6, &iter, &fret, quadratic_2d);
    if (rc != 0) {
        fprintf(stderr, "sr_powell failed: %d\n", rc);
        cleed_test_free_vector_1based(p);
        cleed_test_free_matrix_1based(xi);
        return 1;
    }

    CLEED_TEST_ASSERT_NEAR(p[1], 1.0, 1e-3);
    CLEED_TEST_ASSERT_NEAR(p[2], -2.0, 1e-3);
    CLEED_TEST_ASSERT_NEAR(fret, 0.0, 1e-6);
    CLEED_TEST_ASSERT(iter > 0);
    CLEED_TEST_ASSERT(iter <= 100);

    cleed_test_free_vector_1based(p);
    cleed_test_free_matrix_1based(xi);

    return 0;
}

int main(void)
{
    return run_powell_regression();
}

