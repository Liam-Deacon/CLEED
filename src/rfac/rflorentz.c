/*********************************************************************
 *                       RFLORENTZ.C
 *
 *  Copyright 1992-2014 Georg Held
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1992.10.05 - creation.
 ********************************************************************/

/*! \file
 *  \brief Implementation for rfac_lorentz() function.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "rfac.h"          /* specific definitions etc. */

static const double EPSILON = 0.001; /*!< determines the integration range for smooth */

/*!
 * \brief Perform Lorentzian smoothing on IV curve.
 *
 * Do a Lorentzian smooth for experimental or theoretical IV curve
 * (equidistant or not):
 *
 *   \f$I(E') = c * S { I(E')* vi dE /[(E-E')^2 + vi^2] }$\f
 *
 *   where \f$c = 1/ S { vi dE /[(E-E')^2 + vi^2] }.
 *
 * If the IV curve is not sorted yet (iv_cur->the/exp_sort = 0), the
 * routine cr_(t)sort will be called to perform sorting.
 *
 * \param iv Data structure containing all essential data
 * such as theoretical IV curves. The theoretical IV curves
 * will be modified after return.
 *
 * \param vi Width of Lorentzian.
 *
 * \retval 0 if successful.
 *
 */
int rfac_iv_lorentz_smooth(rfac_iv *iv, real vi)
{
  size_t i, i_hi, i_lo;
  size_t i_sum;
  size_t n_range;

  real faux;
  real e_step;
  real norm_sum;             /* norm_sum is the sum over the prefactors
                              * of all summands in the Lorentz sum. I.e.
                              * dividing the sum by this number leads to
                              * normalisation */

  real intbuf[iv->n_eng+1]; /* this array is used to store original
                             * intensity values */
  real prefac[iv->n_eng+1];

  /* First check if vi is nonzero */
  if (vi < ENG_TOLERANCE)
  {
    ERROR_MSG("Vi is too small\n");
    return(RFAC_VI_TOO_SMALL);
  }

  /* Sort IV curve if not yet done */
  if( !iv->sort ) {
    rfac_iv_sort(iv);
  }

  /* smooth IV curve */
  if (iv->equidist)
  {
    e_step = iv->data[1].energy - iv->data[0].energy;

    if (e_step < ENG_TOLERANCE)
    {
      ERROR_MSG("e_step is too small\n");
      return(RFAC_ESTEP_TOO_SMALL);
    }

    /* Find energy range for integral */
    faux = vi * sqrt(1./EPSILON - 1.);
    n_range = (size_t)(faux / e_step);

    CONTROL_MSG(CONTROL, "e_step: %.1f n_range: %d\n", e_step, n_range);

    for (i = 0; i < n_range; i ++)
    {
      prefac[i]  = e_step * vi / ( SQUARE(e_step*(real)i) + SQUARE(vi) );
    }

    /* scan over all energies */
    for (i = 0; i < iv->n_eng; i++ )
    {
      /*
       * store original intensities and calculate
       * first element of integral sum
       */
      intbuf[i] = iv->data[i].intens;
      iv->data[i].intens *= prefac[0];
      norm_sum = prefac[0];

      /* upper branch: */
      i_hi = MIN(i+n_range, iv->n_eng);
      for (i_sum = i+1; i_sum < i_hi; i_sum++)
      {
        iv->data[i].intens += iv->data[i_sum].intens * prefac[i_sum-i];
        norm_sum += prefac[i_sum-i];
      } /* upper branch */

      /* lower branch: */
      i_lo = MAX(i - n_range + 1, (size_t)0);
      for (i_sum = i_lo; i_sum < i; i_sum++)
      {
        iv->data[i].intens += intbuf[i_sum] * prefac[i-i_sum];
        norm_sum += prefac[i-i_sum];
      } /* lower branch */

      /* normalize intensity */
      iv->data[i].intens /= norm_sum;

    } /* energy scan */

    /* set smooth flag */
    iv->smooth = SM_LORENTZ;

  } /* if equidistant */
  else
  {
    ERROR_MSG("IV curve is not equidistant\n");
    return(RFAC_IV_NOT_EQUIDISTANT);
  }

  return (RFAC_SUCCESS);
}
