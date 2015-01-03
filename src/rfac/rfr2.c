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

/*! \file
 *
 * Implementation of R2 factor calculation.
 */

#include <stdio.h>
#include <math.h>          /* needed for fabs */

#include "rfac.h"          /* specific definitions etc. */

/*!
 * \brief Calculate R2-factor.
 *
 * Calculate R2-factor:
 * \f$ R_2 = \sqrt{ S(I_t - norm_te*I_e)^2 / {S * I_t^2} } \f$
 *
 * \f$ norm_te = \sqrt( S|It|^2 / S|Ie|^2) \f$
 *
 * \note Normalisation is changed with respect to common use. Instead of
 * \f$ S {(I_t)}^2 \f$ it is now \f$ S {(I_t - <I_t>)}^2 \f$,
 * where \f$ <I_t> = \frac{(S I_t)}{dE} \f$ .
 *
 * \param[in] eng pointer to list of energy values.
 * \param[in] e_int pointer to list of experimental intensity values.
 * \param[in] t_int pointer to list of theoretical intensity values.
 *
 * \note: \p eng must be terminated by #F_END_OF_LIST.
 *
 * \retval R2-factor if successful.
 * \retval #RFAC_FAILURE if failed.
 */
real rfac_r2(const real *eng, const real *e_int, const real *t_int)
{
  size_t i_eng, n_eng;
  real exp_sq_sum, the_sq_sum;  /* sum of expt. / theor. intensities */
  real rfac_sum, norm_sum;
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

  the_avg =  the_avg / ( (real)n_eng );
  norm_te = R_sqrt(the_sq_sum/exp_sq_sum);

  CONTROL_MSG(CONTROL, "<It>: %e\n", (float)the_avg);
  CONTROL_MSG(CONTROL, "norm: %f \n", (float)norm_te);

  /* compute R-factor (rfac_sum) and normalisation factor (norm_sum) */
  rfac_sum = 0.;
  norm_sum = 0.;
  for(i_eng = 0; i_eng < n_eng; i_eng ++)
  {
    aux = t_int[i_eng] - norm_te * e_int[i_eng];
    rfac_sum +=  SQUARE(aux);

    aux = t_int[i_eng] - the_avg;
    norm_sum += SQUARE(aux);
  }

  if (norm_sum < 0.)
  {
    ERROR_MSG("norm_sum is negative: %e\n", (float)norm_sum);
    return((real)RFAC_FAILURE);
  }
  else aux = R_sqrt(rfac_sum/norm_sum);

  return(aux);
}  /* end of function cr_r2 */
