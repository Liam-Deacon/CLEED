/*********************************************************************
 *                           RFRB.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1993.02.19 - Creation
 *   GH/1994.02.08  Change Normalistion (Cuncorr = 0.67)
 *   GH/1995.09.07 -  Adaption for CRFAC
 ********************************************************************/

/*! \file
 *
 * Implemenation file for Rb1 factor calculation.
 */

#include <stdio.h>
#include <math.h>           /* needed for sqrt */
#include "rfac.h"           /* specific definitions etc. */

static const double CNORM = 0.666667; /*!< 2/3 normalisation to uncorrelated curves */

/*!
 * \brief Calculate Rb1-factor.
 *
 * Calculate Rb1-factor:
 *
 * \f$ R_{b1} = \frac{(1 - C)}{(1 - C_NORM)} \f$
 *
 * \f$ C = \frac{S(I_t \times I_e)}{\sqrt{ (S I_t^2) \cdot (S I_e^2)}} \f$
 * \f$ C_{NORM} = \frac{2}{3} \f$ (cf. G. Held, Dissertation)
 *
 * \param[in] eng pointer to list of energies.
 * \param[in] e_int pointer to list of experimental intensities.
 * \param[in] t_int lists pointer to list of theoretical intensities.
 *
 * \note \p eng must be terminated by #F_END_OF_LIST.
 *
 * \return R2-factor if successful.
 * \retval #RFAC_FAILURE, if failed.
 *
 */
real rfac_rb(const real *eng, const real *e_int, const real *t_int)
{
  size_t i_eng;
  real exp_sq_sum, the_sq_sum;
  real rfac_sum;

  /* Calculate integrals and R-factor */
  exp_sq_sum = 0.;
  the_sq_sum = 0.;
  rfac_sum = 0.;

  for(i_eng = 0; ! IS_EQUAL_REAL(eng[i_eng], F_END_OF_LIST); i_eng ++)
  {
    rfac_sum += e_int[i_eng] * t_int[i_eng];

    exp_sq_sum += SQUARE(e_int[i_eng]);
    the_sq_sum += SQUARE(t_int[i_eng]);
  }

  rfac_sum /= R_sqrt(the_sq_sum * exp_sq_sum);

  CONTROL_MSG(CONTROL, "rfac_sum: %f,", rfac_sum);

  rfac_sum = (real)( (1. - rfac_sum) / (1. - CNORM) );

  CONTROL_MSG(CONTROL, "%f \n", rfac_sum);

  return (rfac_sum);
}
