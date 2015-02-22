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
 * \fn rfac_lorentz
 *
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
 * \param iv_cur Data structure containing all essential data
 * such as theoretical IV curves. The theoretical IV curves
 * will be modified after return.
 *
 * \param vi Width of Lorentzian.
 *
 * \param ctr Either "t" to smooth theoretical data or
 * "e" to smooth experimental data.
 *
 * \retval RETURN_SUCCESS if successful.
 * \retval RETURN_FAIL if failed.
 *
 */
int rfac_lorentz(rfac_ivcur *iv_cur, real vi, const char *ctr)
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

  real *intbuf;             /* this array is used to store original
                             * intensity values */
  real *prefac;

  /* First check if vi is nonzero */
  if (vi < ENG_TOLERANCE)
  {
    ERROR_MSG("Vi is too small\n");
    return(RFAC_VI_TOO_SMALL);
  }

  /* Sort IV curve if not yet done */
  if( !iv_cur->theory->sort )
    rfac_iv_sort(iv_cur->theory);
  if ( !iv_cur->experimental->sort)
    rfac_iv_sort(iv_cur->experimental);

  switch (ctr[0])
  {
    /* smooth theoretical IV curve */
    case ('t'):
    {
      if (iv_cur->theory->equidist)
      {
        e_step = iv_cur->theory->data[1].energy -
                                  iv_cur->theory->data[0].energy;

        if (e_step < ENG_TOLERANCE)
        {
          ERROR_MSG("e_step is too small\n");
          return(RFAC_ESTEP_TOO_SMALL);
        }

        /* Find energy range for integral */
        faux = vi * cleed_real_sqrt(1./EPSILON - 1.);
        n_range = (size_t)cleed_real_nint(faux / e_step);

        CONTROL_MSG(CONTROL, "[theory] e_step: %.1f n_range: %d\n",
                    e_step, n_range);

        if( (prefac  = (real *)malloc((size_t)n_range * sizeof(real)) ) == NULL)
        {

          ERROR_MSG("allocation error (prefac)\n");
          exit(RFAC_ALLOCATION_ERROR);
        }

      if( (intbuf = (real *) malloc(
                        iv_cur->theory->n_eng * sizeof(real)) ) == NULL)
      {
        ERROR_MSG("allocation error (intbuf)\n");
        exit(RFAC_ALLOCATION_ERROR);
      }

      for (i = 0; i < n_range; i ++)
      {
        prefac[i]  = e_step * vi / ( SQUARE(e_step*(real)i) + SQUARE(vi) );
      }


      /* scan over all energies */
      for (i = 0; i < iv_cur->theory->n_eng; i++ )
      {
        /*
         * store original intensities and calculate
         * first element of integral sum
         */
        intbuf[i] = iv_cur->theory->data[i].intens;
        iv_cur->theory->data[i].intens *= prefac[0];
        norm_sum = prefac[0];

        /* upper branch: */
        i_hi = MIN(i+n_range, iv_cur->theory->n_eng);
        for (i_sum = i+1; i_sum < i_hi; i_sum++)
        {
          iv_cur->theory->data[i].intens +=
             iv_cur->theory->data[i_sum].intens * prefac[i_sum-i];
          norm_sum += prefac[i_sum-i];
        } /* upper branch */

        /* lower branch: */
        i_lo = MAX(i - n_range + 1, (size_t)0);
        for (i_sum = i_lo; i_sum < i; i_sum++)
        {
          iv_cur->theory->data[i].intens +=
             intbuf[i_sum] * prefac[i-i_sum];
          norm_sum += prefac[i-i_sum];
        } /* lower branch */

        /* normalize intensity */
        iv_cur->theory->data[i].intens /= norm_sum;

      } /* energy scan */

      /* set smooth flag */
      iv_cur->theory->smooth = SM_LORENTZ;

      free(prefac);
      free(intbuf);
    } /* if equidistant */
    else
    {
      ERROR_MSG("theoretical IV curve is not equidistant\n");
      return(RFAC_IV_NOT_EQUIDISTANT);
    }

    break;

  } /* end: case 't' */


  /* smooth experimental IV curve */
  case ('e'):
  {
    /*if (iv_cur->exp_equidist) */
    if (1) /*!FIXME: */
    {
      e_step = iv_cur->experimental->data[1].energy -
               iv_cur->experimental->data[0].energy;
      if (e_step < ENG_TOLERANCE)
      {
        ERROR_MSG("e_step is too small\n");
        return(RFAC_ESTEP_TOO_SMALL);
      }

      /* Find energy range for integral */
      faux = vi * cleed_real_sqrt(1./EPSILON - 1.);
      n_range = (size_t)cleed_real_nint(faux / e_step);

      CONTROL_MSG(CONTROL, "[expt] e_step: %.1f n_range: %d\n", e_step, n_range);

      if( (prefac  = (real *)malloc((size_t)n_range * sizeof(real)) ) == NULL)
      {
        ERROR_MSG("allocation error (prefac)\n");
        exit(RFAC_ALLOCATION_ERROR);
      }

      if( (intbuf = (real *) malloc(
                    iv_cur->experimental->n_eng * sizeof(real)) ) == NULL)
      {
        ERROR_MSG("allocation error (intbuf)\n");
        exit(RFAC_ALLOCATION_ERROR);
      }

      for (i = 0; i < n_range; i ++)
      {
        prefac[i]  = (e_step * vi / ( SQUARE(e_step*(real)i) + SQUARE(vi) ));
      }

      /* scan over all energies */
      for (i = 0; i< iv_cur->experimental->n_eng; i++ )
      {

        /*
         * store original intensities and calculate
         * first element of integral sum
         */
        intbuf[i] = iv_cur->experimental->data[i].intens;
        iv_cur->experimental->data[i].intens *= prefac[0];
        norm_sum = prefac[0];

        /* upper branch: */
        i_hi = MIN(i+n_range, iv_cur->experimental->n_eng);
        for (i_sum = i+1; i_sum < i_hi; i_sum++)
        {
          iv_cur->experimental->data[i].intens +=
              iv_cur->experimental->data[i_sum].intens * prefac[i_sum-i];
          norm_sum += prefac[i_sum-i];
        } /* upper branch */

        /* lower branch: */
        i_lo = MAX(i-n_range+1, (size_t)0);
        for (i_sum = i_lo; i_sum < i; i_sum++)
        {
          iv_cur->experimental->data[i].intens += intbuf[i] * prefac[i-i_sum];
          norm_sum += prefac[i-i_sum];
        } /* lower branch */

        /* normalize intensity */
        iv_cur->experimental->data[i].intens /= norm_sum;

      } /* energy scan */

      /* set smooth flag */
      iv_cur->experimental->smooth = SM_LORENTZ;

      free(prefac);
      free(intbuf);
    } /* if equidistant */
    else
    {
      ERROR_MSG("experimental IV curve is not equidistant\n");
      return(RFAC_IV_NOT_EQUIDISTANT);
    }
    break;

    } /* case 'e' */

  } /* switch */

  return (RFAC_SUCCESS);
}

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

  real *intbuf;             /* this array is used to store original
                             * intensity values */
  real *prefac;

  /* First check if vi is nonzero */
  if (vi < ENG_TOLERANCE)
  {
    ERROR_MSG("Vi is too small\n");
    return(RFAC_VI_TOO_SMALL);
  }

  /* Sort IV curve if not yet done */
  if( !iv->sort )
    rfac_iv_sort(iv);

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
    faux = vi * cleed_real_sqrt(1./EPSILON - 1.);
    n_range = (size_t)cleed_real_nint(faux / e_step);

    CONTROL_MSG(CONTROL, "e_step: %.1f n_range: %d\n", e_step, n_range);

    if( (prefac  = (real *)malloc((size_t)n_range * sizeof(real)) ) == NULL)
    {
      ERROR_MSG("allocation error (prefac)\n");
      exit(RFAC_ALLOCATION_ERROR);
    }

    if( ( intbuf = (real *) malloc(iv->n_eng * sizeof(real)) ) == NULL)
    {
      free(prefac);
      ERROR_MSG("allocation error (intbuf)\n");
      exit(RFAC_ALLOCATION_ERROR);
    }

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

    free(prefac);
    free(intbuf);
  } /* if equidistant */
  else
  {
    ERROR_MSG("IV curve is not equidistant\n");
    return(RFAC_IV_NOT_EQUIDISTANT);
  }

  return (RFAC_SUCCESS);
}
