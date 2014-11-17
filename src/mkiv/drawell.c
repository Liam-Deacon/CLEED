/*********************************************************************
 *                       DRAWELL.C
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
 * Implements the function drawell()
 */

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
int drawell(int nspot, mkiv_reflex spot[], mkiv_image *image, mkiv_vector *scale,
            float angle, float verh)
{
  register int i,j,k,pos;
  int a_back;
  int cols, rows, n_size;

  float x_fac, y_fac, xy_fac, ell;
  float h = scale->xx;
  float v = scale->yy;

  unsigned short max_val;
  unsigned short *im;

  /* initialize values */
  cols = image->rows;
  rows = image->cols;
  im = (unsigned short *)image->imagedata;

  /* find maximum value in image->imagedata */
  n_size = cols * rows;
  max_val = 1;

  for( i = 0; i < n_size; i ++)
  {
    max_val = MAX(max_val, im[i]);
  }

  angle *= PI/180.;
  x_fac = PYTH2(cos(angle)/h,sin(angle)/v);
  y_fac = PYTH2(sin(angle)/h,cos(angle)/v);
  xy_fac = sin(angle) * cos(angle) * ( 2./(h*h) - 2./(v*v) );

  a_back = RND( verh * MAX(h,v) );
  for (j = -a_back; j <= a_back; j++)
  {
    for (i = -a_back; i <= a_back; i++)
    {
      ell = i*i*x_fac + j*j*y_fac + i*j*xy_fac;

      if (fabs(ell-1.)>.07 && fabs(ell-SQUARE(verh))>.13) continue;

      /* loop over spots */
      for (k = 0; k < nspot; k++)
      {
        if ( spot[k].control & SPOT_OUT ) continue;

        pos = ((int)spot[k].yy + j)*cols + (int)spot[k].xx + i;

        if( pos < cols*rows && pos > 0 ) im[pos] = max_val;
      }
    }
  }
  return(0);
}
