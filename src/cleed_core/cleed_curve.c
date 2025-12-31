/*********************************************************************
 *                       CLEED_CURVE.C
 *
 *  In-memory curve helpers for CLEED.
 *********************************************************************/

// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>
// cppcheck-suppress missingIncludeSystem
#include <string.h>

#include "cleed_core.h"

int cleed_curve_alloc(cleed_curve *curve, size_t n)
{
  if (!curve || n == 0) {
    return -1;
  }

  curve->E = (double *)calloc(n, sizeof(double));
  curve->I = (double *)calloc(n, sizeof(double));
  if (!curve->E || !curve->I) {
    free(curve->E);
    free(curve->I);
    curve->E = NULL;
    curve->I = NULL;
    curve->n = 0;
    return -1;
  }

  curve->n = n;
  return 0;
}

void cleed_curve_free(cleed_curve *curve)
{
  if (!curve) return;
  free(curve->E);
  free(curve->I);
  curve->E = NULL;
  curve->I = NULL;
  curve->n = 0;
}

int cleed_curve_copy(cleed_curve *dst, const cleed_curve *src)
{
  if (!dst || !src || src->n == 0 || !src->E || !src->I) {
    return -1;
  }
  if (dst == src) {
    return 0;
  }

  cleed_curve_free(dst);
  if (cleed_curve_alloc(dst, src->n) != 0) {
    return -1;
  }

  (void)memcpy(dst->E, src->E, src->n * sizeof(double));
  (void)memcpy(dst->I, src->I, src->n * sizeof(double));
  return 0;
}

static size_t cleed_curve_upper_index(const cleed_curve *curve, double energy)
{
  size_t lo = 0;
  size_t hi = curve->n - 1;

  while (hi - lo > 1) {
    size_t mid = lo + (hi - lo) / 2;
    if (energy <= curve->E[mid]) {
      hi = mid;
    } else {
      lo = mid;
    }
  }

  return hi;
}

double cleed_curve_interp(const cleed_curve *curve, double energy)
{
  if (!curve || curve->n == 0 || !curve->E || !curve->I) {
    return NAN;
  }

  if (curve->n == 1) {
    return curve->I[0];
  }

  if (energy <= curve->E[0]) {
    return curve->I[0];
  }
  if (energy >= curve->E[curve->n - 1]) {
    return curve->I[curve->n - 1];
  }

  size_t hi = cleed_curve_upper_index(curve, energy);
  size_t lo = hi - 1;
  double e0 = curve->E[lo];
  double e1 = curve->E[hi];
  if (e1 <= e0) {
    return curve->I[lo];
  }

  double t = (energy - e0) / (e1 - e0);
  return curve->I[lo] + t * (curve->I[hi] - curve->I[lo]);
}
