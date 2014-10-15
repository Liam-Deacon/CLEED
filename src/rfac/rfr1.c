/********************************************************************
GH/07.09.95
file contains function:

   real cr_r1( real *eng, real *e_int, real *t_int)

Calculate R2-factor

Changes:
  GH/01.10.92
  GH/26.10.92 - Change normalisation.
********************************************************************/

/*! \file rfr1.c
 *  \brief Implementation of R1 calculation using \ref rf_r1 function.
 */

#include <stdio.h>
#include <math.h>          /* needed for fabs */

#include "rfac.h"          /* specific definitions etc. */

/*!
 * \fn rf_r1
 *
 * \brief compute R1-factor.
 *
 * This function calculates the R1-factor between experimental and
 * theoretical IV curves.
 *
 * R1 = S|It - norm_te*I_experiment| / S |I_theory|
 *
 * norm_te = S|I_theory| / S|I_experiment|;
 *
 * \note Normalisation factor (denominator) is changed with respect to
 * common use. Instead S |I_theory|, now S |I_theory - S*I_theory / \delta E|
 * is used.
 *
 * \param[in] *eng pointer to list of energy values.
 * \param[in] *e_int pointer to list of experimental intensity values.
 * \param[in] *t_int pointer to list of theoretical intensities.
 *
 * \note At least eng must be terminated by \ref F_END_OF_LIST.
 *
 * \retval R1-factor, if successful.
 * \retval F_FAIL, if failed.
 */
real rf_r1(const real *eng, const real *e_int, const real *t_int)
{
  int i_eng, n_eng;
  real exp_sum, the_sum;
  real rf_sum, norm_sum;
  real norm_te;

  /* calculate integrals */
  exp_sum = 0.;
  the_sum = 0.;

  for(i_eng = 0;
     ! IS_EQUAL_REAL(eng[i_eng], F_END_OF_LIST); i_eng ++)
  {
    exp_sum += R_fabs( e_int[i_eng] );
    the_sum += t_int[i_eng];
  }
  n_eng = i_eng;

  norm_te = the_sum / exp_sum;             /* ratio theor. / expt. curve */
  the_sum = the_sum / (real)n_eng;         /* average theor. intensity */

  /* calculate R-factor */
  rf_sum = 0.;
  norm_sum = 0.;
  for(i_eng = 0; i_eng < n_eng; i_eng ++)
  {
    rf_sum   += R_fabs( t_int[i_eng] - norm_te * e_int[i_eng] );
    norm_sum += R_fabs( t_int[i_eng] - the_sum );
  }

  return (rf_sum/norm_sum);
}
