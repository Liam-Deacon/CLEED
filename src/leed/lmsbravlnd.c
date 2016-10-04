/*********************************************************************
 *                    LMSBRAVLND.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/20.07.95 - Creation (leed_ms)
 *   GH/31.07.95 - include matrix multiplication Y * Tii *Y (i.e. make
 *                 msltok obsolete).
 *   GH/03.09.97 - set return value to 1
 *   GH/23.09.00 - extension for non-diagonal atomic scattering matrix.
 *   GH/05.07.03 - bug fix: update all "old" values at the end of function.
 *********************************************************************/

/*! \file
 *
 * Implements calculations for scattering matrix of Bravais layer.
 */

#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "leed.h"

/*!
 * Calculates scattering matrix for Bravais layer.
 *
 * \f[ R/T(k'k) = pref_i * |\vec{k'}><l,m|M(lm,l'm')|l'm'><k| + dkk' \f]
 *
 * \f[ Y_{lm} =  |\vec{k'}><l,m| = Y_{lm}(k') \f]
 * (k_max rows) x ((l_max+1)^2 columns)
 *
 * \param[out] p_Tpp Pointer to the Bravais layer diffraction matrix given by
 * k(+) -> k(+) transmission matrix \f$ \textbf{T}^{++} \f$
 * \param[out] p_Tmm Pointer to the Bravais layer diffraction matrix given by
 * k(-) -> k(-) transmission matrix \f$ \textbf{T}^{--} \f$
 * \param[out] p_Rpm Pointer to the Bravais layer diffraction matrix given by
 * k(-) -> k(+) reflection matrix \f$ \textbf{R}^{-+} \f$
 * \param[out] p_Rmp Pointer to the Bravais layer diffraction matrix given by
 * k(+) -> k(-) reflection matrix \f$ \textbf{R}^{+-} \f$
 * \param[in] v_par parameters which vary during the energy loop.
 * Used: k_in    (parallel components of the incoming beam),
 *       l_max   (max. l quantum number),
 *       tl      (atomic scattering factors: either diagonal (t_type = T_DIAG)
 *                or non-diagonal (t_type = T_NOND)),
 *       epsilon (smallest relative wave amplitude to be considered).
 * \param[in] layer Pointer to array containing all relevant information about
 * the Bravais layer.
 * \param[in] beams Pointer to structure containing additional information
 * about the \f$ \vec{k} \f$ vectors involved. The wave vectors
 * @leed_beam::k_r and @leed_beam::k_i are used.
 * \return
 * \retval 0 on successful completion.
 * \see leed_ms_lsum_ii() , leed_ms_tmat_ii() , leed_ms_tmat_nd_ii() ,
 * leed_ms_ymat() , leed_ms_yp_ym() , leed_ms_yp_yxp() , leed_ms_yp_yxm()
 * and matmul() .
 */
int leed_ms_nd(mat *p_Tpp, mat *p_Tmm, mat *p_Rpm, mat *p_Rmp,
               leed_var *v_par, leed_layer *layer, leed_beam *beams)
{
  /*!FIXME: change statics assignments to constants as in gh_stddef.h */
  static size_t old_set = 0;
  static size_t old_n_beams = 0;
  static size_t old_l_max = 0;

  static int old_type = -9999;

  static real old_eng = -9999.;

  static mat Llm = NULL, Tii = NULL;
  static mat Yin_p = NULL, Yin_m = NULL, Yout_p = NULL, Yout_m = NULL;

  size_t n_beams, i_beams;
  size_t l_max;
  int i_type, t_type;
  size_t i_c;

  real pref_i, faux_r, faux_i;
  real *ptr_r, *ptr_i;

  mat Maux = NULL;

  /* Preset often used values: i_type, l_max, n_beams */
  t_type = (layer->atoms)->t_type;
  i_type = (layer->atoms)->type;
  l_max = v_par->l_max;

  for(n_beams = 0;
      ! IS_EQUAL_REAL((beams + n_beams)->k_par, F_END_OF_LIST);
      n_beams ++);

  CONTROL_MSG(CONTROL, "l_max = %d, "
            "No of beams = %d, atom type = %d\n", l_max, n_beams, i_type);

  /* Check if t_type has the right value */
  if( (t_type != T_DIAG) && (t_type != T_NOND) )
  {
    ERROR_MSG("unknown matrix type of t-matrix.\n");
    ERROR_RETURN(-1);
  }

  /* Check if the current beam set was used in the previous call.
   * - if not, recalculate lattice sum, scattering matrix, and spherical harmonics.
   * - if yes, reuse lattice sum, scattering matrix, and spherical harmonics and
   *   recalculate the (lm) scattering matrix only if the atom type has changed.
   */
  if( ! ((IS_EQUAL_REAL(old_eng, v_par->eng_r))) ||
        (old_set     != beams->set)   ||
        (old_n_beams != n_beams)      ||
        (old_l_max   != l_max)           )
  {
    CONTROL_MSG(CONTROL, "recalculate lattice sum etc.\n");

    /* calculate lattice sum */
    Llm = leed_ms_lsum_ii ( Llm, beams->k_r[0], beams->k_i[0], beams->k_r,
                            layer->a_lat, 2*l_max, v_par->epsilon );

    /* calculate scattering matrix */
    if(t_type == T_DIAG)
    {
      Tii = leed_ms_tmat_ii( Tii, Llm, v_par->p_tl[i_type], l_max);
      Tii = mattrans(Tii, Tii);
    }
    else if(t_type == T_NOND)
    {
      Tii = leed_ms_tmat_nd_ii( Tii, Llm, v_par->p_tl[i_type], l_max);
    }

    /* Yout_p = Y(k+) */
    Yout_p = leed_ms_ymat(Yout_p, l_max, beams, n_beams);

    /* Yout_m = Y(k-) */
    Yout_m = leed_ms_yp_ym(Yout_m, Yout_p);

    /* Yin_p: Y*(k+) for transmission matrix. */
    Yin_p = leed_ms_yp_yxp(Yin_p, Yout_p);

    /* Yin_m: Y*(k-) for reflection matrix. */
    Yin_m = leed_ms_yp_yxm(Yin_m, Yout_p);

    /* Loop over k' (exit beams: rows of Yout_p):
     * - Multiply with factor i 8 PI^2 / (|k|*A*k'_z)
     * - i_beams is (row number - 1) and equal to the index of exit beams
     */

    pref_i = 8.*PI*PI / (beams->k_r[0] * layer->rel_area);

    ptr_r = Yout_p->rel + 1;
    ptr_i = Yout_p->iel + 1;
    for(i_beams = 0; i_beams < Yout_p->rows; i_beams ++)
    {
      cri_mul(&faux_r, &faux_i, 0., pref_i,
              (beams+i_beams)->Akz_r, (beams+i_beams)->Akz_i);
      for(i_c = 0; i_c < Yout_p->cols; i_c ++, ptr_r ++, ptr_i ++ )
      {
        cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i);
      }
    }  /* i_beams */

    ptr_r = Yout_m->rel + 1;
    ptr_i = Yout_m->iel + 1;
    for(i_beams = 0; i_beams < Yout_m->rows; i_beams ++)
    {
      cri_mul(&faux_r, &faux_i, 0., pref_i,
              (beams+i_beams)->Akz_r, (beams+i_beams)->Akz_i);
      for(i_c = 0; i_c < Yout_m->cols; i_c ++, ptr_r ++, ptr_i ++ )
      {
        cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i);
      }
    }  /* i_beams */

  }
  else
  {
    /* The current beam set was used already in the previous call: only
     * the scattering matrix has to be recalculated if atom type is different. */
    if( old_type != i_type )
    {
      CONTROL_MSG(CONTROL, "recalculate scattering matrix.\n");

      /* calculate scattering matrix */
      if(t_type == T_DIAG)
      {
        Tii = leed_ms_tmat_ii( Tii, Llm, v_par->p_tl[i_type], l_max);
        Tii = mattrans(Tii, Tii);
      }
      else if(t_type == T_NOND)
      {
        Tii = leed_ms_tmat_nd_ii( Tii, Llm, v_par->p_tl[i_type], l_max);

        CONTROL_MSG(CONTROL, "T_NOND \n");
      }

    } /* if i_type */
  }
 
  /* Matrix product Yout (exit beams) * Tii * Yin (inc. beams) */
  Maux = matmul(Maux, Yout_p, Tii);

  *p_Rpm = matmul(*p_Rpm, Maux, Yin_m);
  *p_Tpp = matmul(*p_Tpp, Maux, Yin_p);

  Maux = matmul(Maux, Yout_m, Tii);

  *p_Tmm = matmul(*p_Tmm, Maux, Yin_m);
  *p_Rmp = matmul(*p_Rmp, Maux, Yin_p);

  /* Add unscattered wave (identity) to transmission matrices Tpp and Tmm */

  for(i_c = 1;
      i_c <= (*p_Tpp)->rows * (*p_Tpp)->cols;
      i_c += (*p_Tpp)->cols + 1)
  {
    (*p_Tpp)->rel[i_c] += 1.;
    (*p_Tmm)->rel[i_c] += 1.;
  }
 
  /* Update energy, beam_set, and beam_num, l_max, i_type */
  old_eng = v_par->eng_r;
  old_set = beams->set;
  old_n_beams = n_beams;
  old_l_max = l_max;
  old_type = i_type;
   
  return(0);
} /* end of function leed_ms_nd */
