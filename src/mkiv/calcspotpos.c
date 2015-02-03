/*********************************************************************
 *                        mkiv_calc_spot_positions.C
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
 *   SU/ 29.04.91
 *   GH/ 20.08.92
 *   CS/ 3.8.93
 *********************************************************************/

#include "mkiv.h"
#include <stdlib.h>

/*!
 * Calculates the positions of all reciprocal lattice points within a
 * reasonable frame and makes a list out of them.
 *
 * \param[in] a pointer to list of origin, as well as \f$ a_1 \f$ and
 * \f$ a_2 \f$ basis vectors.
 * \param n_dom number of domains
 * \param superl Array of domains containng the inverse superlattice
 * matrix elements.
 * \param spot_max
 * \param[out] nspot number of measurable reflexes.
 * \param[out] spot Pointer to array of measurable reflexes. The #mkiv_reflex
 * struct contains the following elements:
 * - spot->kp_len    length of k parallel to surface
 * - spot->lind1/2   lattice indices
 * - spot->x0,y0     calculated spot position
 * - spot->xx,yy     spot position that will be modified by fimax() , mkiv_calc_spot_disc()
 * - spot->cos_th    cosine of k towards e-beam
 * \param n_excl number of excluded reflexes.
 * \param excl Pointer to array of excluded reflexes.
 * \param[in] imask Pointer to matrix of image data containing mask of
 * screen boundaries.
 * \param r_outer Outer radius of screen in pixels.
 * \param r_inner Inner radius of screen in pixels.
 * \param[out] radius Calculated screen radius in pixels.
 * \param energy Beam energy in eV.
 * \param ratio Ratio of screen radius to distance.
 * \param k_10 \f$ k_{//} \f$ value of the (1,0) beam.
 *  * \return
 */
int mkiv_calc_spot_positions(mkiv_vector a[], size_t n_dom, mkiv_domain superl[],
                size_t spot_max, size_t *nspot, mkiv_reflex spot[],
                size_t n_excl, mkiv_index excl[],
                mkiv_image *imask, double r_outer, double r_inner, double *radius,
                double energy, double ratio, double k_10)
{
  register int i, j, pos;
  register size_t ui, uk, k, i_dom;
  int maxorder, orderb;        /* define frame                       */
  double min, len, rposx, rposy;
  double itok;                  /* ratio k/pixel                      */
  double sin_th, cos_th;        /* sine and cosine of theta */
  double cor_fac;               /* correction factor */
  unsigned short *im = (unsigned short*)imask->imagedata;
  size_t cols = imask->rows;
  size_t rows = imask->cols;

  /* some precalculations */
  itok = k_10 / a[1].len;                /* factor pixel to k-value */
  sin_th = k_10 * H2M / sqrt(energy);    /* sine of (1,0)-mkiv_reflex */
  *radius = a[1].len / sin_th;           /* screen-radius (constant) */
  min = MIN( a[1].len , a[2].len );      /* MIN(a1,a2) */
  orderb = int_roundup(*radius / min);         /* maximum integral spot index */
 
  /* compute reciprocal lattice points */
  rposx = rposy = 0.;

  k = 0;
  for (i_dom = 0; i_dom <= n_dom; i_dom++)
  {
    /* max order of substrate reflexes within radius */
    if ( i_dom == n_dom ) maxorder = orderb;
    else /* max order of superstructure reflexes within radius */
    {
      min = MIN( PYTH(superl[i_dom].lind11, superl[i_dom].lind12),
		             PYTH(superl[i_dom].lind21, superl[i_dom].lind22) );
      maxorder = int_roundup(orderb / min);
    }

    for (i = -maxorder; i <= maxorder; i++)
    {
      for (j = -maxorder; j <= maxorder; j++)
      {
        if (i_dom == n_dom)           /* substrate lattice */
	      {
          spot[k].lind1 = (double)i;
	        spot[k].lind2 = (double)j;
	        rposx = a[1].xx *i + a[2].xx *j;
	        rposy = a[1].yy *i + a[2].yy *j;
        }
	      if (i_dom < n_dom)            /* superstructure lattice */
	      {
	         spot[k].lind1 = i*superl[i_dom].lind11 + j*superl[i_dom].lind21;
	         spot[k].lind2 = i*superl[i_dom].lind12 + j*superl[i_dom].lind22;
           rposx = a[1].xx*spot[k].lind1 + a[2].xx*spot[k].lind2;
           rposy = a[1].yy*spot[k].lind1 + a[2].yy*spot[k].lind2;
        }
	      sin_th = PYTH(rposx, rposy) / (*radius);
        if (sin_th > 1.) continue;

        /* compute correction factor for the mkiv_reflex position */
        cos_th = sqrt(1. - SQUARE(sin_th));
        cor_fac = 1. + ratio * (1. - cos_th);
        rposx /= cor_fac;
        rposy /= cor_fac;

        /* store calculated coordinates */
	      spot[k].xx = spot[k].x0 = a[0].xx + rposx;
	      spot[k].yy = spot[k].y0 = a[0].yy + rposy;

	      /* when spot position is out of the visible LEED-screen area -> continue */
	      if ( spot[k].y0 < 0 || spot[k].y0 >= rows ) continue;
	      if ( spot[k].x0 < 0 || spot[k].x0 >= cols ) continue;
	      pos = (int)spot[k].y0 * (int)cols + (int)spot[k].x0;
	      if ( imask && !im[pos] ) continue;
	      len = PYTH(rposx, rposy);
	      if ( !imask && (len < r_inner || len > r_outer) ) continue;

	      /* store kp_len and cos_th */
	      spot[k].kp_len = itok * len;
	      spot[k].cos_th = cos_th;
	      if (k < spot_max) k++;
      } /* for j */
    } /* for i */
  }
  if ( (*nspot=k) >= spot_max)
	{
    ERROR_MSG("Max. No. of spots was exceeded\n");
    ERROR_RETURN(-1);
	}
  /* remove all reflexes which appear twice or more */
  for (uk=0; uk < *nspot-1; uk++)
  {
    for (ui=uk+1; ui < *nspot; ui++)
    {
      if ( PYTH(spot[uk].lind1-spot[ui].lind1,
                spot[uk].lind2-spot[ui].lind2 ) < TOLERANCE)
	    {
        spot[ui--] = spot[--*nspot];
	    }
    }
  }

  /* remove all reflexes that appear in the list excl */
  for (uk=0; uk < *nspot; uk++)
  {
    for (ui=0; ui < n_excl; ui++)
    {
      if ( PYTH(spot[uk].lind1-excl[ui].lind1,
		            spot[uk].lind2-excl[ui].lind2 ) < TOLERANCE)
      {
        spot[uk--] = spot[--*nspot];
      }
    }
  }
          
  return(0);
}
