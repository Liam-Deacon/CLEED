/********************************************************************
 *                       CRFSPLINE.C
 *
 *  Prepare natural cubic spline second derivatives for a list of
 *  (energy, intensity) points.
 ********************************************************************/

#include <stdlib.h>

#include "crfac.h"

void cr_spline(struct crelist *list, int leng)
{
  if (list == NULL) return;
  if (leng <= 1) {
    if (leng == 1) list[0].deriv2 = 0.0;
    return;
  }

  /* Natural spline boundary conditions. */
  list[0].deriv2 = 0.0;
  list[leng - 1].deriv2 = 0.0;

  if (leng <= 2) return;

  int n_unknown = leng - 2;
  real *c_prime = (real *)calloc((size_t)n_unknown, sizeof(real));
  real *d_prime = (real *)calloc((size_t)n_unknown, sizeof(real));
  if (c_prime == NULL || d_prime == NULL) {
    free(c_prime);
    free(d_prime);
    return;
  }

  /* Build and solve the tridiagonal system for M[1..leng-2]. */
  for (int k = 0; k < n_unknown; k++) {
    int i = k + 1; /* original point index */
    real h_im1 = list[i].energy - list[i - 1].energy;
    real h_i = list[i + 1].energy - list[i].energy;
    if (IS_EQUAL_REAL(h_im1, 0.0) || IS_EQUAL_REAL(h_i, 0.0)) {
      /* Degenerate spacing; fall back to zero curvature locally. */
      list[i].deriv2 = 0.0;
      continue;
    }

    real a = (i == 1) ? 0.0 : h_im1;
    real b = 2.0 * (h_im1 + h_i);
    real c = (i == leng - 2) ? 0.0 : h_i;
    real d = 6.0 * ((list[i + 1].intens - list[i].intens) / h_i -
                    (list[i].intens - list[i - 1].intens) / h_im1);

    if (k == 0) {
      c_prime[k] = c / b;
      d_prime[k] = d / b;
    } else {
      real denom = b - a * c_prime[k - 1];
      c_prime[k] = (IS_EQUAL_REAL(denom, 0.0)) ? 0.0 : (c / denom);
      d_prime[k] = (IS_EQUAL_REAL(denom, 0.0)) ? 0.0 : ((d - a * d_prime[k - 1]) / denom);
    }
  }

  /* Back substitution. */
  list[leng - 2].deriv2 = d_prime[n_unknown - 1];
  for (int k = n_unknown - 2; k >= 0; k--) {
    int i = k + 1;
    list[i].deriv2 = d_prime[k] - c_prime[k] * list[i + 1].deriv2;
  }

  free(c_prime);
  free(d_prime);
}
