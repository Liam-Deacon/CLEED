/*********************************************************************
 *                           RFSORT.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1992.10.02 - Creation
 *   GH/1995.08.30 - Adaption to CRFAC
 *********************************************************************/

/*! \file
 *  \brief C implementation of rf_sort() function.
 *
 * Sort IV curves according to the energy and check if the
 * energy values are equidistant.
 */

#include <stdio.h>
#include <math.h>

#include "rfac.h"          /* specific definitions etc. */

/*!
 * \brief sort IV curves in increasing energy order.
 *
 * Sort experimental and theoretical IV curves according to their
 * energy and check if the energy values are equidistant.
 *
 * \param iv_cur data structure containing all essential data
 * such as theoretical IV curves. The theoretical IV curves
 * will be modified after return.
 *
 * \retval #RFAC_SUCCESS if successful.
 * \retval #RFAC_FAILURE if failed.
 *
 * \todo implement \ref RFAC_FAILURE.
 */
int rf_sort(rfac_ivcur *iv_cur)
{
  size_t i, j;
  real f_aux;

/*********************************************************************
 * First: sort theoretical data according to energy values
 *********************************************************************/
  for (i = 0; i< iv_cur->theory->n_eng-1; i++)
  {
    for (j = i+1; j< iv_cur->theory->n_eng; j++)
    {
      if(iv_cur->theory->data[j].energy < iv_cur->theory->data[i].energy)
      {
        /* exchange energy values */
        f_aux = iv_cur->theory->data[i].energy;
        iv_cur->theory->data[i].energy = iv_cur->theory->data[j].energy;
        iv_cur->theory->data[j].energy = f_aux;

        /* exchange intensities */
        f_aux = iv_cur->theory->data[i].intens;
        iv_cur->theory->data[i].intens = iv_cur->theory->data[j].intens;
        iv_cur->theory->data[j].intens = f_aux;
      }

    } /* for j */

    #ifdef CONTROL
    fprintf(STDCTR, "(cr_sort): %.1f\n", iv_cur->theory->data[i].energy);
    #endif

  } /* for i */

  /* check equidistance */
  for (i = 1, iv_cur->theory->equidist = 1; i< iv_cur->theory->n_eng-1; i++)
  {
    if (R_fabs ((2*iv_cur->theory->data[i].energy -
	      iv_cur->theory->data[i+1].energy - iv_cur->theory->data[i-1].energy) )
      >  ENG_TOLERANCE ) 
    {
      iv_cur->theory->equidist = false;
    }
  }

  #ifdef CONTROL /* print control output if required */
  if(iv_cur->theory->equidist == true)
  {
    fprintf(STDCTR, "(cr_tsort): theor. IV curve is equidistant\n");
  }
  else
  {
    fprintf(STDCTR, "(cr_tsort): theor. IV curve is not equidistant\n");
  }
  #endif

  iv_cur->theory->first_eng = iv_cur->theory->data[0].energy;
  iv_cur->theory->last_eng =
  iv_cur->theory->data[iv_cur->theory->n_eng - 1].energy;
  iv_cur->theory->sort = true;

/*********************************************************************
 * Now: sort experimental data according to energy values
 *********************************************************************/

  for (i = 0; i< iv_cur->experimental->n_eng-1; i++)
  {
    for (j = i+1; j< iv_cur->experimental->n_eng; j++)
    {
      if(iv_cur->experimental->data[j].energy <
       iv_cur->experimental->data[i].energy)
      {
        /* exchange energy values */
        f_aux = iv_cur->experimental->data[i].energy;
        iv_cur->experimental->data[i].energy =
                  iv_cur->experimental->data[j].energy;
        iv_cur->experimental->data[j].energy = f_aux;

        /* exchange intensities */
        f_aux = iv_cur->experimental->data[i].intens;
        iv_cur->experimental->data[i].intens =
                  iv_cur->experimental->data[j].intens;
        iv_cur->experimental->data[j].intens = f_aux;

        fprintf(STDCTR, "(cr_tsort): %i - %i - %f\n", i, j, f_aux);
      }

    } /* for j */

    #ifdef CONTROL
    fprintf(STDCTR, "(cr_sort): %.1f\n", iv_cur->experimental->data[i].energy);
    #endif

  } /* for i */

  /* check equidistance */
  for (i = 1, iv_cur->experimental->equidist = true;
             i< iv_cur->experimental->n_eng-1; i++)
  {
    if (R_fabs ((2*iv_cur->experimental->data[i].energy -
	      iv_cur->experimental->data[i+1].energy -
	      iv_cur->experimental->data[i-1].energy) ) >  ENG_TOLERANCE )
    {
      iv_cur->experimental->equidist = false;
    }
  }

  #ifdef CONTROL /* print control output if required */
  if(iv_cur->experimental->equidist == true)
  {
    fprintf(STDCTR,"(cr_tsort): experimental. IV curve is equidistant\n");
  }
  else
  {
    fprintf(STDCTR,"(cr_tsort): experimental. IV curve is not equidistant\n");
  }
  #endif

  iv_cur->experimental->first_eng = iv_cur->experimental->data[0].energy;
  iv_cur->experimental->last_eng =
     iv_cur->experimental->data[iv_cur->experimental->n_eng - 1].energy;
  iv_cur->experimental->sort = 1;

  return (RFAC_SUCCESS);
}
