/*********************************************************************
 *                        PLOTIND.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/27.07.03 - add max_val to sign
 *   GH/16.11.05 - change coordinates of indices
 *                 (to make sure they are inside boundaries)
 *********************************************************************/

/*! \file
 *
 * Contains mkiv_plot_indices() function for labelling reflexes within a LEED image.
 */

#include <stdlib.h>
#include "mkiv.h"

/*!
 * Plots the corresponding indices above all reflexes.
 *
 * \param[in,out] image Image to modify.
 * \param nspot Number of spots in \p array.
 * \param[in] spot Pointer to #mkiv_reflex array containing list of
 * measurable reflexes.
 *  * \return
 */
int mkiv_plot_indices(mkiv_image *image, size_t nspot, const mkiv_reflex *spot)
{
  int xx, yy;        /* coordinates assigned to a sign or number */
  size_t k, j;
  size_t n_size = image->cols * image->rows;
  uint8_t chr;
  double ind[2];            /* auxiliary for mkiv_reflex indices */

  uint16_t *im = (uint16_t *)image->imagedata;
  uint16_t max_val = 1;

  /* find maximum intensity in image */
  for(k=0; k < n_size; k++) max_val = MAX(max_val, im[k]);

  /* process image and add (h,k) index above each mkiv_reflex */
  for (k=0; k < nspot; k++)
  {
    ind[0] = spot[k].lind1;
    ind[1] = spot[k].lind2;
    xx = (int)spot[k].xx;
    yy = (int)spot[k].yy;

    if (ind[0] < 0 ) xx -= 7;
    if (fabs(ind[0]) > 1. && !NAT(ind[0]) ) xx -= 7;

    for (j=0; j < 2; j++)
    {
      /* Check if index has a negative sign */
      if (ind[j] < 0) mkiv_sign (image, 45, xx+=7, yy, max_val);

      /* Check if index has an integer part */
      ind[j] = fabs(ind[j]);
      if (fabs(ind[j] - 0.) < DBL_EPSILON || ind[j] >= 1. )
      {
        mkiv_sign (image, (uint8_t)ind[j]%10 + 48, xx+=7, yy, max_val);
      }

      /* Check if index has a fractional part */
      if ( NAT(ind[j]) == 0 )
      {
        switch( (uint8_t)( 100 * ind[j] ) % 100 )
        {
          case (uint8_t)50.00: chr = 60; break; /* 1/2 */
          case (uint8_t)33.33: chr = 61; break; /* 1/3 */
          case (uint8_t)16.67: chr = 62; break; /* 1/6 */
          case (uint8_t)14.28: chr = 63; break; /* 1/7 */
          case (uint8_t)66.67: chr = 64; break; /* 2/3 */
          case (uint8_t)83.33: chr = 65; break; /* 5/6 */
          case (uint8_t)12.50: chr = 68; break; /* 1/8 */
          case (uint8_t)37.50: chr = 69; break; /* 3/8 */
          case (uint8_t)62.50: chr = 70; break; /* 5/8 */
          case (uint8_t)87.50: chr = 71; break; /* 7/8 */
          case (uint8_t)28.57: chr = 72; break; /* 2/7 */
          case (uint8_t)42.86: chr = 73; break; /* 3/7 */
          case (uint8_t)57.14: chr = 74; break; /* 4/7 */
          case (uint8_t)71.43: chr = 75; break; /* 5/7 */
          case (uint8_t)85.71: chr = 76; break; /* 6/7 */
          case (uint8_t)25.00: chr = 77; break; /* 1/4 */
          case (uint8_t)75.00: chr = 78; break; /* 3/4 */
          default : chr = 66;
        }
        mkiv_sign (image, chr, xx+=7, yy, max_val);
      }

      /* Plot a semicolon between indices */
      if (j == 0) mkiv_sign (image, 59, xx+=7, yy, max_val);
    }
  }

  return(0);
}
