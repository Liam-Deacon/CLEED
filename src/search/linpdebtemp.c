/*********************************************************************
 *                      LINDEBTEMP.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/2000.09.27 - bug fix in Debye Waller-Temp input
 *********************************************************************/

/*!
 * \file
 * \author Georg Held <g.held@reading.ac.uk>
 *
 * Calculates the mean square displacement using leed_inp_debye_temp(). This
 * function is used in both \c cleed and \c csearch programs.
 *
 * \note \ref EXIT_ON_ERROR should be defined while compiling if you
 * wish the leed_inp_debye_temp() function to stop program execution on failure.
 *
 * \note Additional output is available by adding the defines
 * \c WARNING_LOG , \c CONTROL and \c ERROR_LOG during compilation.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>

#include "leed.h"

#define PREF_DEBWAL 1559.04170632481439  /*!< prefactor for the evaluation
                                          * of <dr^2> from temperature
                                          * and Debye temperature */

/*!
 * Calculate mean square displacement \f$ <dr^2> \f$ from Debye temperature,
 * mass and temperature.
 *
 * Use equations (27/28, p.30 VHT) to calculate <r^2>:
 *
 * \f[ <r^2> = \frac{1}{2} \times \frac{9 T}{m k_B T_D^2} \mbox{for large}
 * T (> T_D) \f]
 *
 * \f[ <r^2> = \frac{1}{2} \times \frac{9}{m k_B T_D} \times ( (\frac{1}{4}
 * + 1.642) \times {\frac{T}{T_D}}^2 ) \mbox{for small} T (< T_D) \f]
 *
 * \f[ <r^2> = \frac{1}{2} \times \frac{9}{m k_B T_D} \times \sqrt{
 * \frac{1}{16} + {\frac{T}{T_D}}^2} \mbox{for intermediate} T \f]
 *
 * (PREF_DEBWAL \f$ m = \frac{9}{m k_B} \f$ )
 *
 * The factor \f$ \frac{1}{2} \f$ (not in VHT) makes \f$ <r^2> \f$
 * the mean square displacement rather than the amplitude.
 *
 * \param deb_temp Debye temperature \f$ T_D \f$ in Kelvin
 * \param mass mass \f$ m \f$ in amu (1 amu = \f$ 1.66053892 * 10^{-27} \f$ kg).
 * \param temp absolute temperature \f$ T \f$ in Kelvin
 * (0 K = -273.16 \f$ \degree \f$ C).
 *
 * \return mean square displacement \f$ <dr^2> \f$
 * \retval \c -1. if failed and \ref EXIT_ON_ERROR is not defined.
 *
 * \note exits with \c -1 if \c EXIT_ON_ERROR is defined.
 *
 **********************************************************************/
real leed_inp_debye_temp(real deb_temp, real mass, real temp)
{
  real dr2;
  real faux;                    /* dummy variable */

/*********************************************************************
 * Check parameters
 *********************************************************************/

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

/*********************************************************************
 * calculate dr2
 *********************************************************************/

  CONTROL_MSG(CONTROL, "Debye = %.1f, Mass = %.1f, Temp. = %.1f\n",
                        deb_temp, mass, temp);

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
