/*********************************************************************
 *                      LMSYMAT.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/15.08.94 - creation
 *********************************************************************/

/*! \file
 *
 * Creates transformation matrix from angular momentum space into k-space.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "leed.h"

static mat Ylm = NULL;

/*!
 * Creates the transformation matrix Ymat from angular momentum space into
 * k-space: Ylm(k).
 *
 *   The order of Ymat is:
 *
 *  k / (l,m) (0,0)     (-1,1)    (0,1)   (1,1)   (-2,2) ....
 *
 *  k1       Y00(k1)   Y-11(k1)  Y01(k1)
 *  k2       Y00(k2)   Y-11(k2)  Y01(k2)
 *  k3       Y00(k3)
 *  ...                                     Y(l,m)(ki)
 *
 * \param[out] Ymat Transformation matrix.
 * \param l_max Maximum angular momentum quantum number spanning the (l,m)-space
 * \param[in] beams Pointer to array of #leed_beams spanning the k-space.
 * \param n_beams Number of beams in \p beams array.
 * \return transformation matrix (may be different from \p Ymat input)
 * \retval \c NULL if failed and #EXIT_ON_ERROR not defined.
 */
mat leed_ms_ymat(mat Ymat, size_t l_max, const leed_beam *beams, size_t n_beams)
{
  size_t ll_max = (l_max+1)*(l_max+1);  /* total number of (l,m) pairs */
  size_t off;                           /* offset in the arrays Ylm and Llm */
  size_t size;
  size_t i_beams;

  /* Check arguments: */

  /* Allocate memory for Ymat */
  Ymat = matalloc( Ymat, n_beams, ll_max, NUM_COMPLEX );

  /* Loop over k's:
   * Calculate the sperical harmonics Ylm(k) and copy into Ymat
   */
  size = ll_max*sizeof(real);

  for (i_beams = 0, off = 1; i_beams < n_beams; i_beams ++, off += ll_max)
  {

    CONTROL_MSG(CONTROL, "cos(th): (%.3f %.3f), phi: %.3f\n",
             (beams+i_beams)->cth_r,
             (beams+i_beams)->cth_i, (beams+i_beams)->phi);

    Ylm = c_ylm(Ylm, (beams+i_beams)->cth_r, (beams+i_beams)->cth_i,
                     (beams+i_beams)->phi, l_max);
    memcpy( Ymat->rel+off, Ylm->rel+1, size);
    memcpy( Ymat->iel+off, Ylm->iel+1, size);
  }

  return(Ymat);
}
