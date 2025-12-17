/*********************************************************************
 *                       SR_AMEBSA.C
 *
 *  GPL-3.0-or-later
 *
 *  Lightweight simulated annealing wrapper around the simplex method.
 *
 *  Legacy SEARCH conventions:
 *  - simplex vertices are stored in p[1..ndim+1][1..ndim]
 *  - function values are stored in y[1..ndim+1]
 *  - best point is returned in pb[1..ndim] and *yb
 *********************************************************************/

#include <math.h>
#include <stdlib.h>

#include "search.h"
#include "sr_rng.h"

/* The SEARCH driver keeps a global seed. */
extern long sa_idum;

static void sr_copy_point(real *dst, const real *src, int ndim)
{
  for (int j = 1; j <= ndim; j++) dst[j] = src[j];
}

static void sr_simplex_extremes(const real *y, int ndim, int *ilo, int *ihi)
{
  int mpts = ndim + 1;
  *ilo = 1;
  *ihi = 1;
  for (int i = 2; i <= mpts; i++) {
    if (y[i] < y[*ilo]) *ilo = i;
    if (y[i] > y[*ihi]) *ihi = i;
  }
}

static void sr_centroid_excluding(const real **p, real *centroid, int ndim, int exclude_index)
{
  int mpts = ndim + 1;
  for (int j = 1; j <= ndim; j++) centroid[j] = 0.0;
  for (int i = 1; i <= mpts; i++) {
    if (i == exclude_index) continue;
    for (int j = 1; j <= ndim; j++) centroid[j] += p[i][j];
  }
  for (int j = 1; j <= ndim; j++) centroid[j] /= (real)ndim;
}

static int sr_accept_move(sr_rng *rng, real current, real proposed, real temp)
{
  if (proposed <= current) return 1;
  if (temp <= (real)0.0) return 0;
  double u = sr_rng_uniform01(rng);
  double prob = exp(-(double)(proposed - current) / (double)temp);
  return u < prob;
}

int sr_amebsa(real **p, real *y, int ndim, real *pb, real *yb,
              real ftol, real (*funk)(real *), int *iter, real temptr)
{
  if (p == NULL || y == NULL || pb == NULL || yb == NULL || funk == NULL || iter == NULL) {
    return -1;
  }
  if (ndim <= 0) return -1;

  int mpts = ndim + 1;

  /* Seed deterministic RNG from the legacy global seed. */
  sr_rng rng;
  uint64_t seed = (sa_idum < 0) ? (uint64_t)(-sa_idum) : (uint64_t)sa_idum;
  if (seed == 0) seed = 1;
  sr_rng_seed(&rng, seed);

  int ilo = 1, ihi = 1;
  sr_simplex_extremes(y, ndim, &ilo, &ihi);
  sr_copy_point(pb, p[ilo], ndim);
  *yb = y[ilo];

  real *centroid = (real *)calloc((size_t)ndim + 1, sizeof(real));
  real *trial = (real *)calloc((size_t)ndim + 1, sizeof(real));
  if (centroid == NULL || trial == NULL) {
    free(centroid);
    free(trial);
    return -1;
  }

  int budget = *iter;
  if (budget < 0) budget = 0;
  int used = 0;

  for (int k = 0; k < budget; k++) {
    sr_simplex_extremes(y, ndim, &ilo, &ihi);

    if (fabs((double)(y[ihi] - y[ilo])) < (double)ftol) {
      break;
    }

    sr_centroid_excluding((const real **)p, centroid, ndim, ihi);

    /* Reflection proposal with optional random jitter proportional to temperature. */
    for (int j = 1; j <= ndim; j++) {
      double jitter = (sr_rng_uniform01(&rng) - 0.5) * 2.0;
      trial[j] = centroid[j] + (real)1.0 * (centroid[j] - p[ihi][j]) + (real)(temptr * (real)0.05) * (real)jitter;
    }

    real fr = (*funk)(trial);
    used++;

    if (sr_accept_move(&rng, y[ihi], fr, temptr)) {
      sr_copy_point(p[ihi], trial, ndim);
      y[ihi] = fr;
      if (fr < *yb) {
        sr_copy_point(pb, trial, ndim);
        *yb = fr;
      }
    }
  }

  *iter = used;

  free(centroid);
  free(trial);

  /* Update legacy seed to keep subsequent calls deterministic but non-constant. */
  sa_idum = (long)(seed + (uint64_t)used + 1);

  /* Ensure pb/yb reflect current best. */
  sr_simplex_extremes(y, ndim, &ilo, &ihi);
  if (y[ilo] < *yb) {
    sr_copy_point(pb, p[ilo], ndim);
    *yb = y[ilo];
  }

  (void)mpts;
  return 0;
}
