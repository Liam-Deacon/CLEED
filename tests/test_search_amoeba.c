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

static int setup_sr_project(void)
{
    sr_project = (char *)malloc((size_t)STRSZ + 1);
    if (!sr_project) {
        fprintf(stderr, "allocation failed: sr_project\n");
        return 1;
    }
    snprintf(sr_project, (size_t)STRSZ + 1, "%s", "amoeba_test");

    if (cleed_test_write_text_file("amoeba_test.ver", "seed\n") != 0) {
        fprintf(stderr, "failed to create amoeba_test.ver\n");
        free(sr_project);
        sr_project = NULL;
        return 1;
    }

    return 0;
}

static void teardown_sr_project(void)
{
    cleed_test_remove_file("amoeba_test.ver");
    cleed_test_remove_file("amoeba_test.vbk");
    free(sr_project);
    sr_project = NULL;
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

static int verify_amoeba_result(real **p, real *y, int nfunk)
{
    const int mpts = 3;
    const real tol = 1e-3;
    const int best = find_min_index(y, mpts);
    const real best_x = p[best][1];
    const real best_y = p[best][2];

    if (fabs(best_x - 1.0) > tol || fabs(best_y + 2.0) > tol) {
        fprintf(stderr, "unexpected optimum: (%g, %g)\n", (double)best_x, (double)best_y);
        return 1;
    }

    if (nfunk <= 0 || nfunk > 500) {
        fprintf(stderr, "unexpected function evaluation count: %d\n", nfunk);
        return 1;
    }

    return 0;
}

static int run_amoeba_regression(void)
{
    const int ndim = 2;
    const int mpts = ndim + 1;
    int nfunk = 0;

    real **p = cleed_test_alloc_matrix_1based(mpts, ndim);
    real *y = cleed_test_alloc_vector_1based(mpts);
    if (!p || !y) {
        fprintf(stderr, "allocation failed: simplex\n");
        if (p) {
            cleed_test_free_matrix_1based(p);
        }
        if (y) {
            cleed_test_free_vector_1based(y);
        }
        return 1;
    }

    configure_simplex(p, y);
    const int rc = sr_amoeba(p, y, ndim, 1e-6, quadratic_2d, &nfunk);
    if (rc != 0) {
        fprintf(stderr, "sr_amoeba failed: %d\n", rc);
        cleed_test_free_matrix_1based(p);
        cleed_test_free_vector_1based(y);
        return 1;
    }

    const int result = verify_amoeba_result(p, y, nfunk);
    cleed_test_free_matrix_1based(p);
    cleed_test_free_vector_1based(y);

    return result;
}

static int verify_vertex_files_written(void)
{
    FILE *vbk = fopen("amoeba_test.vbk", "r");
    if (!vbk) {
        fprintf(stderr, "expected amoeba_test.vbk to exist\n");
        return 1;
    }
    fclose(vbk);

    FILE *ver = fopen("amoeba_test.ver", "r");
    if (!ver) {
        fprintf(stderr, "expected amoeba_test.ver to exist\n");
        return 1;
    }
    char header[128];
    if (!fgets(header, (int)sizeof(header), ver)) {
        fclose(ver);
        fprintf(stderr, "failed to read amoeba_test.ver header\n");
        return 1;
    }

    int ndim = 0;
    int mpts = 0;
    char project[64] = {0};
    if (sscanf(header, "%d %d %63s", &ndim, &mpts, project) != 3) {
        fclose(ver);
        fprintf(stderr, "unexpected amoeba_test.ver header: %s\n", header);
        return 1;
    }
    if (ndim != 2 || mpts != 3 || strcmp(project, "amoeba_test") != 0) {
        fclose(ver);
        fprintf(stderr, "unexpected amoeba_test.ver header: %s\n", header);
        return 1;
    }

    char line[256];
    if (!fgets(line, (int)sizeof(line), ver)) {
        fclose(ver);
        fprintf(stderr, "failed to read amoeba_test.ver vertex line\n");
        return 1;
    }
    fclose(ver);

    double fy = 0.0, x = 0.0, y = 0.0;
    if (sscanf(line, "%lf %lf %lf", &fy, &x, &y) != 3) {
        fprintf(stderr, "unexpected amoeba_test.ver vertex line: %s\n", line);
        return 1;
    }

    return 0;
}

int main(void)
{
    if (setup_sr_project() != 0) {
        return 1;
    }

    /* Remove any stale outputs before running. */
    (void)cleed_test_remove_file("amoeba_test.vbk");
    (void)cleed_test_remove_file("amoeba_test.ver");
    if (cleed_test_write_text_file("amoeba_test.ver", "seed\n") != 0) {
        fprintf(stderr, "failed to create amoeba_test.ver\n");
        teardown_sr_project();
        return 1;
    }

    const int status = run_amoeba_regression();
    if (status == 0 && verify_vertex_files_written() != 0) {
        teardown_sr_project();
        return 1;
    }
    teardown_sr_project();

    return status;
}
