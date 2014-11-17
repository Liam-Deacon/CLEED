/*********************************************************************
 *                       BSMOOTH.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 1993-2014 Christian Stellwag <leed@iron.E20.physik.tu-muenchen.de>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/02.09.92 - creation
 *   CS/21.09.93 - allow nstep
 *********************************************************************/

/*! \file
 * Implements bkgnd_smooth() function.
 */

/*!
 * Calculate smoothed value for the "numb". element of int_norm with
 * the foregoing "width" values.
 *
 * \param int_norm
 * \param numb
 * \param nstart
 * \param nstep
 * \param width
 * \return The averaged (smoothed) intensity value.
 */
float bkgnd_smooth(float *int_norm, int numb, int nstart, int nstep, int width)
{
  int i, n_ini;
  float int_avg = 0;
  float fac = 1.;

  if ( nstep > 0 )
  {
    n_ini = (numb - nstep*width < nstart) ? nstart : (numb - nstep*width);
  }
  else
  {
    n_ini = (numb - nstep*width > nstart) ? nstart : (numb - nstep*width);
  }

  /* linearly weighted average over the last n_max numbers */
  for ( i = n_ini;
        (i <= numb && nstep > 0) || (i >= numb && nstep < 0);
        i += nstep, fac += 1.)
  {
    int_avg += int_norm[i] * fac;
  }
  fac = fac * (fac-1.) / 2.;
  int_avg /= fac;

  return(int_avg);
}
