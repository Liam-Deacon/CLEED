/*********************************************************************
 *                       RFMKLIST.C
 *
 *  Copyright 1992-2014 Georg Held
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Description:  Program calculates different R factors.
 *
 * Changes:
 *   GH/1995.08.29 - creation
 *********************************************************************/

/*! \file rfmklist.c
 *  \brief Implementation file for \ref rf_mklist function.
 *
 * Prepare R factor calculations.
 */

#include <math.h>
#include "rfac.h"

/*!
 * \fn rf_mklist
 *
 * \brief First stage of cubic spline.
 *
 * \param[out] *eng pointer to energy values within the overlapping energy
 * range. If there is a non-zero shift between the energy axes, the energy
 * value will correspond to the experimental energy axis.
 *
 * \param[out] *e_int pointer to experimental intensity values
 * corresponding to \p eng.
 *
 * \param[out] *t_int pointer to theoretical intensity values corresponding
 * to \p eng.
 *
 * \param shift offset between experimental and theoretical energy axes
 * (Et = Ee - shift).
 *
 * \param[in] *exp_list pointer to list of energy/intensity/deriv2 values
 * to be interpolated by cubic spline.
 *
 * \param exp_leng number of elements in \p exp_list.
 *
 * \param[in] *the_list pointer to list of energy/intensity/deriv2 values
 * to be interpolated by cubic spline.
 *
 * \param the_leng number of elements in exp_list.
 *
 * \note The lists exp_list and the_list must be prepared for a cubic spline;
 * i.e. the structure elements deriv2 must exist.
 *
 * \warning Enough memory for \p eng, \p e_int, and \p t_int must be allocated.
 *
 * \return Number of energy points in lists.
 */
size_t rf_mklist(real *eng, real *e_int, real *t_int,
              real shift,
              rfac_iv_data *exp_list, size_t exp_leng,
              rfac_iv_data *the_list, size_t the_leng)
{
  size_t i_elo, i_eng, n_eng;
  real faux_e, faux_t;
  real e_max = 0., t_max = 0.;

  #ifdef WRITE
  FILE *str_e_int, *str_t_int;
  #endif

  /* Calculate density of energy points for expt. and theor. lists */
  faux_e = exp_leng / ( (exp_list+exp_leng-1)->energy - exp_list->energy );
  faux_t = the_leng / ( (the_list+the_leng-1)->energy - the_list->energy );

  if( faux_e > faux_t)
  /*
   * Higher density of energy points in expt. list:
   * Use energy values of expt. list and interpolate theoretical list.
   */
  {
    /* Find first expt. energy (i_elo) within the theor. energy range */
    for( i_elo = 0;
        (i_elo < exp_leng) &&
        ( (exp_list+i_elo)->energy < (the_list->energy - shift) );
        i_elo ++)
    { ; } /*!FIXME: no content within loop */

    /* Return zero if no overlap */
    if(i_elo == exp_leng) return(0);

    /* Write lists eng, e_int/2 */
    for(i_eng = i_elo, n_eng = 0;
       (i_eng < exp_leng) &&
       ( (exp_list+i_eng)->energy < ((the_list+the_leng-1)->energy - shift) );
       i_eng ++, n_eng ++)
    {
      eng[n_eng]  = (exp_list+i_eng)->energy;
      e_int[n_eng] = (exp_list+i_eng)->intens;
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

  } /* Higher density of energy points in expt. list */
  else
    /*
     * Higher density of energy points in theor. list:
     * Use energy values of theor. list and interpolate experimental list.
     */
  {
    /* Find first theor. energy (i_elo) within the expt. energy range */
    for( i_elo = 0;
        (i_elo < the_leng) &&
        ( (the_list+i_elo)->energy < (exp_list->energy + shift) );
        i_elo ++)
    { ; }
    /* Return zero if no overlap */
    if(i_elo == the_leng) return(0);

    /* Write lists eng, e_int, and t_int */
    for(i_eng = i_elo, n_eng = 0;
       (i_eng < the_leng) &&
       ( (the_list+i_eng)->energy < ((exp_list+exp_leng-1)->energy + shift) );
       i_eng ++, n_eng ++)
    {
      eng[n_eng]  = (the_list+i_eng)->energy + shift;
      e_int[n_eng] = rf_splint(eng[n_eng], exp_list, exp_leng);
      t_int[n_eng] = (the_list+i_eng)->intens;
      e_max = MAX(e_int[n_eng], e_max);
      t_max = MAX(t_int[n_eng], t_max);
    }

    /* terminate lists */
    eng[n_eng]  = F_END_OF_LIST;
    e_int[n_eng] = F_END_OF_LIST;
    t_int[n_eng] = F_END_OF_LIST;

    #ifdef CONTROL
    fprintf(STDCTR, "(cr_mklist): (the) n_eng: %d\n", n_eng);
    #endif

  } /* Higher density of energy points in theor. list */
 
  #ifdef WRITE
  str_e_int = fopen("rfmklist.exp", "w");
  str_t_int = fopen("rfmklist.the", "w");

  for(i_eng = 0; i_eng < n_eng; i_eng++)
  {
    fprintf(str_e_int, "%f %e\n", eng[i_eng], e_int[i_eng]/e_max);
    fprintf(str_t_int, "%f %e\n", eng[i_eng], t_int[i_eng]/t_max);
  }
  #endif /* WRITE */

  return(n_eng);
}  /* end of function rf_mklist */
