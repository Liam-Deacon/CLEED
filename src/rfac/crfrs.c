/* R_S reliability factor (Imre et al., 2025). */
// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>

#include "crfac.h"

#define RS_ALPHA 4.0
#define RS_BETA 0.15

struct cr_rs_arrays {
  real *e_d1;
  real *e_d2;
  real *t_d1;
  real *t_d2;
  real *e_y;
  real *t_y;
};

/**
 * Compute the smooth Y_s value for a single intensity point.
 */
static real cr_rs_y(real intensity, real deriv, real deriv2, real vi)
{
  const real vi2 = vi * vi;
  const real i2 = intensity * intensity;
  const real d2 = deriv * deriv;
  const real d2_2 = deriv2 * deriv2;
  real denom = 0.0;

  if (vi2 <= 0.0) {
    denom = R_sqrt(i2);
    return denom > 0.0 ? (deriv / denom) : 0.0;
  }

  if (deriv2 > 0.0) {
    const real inv_deriv2 = 1.0 / deriv2;
    const real offset = intensity * inv_deriv2 - (d2 * inv_deriv2 * inv_deriv2) * 0.5;
    const real y1 = (RS_ALPHA / vi2) * offset + RS_BETA;
    if (y1 > 0.0) {
      const real y2 = y1 / R_sqrt(1.0 + y1 * y1);
      denom = R_sqrt(i2 + 4.0 * vi2 * d2 + (y2 * y2) * (vi2 * vi2) * d2_2);
      if (denom > 0.0) {
        return deriv / denom;
      }
    }
  }

  denom = R_sqrt(i2 + 4.0 * vi2 * d2);
  return denom > 0.0 ? (deriv / denom) : 0.0;
}

/**
 * Compute first and second energy derivatives of the intensity curve.
 */
static void cr_rs_derivatives(const real *eng, const real *intens, int n,
                              real *d1, real *d2)
{
  if (n <= 1) {
    return;
  }

  d1[0] = (intens[1] - intens[0]) / (eng[1] - eng[0]);
  d2[0] = 0.0;

  for (int i = 1; i < n - 1; i++) {
    const real h_prev = eng[i] - eng[i - 1];
    const real h_next = eng[i + 1] - eng[i];
    d1[i] = (intens[i + 1] - intens[i - 1]) / (eng[i + 1] - eng[i - 1]);
    d2[i] = 2.0 * ((intens[i + 1] - intens[i]) / h_next -
                   (intens[i] - intens[i - 1]) / h_prev) /
            (h_prev + h_next);
  }

  d1[n - 1] = (intens[n - 1] - intens[n - 2]) / (eng[n - 1] - eng[n - 2]);
  d2[n - 1] = 0.0;
}

/**
 * Count energy points until the end-of-list marker.
 */
static int cr_rs_count_points(const real *eng)
{
  int n = 0;

  while (!IS_EQUAL_REAL(eng[n], F_END_OF_LIST)) {
    n++;
  }

  return n;
}

/**
 * Release arrays used by the R_s calculation.
 */
static void cr_rs_free_arrays(struct cr_rs_arrays *arrays)
{
  free(arrays->e_d1);
  free(arrays->e_d2);
  free(arrays->t_d1);
  free(arrays->t_d2);
  free(arrays->e_y);
  free(arrays->t_y);
  arrays->e_d1 = arrays->e_d2 = NULL;
  arrays->t_d1 = arrays->t_d2 = NULL;
  arrays->e_y = arrays->t_y = NULL;
}

/**
 * Allocate arrays used by the R_s calculation.
 */
static int cr_rs_alloc_arrays(struct cr_rs_arrays *arrays, int n)
{
  arrays->e_d1 = (real *)malloc(n * sizeof(real));
  arrays->e_d2 = (real *)malloc(n * sizeof(real));
  arrays->t_d1 = (real *)malloc(n * sizeof(real));
  arrays->t_d2 = (real *)malloc(n * sizeof(real));
  arrays->e_y = (real *)malloc(n * sizeof(real));
  arrays->t_y = (real *)malloc(n * sizeof(real));

  if (!arrays->e_d1 || !arrays->e_d2 || !arrays->t_d1 || !arrays->t_d2 ||
      !arrays->e_y || !arrays->t_y) {
    cr_rs_free_arrays(arrays);
    return 0;
  }

  return 1;
}

/**
 * Evaluate Y_s values for a full intensity curve.
 */
static void cr_rs_fill_y(const real *intens, const real *d1, const real *d2,
                          int n, real vi, real *out_y)
{
  for (int i = 0; i < n; i++) {
    out_y[i] = cr_rs_y(intens[i], d1[i], d2[i], vi);
  }
}

/**
 * Integrate the numerator and denominator terms of R_s.
 */
static void cr_rs_integrate(const real *eng, const real *e_y, const real *t_y,
                            int n, real *rf_sum, real *exp_sum, real *the_sum)
{
  for (int i = 1; i < n; i++) {
    const real e_step = eng[i] - eng[i - 1];
    const real y_exp = 0.5 * (e_y[i] + e_y[i - 1]);
    const real y_the = 0.5 * (t_y[i] + t_y[i - 1]);

    *rf_sum += (y_the - y_exp) * (y_the - y_exp) * e_step;
    *exp_sum += y_exp * y_exp * e_step;
    *the_sum += y_the * y_the * e_step;
  }
}

/**
 * Compute the improved R_s reliability factor.
 */
real cr_rs(const real *eng, const real *e_int, const real *t_int, real vi)
{
  struct cr_rs_arrays arrays = {0};
  const int n_eng = cr_rs_count_points(eng);
  real rf_sum = 0.0;
  real exp_y_sum = 0.0;
  real the_y_sum = 0.0;

  if (n_eng < 2) {
    return F_FAIL;
  }

  if (!cr_rs_alloc_arrays(&arrays, n_eng)) {
    return F_FAIL;
  }

  cr_rs_derivatives(eng, e_int, n_eng, arrays.e_d1, arrays.e_d2);
  cr_rs_derivatives(eng, t_int, n_eng, arrays.t_d1, arrays.t_d2);
  cr_rs_fill_y(e_int, arrays.e_d1, arrays.e_d2, n_eng, vi, arrays.e_y);
  cr_rs_fill_y(t_int, arrays.t_d1, arrays.t_d2, n_eng, vi, arrays.t_y);
  cr_rs_integrate(eng, arrays.e_y, arrays.t_y, n_eng,
                  &rf_sum, &exp_y_sum, &the_y_sum);
  cr_rs_free_arrays(&arrays);

  if (IS_EQUAL_REAL(exp_y_sum + the_y_sum, 0.0)) {
    return F_FAIL;
  }

  return rf_sum / (exp_y_sum + the_y_sum);
}
