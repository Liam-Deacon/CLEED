/*********************************************************************
 *                        LINPDEBTEMP.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/27.09.00 - bug fix in Debye Waller-Temp input
 *********************************************************************/

/*! \file
 *
 * Implementation of leed_inp_debye_temp() function.
 *
 * \note This function is used in both the \c LEED and \c SEARCH programs.
 */

#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#include "leed.h"

#ifndef PREF_DEBWAL
#define PREF_DEBWAL 1559.04170632481439  /*! prefactor for the evaluation
                                          * of \f$ <dr^2> \f$ from the
                                          * experimental temperature \f$ T \f$
                                          * and Debye temperature \f$ D \f$ */
#endif

/*!
 * Calculates the mean square displacement \f$ <dr^2> \f$ from the
 * Debye temperature \f$ D \f$, mass \f$ m \f$ and temperature \f$ T \f$ .
 *
 * Use equations (27/28, p.30 VHT) to calculate \f$ <r^2> \f$ :
 *
 * \f[ <r^2> = \frac{1}{2} \times \frac{9T}{m k_B T_D^2} \f]
 * for large \f$ T (> T_D) \f$
 *
 * \f[ <r^2> = \frac{1}{2} \times \frac{9}{m k_B T_D} \times
 * ( 1/4 + 1.642 \times \frac{T}{T_D}^2 ) \f] for small \f$ T (< T_D) \f$
 *
 * \f[ <r^2> = \frac{1}{2} \times \frac{9}{m k_B T_D} \times
 * \sqrt{\frac{1}{16} + \frac{T}{T_D}^2} \f] for intermediate \f$ T \f$
 *
 * \c PREF_DEBWAL \f$ m = \frac{9}{m k_B} \f$
 *
 * The factor \f$ \frac{1}{2} \f$ (not in VHT) makes \f$ <r^2> \f$ the mean
 * square displacement rather than the amplitude.
 *
 * \param deb_temp Debye temperature \f$ D \f$ in Kelvin.
 * \param mass Mass \f$ m \f$ in atomic mass units (amu).
 * \param temp Temperature \f$ T \f$ in Kelvin.
 * \return \f$ <dr^2> \f$
 * \retval -1 if function fails and #EXIT_ON_ERROR is not defined.
 */
real leed_inp_debye_temp(real deb_temp, real mass, real temp)
{
  real dr2;
  real faux;                    /* dummy variable */

  /* Check parameters */
  if (deb_temp <= 0.)
  {
    ERROR_MSG("wrong value for Debye temperature: %f\n", deb_temp);
    ERROR_RETURN(-1.);
  }

  if (mass <= 0.)
  {
    ERROR_MSG("wrong value for mass: %f\n", mass);
    ERROR_RETURN(-1.);
  }

  if (temp < 0.)
  {
    ERROR_MSG("wrong value for temperature: %f\n", temp);
    ERROR_RETURN(-1.);
  }

  /* calculate dr2 */
  CONTROL_MSG(CONTROL,
      "Debye = %.1f, Mass = %.1f, Temp. = %.1f\n", deb_temp, mass, temp);

  faux = temp / deb_temp;

  if( faux < 0.125 )       /* small T < T_D / 8 */
  {
    dr2 = 0.5 * PREF_DEBWAL / (mass * deb_temp) * (0.25 + 1.642 * faux * faux);
  }
  else if( faux > 8.0 )    /* large T  > T_D * 8 */
  {
    dr2 = 0.5 * PREF_DEBWAL / (mass * deb_temp) * faux;
  }
  else                     /* intermediate T */
  {
    dr2 = 0.5 * PREF_DEBWAL / (mass * deb_temp) * R_sqrt(0.0625 + faux*faux);
  }

  CONTROL_MSG(CONTROL, "dr2 = %.3f dr1 = %.3f \n",
              dr2 * BOHR * BOHR, R_sqrt(dr2)*BOHR );

  return(dr2);
}  /* end of function leed_inp_debye_temp */
