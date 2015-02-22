/*********************************************************************
 *                      LPCMKTL.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 * GH/20.08.94 - Creation
 * GH/18.07.95 - temperature dependent phase shifts.
 *********************************************************************/

/*! \file
 *
 * Implements leed_par_mktl() function for calculating atomic scattering
 * factors for a given energy.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <strings.h>

#include "leed.h"

/*!
 * Updates matrix \p p_tl containing the atomic scattering factors for all types
 * of atoms used in the calculation.
 *
 * \param[in,out] p_tl Array of scattering factor matrices. The function
 * returns its first argument. If \p p_tl is \c NULL then the structure will be
 * created and allocated memory.
 * \param[in] phs_shifts Pointer to phase shifts.
 * \param l_max Maximum linear angular momentum.
 * \param energy New energy (real part).
 * \return Pointer to the new set of scattering factor matrices.
 * \retval NULL if any error occurred and #EXIT_ON_ERROR is not defined.
 */
mat *leed_par_mktl(mat *p_tl, const leed_phase *phs_shifts,
                   size_t l_max, real energy)
{

  size_t l, l_set_1;
  size_t i_set, n_set;
  size_t i_eng;
  size_t iaux;

  real delta;
  real faux_r, faux_i;

  /* Search through list "phs_shifts".
   * - Find number of sets (n_set) and
   * - max. number of l (l_max: > input value if there is one larger
   *   set of phase shifts);
   * - call mkcg_coeff to ensure all C.G. coefficients are available.
   * - allocate p_tl.
   */
  for(n_set = 0; phs_shifts[n_set].lmax != I_END_OF_LIST; n_set ++)
  { ; }

  CONTROL_MSG(CONTROL, "energy = %.2f H, n_set = %d, l_max = %d\n",
                  energy /*HART*/, n_set, l_max);

  if(p_tl == NULL)
  {
    p_tl = (mat *)malloc(n_set * sizeof(mat));
    for(i_set = 0; i_set < n_set; i_set ++) p_tl[i_set] = NULL;
  }
 
  /*  Calculate tl for each set of phase shifts. */
  for(i_set = 0; i_set < n_set; i_set ++)
  {

    l_set_1 = phs_shifts[i_set].lmax + 1;

    CONTROL_MSG(CONTROL_X, "i_set = %u, l_max(set) = %u, n_eng = %u\n",
            i_set, l_set_1 - 1, phs_shifts[i_set].n_eng);

    p_tl[i_set] = matalloc(p_tl[i_set], l_set_1, 1, NUM_COMPLEX);

    /* Check if energy is inside the energy range, exit if too small */

    if(energy < phs_shifts[i_set].eng_min)
    {

      /* Abort for too low energies */
      ERROR_MSG("%.1f H is lower than the min. energy for set No. %u\n",
                energy, i_set);
      free(p_tl);
      ERROR_RETURN(NULL);
    } /* if (energy too low) */
    else if(energy >= phs_shifts[i_set].eng_max)
    {
      /* Extrapolate for too high energies */
      WARNING_MSG("%.2f H is higher than the max. energy for set No. %u\n"
                  "\t => calculate extrapolated phase shift values\n",
                  energy, i_set);

      i_eng = phs_shifts[i_set].n_eng - 1;
      for(l = 0; l < l_set_1; l ++)
      {
        delta =  phs_shifts[i_set].pshift[i_eng*l_set_1 + l] -
          (
              ( phs_shifts[i_set].pshift[i_eng*l_set_1 + l] -
                phs_shifts[i_set].pshift[(i_eng - 1)*l_set_1 + l] )
            / ( phs_shifts[i_set].energy[i_eng] -
                phs_shifts[i_set].energy[i_eng - 1] )
          ) * ( phs_shifts[i_set].energy[i_eng] - energy);

        iaux = 1 + l;
        faux_r = cleed_real_cos(delta);
        faux_i = cleed_real_sin(delta);
        cri_mul(p_tl[i_set]->rel+iaux, p_tl[i_set]->iel+iaux,
                faux_r, faux_i, faux_i, 0.);
      }

      /* Include temperature in atomic scattering factors. */
      leed_par_temp_tl(p_tl[i_set], p_tl[i_set], phs_shifts[i_set].dr[0],
                       energy, l_max, phs_shifts[i_set].lmax);


      CONTROL_MSG(CONTROL, "after leed_par_temp_tl, dr[0] = %.3f A^2:\n",
                  phs_shifts[i_set].dr[0]*BOHR*BOHR);
#if CONTROL
      matshow(p_tl[i_set]);
#endif

    } /* else if (energy too high) */
    else
    {
      /* If the energy is within the range of phase shifts:
       * scan through the energy list and find the right values to interpolate:
       * dl(E) = dl(i) - (dl(i) - dl(i-1))/(E(i) - E(i-1)) * ((E(i) - E)
       */
      for(i_eng = 0; phs_shifts[i_set].energy[i_eng] < energy; i_eng ++)
      { ; }
     
      for(l = 0; l < l_set_1; l ++)
      {
        delta =  phs_shifts[i_set].pshift[i_eng*l_set_1 + l] -
            (
                ( phs_shifts[i_set].pshift[i_eng*l_set_1+l] -
                    phs_shifts[i_set].pshift[(i_eng - 1)*l_set_1+l] )
              / ( phs_shifts[i_set].energy[i_eng] -
                  phs_shifts[i_set].energy[i_eng - 1] )
            ) * ( phs_shifts[i_set].energy[i_eng] - energy);

        iaux = 1 + l;
        faux_r = cleed_real_cos(delta);
        faux_i = cleed_real_sin(delta);
        cri_mul(p_tl[i_set]->rel+iaux, p_tl[i_set]->iel+iaux,
                faux_r, faux_i, faux_i, 0.);
      }

      /* Include temperature in atomic scattering factors. */
      leed_par_temp_tl(p_tl[i_set], p_tl[i_set], phs_shifts[i_set].dr[0],
                       energy, l_max, phs_shifts[i_set].lmax);

      CONTROL_MSG(CONTROL, "after leed_par_temp_tl, "
                  "dr[0] = %.3f A^2:\n", phs_shifts[i_set].dr[0]*BOHR*BOHR);
#if CONTROL
      matshow(p_tl[i_set]);
#endif

    } /* else (in the right energy range) */

  } /* for i_set */

  return(p_tl);
} /* end of function leed_par_mktl */
