/*********************************************************************
 *                       RFSPLINE.C
 *
 *  Copyright 1992-2014 Georg Held
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1995.08.11 - Creation (copy from Numerical Recipes spline.c )
 ********************************************************************/

/*! \file
 *
 * Implementation file for rfac_iv_spline() function.
 */

#include <malloc.h>
#include "rfac.h"

/*!
 * Performs the first stage of a cubic spline.
 *
 * \param[in,out] *list list of energy/intensity values to be
 * interpolated by cubic spline.
 * The function will generate the structure elements deriv2, which will
 * be used for interpolation in function rfac_splint() .
 *
 * For a description see Numerical Recipes in C Chapter 3.3.
 * The boundary conditions are set for a natural spline
 * (zero second derivative on both boundaries).
 *
 */
void rfac_iv_spline(rfac_iv *iv)
{
  size_t i;
  int k;
  real p, qn, sig, un;
  real *buf;

  buf = (real *) malloc(iv->n_eng * sizeof(real));

  iv->data[0].deriv2 = buf[0] = 0.0;

  for (i=1; i < iv->n_eng-1; i++)
  {
    sig = ( iv->data[i].energy - iv->data[i-1].energy )/
          ( iv->data[i+1].energy - iv->data[i-1].energy);
    p = (real)(sig * iv->data[i-1].deriv2 + 2.0);
    iv->data[i].deriv2 = (real)((sig-1.0)/p);
    buf[i] = ( iv->data[i+1].intens - iv->data[i].intens )/
             ( iv->data[i+1].energy - iv->data[i].energy ) -
             ( iv->data[i].intens - iv->data[i-1].intens )/
             ( iv->data[i].energy - iv->data[i-1].energy );
    buf[i] = (real)(( 6.0*buf[i] / ( iv->data[i+1].energy -
                      iv->data[i-1].energy ) - sig*buf[i-1])/p);
  }

  qn = un = 0.0;

  iv->data[iv->n_eng-1].deriv2 = (real)((un - qn*buf[iv->n_eng-2] ) /
                         ( qn*iv->data[iv->n_eng-2].deriv2 + 1.0));
  for (k=(int)(iv->n_eng-2); k>=0; k--)
  {
    iv->data[k].deriv2 = iv->data[k].deriv2 * iv->data[k+1].deriv2 + buf[k];
  }

  free(buf);

  iv->spline = true; /* update spline flag */

}  /* end of function cr_spline */
