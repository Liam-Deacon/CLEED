/*********************************************************************
 *                     SR_VERTEX_STATS.C
 *
 *  GPL-3.0-or-later
 *********************************************************************/

/**
 * @file sr_vertex_stats.c
 * @brief Statistics helpers for SEARCH simplex vertex arrays.
 *
 * These utilities operate on legacy 1-based vectors/matrices and are used by
 * refactored tooling (e.g. `verstat`) to keep the I/O layer small and the
 * statistics logic testable.
 */

// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stddef.h>

#include "sr_vertex_stats.h"

static void sr_vertex_minmax_y(const real *y, int mpar, real *out_min_y, real *out_max_y)
{
  real min_y = y[1];
  real max_y = y[1];
  for (int i = 2; i <= mpar; i++) {
    if (y[i] < min_y) min_y = y[i];
    if (y[i] > max_y) max_y = y[i];
  }
  if (out_min_y) *out_min_y = min_y;
  if (out_max_y) *out_max_y = max_y;
}

static real sr_vertex_min_dim(real **p, int mpar, int j)
{
  real v = p[1][j];
  for (int i = 2; i <= mpar; i++) {
    if (p[i][j] < v) v = p[i][j];
  }
  return v;
}

static real sr_vertex_max_dim(real **p, int mpar, int j)
{
  real v = p[1][j];
  for (int i = 2; i <= mpar; i++) {
    if (p[i][j] > v) v = p[i][j];
  }
  return v;
}

static void sr_vertex_min_p(real **p, int ndim, int mpar, real *out_min_p)
{
  for (int j = 1; j <= ndim; j++) {
    out_min_p[j] = sr_vertex_min_dim(p, mpar, j);
  }
}

static void sr_vertex_max_p(real **p, int ndim, int mpar, real *out_max_p)
{
  for (int j = 1; j <= ndim; j++) {
    out_max_p[j] = sr_vertex_max_dim(p, mpar, j);
  }
}

void sr_vertex_minmax(const real *y, real **p, int ndim, int mpar,
                      real *out_min_y, real *out_max_y,
                      real *out_min_p, real *out_max_p)
{
  if (y == NULL || p == NULL || ndim <= 0 || mpar <= 0) return;

  sr_vertex_minmax_y(y, mpar, out_min_y, out_max_y);
  if (out_min_p) sr_vertex_min_p(p, ndim, mpar, out_min_p);
  if (out_max_p) sr_vertex_max_p(p, ndim, mpar, out_max_p);
}

static real sr_vertex_avg_y(const real *y, int mpar)
{
  double sum = 0.0;
  for (int i = 1; i <= mpar; i++) sum += (double)y[i];
  return (real)(sum / (double)mpar);
}

static void sr_vertex_avg_p(real **p, int ndim, int mpar, real *out_avg_p)
{
  for (int j = 1; j <= ndim; j++) {
    double sum = 0.0;
    for (int i = 1; i <= mpar; i++) sum += (double)p[i][j];
    out_avg_p[j] = (real)(sum / (double)mpar);
  }
}

void sr_vertex_avg(const real *y, real **p, int ndim, int mpar,
                   real *out_avg_y, real *out_avg_p)
{
  if (y == NULL || p == NULL || ndim <= 0 || mpar <= 0) return;

  real avg_y = sr_vertex_avg_y(y, mpar);
  if (out_avg_y) *out_avg_y = avg_y;
  if (out_avg_p) sr_vertex_avg_p(p, ndim, mpar, out_avg_p);
}

static real sr_vertex_dev_y(const real *y, int mpar, real avg_y)
{
  double var = 0.0;
  for (int i = 1; i <= mpar; i++) {
    double d = (double)y[i] - (double)avg_y;
    var += d * d;
  }
  return (real)sqrt(var / (double)mpar);
}

static void sr_vertex_dev_p(real **p, int ndim, int mpar, const real *avg_p, real *out_dev_p)
{
  for (int j = 1; j <= ndim; j++) {
    double var = 0.0;
    for (int i = 1; i <= mpar; i++) {
      double d = (double)p[i][j] - (double)avg_p[j];
      var += d * d;
    }
    out_dev_p[j] = (real)sqrt(var / (double)mpar);
  }
}

void sr_vertex_dev(const real *y, real **p, int ndim, int mpar,
                   real avg_y, const real *avg_p,
                   real *out_dev_y, real *out_dev_p)
{
  if (y == NULL || p == NULL || avg_p == NULL || ndim <= 0 || mpar <= 0) return;

  if (out_dev_y) *out_dev_y = sr_vertex_dev_y(y, mpar, avg_y);
  if (out_dev_p) sr_vertex_dev_p(p, ndim, mpar, avg_p, out_dev_p);
}
