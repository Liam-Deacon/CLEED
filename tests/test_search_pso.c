#include "search.h"

// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdio.h>

#include "test_support.h"

static real quadratic_2d(real *x)
{
    const real dx = x[1] - (real)1.0;
    const real dy = x[2] + (real)2.0;
    return (dx * dx) + (dy * dy);
}

static int run_pso_regression(void)
{
    const int ndim = 2;
    sr_pso_cfg cfg;
    sr_pso_cfg_init(&cfg, ndim, (real)1.0);
    cfg.swarm_size = 12;
    cfg.max_evals = 2000;
    cfg.seed = 42;

    real best[3] = {0};
    real best_val = 0.0;
    int evals = 0;

    if (sr_pso_optimize(&cfg, ndim, quadratic_2d, best, &best_val, &evals) != 0) {
        fprintf(stderr, "sr_pso_optimize failed\n");
        return 1;
    }

    const real tol = (real)0.2;
    if (fabs(best[1] - (real)1.0) > tol || fabs(best[2] + (real)2.0) > tol) {
        fprintf(stderr, "unexpected optimum: (%g, %g)\n", (double)best[1], (double)best[2]);
        return 1;
    }
    CLEED_TEST_ASSERT(best_val >= 0.0);
    CLEED_TEST_ASSERT(evals > 0);
    return 0;
}

int main(void)
{
    if (run_pso_regression() != 0) {
        return 1;
    }
    return 0;
}
