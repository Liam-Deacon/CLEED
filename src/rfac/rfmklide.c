/*********************************************************************
 *                           RFMKLIDE.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1995.08.29 - Creation
 *********************************************************************/

/*!
 * \file
 * \brief Prepare R factor calculations.
 *
 * Implementation file for the cr_mklinde() function.
 */

#include <math.h>
#include "rfac.h"

/*!
 * \brief First stage of cubic spline.
 *
 * \param[out] *eng energy values within the overlapping energy range.
 * If there is a non-zero shift between the energy axes, the energy value
 * will correspond to the experimental energy axis.
 *
 * \param[out] *e_int pointer to experimental intensity values corresponding
 * to \p eng.
 *
 * \param[out] *t_int pointer to theoretical intensity values corresponding
 * to \p eng.
 *
 * \param de step for energy axis.
 *
 * \param shift offset between experimental and theoretical
 *             energy axes (Et = Ee - shift).
 * \param[in] *exp_list pointer to list of energy/intensity/deriv2
 * values to be interpolated by cubic spline.
 *
 * \param exp_leng number of elements in exp_list.
 *
 * \param[in] *the_list pointer to list of energy/intensity/deriv2
 * values to be interpolated by cubic spline.
 *
 * \param the_leng number of elements in the_list.
 *
 * \note The lists exp_list and the_list must be prepared for a cubic spline;
 * i.e. the structure elements deriv2 must exist.
 *
 * Enough memory for \p eng, \p e_int, and \p t_int must be allocated.
 *
 * \return Number of energy points in lists.
 *
 * \retval 0 if no overlap.
 */
size_t rf_mklide(real *eng, real *e_int, real *t_int,
              real de, real shift,
              rfac_iv_data *exp_list, size_t exp_leng,
              rfac_iv_data *the_list, size_t the_leng)
{
  size_t i_elo, n_eng;
  real energy;
  real e_max = 0., t_max = 0.;

  #ifdef WRITE
  FILE *str_e_int, *str_t_int;
  #endif

  /* Find first expt. energy (i_elo) within the theor. energy range */
  for( i_elo = 0;
      (i_elo < exp_leng) &&
      ( (exp_list+i_elo)->energy < (the_list->energy - shift) );
      i_elo ++)
  { ; }

  /* Return zero if no overlap */
  if(i_elo == exp_leng) return(0);

  /* Write lists eng, e_int/2 */
  for(energy = (exp_list+i_elo)->energy, n_eng = 0;
     (energy <= (exp_list+exp_leng-1)->energy) && 
     (energy < ((the_list+the_leng-1)->energy - shift) );
     energy += de, n_eng ++)
  {
    eng[n_eng]  = energy;
    e_int[n_eng] = rf_splint(eng[n_eng],         exp_list, exp_leng);
    t_int[n_eng] = rf_splint(eng[n_eng] - shift, the_list, the_leng);
    e_max = MAX(e_int[n_eng], e_max);
    t_max = MAX(t_int[n_eng], t_max);
  }

  /* terminate lists */
  eng[n_eng]  = F_END_OF_LIST;
  e_int[n_eng] = F_END_OF_LIST;
  t_int[n_eng] = F_END_OF_LIST;

  #ifdef CONTROL
  fprintf(STDCTR, "(cr_mklist): (exp) n_eng: %d\n", n_eng);
  #endif
 
  #ifdef WRITE
  str_e_int = fopen("rfmklist.exp", "w");
  str_t_int = fopen("rfmklist.the", "w");

  for(i_eng = 0; i_eng < n_eng; i_eng++)
  {
    fprintf(str_e_int, "%f %e\n", eng[i_eng], e_int[i_eng]/e_max);
    fprintf(str_t_int," %f %e\n", eng[i_eng], t_int[i_eng]/t_max);
  }
  #endif

  return(n_eng);
}  /* end of function rf_mklist */
