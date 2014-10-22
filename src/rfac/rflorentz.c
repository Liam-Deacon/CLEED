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
 *  \brief Implementation for rf_lorentz() function.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "rfac.h"          /* specific definitions etc. */

#define  EPSILON  0.001    /*!< determines the integration range for smooth */

/*!
 * \fn rf_lorentz
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
int rf_lorentz(rfac_ivcur *iv_cur, real vi, const char *ctr)
{
  int i, i_hi, i_lo;
  int i_sum;
  int n_range;

  real faux;
  real e_step;
  real norm_sum;             /* norm_sum is the sum over the prefactors
			                        * of all summands in the Lorentz sum. I.e.
			                        * dividing the sum by this number leads to
			                        * normalisation */

  real *intbuf;             /* this array is used to store original
                             * intensity values */
  real *prefac;

  #ifdef WRITE
  FILE *str_old, *str_new;
  #endif

  /* First check if vi is nonzero */
  if (vi < ENG_TOLERANCE)
  {
    #ifdef ERROR
    fprintf(STDERR,"*** error (cr_lorentz): Vi is too small\n");
    #endif

    return(RFAC_VI_TOO_SMALL);
  }

  /* Sort IV curve if not yet done */
  if( (!iv_cur->theory->sort) || (!iv_cur->experimental->sort) )
  {
    rf_sort(iv_cur);
  }

  switch (ctr[0])
  {
    /* smooth theoretical IV curve */
    case ('t'):
    {

      #ifdef WRITE /* open control output if required */
      str_old = fopen("rflorentz.old.the", "w");
      str_new = fopen("rflorentz.new.the", "w");
      #endif

      if (iv_cur->theory->equidist)
      {
        e_step = iv_cur->theory->data[1].energy -
                                  iv_cur->theory->data[0].energy;

        if (e_step < ENG_TOLERANCE)
        {

          #ifdef ERROR
          fprintf(STDERR,"*** error (cr_lorentz): e_step is too small\n");
          #endif

          return(RFAC_ESTEP_TOO_SMALL);
        }

        /* Find energy range for integral */
        faux = vi * R_sqrt(1./EPSILON - 1.);
        n_range = (int) R_nint(faux / e_step);

        #ifdef CONTROL
        fprintf(STDCTR,"(cr_lorentz): (the) e_step: %.1f n_range: %d\n", 
                e_step, n_range);
        #endif

        if( (prefac  = (real *)malloc(n_range * sizeof(real)) ) == NULL)
        {

          #ifdef ERROR
          fprintf(STDERR, "*** error (cr_lorentz): allocation error (prefac)\n");
          #endif
        exit(RFAC_ALLOCATION_ERROR);
      }

      if( (intbuf = (real *) malloc(
                        iv_cur->theory->n_eng * sizeof(real)) ) == NULL)
      {

        #ifdef ERROR
        fprintf(STDERR,"*** error (cr_lorentz): allocation error (intbuf)\n");
        #endif

        exit(RFAC_ALLOCATION_ERROR);
      }

      for (i = 0; i < n_range; i ++)
      {
        prefac[i]  = e_step * vi / ( SQUARE(e_step*i) + SQUARE(vi) );
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
        }  /* upper branch */

        /* lower branch: */
        i_lo = MAX(i - n_range + 1, 0);
        for (i_sum = i_lo; i_sum < i; i_sum++)
        {
          iv_cur->theory->data[i].intens +=
             intbuf[i_sum] * prefac[i-i_sum];
          norm_sum += prefac[i-i_sum];
        }  /* lower branch */

        /* normalize intensity */
        iv_cur->theory->data[i].intens /= norm_sum;

        #ifdef WRITE
        fprintf(str_old,"%f %f\n", iv_cur->the_list[i].energy,
                intbuf[i] );
        fprintf(str_new,"%f %f\n", iv_cur->the_list[i].energy,
                iv_cur->the_list[i].intens);
        #endif
      }   /* energy scan */

      /* set smooth flag */
      iv_cur->theory->smooth = SM_LORENTZ;

      free(prefac);
      free(intbuf);
    }    /* if equidistant */
    else
    {

      #ifdef ERROR
      fprintf(STDERR, "*** error (cr_lorentz): "
              "theoretical IV curve is not equidistant");
      #endif
      return(RFAC_IV_NOT_EQUIDISTANT);
    }

    break;

  }     /* end: case 't' */


  /* smooth experimental IV curve */
  case ('e'):
  {

    #ifdef WRITE /* open control output if required */
    str_old = fopen("rflorentz.old.exp", "w");
    str_new = fopen("rflorentz.new.exp", "w");
    #endif


    /*if (iv_cur->exp_equidist) */
    if (1) /*!FIXME: */
    {
      e_step = iv_cur->experimental->data[1].energy -
               iv_cur->experimental->data[0].energy;
      if (e_step < ENG_TOLERANCE)
      {

        #ifdef ERROR
        fprintf(STDERR,"*** error (cr_lorentz): e_step is too small\n");
        #endif

        return(RFAC_ESTEP_TOO_SMALL);
      }

      /* Find energy range for integral */
      faux = vi * R_sqrt(1./EPSILON - 1.);
      n_range = (int) R_nint(faux / e_step);

      #ifdef CONTROL
      fprintf(STDCTR,"(cr_lorentz): (exp) e_step: %.1f n_range: %d\n",
              e_step, n_range);
      #endif

      if( (prefac  = (real *)malloc(n_range * sizeof(real)) ) == NULL)
      {
        #ifdef ERROR
        fprintf(STDERR, "*** error (cr_lorentz): allocation error (prefac)\n");
        #endif
        exit(RFAC_ALLOCATION_ERROR);
      }

      if( (intbuf = (real *) malloc(
                    iv_cur->experimental->n_eng * sizeof(real)) ) == NULL)
      {
        #ifdef ERROR
        fprintf(STDERR, "*** error (cr_lorentz): allocation error (intbuf)\n");
        #endif
        exit(RFAC_ALLOCATION_ERROR);
      }

      for (i = 0; i < n_range; i ++)
      {
        prefac[i]  = e_step * vi / ( SQUARE(e_step*i) + SQUARE(vi) );
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
        }  /* upper branch */

        /* lower branch: */
        i_lo = MAX(i-n_range+1, 0);
        for (i_sum = i_lo; i_sum < i; i_sum++)
        {
          iv_cur->experimental->data[i].intens += intbuf[i] * prefac[i-i_sum];
          norm_sum += prefac[i-i_sum];
        }  /* lower branch */

        /* normalize intensity */
        iv_cur->experimental->data[i].intens /= norm_sum;

        #ifdef WRITE
        fprintf(str_old,"%f %f\n", iv_cur->exp_list[i].energy, 
                intbuf[i] );
        fprintf(str_new,"%f %f\n", iv_cur->exp_list[i].energy, 
                iv_cur->exp_list[i].intens);
        #endif
     }   /* energy scan */

      /* set smooth flag */
      iv_cur->experimental->smooth = SM_LORENTZ;

      free(prefac);
      free(intbuf);
    }    /* if equidistant */
    else
    {
      #ifdef ERROR
      fprintf(STDERR, "*** error (cr_lorentz):"
              " expt. IV curve is not equidistant");
      #endif
      return(RFAC_IV_NOT_EQUIDISTANT);
    }
    break;

    }     /* case 'e' */

  }      /* switch */

  #ifdef WRITE /* print control output if required */
  fflush(str_old);
  close(str_old);
  fflush(str_new);
  close(str_new);
  #endif /* WRITE */

  return (RFAC_SUCCESS);
}
