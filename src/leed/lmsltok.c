/*********************************************************************
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2016 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *  GH/05.09.94
 *  GH/27.01.95 - Remove "-" from pref_i. Prefactor is now:
 *                i*8*PI^2 / (|k|*A*kz+)
 *
 *********************************************************************/

/*!
 * \file
 *
 * Transformation of the multiple scattering matrix from angular
 * momentum space into k-space.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "leed.h"

/*!
 * Transformation of the multiple scattering matrix from angular momentum
 * space into k-space.
 *
 * Mk'k = pre_fac * sum(l,m,l',m') |k'><l,m|M(lm,l'm')|l'm'><k| + dkk'
 *
 * Ylm =  |k'><l,m| = Ylm(k')     (k_max rows) x ((l_max+1)^2 columns)
 * Yxlm = |k><l',m'| = Y*l'm'(k)  ((l_max+1)^2 rows) x (k_max columns)
 *
 * pre_fac = i*8*PI^2 / (|k|*A*kz+)
 *
 * \param[out] Mkk  multiple scattering matrix in k-space.
 * \param[in] Mlm  multiple scattering matrix in (l,m)-space.
 * \param[in] Ylm  transformation matrix for exit beams (l,m) -> (k) of
 * dimension: n_beams x (l_max+1)^2 (e.g. output from leed_ms_ymat)
 * \param[in] Yxlm transformation matrix for incoming beams (k) -> (l,m)
 * of dimension (l_max+1)^2 x n_beams (e.g. output from ms_ytoyx)
 *
 * If either \p Ykl or \p Yxkl are \c NULL, it will be calculated as the
 * conjugate transposed matrix from the opposite transformation
 * using ms_ytoyx.
 *
 * beams - additional information about the k vectors
 * involved (in this case: (A kz)^-1).
 * The order of beams must be equal to the first dimension of Ykl (not checked).
 *
 * real unsc - relative intensity of the unscattered beams (either 1. for
 * transmission or 0. for reflection)
 *
 * real rel_area - area of the unit cell relative to 1x1.
 *
 * Depending on the k-vectors involved in the transformation matrices \p Ylm
 * and \p Yxlm, either a reflection (+,- or -,+) or a transmission matrix
 * (+,+ or -,-) will be calculated.
 *
 * \retval \c NULL if failed (and \c EXIT_ON_ERROR is not defined)
 *
 * \return \p Mkk (may be different from input parameter). The storage
 * scheme for \p Mkk is defined by the order of Ylm and Yxlm.
 */
mat leed_ms_ltok ( mat Mkk, mat Mlm, mat Ylm, mat Yxlm,
    leed_beam * beams, real rel_area, real unsc)
{
  int iaux;           /* dummy counter */
  int mchk[4];        /* dummy variables for matcheck calls */
  int i_c, i_r;
  real *ptr_r = NULL,
       *ptr_i = NULL;

  real faux_r, faux_i;
  real pref_i;

  /**********************************************************************
   * - Check the validity of the input matrices Mkk, Mlm, Ylm, Yxlm
   * (Different requirements if FIND_Yx is defined)
   * - Check arguments Ylm, Yxlm and create the missing matrix if FIND_Yx
   * is defined.
   **********************************************************************/

#ifndef FIND_Yx

    if((mchk[0] = matcheck(Mlm)) < 1)
      ERROR_MSG("invalid input matrix Mlm\n");
    if ((mchk[1] = matcheck(Ylm)) < 1)
      ERROR_MSG("invalid input matrix Ylm\n");
    if ((mchk[2] = matcheck(Yxlm)) < 1)
      ERROR_MSG("invalid input matrix Yxlm\n");
    if ((mchk[3] = matcheck(Mkk)) < 0)
      ERROR_MSG("invalid matrix pointer Mkk\n");

    if (mchk[0] < 1 || mchk[1] < 1 || mchk[2] < 1 || mchk[3] < 0)
      ERROR_EXIT_RETURN(1, NULL);


#else /* FIND_Yx is defined */

  if( (matcheck(Mlm)  < 1) ||
      ((matcheck(Ylm) + (matcheck(Yxlm)) < 1) ||
          (matcheck(Mkk)  < 0) )
  {
    ERROR_MSG("invalid input matrices\n");
    ERROR_EXIT_RETURN(1, NULL);
  }

  if(Yxlm == NULL)
  {
    if(Ylm != NULL)
      Yxlm = ms_ytoyx(Yxlm, Ylm);
    else /* Ylm == NULL */
    {
      ERROR_MSG("Yxlm = Ylm = NULL\n");
      ERROR_EXIT_RETURN(1,NULL);
    }
  }
  if(Ylm == NULL)  /* for Yxlm == NULL already checked */
    Ylm = ms_ytoyx(Ylm, Yxlm);

#endif /* FIND_Yx */

  /* Calculate the matrix product Ylm(k') * M(lm,l'm') * Yl'm'(k) */
  Mkk = matmul(Mkk, Ylm, Mlm);
  Mkk = matmul(Mkk, Mkk, Yxlm);

  CONTROL_MSG(CONTROL, "Ylm * Mlm * Yxlm = \n%s", matshow(NULL, Mkk));

  /**********************************************************************
   * Loop over k' (exit beams: rows of Mk'k):
   * - Multiply with factor i 8 PI^2 / (|k|*A*k'_z)
   * - i_r is (row number - 1) and equal to the index of exit beams
   **********************************************************************/
  pref_i = 8.*PI*PI / (beams->k_r[0] * rel_area);

  ptr_r = Mkk->rel + 1;
  ptr_i = Mkk->iel + 1;
  for(i_r = 0; i_r < Mkk->rows; i_r ++)
  {
    cri_mul(&faux_r, &faux_i, 0., pref_i,
        (beams+i_r)->Akz_r, (beams+i_r)->Akz_i);
    for(i_c = 0; i_c < Mkk->cols; i_c ++, ptr_r ++, ptr_i ++ )
    {
      cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i);
    }
  }  /* i_r */

  /* Add unscattered beams: */
  if( ! IS_EQUAL_REAL(unsc, 0.))
  {
    iaux = Mkk->rows * Mkk->cols;
    for(i_r = 1; i_r <= iaux; i_r += Mkk->cols + 1)
      *(Mkk->rel+i_r) += unsc;
  }
  return(Mkk);

} /* end of function ms_ltok */
