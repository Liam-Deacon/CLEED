/*********************************************************************
 *                       SR_SIMPLEX.C
 *
 *  GPL-3.0-or-later
 *********************************************************************/

/**
 * @file sr_simplex.c
 * @brief Implementation of shared simplex helpers for SEARCH.
 */

#include "sr_simplex.h"

// cppcheck-suppress missingIncludeSystem
#include <string.h>

#include "sr_alloc.h"

static void sr_simplex_zero(real *v, int n)
{
  for (int j = 1; j <= n; j++) v[j] = 0.0;
}

static void sr_simplex_add(real *acc, const real *x, int n)
{
  for (int j = 1; j <= n; j++) acc[j] += x[j];
}

static void sr_simplex_scale(real *v, int n, real s)
{
  for (int j = 1; j <= n; j++) v[j] *= s;
}

void sr_simplex_copy_point(real *dst, const real *src, int ndim)
{
  if (dst == NULL || src == NULL || ndim <= 0) return;
  for (int j = 1; j <= ndim; j++) dst[j] = src[j];
}

void sr_simplex_centroid_excluding(const real **p, real *centroid, int ndim, int exclude_index)
{
  if (p == NULL || centroid == NULL || ndim <= 0) return;

  int mpts = ndim + 1;
  sr_simplex_zero(centroid, ndim);

  for (int i = 1; i <= mpts; i++) {
    if (i == exclude_index) continue;
    sr_simplex_add(centroid, p[i], ndim);
  }

  sr_simplex_scale(centroid, ndim, (real)1.0 / (real)ndim);
}

static void sr_simplex_extremes_init(const real *y, int *ilo, int *ihi, int *inhi)
{
  *ilo = 1;
  *ihi = (y[1] > y[2]) ? 1 : 2;
  *inhi = (y[1] > y[2]) ? 2 : 1;
}

static void sr_simplex_extremes_update(const real *y, int i, int *ilo, int *ihi, int *inhi)
{
  if (y[i] < y[*ilo]) *ilo = i;
  if (y[i] > y[*ihi]) {
    *inhi = *ihi;
    *ihi = i;
    return;
  }
  if (i != *ihi && y[i] > y[*inhi]) {
    *inhi = i;
  }
}

void sr_simplex_extremes(const real *y, int ndim, int *ilo, int *ihi, int *inhi)
{
  if (y == NULL || ilo == NULL || ihi == NULL || inhi == NULL || ndim <= 0) return;

  int mpts = ndim + 1;

  sr_simplex_extremes_init(y, ilo, ihi, inhi);

  for (int i = 1; i <= mpts; i++) {
    sr_simplex_extremes_update(y, i, ilo, ihi, inhi);
  }
}

int sr_simplex_buffers_alloc(sr_simplex_buffers *b, int ndim)
{
  if (b == NULL || ndim <= 0) return -1;

  (void)memset(b, 0, sizeof(*b));
  b->ndim = ndim;
  b->mpar = ndim + 1;

  b->x = sr_alloc_vector((size_t)ndim);
  b->y = sr_alloc_vector((size_t)b->mpar);
  b->p = sr_alloc_matrix((size_t)b->mpar, (size_t)ndim);
  if (b->x == NULL || b->y == NULL || b->p == NULL) {
    sr_simplex_buffers_free(b);
    return -1;
  }

  return 0;
}

void sr_simplex_buffers_free(sr_simplex_buffers *b)
{
  if (b == NULL) return;
  sr_free_matrix(b->p);
  sr_free_vector(b->y);
  sr_free_vector(b->x);
  b->p = NULL;
  b->y = NULL;
  b->x = NULL;
  b->ndim = 0;
  b->mpar = 0;
}

static void sr_simplex_build_vertex(sr_simplex_buffers *b, int i, real dpos)
{
  for (int j = 1; j <= b->ndim; j++) {
    if (i == (j + 1)) {
      b->x[j] = b->p[i][j] = b->p[1][j] + dpos;
    } else {
      b->x[j] = b->p[i][j] = b->p[1][j];
    }
  }
}

int sr_simplex_build_initial(sr_simplex_buffers *b, real dpos, real (*func)(real *))
{
  if (b == NULL || b->p == NULL || b->y == NULL || b->x == NULL || b->ndim <= 0 || func == NULL) return -1;

  sr_simplex_zero(b->p[1], b->ndim);

  for (int i = 1; i <= b->mpar; i++) {
    sr_simplex_build_vertex(b, i, dpos);
    b->y[i] = (*func)(b->x);
  }

  return 0;
}

int sr_simplex_read_vertex(sr_simplex_buffers *b, const char *bak_file)
{
  if (b == NULL || b->p == NULL || b->y == NULL || bak_file == NULL) return -1;
  return (sr_rdver(bak_file, b->y, b->p, b->ndim) == 1) ? 0 : -1;
}
