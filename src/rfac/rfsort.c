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
 *  \brief C implementation of rfac_sort() function.
 *
 * Sort IV curves according to the energy and check if the
 * energy values are equidistant.
 */

#include <stdio.h>
#include <math.h>

#include "rfac.h"          /* specific definitions etc. */

/*!
 * \brief Sorts IV curve in increasing energy order.
 *
 * Sorts an experimental or theoretical IV curve according to its
 * energy and check if the energy values are equidistant.
 *
 * \param iv data structure containing all essential IV data. The curve will be
 * modified after return.
 *
 * \retval #RFAC_SUCCESS if successful.
 * \retval #RFAC_FAILURE if failed.
 *
 * \todo implement \ref RFAC_FAILURE.
 */
int rfac_iv_sort(rfac_iv *iv)
{
  size_t i, j;
  real f_aux;

  /* sort data according to energy values */
  for (i = 0; i< iv->n_eng-1; i++)
  {
    for (j = i+1; j< iv->n_eng; j++)
    {
      if(iv->data[j].energy < iv->data[i].energy)
      {
        /* exchange energy values */
        f_aux = iv->data[i].energy;
        iv->data[i].energy = iv->data[j].energy;
        iv->data[j].energy = f_aux;

        /* exchange intensities */
        f_aux = iv->data[i].intens;
        iv->data[i].intens = iv->data[j].intens;
        iv->data[j].intens = f_aux;
      }

    } /* for j */

  } /* for i */

  /* check equidistance */
  for (i = 1, iv->equidist = 1; i< iv->n_eng-1; i++)
  {
    if (R_fabs ((2*iv->data[i].energy -
        iv->data[i+1].energy - iv->data[i-1].energy) )
      >  ENG_TOLERANCE )
    {
      iv->equidist = false;
    }
  }

  iv->first_eng = iv->data[0].energy;
  iv->last_eng =
      iv->data[iv->n_eng - 1].energy;
  iv->sort = true;

  return (RFAC_SUCCESS);
}

