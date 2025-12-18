// cppcheck-suppress missingIncludeSystem
#include <math.h>

#include "sr_vertex_stats.h"
#include "test_support.h"

static int test_vertex_stats(void)
{
    const int ndim = 2;
    const int mpar = 3;

    real *y = cleed_test_alloc_vector_1based(mpar);
    real **p = cleed_test_alloc_matrix_1based(mpar, ndim);
    real *min_p = cleed_test_alloc_vector_1based(ndim);
    real *max_p = cleed_test_alloc_vector_1based(ndim);
    real *avg_p = cleed_test_alloc_vector_1based(ndim);
    real *dev_p = cleed_test_alloc_vector_1based(ndim);

    CLEED_TEST_ASSERT(y != NULL);
    CLEED_TEST_ASSERT(p != NULL);
    CLEED_TEST_ASSERT(min_p != NULL);
    CLEED_TEST_ASSERT(max_p != NULL);
    CLEED_TEST_ASSERT(avg_p != NULL);
    CLEED_TEST_ASSERT(dev_p != NULL);

    p[1][1] = 0.0;
    p[1][2] = 0.0;
    y[1] = 1.0;

    p[2][1] = 2.0;
    p[2][2] = 4.0;
    y[2] = 3.0;

    p[3][1] = 1.0;
    p[3][2] = -1.0;
    y[3] = 2.0;

    real min_y = 0.0, max_y = 0.0, avg_y = 0.0, dev_y = 0.0;
    sr_vertex_minmax(y, p, ndim, mpar, &min_y, &max_y, min_p, max_p);
    sr_vertex_avg(y, p, ndim, mpar, &avg_y, avg_p);
    sr_vertex_dev(y, p, ndim, mpar, avg_y, avg_p, &dev_y, dev_p);

    CLEED_TEST_ASSERT_NEAR(min_y, 1.0, 1e-12);
    CLEED_TEST_ASSERT_NEAR(max_y, 3.0, 1e-12);

    CLEED_TEST_ASSERT_NEAR(min_p[1], 0.0, 1e-12);
    CLEED_TEST_ASSERT_NEAR(min_p[2], -1.0, 1e-12);
    CLEED_TEST_ASSERT_NEAR(max_p[1], 2.0, 1e-12);
    CLEED_TEST_ASSERT_NEAR(max_p[2], 4.0, 1e-12);

    CLEED_TEST_ASSERT_NEAR(avg_y, 2.0, 1e-12);
    CLEED_TEST_ASSERT_NEAR(avg_p[1], 1.0, 1e-12);
    CLEED_TEST_ASSERT_NEAR(avg_p[2], 1.0, 1e-12);

    CLEED_TEST_ASSERT_NEAR(dev_y, sqrt(2.0 / 3.0), 1e-6);
    CLEED_TEST_ASSERT_NEAR(dev_p[1], sqrt(2.0 / 3.0), 1e-6);
    CLEED_TEST_ASSERT_NEAR(dev_p[2], sqrt(14.0 / 3.0), 1e-6);

    cleed_test_free_vector_1based(y);
    cleed_test_free_matrix_1based(p);
    cleed_test_free_vector_1based(min_p);
    cleed_test_free_vector_1based(max_p);
    cleed_test_free_vector_1based(avg_p);
    cleed_test_free_vector_1based(dev_p);

    return 0;
}

int main(void)
{
    return test_vertex_stats();
}

