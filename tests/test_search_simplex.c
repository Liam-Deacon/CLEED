#include "test_support.h"

#include "sr_simplex.h"

static int g_ndim = 0;

static real test_obj(real *x)
{
  real sum = 0.0;
  for (int j = 1; j <= g_ndim; j++) {
    sum += x[j] * x[j];
  }
  return sum;
}

static int test_build_initial_simplex(void)
{
  const int ndim = 3;
  const real dpos = (real)0.25;
  g_ndim = ndim;

  sr_simplex_buffers b;
  CLEED_TEST_ASSERT(sr_simplex_buffers_alloc(&b, ndim) == 0);
  CLEED_TEST_ASSERT(b.ndim == ndim);
  CLEED_TEST_ASSERT(b.mpar == ndim + 1);

  CLEED_TEST_ASSERT(sr_simplex_build_initial(&b, dpos, test_obj) == 0);

  for (int j = 1; j <= ndim; j++) {
    CLEED_TEST_ASSERT_NEAR(b.p[1][j], 0.0, 1e-12);
  }
  CLEED_TEST_ASSERT_NEAR(b.y[1], 0.0, 1e-12);

  for (int i = 2; i <= b.mpar; i++) {
    const int displaced_dim = i - 1;
    for (int j = 1; j <= ndim; j++) {
      const real expected = (j == displaced_dim) ? dpos : (real)0.0;
      CLEED_TEST_ASSERT_NEAR(b.p[i][j], expected, 1e-12);
    }
    CLEED_TEST_ASSERT_NEAR(b.y[i], dpos * dpos, 1e-12);
  }

  sr_simplex_buffers_free(&b);
  return 0;
}

static int test_extremes_and_centroid(void)
{
  const int ndim = 2;
  g_ndim = ndim;

  sr_simplex_buffers b;
  CLEED_TEST_ASSERT(sr_simplex_buffers_alloc(&b, ndim) == 0);
  CLEED_TEST_ASSERT(sr_simplex_build_initial(&b, (real)1.0, test_obj) == 0);

  /* Override y for deterministic extremes selection. */
  b.y[1] = (real)2.0;
  b.y[2] = (real)5.0;
  b.y[3] = (real)1.0;

  int ilo = 0, ihi = 0, inhi = 0;
  sr_simplex_extremes(b.y, ndim, &ilo, &ihi, &inhi);
  CLEED_TEST_ASSERT(ilo == 3);
  CLEED_TEST_ASSERT(ihi == 2);
  CLEED_TEST_ASSERT(inhi == 1);

  /* Centroid excluding ihi=2: average of vertices 1 and 3. */
  real centroid[3] = {0.0, 0.0, 0.0};
  sr_simplex_centroid_excluding((const real **)b.p, centroid, ndim, 2);
  CLEED_TEST_ASSERT_NEAR(centroid[1], (b.p[1][1] + b.p[3][1]) / (real)ndim, 1e-12);
  CLEED_TEST_ASSERT_NEAR(centroid[2], (b.p[1][2] + b.p[3][2]) / (real)ndim, 1e-12);

  sr_simplex_buffers_free(&b);
  return 0;
}

int main(void)
{
  if (test_build_initial_simplex() != 0) return 1;
  if (test_extremes_and_centroid() != 0) return 1;
  return 0;
}

