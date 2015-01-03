/*********************************************************************
 *                           RFOUTPUT.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1992.10.06 - Creation
 *   GH/1995.08.29 - adjust to crfac format.
 ********************************************************************/

/*! \file
 *
 * Implementation file for calculating Pendry R-factor.
 */

#include <stdio.h>
#include <math.h>          /* needed for sqrt */

#include "rfac.h"          /* specific definitions etc. */


/*!
 * \brief Compute Pendry's R-factor.
 *
 * Compute Pendry's R-factor:
 *
 * \f$ R_p = S(Y_e - Y_t)^2 / S(Y_e^2 + Y_t^2) \f$
 *
 * \param[in] eng pointer to list of energies.
 * \param[in] e_int pointer to list of experimental intensities.
 * \param[in] t_int pointer to list of theoretical intensities.
 *
 * \warning \p eng must be terminated by F_END_OF_LIST.
 *
 * \param vi Imaginary part of the optical potential.
 *
 * \return Rp if successful.
 */
real rfac_rp(const real *eng, const real *e_int, const real *t_int, real vi)
{
  size_t i_eng;
  real rfac_sum, exp_y_sum, the_y_sum;
  real Y_exp, Y_the, L, e_step;

  /* compute integrals */
  exp_y_sum = 0.;
  the_y_sum = 0.;
  rfac_sum = 0.;
  for(i_eng = 1;
     ! IS_EQUAL_REAL(eng[i_eng], F_END_OF_LIST); i_eng ++)
  {
    e_step = eng[i_eng] - eng[i_eng-1];

    /* theoretical Y function */
    L = (real)(( t_int[i_eng] - t_int[i_eng-1] ) /
        ( e_step * 0.5 * (t_int[i_eng] + t_int[i_eng-1]) ));

    Y_the = (real)(L / ( 1. + L*L*vi*vi));
    the_y_sum += SQUARE(Y_the)*e_step;

    /* experimental Y function */
    L = (real)((e_int[i_eng] - e_int[i_eng-1]) /
        ( e_step * 0.5 * (e_int[i_eng] + e_int[i_eng-1]) ));

    Y_exp = (real)(L / ( 1. + L*L*vi*vi));
    exp_y_sum += SQUARE(Y_exp)*e_step;

    /* compute R-factor sum */
    rfac_sum += SQUARE(Y_the - Y_exp)*e_step;
  }

  #ifdef CONTROL
  fprintf(STDCTR, "(cr_rp): exp_sum: %f the_sum: %f rfac_sum: %f\n", 
          exp_y_sum, the_y_sum, rfac_sum);
  #endif

  /*
   * Divide rfac_sum by the normalizing factor (exp_y_sum + the_y_sum)
   * and return result
   */
  rfac_sum /= (exp_y_sum + the_y_sum);

  return (rfac_sum);
}
