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
 *   CS/21.09.93 - allow n_step
 *********************************************************************/

/*! \file
 * Implements mkiv_bkgnd_smooth() function.
 */

/*!
 * Calculate smoothed value for the "numb". element of int_norm with
 * the foregoing "width" values.
 *
 * \param int_norm
 * \param numb
 * \param n_start
 * \param n_step
 * \param width
 * \return The averaged (smoothed) intensity value.
 */
double mkiv_bkgnd_smooth(double *int_norm, int numb, int n_start, int n_step, int width)
{
  int i, n_ini;
  double int_avg = 0;
  double fac = 1.;

  if ( n_step > 0 )
  {
    n_ini = (numb - n_step*width < n_start) ? n_start : (numb - n_step*width);
  }
  else
  {
    n_ini = (numb - n_step*width > n_start) ? n_start : (numb - n_step*width);
  }

  /* linearly weighted average over the last n_max numbers */
  for ( i = n_ini;
        (i <= numb && n_step > 0) || (i >= numb && n_step < 0);
        i += n_step, fac += 1.)
  {
    int_avg += int_norm[i] * fac;
  }
  fac = fac * (fac-1.) / 2.;
  int_avg /= fac;

  return(int_avg);
}
