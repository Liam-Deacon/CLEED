/*********************************************************************
 *                      LSMSTMATNDII.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/11.01.00 - copy from lmstmatii.c
 *   GH/02.05.00 - use transposed of X-matrix: ((1 - Tlm*Gii)^-1)^t
 *   GH/22.09.00
 *   GH/16.07.02 - fix bug cg/gaunt in summation over l3 (Gii)
 *********************************************************************/

/*! \file
 * Implements leed_ms_tmat_md_ii() function for creating multiple scattering
 * matrix for a periodic plane of scatters using a non-diagonal atomic/molecular
 * scattering matrix.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>

#include "leed.h"

/*!
 * Creates the multiple scattering Matrix [(1 - Tl*Giilm)^-1]*Tl for a
 * periodic plane of scatterers.
 *
 *  First create:
 *
 * Gii(l1,m1; l2,m2) =
 *      S[ (-1)^((l1-l2-l3_min)/2 - m2) * L(l3,m3) * C(l3,m3,l1,m1,l2,m2)]
 *
 *   L(l3,m3) = lattice sum
 *   C(l3,m3,l1,m1,l2,m2) = C.G. coefficients.
 *
 * Then compute:
 *  Tii =  Tlm * (1 - Gii * Tlm)^-1
 *
 * Multiply each matrix element with i^(l2-l1).
 *
 * \param[in,out] Tii Pointer to multiple scattering matrix in (l,m)-space.
 * \param[in] Llm Pointer to the lattice sum matrix.
 * \f[ Llm = {(-1)}^m \times 4 \pi * Ylm (\cos(\theta) = 0, \phi = 0) \times
 * sum(R) [ H(1)l(k*|R|) \times \exp( i(-kin*R + m \times \phi(R)) ) ] \f]
 * (in natural order: (l,m) = (0,0); (-1,1); (0,1); (1,1); ...)
 *
 * \param[in] Tlm_in Pointer to the non-diagonal atomic scattering matrix
 * (square) stored in natural order i.e. (l,m) = (0,0); (-1,1); (0,1); (1,1); ...
 * \param l_max Maximum angular momentum quantum number.
 * \return \p Tii = [Tlm (1 - Gii*Tlm)^-1] * i^(l-l')
 */
mat leed_ms_tmat_nd_ii ( mat Tii, const mat Llm, mat Tlm_in, size_t l_max)
{

  size_t l1, l2, l3;
  int m1, m2, m3;
  size_t l3_min, l3_max;

  size_t i3;

  size_t ilm1, ilm2;
  size_t m1u, m2u;

  real faux_r, faux_i;
  real sum_r, sum_i;
  real sign;

  mat Tlm;
  mat Gii;

  size_t n = (l_max + 1)*(l_max + 1);

/* Check matrix dimensions of Tlm_in and compatibilities with l_max.
 * - allocate Tii and Tlm.
 * - check dimension of Tlm_in:
 *    increase or reduce if necessary.
 *    inside the fuction a copy (Tlm) is used.
 * - check number of C.G> coefficients
 */

  /* Allocate Tii and Tlm.  */
  Tlm = NULL;
  Tlm = matalloc( Tlm, n, n, NUM_COMPLEX);

  if (matcheck (Tlm_in) == 1)
  {
    if ((Tlm_in->num_type) == (MAT_SQUARE | NUM_COMPLEX))
    {
      if (Tlm_in->rows < Tlm->rows)
        /* insert Tlm_in in greater Tlm */
      {
        WARNING_MSG("increase dimension of Tlm from %d to %d\n",
                    Tlm_in->rows, Tlm->rows);
        Tlm = matins(Tlm, Tlm_in, 1, 1);
      }
      else if (Tlm_in->rows == Tlm->rows)
      {
        Tlm = matcopy(Tlm, Tlm_in);
      }
      else if (Tlm_in->rows > Tlm->rows)
        /* Reduce dimesions of Tlm_in.
         * Use l for rows and m for columns (1 for Tlm_in, 2 for Tlm).
         * ilm counts elements.
         */
      {
        WARNING_MSG("reduce dimension of Tlm from %d to %d\n",
                    Tlm_in->rows, Tlm->rows);
        for(ilm1 = 1, ilm2 = 1 , l2 = 1; l2 <= Tlm->rows; l2 ++)
        {
          for(m1u = 1, m2u = 1; m1u <= Tlm_in->cols; m1u ++, ilm1 ++)
          {
            if ( m2u <= Tlm->cols)
            {
              Tlm->rel[ilm2] = Tlm_in->rel[ilm1];
              Tlm->iel[ilm2] = Tlm_in->iel[ilm1];
              m2u ++;
              ilm2 ++;
            }
          } /* l2 */
        } /* m1, m2 */
      } /* else if (size) */
    } /* if num_type */
    else
    {
      ERROR_MSG("Tlm does not have the right matrix type (square, complex)\n");
    }
  }
  else ERROR_MSG("Tlm does not point to a matrix\n");

  /* Allocate:
   * - C.G. coefficients for G matix.
   * - memory for G matrix
   */
  if(mk_cg_coef(2*l_max) != 0)
  {
    WARNING_MSG("C.G. coefficients had to be recalculated\n");
  }

  /* Loop over (l1,m1),(l2,m2): Set up  - Gii(l1,m1; l2,m2).
   *
   * Gii(l1,m1; l2,m2) =
   *     S[ (-1)^((l1-l2-l3_min)/2 - m2) * L(l3,m3) * C(l3,m3,l1,m1,l2,m2)]
   *  L(l3,m3) = lattice sum
   *  C(l3,m3,l1,m1,l2,m2) = C.G. coefficients.
   *
   * - Gii(l1,m1; l2,m2)  is symmetric under exchange of (l1,m1) and (l2,m2),
   *  (under most conditions)
   *  therefore only (l2,m2) <= (l1,m1) could be calculated which is,
   *  however not implemented (see comments in m2 loops).
   */
  Gii = NULL;
  Gii = matalloc( Gii, n, n, NUM_COMPLEX);

  for(l1 = 0, ilm1 = 1; l1 <= l_max; l1 ++)
  {
    for(m1 = -(int)l1; m1 <= (int)l1; m1 ++)
    {
      for(l2 = 0; l2 <= l_max; l2 ++)
      {
        for(m2 = -(int)l2; m2 <= (int)l2; m2 ++, ilm1 ++)
        {
          /* Loop over l3 (inner--most loop): calculate the elements of Gii
           * - l3_min: l3 >= |m3|, |l2-l1|.
           * - Only even (l1 + l2 + l3) lead to nonzero C.G.-coefficients
           * => if (l1 + l2 + l3_min) is odd, increment l3_min by 1.
           */

          /* m3 = m2 - m1; */
          m3 = m1 - m2;

          l3_min = (size_t)MAX(abs(m3), abs((int)l2-(int)l1));
          l3_min += (l1 + l2 + l3_min)%2;
          l3_max = l2+l1;

          sum_r = sum_i = 0.;
          if (m3 < 0)
            i3 = l3_min*(l3_min + 1) + (size_t)abs(m3) + 1;
          else
            i3 = l3_min*(l3_min + 1) - (size_t)m3 + 1;

          /* sign = M1P( (l2 - l1 - l3_min)/2 - m2); */
          sign = M1P( (int)(l1 - l2 - l3_min)/2 - m2);

          for(l3 = l3_min; l3 <= l3_max; l3 += 2 )
          {
            /* faux_r = sign*gaunt(l1, m1, l3, m3, l2,m2); */
            faux_r = sign*cg((int)l3,m3, (int)l1,m1, (int)l2,-m2);

            sum_r += Llm->rel[i3] * faux_r;
            sum_i += Llm->iel[i3] * faux_r;

            /* l3 is incremented by 2
             * => l3/2 is incremented by 1; the sign is inverted.
             * => i3 is incremented by 2*(l3+1) + 2*(l3+2) = 4*l3 + 6
             */
            sign = -sign;
            i3 += 4*l3 + 6;
          } /* l3 */

          /* Store -i * sum in Gii (to be multiplied with Tlm). The factor i
           * is a correction necessary to get the same results as in XM (VHT).
           * Don't ask why!
           */
          Gii->rel[ilm1] =  sum_i;
          Gii->iel[ilm1] = -sum_r;

        } /* m2 */
      } /* l2 */
    } /* m1 */
  } /* l1 */

  /* Multiply with Tlm from the r.h.s. : -Gii * Tlm
   * Store in Tii, Gii not needed anymore.
   */
  Tii = matmul(Tii, Gii, Tlm);
  matfree(Gii);

  /* Add the identity matrix: (1 - Gii * Tlm ) and Store in Tii */
  n = Tii->cols*Tii->rows;
  for( ilm1 = 1; ilm1 <= n; ilm1 += Tii->rows + 1)
    Tii->rel[ilm1] += 1.;

  CONTROL_MSG(CONTROL, "(1 - Gii * Tlm ): \n");

  /* Matrix inversion: (1 - Gii * Tlm )^-1 and store in Tii */
  Tii = matinv(Tii, Tii);

  CONTROL_MSG(CONTROL, "(1 - Gii * Tlm )^-1: \n");
 
  /* Multiply with Tl from the l.h.s:
   * Tlm * (1 - Gii * Tlm)^-1
   */
  Tii = matmul(Tii, Tlm, Tii);

  /* Multiply matrix elements with i^(l2-l1).
   * l1 is the index which goes with the Ylm of the outgoing beams (-)
   * l2                  -"-                        incoming beams (+)
   */
  for(l1 = 0, ilm1 = 1; l1 <= l_max; l1 ++)
  {
    for(m1 = -(int)l1; m1 <= (int)l1; m1 ++)
    {
      for(l2 = 0; l2 <= l_max; l2 ++)
      {
        for(m2 = -(int)l2; m2 <= (int)l2; m2 ++, ilm1 ++)
        {
          cri_powi(&faux_r, &faux_i, (int)l2-(int)l1);
          cri_mul(Tii->rel+ilm1, Tii->iel+ilm1, Tii->rel[ilm1], Tii->iel[ilm1],
                  faux_r, faux_i);
        } /* m2 */
      } /* l2 */
    } /* m1 */
  } /* l1 */

  CONTROL_MSG(CONTROL, "Tii = Tlm * (1 - Gii * Tlm)^-1:\n");

  return(Tii);
} /* end of function leed_ms_tmat_nd_ii */
