/*********************************************************************
 *                        FIMAX4.C
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
 *   SU/10.07.91 - creation
 *   GH/31.08.92 - kernel method
 *   CS/05.08.93 - all positive kernel
 *   CS/20.08.93
 *********************************************************************/

/*! \file
 *
 *
 */

#include "mkiv.h"

/*!
 * Scans for each spot through a disc (radius = range) searching for
 * the maximum intensity. A kernel is used, which is described below.
 * If spot->control -bit SPOT_GOOD_S2N is set, no max. search is performed.
 * The (maximum) coordinates for @mkiv_reflex::xx and @mkiv_reflex::yy are set for each
 * element in \p spot .
 *
 * \param n_spot number of measurable spots
 * \param[in,out] spot pointer to array of #mkiv_reflex containing information on the
 * measurable reflexes.
 * \param step step size for maximum search.
 * \param range radius of search area in pixels.
 * \param[in] image pointer to the matrix of image data.
 */
void mkiv_find_imax(size_t n_spot, mkiv_reflex spot[], int step, double range,
                    const mkiv_image *image)
{
  register int h, v, sum, adr0;  /* auxiliaries */
  int lowh, lowv,                /* lower limits of search area */
      high, higv;                /* upper limits of search area */
  int64_t max_sum;               /* integration sum, total maximum of all sum */
  double h0, v0;                 /* calculated spot positions */

  /* the kernel that serves for a weighted integration [sum(kernel)=112] */
  int KL_H_SIZE = 3;          /*  half size of kernel */
  size_t n_kernel = 7*7;
  int ad[7*7];
  static int64_t kernel[7*7] = {  0, 1, 1, 1, 1, 1, 0,
                                  1, 1, 2, 2, 2, 1, 1,
                                  1, 2, 4, 8, 4, 2, 1,
                                  1, 2, 8,16, 8, 2, 1,
                                  1, 2, 4, 8, 4, 2, 1,
                                  1, 1, 2, 2, 2, 1, 1,
                                  0, 1, 1, 1, 1, 1, 0 };

  register size_t i, k = 0, cols = image->rows;
  size_t rows = image->cols;
  uint16_t *im = (uint16_t *)image->imagedata;

  if (im == NULL)
  {
    ERROR_MSG("pointer to image data has not been allocated\n");
    return;
  }
     
  for (v = -KL_H_SIZE; v <= KL_H_SIZE; v++)
  {
    for (h = -KL_H_SIZE; h <= KL_H_SIZE; h++)
    {
      ad[k++] = (int)cols*v + h;
    }
  }

  for (i=0; i<n_spot; i++)
  {
    if( spot[i].control & SPOT_GOOD_S2N ) continue;
    h0 = spot[i].xx;
    v0 = spot[i].yy;

    /* find boundaries */
    lowh = MAX( (int)(h0 - range), KL_H_SIZE );
    high = MIN( (int)(h0 + range), (int)cols - KL_H_SIZE - 1 );

    lowv = MAX( (int)(v0 - range), KL_H_SIZE );
    higv = MIN( (int)(v0 + range), (int)rows - KL_H_SIZE - 1 );

    /* loop over searching area */
    /* v and h point to the center of the kernel = center of integration area */
    max_sum = 0L;
    for (v = lowv; v <= higv; v+=step)
    {
      for (h = lowh; h <= high; h+=step)
      {
        if ( PYTH(v-v0, h-h0) > range ) continue; /* spot out of range */
        adr0 = v*(int)cols + h;
        sum = 0L;
        for (k=0; k < n_kernel; k++) sum += kernel[k] * (int)im[adr0 + ad[k]];

        /* store coordinates when intensity is max */
        if (sum > max_sum)
        {
          spot[i].yy = (double)v;
          spot[i].xx = (double)h;
          max_sum = sum;
        }

      }       /* for(h = ...) */

    }       /* for(v = ...) */

  }       /* for(i < n_spot) */
    
} /* end of function mkiv_find_imax */
