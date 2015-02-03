/*********************************************************************
 *                       mkiv_draw_ellipse.C
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
 *   CS/09.08.93 - creation
 *   GH/27.07.03 - circle colour = max_value
 *********************************************************************/

/*! \file
 *
 * Implements the function mkiv_draw_ellipse()
 */
#include <math.h>
#include "mkiv.h"

/*!
 * Draws the elliptical integration area boundaries into image.
 *
 * \param nspot number of spots
 * \param spot list of measurable reflexes
 * \param image pointer to image matrix structure
 * \param scale pointer to half axies of elliptical integration area
 * \param angle angle of int.area versus horizontal
 * \param verh ratio background/integration signal of ellipses
 * \return
 */
int mkiv_draw_ellipse(size_t n_spot, mkiv_reflex spot[], mkiv_image *image,
                      mkiv_vector *scale, double angle, double verh)
{
  int a_back;
  register int i, j;
  register size_t k;
  register uint32_t pos;
  uint32_t cols, rows, n_size;

  double x_fac, y_fac, xy_fac, ell;
  double h = scale->xx;
  double v = scale->yy;

  uint16_t max_val;
  uint16_t *im = (uint16_t *)image->imagedata;

  /* return if key params are null */
  if (image == NULL || scale == NULL || im == NULL) return(-1);

  /* initialize values */
  cols = image->rows;
  rows = image->cols;

  /* find maximum value in image->imagedata */
  n_size = cols * rows;
  max_val = 1;

  for( pos = 0; pos < n_size; pos++)
  {
    max_val = MAX(max_val, im[pos]);
  }

  angle *= PI/180.;
  x_fac = PYTH2(cos(angle)/h,sin(angle)/v);
  y_fac = PYTH2(sin(angle)/h,cos(angle)/v);
  xy_fac = sin(angle) * cos(angle) * ( 2./(h*h) - 2./(v*v) );

  a_back = int_round( verh * fmax(h, v) );
  for (j = -a_back; j <= a_back; j++)
  {
    for (i = -a_back; i <= a_back; i++)
    {
      ell = (double)(i*i)*x_fac + (double)(j*j)*y_fac + (double)(i*j)*xy_fac;

      if (fabs(ell - 1.) > .07 && fabs(ell - SQUARE(verh)) > .13) continue;

      /* loop over spots */
      for (k = 0; k < n_spot; k++)
      {
        if ( spot[k].control & SPOT_OUT ) continue;

        pos = ((int)spot[k].yy + j)*cols + (int)spot[k].xx + i;

        if( pos < cols*rows && pos > 0 ) im[pos] = max_val;
      }
    }
  }
  return(0);
}
