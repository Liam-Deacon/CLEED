/*********************************************************************
 *                      LOUTINT.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/14.03.95 - Creation
 *********************************************************************/

/*! \file
 *
 * Implements leed_ms_tmat_ii() function for calculating multiple scattering
 * matrix for a periodic plane of scatterers.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>

#include "leed.h"

/*!
 * Creates the multiple scattering Matrix [(1 - Tl*Giilm)^-1]*Tl for a
 * periodic plane of scatterers.
 *
 * Design
 * ======
 *
 * First create:
 *
 * Gii(l1,m1; l2,m2) =
 *      S[ (-1)^((l1-l2-l3_min)/2 - m2) * L(l3,m3) * C(l3,m3,l1,m1,l2,m2)]
 *
 *   L(l3,m3) = lattice sum
 *   C(l3,m3,l1,m1,l2,m2) = C.G. coefficients.
 *
 * - Gii separates in two blocks with even (l1+m1), (l2+m2) and odd
 *   (l1+m1), (l2+m2) which are stored in Gev and God, respectively.
 * - Gii(l1,m1; l2,m2)  is symmetric under exchange of (l1,m1) and (l2,m2),
 *   therefore only (l2,m2) <= (l1,m1) is calculated.
 *
 * Multiply Gev and God with Tl from the l.h.s. (in the same loop).
 *
 * Add the identity matrix: (1 - Gev/od * T) and invert the result:
 *  Tii =  (1 - Gev/od * T)^-1
 *
 * Copy (1 - T*Gev)^-1 and (1 - T*God)^-1 into Tii in the natural order
 * and multiply with Tl (atomic scattering factors) from the r.h.s:
 * Tii = (1 - T*Gii)^-1(l1,m1,l2,m2)*T(l2)
 *
 * - distribute the matrix elements of Gev and God over Gii
 * - multiply with Tl * i^(l1-l2).
 *
 * \param[out] Tii Pointer to multiple scattering matrix in (l,m)-space.
 * \param[in] Llm Pointer to lattice summation matrix:
 * Llm =  (-1)^m * 4 PI * Ylm (cos(theta) = 0, phi = 0) *
 * sum(R) [ H(1)l(k*|R|) * exp( i(-kin*R + m*phi(R)) ) ]
 * (in natural order: (l,m) = (0,0); (-1,1); (0,1); (1,1); ...)
 * \param[in] Tl Pointer to the atomic scattering matrix stored in natural
 * order. Tl = sin(dl) * exp(idl)
 * \param l_max Maximum angular momentum quantum number.
 * \return Pointer to multiple scattering matrix Tii = [(1 - Tl*Giilm)^-1]*Tl
 * \retval \c NULL if unsuccessful.
 */
mat leed_ms_tmat_ii(mat Tii, mat Llm, mat Tl, size_t l_max)
{
  size_t iaux;
  size_t l1, l2, l3;
  int m1, m2, m3;
  size_t l3_min, l3_max;

  size_t i3;
  size_t iev1, iev2;
  size_t iod1, iod2;
  int odd1;

  real faux_r, faux_i;
  real sum_r, sum_i;
  real sign;

  real *ptr_r, *ptr_i;

  mat Gev = NULL, God = NULL;

  if(mk_cg_coef(2*l_max) != 0)
  {
    WARNING_MSG("C.G. coefficients had to be recalculated\n");
  }

  /* Allocate Gev, God */
  iaux = (l_max + 1)*(l_max + 2)/2;
  Gev = matalloc( Gev, iaux, iaux, NUM_COMPLEX);
  iaux = (l_max +1 ) * l_max/2;
  God = matalloc( God, iaux, iaux, NUM_COMPLEX);

#if CONTROL
  CONTROL_MSG(CONTROL, "\nT: \n");
  matshowabs(Tl);
#endif

  /*
   * Loop over (l1,m1),(l2,m2): Set up  - Tl(l1)*Gii(l1,m1; l2,m2).
   *
   * Gii(l1,m1; l2,m2) =
   *    S[ (-1)^((l1-l2-l3_min)/2 - m2) * L(l3,m3) * C(l3,m3,l1,m1,l2,m2)]
   * L(l3,m3) = lattice sum
   * C(l3,m3,l1,m1,l2,m2) = C.G. coefficients.
   *
   * - Gii separates into two blocks with even (l1+m1), (l2+m2) and odd
   *   (l1+m1), (l2+m2) which are stored in Gev and God, respectively.
   * - Gii(l1,m1; l2,m2)  is symmetric under exchange of (l1,m1) and (l2,m2),
   *   therefore only (l2,m2) <= (l1,m1) is calculated.
   *- Multiply with Tl.
   */
  for(l1 = 0, iev1 = 1, iod1 = 1; l1 <= l_max; l1 ++)
  {
    /* First even (l1 + m1) */
    for(m1 = -(int)l1; m1 <= (int)l1; m1 += 2, iev1 ++)  /* even l1 + m1 */
    {
      for(l2 = 0, iev2 = 1; /* (iev2 <= iev1) && */ (l2 <= l_max); l2 ++)
      {
        /* Only even (l2 + m2)  for even (l1 + m1) */
        for(m2 = -(int)l2; /* (iev2 <= iev1) && */ (m2 <= (int)l2); m2 += 2, iev2 ++)
        {
          /* Loop over l3 (inner--most loop): calculate the elements of Gev
           *
           * - l3_min: l3 >= |m3|, |l2-l1|.
           * - Only even (l1 + l2 + l3) lead to nonzero C.G.-coefficients
           * => if (l1 + l2 + l3_min) is odd, increment l3_min by 1.
           */
          m3 = m1 - m2;

          l3_min = (size_t)MAX(abs(m3), abs((int)l2-(int)l1));
          l3_min += (l1 + l2 + l3_min) % 2;
          l3_max = l2+l1;

          sum_r = sum_i = 0.;

          if (m3 < 0)
            i3 = l3_min*(l3_min + 1) + (size_t)abs(m3) + 1;
          else
            i3 = l3_min*(l3_min + 1) - (size_t)m3 + 1;

          sign = M1P( (int)(l1 - l2 - l3_min)/2 - m2);

          for(l3 = l3_min; l3 <= l3_max; l3 += 2 )
          {
            faux_r = sign*cg((int)l3, m3, (int)l1, m1, (int)l2, -m2);
            sum_r += Llm->rel[i3] * faux_r;
            sum_i += Llm->iel[i3] * faux_r;

            /* l3 is incremented by 2
             * => l3/2 is incremented by 1; the sign is inverted.
             * => i3 is incremented by 2*(l3+1) + 2*(l3+2) = 4*l3 + 6
             */
            sign = -sign;
            i3 += 4*l3 + 6;
          }

          /* Change the sign of sum and multiply with i * Tl from the l.h.s.
           * (i.e. multiply rows of Gev(l1 m1, l2 m2) with Tl(l1))
           * (The i is a correction necessary to get the same results as
           * in XM (VHT). Don't ask where it comes from.
           */
          cri_mul(&faux_r, &faux_i,
                  sum_i, -sum_r, Tl->rel[l1+1], Tl->iel[l1+1]);
          *rmatel(iev1, iev2, Gev) = faux_r;
          *imatel(iev1, iev2, Gev) = faux_i;

        } /* m2 */
      } /* l2 */
    } /* m1 */

    /* Now odd (l1 + m1) */
    for(m1 = -(int)l1+1; m1 < (int)l1; m1 += 2, iod1 ++)  /* even l1 + m1 */
    {
      for(l2 = 0, iod2 = 1; /*(iod2 <= iod1) &&*/ (l2 <= l_max); l2 ++)
      {
        /* Only odd (l2 + m2)  for odd (l1 + m1) */
        for(m2 = -(int)l2+1; /*(iod2 <= iod1) &&*/ (m2 < (int)l2); m2 += 2, iod2 ++)
        {
          /* Loop over l3 (inner--most loop): calculate the elements of God
           * - l3_min: l3 >= |m3|, |l2-l1|.
           * - Only even (l1 + l2 + l3) lead to nonzero C.G.-coefficients
           * => if (l1 + l2 + l3_min) is odd, increment l3_min by 1.
           */
          m3 = m1 - m2;

          l3_min = (size_t)MAX(abs(m3), abs((int)l2-(int)l1));
          l3_min += (l1 + l2 + l3_min) % 2;
          l3_max = l2+l1;

          sum_r = sum_i = 0.;
          if (m3 < 0)
            i3 = l3_min*(l3_min + 1) + (size_t)abs(m3) + 1;
          else
            i3 = l3_min*(l3_min + 1) - (size_t)m3 + 1;

          sign = M1P( (int)(l1 - l2 - l3_min)/2 - m2);

          for(l3 = l3_min; l3 <= l3_max; l3 += 2 )
          {
            faux_r = sign*cg((int)l3, m3, (int)l1,m1, (int)l2,-m2);

            sum_r += Llm->rel[i3] * faux_r;
            sum_i += Llm->iel[i3] * faux_r;

            /* l3 is incremented by 2
             * => l3/2 is incremented by 1; the sign is inverted.
             * => i3 is incremented by 2*(l3+1) + 2*(l3+2) = 4*l3 + 6
             */
            sign = -sign;
            i3 += 4*l3 + 6;
          }

          /* Change the sign of sum and multiply with i * Tl from the l.h.s.
           * (i.e. multiply rows of God(l1 m1, l2 m2) with Tl(l1))
           * (The i is a correction necessary to get the same results as
           * in XM (VHT). Don't ask where it comes from.
           */
          cri_mul(&faux_r, &faux_i,
                  sum_i, -sum_r, Tl->rel[l1+1], Tl->iel[l1+1]);
          *rmatel(iod1, iod2, God) = faux_r;
          *imatel(iod1, iod2, God) = faux_i;

        } /* m2 */
      } /* l2 */
    } /* m1 */
  } /* l1 */

  /* Add the identity matrix: (1 - Gev/od * T) */
  iaux = Gev->cols*Gev->rows;
  for( iev1= 1; iev1 <=iaux; iev1 += Gev->rows + 1) Gev->rel[iev1] += 1.;

  iaux = God->cols*God->rows;
  for( iod1= 1; iod1 <=iaux; iod1 += God->rows + 1) God->rel[iod1] += 1.;

  /* Matrix inversion: (1 - Gev/od * T)^-1 */
  Gev = matinv(Gev, Gev);
  God = matinv(God, God);

 /*
  * Copy (1 - T*Gev)^-1 and (1 - T*God)^-1 into Gii in the natural order
  * and multiply with Tl from the r.h.s: (1 - T*Gii)^-1(l1,m1,l2,m2)*T(l2)
  * (i.e. multiply columns of Gii(l1 m1, l2 m2) with Tl(l2))
  *
  * - first allocate memory for Gii
  * - distribute the matrix elements of Gev and God over Gii
  * - multiply with Tl * i^(l1-l2).
  * - Gev and God are not needed afterwards.
  */

  iaux = (l_max + 1)*(l_max + 1);
  Tii = matalloc(Tii, iaux, iaux, NUM_COMPLEX);

  for(l1 = 0, iev2 = 1, iod2 = 1, ptr_r = Tii->rel+1, ptr_i = Tii->iel+1;
      l1 <= l_max; l1 ++)
  {
    for(m1 = -(int)l1; m1 <= (int)l1; m1 ++)
    {
      odd1 = ODD((int)l1+m1);
      for(l2 = 0; l2 <= l_max; l2 ++)
      {
        for(m2 = -(int)l2; m2 <= (int)l2; m2 ++, ptr_r ++, ptr_i ++)
        {
          if(ODD((int)l2+m2))
          {
            if( odd1)
            {
              cri_powi(ptr_r, ptr_i, (int)l1-(int)l2);
              cri_mul(ptr_r, ptr_i,
                      *ptr_r, *ptr_i, God->rel[iod2], God->iel[iod2]);
              cri_mul(ptr_r, ptr_i,
                      *ptr_r, *ptr_i, Tl->rel[l2+1], Tl->iel[l2+1]);
              iod2++;
            }
          }
          else
          {
            if(!odd1)
            {
              cri_powi(ptr_r, ptr_i, (int)l1-(int)l2);
              cri_mul(ptr_r, ptr_i,
                      *ptr_r, *ptr_i, Gev->rel[iev2], Gev->iel[iev2]);
              cri_mul(ptr_r, ptr_i,
                      *ptr_r, *ptr_i, Tl->rel[l2+1], Tl->iel[l2+1]);
              iev2++;
            }
          } /* else */

        } /* m2 */
      } /* l2 */
    } /* m1 */
  } /* l1 */
 
  matfree(Gev);
  matfree(God);
 
#if CONTROL
  CONTROL_MSG(CONTROL, "\nTii: \n");
  matshowabs(Tii);
#endif
 
  return(Tii);
} /* end of function leed_ms_tmat_ii */
