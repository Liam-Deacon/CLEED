/*********************************************************************
 *                           RFR2.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1992.10.02 - creation
 *   GH/1992.10.26 -  change normalisation
 *   GH/1993.03.01 -  change normalisation factor of T/E IV curves
 *   GH/1995.09.07 -  Adaption for CRFAC
 ********************************************************************/

/*! \file rfr2.c
 *
 * Implementation of R2 factor calculation.
 */

#include <stdio.h>
#include <math.h>          /* needed for fabs */

#include "rfac.h"          /* specific definitions etc. */

/*!
 * \fn rf_r1
 *
 * \brief Calculate R2-factor.
 *
 * Calculate R2-factor:
 * \f$ R_2 = \sqrt{ S(I_t - norm_te*I_e)^2 / {S * I_t^2} }
 *
 * \f$ norm_te = \sqrt( S|It|^2 / S|Ie|^2) \f$
 *
 * \note Normalisation is changed with respect to common use. Instead of
 * S (It)^2 it is now S (It - <It>)^2, where <It> = (S It)/ dE.
 *
 * \param[in] *eng pointer to list of energy values.
 * \param[in] *e_int pointer to list of experimental intensity values.
 * \param[in] *t_int pointer to list of theoretical intensity values.
 *
 * \note: At least eng must be terminated by F_END_OF_LIST.
 *
 * \retval R2-factor if successful.
 * \retval \ref RFAC_FAILURE if failed.
 */
real rf_r2(const real *eng, const real *e_int, const real *t_int)
{
  size_t i_eng, n_eng;
  real exp_sq_sum, the_sq_sum;  /* sum of expt. / theor. intensities */
  real rf_sum, norm_sum;
  real norm_te, the_avg, aux;

  /* compute integrals */
  exp_sq_sum = 0.;
  the_sq_sum = 0.;
  the_avg = 0.;

  for(i_eng = 0;
      ! IS_EQUAL_REAL(eng[i_eng], F_END_OF_LIST); i_eng ++)
  {
    exp_sq_sum += SQUARE( e_int[i_eng] );
    the_sq_sum += SQUARE( t_int[i_eng] );
    the_avg    += t_int[i_eng];
  }
  n_eng = i_eng;

  the_avg = the_avg / n_eng;
  norm_te = R_sqrt(the_sq_sum/exp_sq_sum);

  #ifdef CONTROL
  fprintf(STDCTR, "(rf_r2): <It>: %e\n", (float)the_avg);
  fprintf(STDCTR, "(rf_r2): norm: %f \n", (float)norm_te);
  #endif

  /* compute R-factor (rf_sum) and normalisation factor (norm_sum) */
  rf_sum = 0.;
  norm_sum = 0.;
  for(i_eng = 0; i_eng < n_eng; i_eng ++)
  {
    aux = t_int[i_eng] - norm_te * e_int[i_eng];
    rf_sum +=  SQUARE(aux);

    aux = t_int[i_eng] - the_avg;
    norm_sum += SQUARE(aux);
  }

  #ifdef ERROR /* print error output if required */
  if (norm_sum < 0.)
  {
    printf("*** error (rf_r2): norm_sum is negative: %e\n", (float)norm_sum);
    return((real)RFAC_FAILURE);
  }
  #endif

  aux = R_sqrt(rf_sum/norm_sum);

  return (aux);
}  /* end of function cr_r2 */