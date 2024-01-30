/**********************************************************************
 *                          LPCMKMS.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 * GH/18.09.00 - Creation
 *********************************************************************/

/*! \file
 *
 * Implements pc_mk_ms() function used to calculate the non-diagonal
 * temperature dependent atomic scattering matrix.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "leed.h"

static const double M_TOLERANCE = 1.e-6; /*!< tolerance for floating point zero */
static const double SQRT_4PI3  = 2.0466534158929771; /*!< \f$ \sqrt{\frac{4 \pi}{3}} \f$ */
static const double SQRT_2_1 = 0.7071067811865475;  /*!< \f$ 1 / sqrt(2) \f$ */


/*!
 * Calculates non-diagonal temperature dependent atomic scattering matrix
 * according to the cumulants expansion (P. de Andres / V. Fritzsche).
 *
 * Design
 * ======
 *
 * See: P. de Andres, D. A. King, "Anisotropic and Anharmonic effects through
 * the t-matrix for Low-Energy Electron Diffraction (TMAT V1.1)",
 * Comp. Phys. Comm., sect. 2.4. the equation numbers (30 - 32)
 * refer to this paper.
 *
 * \param[in,out] p_Mx Pointer to ...
 * \param[in,out] p_My Pointer to ...
 * \param[in,out] p_Mz Pointer to ...
 * \param[in,out] p_MxMx Pointer to ...
 * \param[in,out] p_MyMy Pointer to ...
 * \param[in,out] p_MzMz Pointer to ...
 * \param l_max Maximum l quantum number required for output matrix.
 * \return Number of violations if erroneous matrix elements have been detected.
 * \retval 0 if successful.
 *
 * \note if any of the input matrices are \c NULL then the matrix structure
 * will be created.
 *
 * \todo Finish documentation of function parameters.
 */
int pc_mk_ms(mat *p_Mx, mat *p_My, mat *p_Mz,
             mat *p_MxMx, mat *p_MyMy, mat *p_MzMz, size_t l_max)
{

  size_t l1, l3;
  int m1, m3;
  int i_el, i_count;
  size_t l_max_2 = (l_max+1)*(l_max+1);     /* matrix dimension */

  real trace_r, trace_i;

  real faux_r, faux_i, pref;
  /* real sq_2_1, sq_4pi3; */
  real Mp_r, Mp_i, Mm_r, Mm_i;

  /* Ms */
  mat Mx, MxMx;
  mat My, MyMy;
  mat Mz, MzMz;
  mat Unity = NULL;

  CONTROL_MSG(CONTROL, "Enter function lmax = %ld\n", l_max);

  /* Call mk_cg_coef() to make sure that all C.G. coefficients are available. */
  mk_cg_coef(2*l_max);

  /* Preset variables. */
  Mx = *p_Mx; MxMx = *p_MxMx;
  My = *p_My; MyMy = *p_MyMy;
  Mz = *p_Mz; MzMz = *p_MzMz;

  Mx = matalloc(Mx, l_max_2, l_max_2, NUM_COMPLEX);
  My = matalloc(My, l_max_2, l_max_2, NUM_COMPLEX);
  Mz = matalloc(Mz, l_max_2, l_max_2, NUM_COMPLEX);

  Unity = matalloc(Unity, l_max_2, l_max_2, NUM_COMPLEX);

  /* Set up Mz, Mp, Mm
   * (Eq 30)
   * Mz(l1,m1,l3,m3) = sqrt(4*PI/3) * i^(l3-l1) (-1)^m3 * cg(l1,m1,1, 0,l3,-m3)
   * (Eq 31)
   * Mp(l1,m1,l3,m3) = sqrt(4*PI/3) * i^(l3-l1) (-1)^m3 * cg(l1,m1,1, 1,l3,-m3)
   * Mm(l1,m1,l3,m3) = sqrt(4*PI/3) * i^(l3-l1) (-1)^m3 * cg(l1,m1,1,-1,l3,-m3)
   *
   * (Eq 32)
   * Mx = 1/sqrt(2) * (Mm - Mp)
   * My = i/sqrt(2) * (Mm + Mp)
   *
   * !!! Note: BLM(l1,m1,l2,m3,l3,m3) = cg(l1,m1,l2,-m2,l3,-m3) !!!
   */

  CONTROL_MSG(CONTROL, "Start computing M_xyz matrices\n");

  for(i_el = 1, l1 = 0; l1 <= l_max; l1 ++)
  {
    for(m1 = -(int)l1; m1 <= (int)l1; m1 ++)
    {
      for(l3 = 0; l3 <= l_max; l3 ++)
      {
        for(m3 = -(int)l3; m3 <= (int)l3; m3 ++, i_el ++)
        {
          cri_powi(&faux_r, &faux_i, (int)l3 - (int)l1);

          /* Eq. 30: m2 = 0, l2 = 1 */
          /* pref = SQRT_4PI3 * M1P(m3) * blm(l1, m1, 1, 0, l3,-m3); */
          pref = SQRT_4PI3 * M1P(m3) * cg((int)l1, m1, 1, 0, (int)l3, m3);
          Mz->rel[i_el] = pref * faux_r;
          Mz->iel[i_el] = pref * faux_i;

          /* Eq. 31: m2 = +/-1, l2 = 1 for blm/cg */
          /* pref = SQRT_4PI3 * M1P(m3) * blm(l1, m1, 1, 1, l3,-m3); */
          pref = SQRT_4PI3 * M1P(m3) * cg((int)l1, m1, 1,-1, (int)l3, m3);
          Mp_r = pref * faux_r;
          Mp_i = pref * faux_i;

          /* Eq. 31: m2 = -/+1, l2 = 1 for blm/cg */
          /* pref = SQRT_4PI3 * M1P(m3) * blm(l1, m1, 1,-1, l3,-m3); */
          pref = SQRT_4PI3 * M1P(m3) * cg((int)l1, m1, 1, 1, (int)l3, m3);
          Mm_r = pref * faux_r;
          Mm_i = pref * faux_i;

          /* Eq 32 */
          Mx->rel[i_el] = SQRT_2_1 * (Mm_r - Mp_r);
          Mx->iel[i_el] = SQRT_2_1 * (Mm_i - Mp_i);

          My->rel[i_el] = - SQRT_2_1 * (Mm_i + Mp_i);
          My->iel[i_el] =   SQRT_2_1 * (Mm_r + Mp_r);

        } /* m3 */
      } /* l3 */
    } /* m1 */
  } /* i_el */
 
#if CONTROL_X
  fprintf(STDCTR, "(pc_mk_ms): Mz: \n");
  matshow(Mz);
  fprintf(STDCTR, "(pc_mk_ms): Mx: \n");
  matshow(Mx);
  fprintf(STDCTR, "(pc_mk_ms): My: \n");
  matshow(My);
#endif

  /* Create MxMx, MyMy, MzMz */
  MxMx = matmul(MxMx, Mx, Mx);
  MyMy = matmul(MyMy, My, My);
  MzMz = matmul(MzMz, Mz, Mz);

#if CONTROL_X
  fprintf(STDCTR, "(pc_mk_ms): MzMz: \n");
  matshowabs(MzMz);
  fprintf(STDCTR, "(pc_mk_ms): MxMx: \n");
  matshowabs(MxMx);
  fprintf(STDCTR, "(pc_mk_ms): MyMy: \n");
  matshowabs(MyMy);
#endif

  /* Check trace of Sum MxMx + MyMy + MzMz: should be Unity */
  i_count = 0;
  trace_r = trace_i = 0.;

  for(i_el = 1, l1 = 0; l1 <= l_max; l1 ++)
  {
    for(m1 = -(int)l1; m1 <= (int)l1; m1 ++)
    {
      for(l3 = 0; l3 <= l_max; l3 ++)
      {
        for(m3 = -(int)l3; m3 <= (int)l3; m3 ++, i_el ++)
        {
          Unity->rel[i_el] = MxMx->rel[i_el] +
                              MyMy->rel[i_el] + MzMz->rel[i_el];
          Unity->iel[i_el] = MxMx->iel[i_el] +
                              MyMy->iel[i_el] + MzMz->iel[i_el];

          /* diagonal elements */
          if( (l1 == l3) && ( m1 == m3) )
          {
            trace_r += Unity->rel[i_el];
            trace_i += Unity->iel[i_el];
           
            if( (cleed_real_cabs(Unity->rel[i_el] -1., Unity->iel[i_el]) > M_TOLERANCE)
               && (l1 != l_max) )
            {
              ERROR_MSG("trace element (%d %d, %d %d) deviates from 1.: (%f,%f)\n",
                        l1, m1, l3, m3, Unity->rel[i_el], Unity->iel[i_el]);
              i_count ++;
            } /* if */
          } /* if l1 == l3 */
          else /* off-diagonal elements */
          {
            if( ( cleed_real_cabs(Unity->rel[i_el], Unity->iel[i_el]) > M_TOLERANCE )
               && (l1 != l_max) && (l3 != l_max) )
            {
              ERROR_MSG("element (%d %d, %d %d) deviates from 0.: (%f,%f)\n",
                        l1, m1, l3, m3, Unity->rel[i_el], Unity->iel[i_el]);
              i_count ++;
            } /* if */
          } /* else */

        } /* m3 */
      } /* l3 */

    } /* m1 */
  } /* l1 */

#if CONTROL_X
  CONTROL_MSG(CONTROL_X, "MxMx + MyMy + MzMz: \n");
  matshowabs(Unity);
#endif

  if( ! IS_EQUAL_REAL(cleed_real_cabs(trace_r, trace_i), 0.))
  {
    faux_r = cleed_real_cabs(trace_r - l_max_2, trace_i) / cleed_real_cabs(trace_r, trace_i);

    CONTROL_MSG(CONTROL, "rel. error in trace of "
            "MxMx + MyMy + MzMz: %.3f\n", faux_r);
  }

  *p_Mx = Mx; *p_MxMx = MxMx;
  *p_My = My; *p_MyMy = MyMy;
  *p_Mz = Mz; *p_MzMz = MzMz;

  ERROR_MSG("error counts (l < %d): %d\n", l_max, i_count);

  return (i_count);
} /* end of function pc_mk_ms */
