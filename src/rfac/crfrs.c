/********************************************************************
GH/29.08.95
file contains function:

   real cr_rs( real *eng, real *e_int, real *t_int, real vi)

Calculate R_S reliability factor (Imre et al., 2025)

********************************************************************/
#include <math.h>
#include <stdlib.h>

#include "crfac.h"

#define RS_ALPHA 4.0
#define RS_BETA 0.15

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

static void cr_rs_derivatives(const real *eng, const real *intens, int n,
                              real *d1, real *d2)
{
  int i;

  if (n <= 1) {
    return;
  }

  d1[0] = (intens[1] - intens[0]) / (eng[1] - eng[0]);
  d2[0] = 0.0;

  for (i = 1; i < n - 1; i++) {
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

real cr_rs(real *eng, real *e_int, real *t_int, real vi)
{
  int i_eng;
  int n_eng;
  real rf_sum = 0.0;
  real exp_y_sum = 0.0;
  real the_y_sum = 0.0;

  real *e_d1 = NULL;
  real *e_d2 = NULL;
  real *t_d1 = NULL;
  real *t_d2 = NULL;
  real *e_y = NULL;
  real *t_y = NULL;

  for (i_eng = 0; !IS_EQUAL_REAL(eng[i_eng], F_END_OF_LIST); i_eng++) {
    ;
  }
  n_eng = i_eng;
  if (n_eng < 2) {
    return F_FAIL;
  }

  e_d1 = (real *)malloc(n_eng * sizeof(real));
  e_d2 = (real *)malloc(n_eng * sizeof(real));
  t_d1 = (real *)malloc(n_eng * sizeof(real));
  t_d2 = (real *)malloc(n_eng * sizeof(real));
  e_y = (real *)malloc(n_eng * sizeof(real));
  t_y = (real *)malloc(n_eng * sizeof(real));

  if (!e_d1 || !e_d2 || !t_d1 || !t_d2 || !e_y || !t_y) {
    free(e_d1);
    free(e_d2);
    free(t_d1);
    free(t_d2);
    free(e_y);
    free(t_y);
    return F_FAIL;
  }

  cr_rs_derivatives(eng, e_int, n_eng, e_d1, e_d2);
  cr_rs_derivatives(eng, t_int, n_eng, t_d1, t_d2);

  for (i_eng = 0; i_eng < n_eng; i_eng++) {
    e_y[i_eng] = cr_rs_y(e_int[i_eng], e_d1[i_eng], e_d2[i_eng], vi);
    t_y[i_eng] = cr_rs_y(t_int[i_eng], t_d1[i_eng], t_d2[i_eng], vi);
  }

  for (i_eng = 1; i_eng < n_eng; i_eng++) {
    const real e_step = eng[i_eng] - eng[i_eng - 1];
    const real y_exp = 0.5 * (e_y[i_eng] + e_y[i_eng - 1]);
    const real y_the = 0.5 * (t_y[i_eng] + t_y[i_eng - 1]);

    rf_sum += (y_the - y_exp) * (y_the - y_exp) * e_step;
    exp_y_sum += y_exp * y_exp * e_step;
    the_y_sum += y_the * y_the * e_step;
  }

  free(e_d1);
  free(e_d2);
  free(t_d1);
  free(t_d2);
  free(e_y);
  free(t_y);

  if (IS_EQUAL_REAL(exp_y_sum + the_y_sum, 0.0)) {
    return F_FAIL;
  }

  return rf_sum / (exp_y_sum + the_y_sum);
}
