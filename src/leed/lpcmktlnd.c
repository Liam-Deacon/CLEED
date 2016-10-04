/*********************************************************************
 *                        LPCMKTLND.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/20.08.94 - Creation
 *   GH/18.07.95 - temperature dependent phase shifts.
 *   GH/03.05.00 - read non-diagonal t-matrix
 *   GH/16.09.00 - calculate non-diagonal t-matrix.
 *********************************************************************/

/*! \file
 *
 * Implements leed_par_mktl_nd() function for calculating the atomic
 * scattering factors for a given energy.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "leed.h"

/*!
 * Calculates phaseshift delta
 *
 * Formula used:
 *
 * delta =  phs_shifts[i_set].pshift[i_eng*l_max_plus_1 + l] -
 *            ( ( phs_shifts[i_set].pshift[i_eng*l_max_plus_1 + l] -
 *                phs_shifts[i_set].pshift[(i_eng - 1)*l_max_plus_1 + l] )
 *            / ( phs_shifts[i_set].energy[i_eng] -
 *                phs_shifts[i_set].energy[i_eng - 1] ) )
 *            * ( phs_shifts[i_set].energy[i_eng] - energy);
 *
 * \param pshift Pointer to the higher offset for phase shift value.
 * \param phsh_energy Pointer to the higher offset for phase shift energy.
 * \param pshift_offset Stride size for phase shift set.
 * \param energy Energy baseline?
 *
 * \warning Uses pointer arithmetic so potentially insecure.
 */
__attribute__((nonnull)) static inline
  real leed_par_mktl_phaseshift_delta(real *pshift,
    real *phsh_energy, const size_t pshift_offset, const real energy)
{
  return *pshift - ( (*pshift - *(pshift-pshift_offset) )
      / ( *phsh_energy - *(phsh_energy-1) ) ) * (*phsh_energy - energy);
}

/*!
 * Populates current scattering factor matrix \p p_tl_i
 *
 * \param p_tl_i Scattering matrix for the given set
 * \param phsh Phaseshifts for the given set
 * \param n_eng Number of energies for given set
 * \param energy Maximum energy?
 */
__attribute__((nonnull)) static inline
  void leed_par_mktl_populate_set(mat p_tl_i,
      const leed_phase * const phsh, const size_t n_eng, const real energy)
{
  const size_t l_set_1 = phsh->lmax + 1;
  real faux_r, faux_i, delta;
  size_t l, iaux;

  for(l = 0; l < l_set_1; l++)
  {
    delta = leed_par_mktl_phaseshift_delta(
              &phsh->pshift[n_eng*l_set_1 + l],
              &phsh->energy[n_eng], l_set_1+l, energy);
    iaux = 1 + l;
    faux_r = cleed_real_cos(delta);
    faux_i = cleed_real_sin(delta);
    cri_mul(&p_tl_i->rel[iaux], &p_tl_i->iel[iaux],
            faux_r, faux_i, faux_i, 0.);
  }
}

inline mat *leed_par_mktl(mat *p_tl, const leed_phase *phs_shifts,
                   size_t l_max, real energy)
{
  return leed_par_mktl_nd(p_tl, phs_shifts, l_max, energy, false);
}

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
 * \param symmetry Flag to indicate whether to use symmetry calculations.
 * \return Pointer to the new set of scattering factor matrices.
 * \retval \c NULL if any error occurred and #EXIT_ON_ERROR is not defined.
 */
mat *leed_par_mktl_nd(mat *p_tl, const leed_phase *phs_shifts,
                      size_t l_max, real energy, bool symmetry)
{

  size_t l, l_set_1;
  size_t i_set, n_set;
  size_t i_eng, iaux;

  real delta;
  real faux_r, faux_i;

  /* Search through list "phs_shifts".
   * - Find number of sets (n_set) and
   * - max. number of l (l_max: > input value if there is one larger
   *   set of phase shifts);
   * - call mkcg_coeff to ensure all C.G. coefficients are available.
   * - allocate p_tl.
   */
  for(n_set = 0; phs_shifts[n_set].lmax != LMAX_TOO_LARGE; n_set ++)
  { ; }

  CONTROL_MSG(CONTROL, "energy = %.2f H, n_set = %u, l_max = %u\n",
                  energy /*HART*/, n_set, l_max);

  if(p_tl == NULL)
  {
    CLEED_ALLOC_CHECK(p_tl = (mat *)calloc(n_set, sizeof(mat)));
  }

  /* Calculate tl (diagonal or non-diagonal scattering matrix
   * for each set of phase shifts. */
  for(i_set = 0; i_set < n_set; i_set ++)
  {
    l_set_1 = (size_t)phs_shifts[i_set].lmax + 1;

    /* check for unknown t_type */
    switch(phs_shifts[i_set].t_type) {
      case T_DIAG: case T_NOND:
      {
        CONTROL_MSG(CONTROL_X, "i_set = %u, lmax(set) = %u, "
                        "n_eng = %u, t_type = %d\n", i_set, l_set_1 - 1,
                    phs_shifts[i_set].n_eng, phs_shifts[i_set].t_type);
        break;
      }
      case T_IGNR:
      {
        CONTROL_MSG(CONTROL_X, "i_set = %u, l_max(set) = %u, n_eng = %u\n",
                i_set, l_set_1 - 1, phs_shifts[i_set].n_eng);
        break;
      }
      default:
      {
        ERROR_MSG("t_type %d has no valid value for set No. %d\n",
                  phs_shifts[i_set].t_type, i_set);
        free(p_tl);
        ERROR_RETURN(NULL);
      } /* neither T_DIAG nor T_NOND */
    }

    CONTROL_MSG(CONTROL, "d %u: (%s)\n", i_set, phs_shifts[i_set].input_file);

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
      WARNING_MSG("%.2f H is higher than max. energy for set No. %u\n"
                  "\t => calculate extrapolated phase shift values\n",
                  energy, i_set);

      i_eng = phs_shifts[i_set].n_eng - 1;
      leed_par_mktl_populate_set(p_tl[i_set], &phs_shifts[i_set], i_eng, energy);

      /* Include temperature in atomic scattering factors. */
      if(phs_shifts[i_set].t_type == T_DIAG)
      {
        leed_par_temp_tl(p_tl[i_set], p_tl[i_set], phs_shifts[i_set].dr[0],
                         energy, l_max, phs_shifts[i_set].lmax);

        CONTROL_MSG(CONTROL_X, "after %s, dr[0] = %.3f A^2:\n%s", __func__,
                    phs_shifts[i_set].dr[0]*BOHR*BOHR,
                    matshowabs(NULL, p_tl[i_set]));
      } /* T_DIAG */
      else if(phs_shifts[i_set].t_type == T_NOND || !symmetry)
      {
        leed_par_cumulative_tl(p_tl[i_set],
                               p_tl[i_set],
                               phs_shifts[i_set].dr[1],
                               phs_shifts[i_set].dr[2],
                               phs_shifts[i_set].dr[3],
                               energy,
                               l_max,
                               phs_shifts[i_set].lmax);

        CONTROL_MSG(CONTROL_X, "non-diag. Tlm for set %u:\n%s",
                    i_set, matshowabs(NULL, p_tl[i_set]));
      } /* T_NOND */
    } /* else if (energy too high) */
    else
    {
      /* If the energy is within the range of phase shifts:
       * scan through the energy list and find the right values to
       * interpolate:
       *
       * dl(E) = dl(i) - (dl(i) - dl(i-1))/(E(i) - E(i-1)) * ((E(i) - E)
       */
      for(i_eng = 0; phs_shifts[i_set].energy[i_eng] < energy; i_eng ++) {;}
      leed_par_mktl_populate_set(p_tl[i_set], &phs_shifts[i_set], i_eng, energy);

      /* Include temperature in atomic scattering factors. */
      if(phs_shifts[i_set].t_type == T_DIAG ||
         phs_shifts[i_set].t_type == T_IGNR || !symmetry)
      {
        leed_par_temp_tl(p_tl[i_set], p_tl[i_set], phs_shifts[i_set].dr[0],
                         energy, l_max, phs_shifts[i_set].lmax);

        CONTROL_MSG(CONTROL_X,
                    "after %s, dr[0] = %.3f A^2:\n", __func__,
                    phs_shifts[i_set].dr[0]*BOHR*BOHR,
                    matshowabs(NULL, p_tl[i_set]));
      } /* T_DIAG */
      else if(phs_shifts[i_set].t_type == T_NOND)
      {
        leed_par_cumulative_tl(p_tl[i_set],
                               p_tl[i_set],
                               phs_shifts[i_set].dr[1],
                               phs_shifts[i_set].dr[2],
                               phs_shifts[i_set].dr[3],
                               energy,
                               l_max,
                               phs_shifts[i_set].lmax);

        CONTROL_MSG(CONTROL_X, "non-diag. Tlm for set %u:\n%s",
                    i_set,  matshowabs(NULL, p_tl[i_set]));
      } /* T_NOND */

    } /* else (in the right energy range) */

  } /* for i_set */

  return(p_tl);
} /* end of function leed_par_mktl_nd */
