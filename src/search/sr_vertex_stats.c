/*********************************************************************
 *                     SR_VERTEX_STATS.C
 *
 *  GPL-3.0-or-later
 *********************************************************************/

// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stddef.h>

#include "sr_vertex_stats.h"

void sr_vertex_minmax(const real *y, real **p, int ndim, int mpar,
                      real *out_min_y, real *out_max_y,
                      real *out_min_p, real *out_max_p)
{
  if (y == NULL || p == NULL || ndim <= 0 || mpar <= 0) return;

  real min_y = y[1];
  real max_y = y[1];

  for (int i = 2; i <= mpar; i++) {
    if (y[i] < min_y) min_y = y[i];
    if (y[i] > max_y) max_y = y[i];
  }

  if (out_min_y) *out_min_y = min_y;
  if (out_max_y) *out_max_y = max_y;

  if (out_min_p) {
    for (int j = 1; j <= ndim; j++) {
      real v = p[1][j];
      for (int i = 2; i <= mpar; i++) {
        if (p[i][j] < v) v = p[i][j];
      }
      out_min_p[j] = v;
    }
  }

  if (out_max_p) {
    for (int j = 1; j <= ndim; j++) {
      real v = p[1][j];
      for (int i = 2; i <= mpar; i++) {
        if (p[i][j] > v) v = p[i][j];
      }
      out_max_p[j] = v;
    }
  }
}

void sr_vertex_avg(const real *y, real **p, int ndim, int mpar,
                   real *out_avg_y, real *out_avg_p)
{
  if (y == NULL || p == NULL || ndim <= 0 || mpar <= 0) return;

  double sum_y = 0.0;
  for (int i = 1; i <= mpar; i++) sum_y += (double)y[i];
  real avg_y = (real)(sum_y / (double)mpar);
  if (out_avg_y) *out_avg_y = avg_y;

  if (out_avg_p) {
    for (int j = 1; j <= ndim; j++) {
      double sum = 0.0;
      for (int i = 1; i <= mpar; i++) sum += (double)p[i][j];
      out_avg_p[j] = (real)(sum / (double)mpar);
    }
  }
}

void sr_vertex_dev(const real *y, real **p, int ndim, int mpar,
                   real avg_y, const real *avg_p,
                   real *out_dev_y, real *out_dev_p)
{
  if (y == NULL || p == NULL || avg_p == NULL || ndim <= 0 || mpar <= 0) return;

  double var_y = 0.0;
  for (int i = 1; i <= mpar; i++) {
    double d = (double)y[i] - (double)avg_y;
    var_y += d * d;
  }
  if (out_dev_y) *out_dev_y = (real)sqrt(var_y / (double)mpar);

  if (out_dev_p) {
    for (int j = 1; j <= ndim; j++) {
      double var = 0.0;
      for (int i = 1; i <= mpar; i++) {
        double d = (double)p[i][j] - (double)avg_p[j];
        var += d * d;
      }
      out_dev_p[j] = (real)sqrt(var / (double)mpar);
    }
  }
}
