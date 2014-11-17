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
 * \param nspot number of measurable spots
 * \param[in,out] spot pointer to array of #mkiv_reflex containing information on the
 * measurable reflexes.
 * \param step step size for maximum search.
 * \param range radius of search area in pixels.
 * \param[in] image pointer to the matrix of image data.
 */
void fimax4(size_t nspot, mkiv_reflex spot[], int step, float range,
           const mkiv_image *image)
{
  register int h, v, sum, adr0;  /* auxiliaries */
  int lowh, lowv,                /* lower limits of search area */
      high, higv;                /* upper limits of search area */
  long max_sum;                  /* integration sum, total maximum of all sum */
  float h0, v0;                  /* calculated spot positions */

  /* the kernel that serves for a weighted integration [sum(kernel)=112] */
  int KL_H_SIZE = 3;          /*  half size of kernel */
  size_t nkernel = 7*7;
  int ad[7*7];
  static long kernel[7*7] = {  0, 1, 1, 1, 1, 1, 0,
                               1, 1, 2, 2, 2, 1, 1,
                               1, 2, 4, 8, 4, 2, 1,
                               1, 2, 8,16, 8, 2, 1,
                               1, 2, 4, 8, 4, 2, 1,
                               1, 1, 2, 2, 2, 1, 1,
                               0, 1, 1, 1, 1, 1, 0 };

  register size_t i, k = 0, cols = image->rows;
  size_t rows = image->cols;
  unsigned short *im = (unsigned short *)image->imagedata;
     
  for (v = -KL_H_SIZE; v <= KL_H_SIZE; v++)
  {
    for (h = -KL_H_SIZE; h <= KL_H_SIZE; h++)
    {
      ad[k++] = v*cols + h;
    }
  }

  for (i=0; i<nspot; i++)
  {
    if( spot[i].control & SPOT_GOOD_S2N ) continue;
    h0 = spot[i].xx;
    v0 = spot[i].yy;

    /* find boundaries */
    lowh = MAX( (int)(h0 - range), KL_H_SIZE );
    high = MIN( (int)(h0 + range), cols - KL_H_SIZE - 1 );

    lowv = MAX( (int)(v0 - range), KL_H_SIZE );
    higv = MIN( (int)(v0 + range), rows - KL_H_SIZE - 1 );

    /* loop over searching area */
    /* v and h point to the center of the kernel = center of integration area */
    max_sum = 0L;
    for (v = lowv; v <= higv; v+=step)
    {
      for (h = lowh; h <= high; h+=step)
      {
        if ( PYTH(v-v0, h-h0) > range ) continue; /* spot out of range */
        adr0 = v*cols + h;
        sum = 0L;
        for (k=0; k < nkernel; k++) sum += kernel[k] * im[adr0 + ad[k]];

        /* store coordinates when intensity is max */
        if (sum > max_sum)
        {
          spot[i].yy = (float)v;
          spot[i].xx = (float)h;
          max_sum = sum;
        }

      }       /* for(h = ...) */

    }       /* for(v = ...) */

  }       /* for(i < nspot) */
    
} /* end of function fimax4 */
