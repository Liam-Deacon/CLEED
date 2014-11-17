/*********************************************************************
 *                        CALCOI.C
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
 *   CS/09.08.93 - creation.
 *********************************************************************/

/*! \file
 *
 */

#include "mkiv.h"

/*!
 * Recalculates the center of each spot using the center of gravity method
 * within the disc (center = position found by fimax() and radius = \p range ).
 *
 * If spot[i].control - bit #SPOT_GOOD_S2N is set, the calculation is suppressed.
 *
 * \param nspot number of spots in array \p spot
 * \param[in,out] spot pointer to array of spots containing all measurable
 * reflexes.
 * \param range radius of area for computation.
 * \param[in] image pointer to image data as an internal matrix structure.
 *  * \return
 *
 * \see fimax()
 */
int calcoi(size_t nspot, mkiv_reflex spot[], float range, mkiv_image *image)
{
  register size_t i;
  register int v, h, val, pos;       /* auxiliaries */
  register int hsum, vsum, valsum;
  register int v0, h0;
  int lowh, high, lowv, higv;           /* boundaries of area */

  unsigned short *im = (unsigned short *)image->imagedata;
  size_t cols = image->rows;
  size_t rows = image->cols;

  /* loop over all apots in list spot */

  for( i=0; i < nspot; i++)
  {
    if( spot[i].control & SPOT_GOOD_S2N ) continue;

    h0 = (int)spot[i].xx;
    v0 = (int)spot[i].yy;

    /* find boundaries */
    lowh = MAX( h0 - (int)range, 0 );
    high = MIN( h0 + (int)range, (int)cols-1 );

    lowv = MAX( v0 - (int)range, 0 );
    higv = MIN( v0 + (int)range, rows-1 );

    /* loop over integration area rectangle */
    hsum = 0;
    vsum = 0;
    valsum = 1;

    for( v=lowv; v<=higv; v++)
    {
      for( h=lowh; h<=high; h++)
      {
        pos = v*cols + h;
        if ( pos >= cols*rows || pos < 0 ) continue; /* out of frame */
        val = im[pos];
        hsum += val * (h - h0);
        vsum += val * (v - v0);
        valsum += val;
      }
    }

    /* calculate the center of gravity according to the formula :
	   * mean(x) = sum(val * x) / sum(val)
	   */
    spot[i].xx = h0 + (float)hsum/(float)valsum;
    spot[i].yy = v0 + (float)vsum/(float)valsum;
  }

  return(0);
}
