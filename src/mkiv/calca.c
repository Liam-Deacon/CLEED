/*********************************************************************
 *                        CALCA.C
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
 *   CS/08.09.93 - creation.
 *********************************************************************/

/*! \file
 *
 * Implements calca() function.
 */

#include "mkiv.h"

/*!
 * Computes the new values of \p a , \p range and \p scale according to the new
 * energy value and given boundaries (i.e. \p range_min , \p scale_min ).
 *
 * \param kappa
 * \param en_old
 * \param energy
 * \param a
 * \param range
 * \param range_min
 * \param rel_range
 * \param scale
 * \param scale_min
 * \param rel_scale
 *  * \return
 * \retval 0 on successful completion.
 */
int mkiv_calc_area(double *kappa, double *en_old, double energy,
      mkiv_vector a[], double *range, double range_min, double rel_range,
      mkiv_vector *scale, double scale_min, mkiv_vector rel_scale)
{
   double fval;

   *kappa = sqrt(*en_old/energy);
   a[1].xx *= *kappa;
   a[1].yy *= *kappa;
   a[1].len = PYTH( a[1].xx, a[1].yy );
   a[2].xx *= *kappa;
   a[2].yy *= *kappa;
   a[2].len = PYTH( a[2].xx, a[2].yy );
   *range = MAX( range_min, rel_range * a[1].len );

   scale->xx = rel_scale.xx * a[1].len;
   scale->yy = rel_scale.yy * a[1].len;

   if ( ( fval = PYTH(scale->xx,scale->yy)/scale_min/ROOT2 ) < 1. )
   {
     scale->xx /= fval;
     scale->yy /= fval;
   }
   *en_old = energy;
   
   return(0);
}
