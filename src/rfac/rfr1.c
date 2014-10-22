/*********************************************************************
 *                       RFR1.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1992.10.01 - Creation
 *   GH/1992.10.26 - Change normalisation.
 ********************************************************************/

/*! \file
 *  \brief Implementation of R1 calculation using rf_r1() function.
 *
 *  Calculate R1 factor.
 */

#include <stdio.h>
#include <math.h>          /* needed for fabs */

#include "rfac.h"          /* specific definitions etc. */

/*!
 * \brief compute R1-factor.
 *
 * This function calculates the R1-factor between experimental and
 * theoretical IV curves.
 *
 * \f[ R1 = \frac{S |I_t - (norm_te \times I_e)|}{S |I_t|} \f]
 *
 * \f[ norm_te = \frac{S|I_t|}{S|I_e|} \f]
 *
 * \note Normalisation factor (denominator) is changed with respect to
 * common use. Instead of \f$ S |I_t| \f$, \f$ S|\frac{I_t - S*I_t}{\delta E}|
 * \f$ is used.
 *
 * \param[in] eng pointer to list of energy values.
 * \param[in] e_int pointer to list of experimental intensity values.
 * \param[in] t_int pointer to list of theoretical intensities.
 *
 * \note \p eng must be terminated by #F_END_OF_LIST.
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
