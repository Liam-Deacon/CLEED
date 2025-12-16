#include "search.h"

// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdio.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>
// cppcheck-suppress missingIncludeSystem
#include <string.h>

#include "test_support.h"

static real quadratic_2d(real *x)
{
    const real dx = x[1] - 1.0;
    const real dy = x[2] + 2.0;
    return (dx * dx) + (dy * dy);
}

static int find_min_index(const real *values, int n)
{
    int min_index = 1;
    for (int i = 2; i <= n; i++) {
        if (values[i] < values[min_index]) {
            min_index = i;
        }
    }
    return min_index;
}

int main(void)
{
    const int ndim = 2;
    const int mpts = ndim + 1;
    int nfunk = 0;

    sr_project = (char *)malloc((size_t)STRSZ + 1);
    if (!sr_project) {
        fprintf(stderr, "allocation failed: sr_project\n");
        return 1;
    }
    snprintf(sr_project, (size_t)STRSZ + 1, "%s", "amoeba_test");

    if (cleed_test_write_text_file("amoeba_test.ver", "seed\n") != 0) {
        fprintf(stderr, "failed to create amoeba_test.ver\n");
        return 1;
    }

    real **p = cleed_test_alloc_matrix_1based(mpts, ndim);
    real *y = cleed_test_alloc_vector_1based(mpts);
    if (!p || !y) {
        fprintf(stderr, "allocation failed: simplex\n");
        return 1;
    }

    p[1][1] = 0.0;
    p[1][2] = 0.0;
    p[2][1] = 1.0;
    p[2][2] = 0.0;
    p[3][1] = 0.0;
    p[3][2] = 1.0;

    for (int i = 1; i <= mpts; i++) {
        y[i] = quadratic_2d(p[i]);
    }

    sr_amoeba(p, y, ndim, 1e-6, (real (*)())quadratic_2d, &nfunk);

    const int best = find_min_index(y, mpts);
    const real best_x = p[best][1];
    const real best_y = p[best][2];

    const real tol = 1e-3;
    if (fabs(best_x - 1.0) > tol || fabs(best_y + 2.0) > tol) {
        fprintf(stderr, "unexpected optimum: (%g, %g)\n", (double)best_x, (double)best_y);
        return 1;
    }

    if (nfunk <= 0 || nfunk > 500) {
        fprintf(stderr, "unexpected function evaluation count: %d\n", nfunk);
        return 1;
    }

    cleed_test_free_matrix_1based(p);
    cleed_test_free_vector_1based(y);

    cleed_test_remove_file("amoeba_test.ver");
    cleed_test_remove_file("amoeba_test.vbk");

    free(sr_project);
    sr_project = NULL;

    return 0;
}
