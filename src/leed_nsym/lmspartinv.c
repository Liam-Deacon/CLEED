/*********************************************************************
 *                      LMSPARTINV.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *  GH/07.08.95 - creation
 *********************************************************************/

/*! \file
 *
 * Invert giant scattering matrix by partitioning using ms_partinv() function.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>

#include "leed.h"

/*!
 * Inverts the giant scattering matrix by partitioning.
 *
 * The function follows the formulae given in Num. Rec. (2.7.22 ff/p. 77).
 * The inverse of the matrix
 * \code
 *           Mbg  = (UL UR)
 *                  (LL LR)
 * \endcode
 * Which can be written as:
 * \code
 *           Minv = (P Q)
 *                  (R S)
 * \endcode
 * where the sub-matrices P, Q, R, and S have the same dimensions as UL, UR,
 * LL, and LR, respectively. They can be calculated as follows:
 *
 *        S = [LR - LL*(UL^-1)*UR ]^-1
 *        R = -S* LL*(UL^-1)
 *        Q = -(UL^-1)*UR * S
 *        P = (UL^-1) + (UL^-1)*UR * S * LL*(UL^-1)
 *
 * If the first (\p first_atoms ) atoms are in the same plane parallel to the
 * surface, the upper left sub-matrix UL (dimension: first_atoms *(l_max +1)^2)
 * can be block-diagonalized into two blocks with even (l1+m1), (l2+m2)
 * (dimension: first_atoms * (l_max + 1)*(l_max + 2)/2) and odd (l1+m1),
 * (l2+m2) (dimension: first_atoms * (l_max +1 ) * l_max/2).
 * Thus, UL can be inverted by inverting the two blocks which requires less
 * cpu time. There are no savings in the other matrix operations of the above
 * formulae, therefore this method saves more time the more atoms are in the
 * same plane.
 *
 * First UL is decomposed into two matrices with even (l1+m1), (l2+m2), Maux_a
 * and odd (l1+m1), (l2+m2), Maux_b. Maux_a and Maux_b are inverted separately
 * and reinserted into UL which has now become (UL^-1).

 * If first_atoms *(l_max +1)^2 is equal to the dimensions of Mbg (i.e. all
 * atoms are in the same plane) (UL^-1) is identical to the inverse of Mbg and
 * the function will return (UL^-1). If {first_atoms *(l_max +1)^2} is smaller
 * than the dimensions of Mbg (i.e. not all atoms are in the same plane) S, R,
 * Q, and P are calculated according to the above formulae, whereby
 * Maux_a = LL*(UL^-1) and Maux_b = (UL^-1)*UR are stored as intermediate
 * results in order to be reused.
 *
 * \param[out] Minv Pointer to the inverse of the input matrix.
 * \param[in] Mbg Pointer to the giant scattering matrix with dimension:
 * \f$ N \times ((l_{max} + 1)^2 \f$
 * \param first_atoms Number of atoms which are in the same plane.
 * \param l_max Maximum angular momentum quantum number \f$ l_{max} \f$ .
 * \return Pointer to the inverse of the giant scattering matrix.
 * \retval \c NULL if function was unsuccessful.
 * \see matalloc() , matfree() , matcheck() , matext() , matins() , matinv()
 * and matmul() .
 */
mat ms_partinv(mat Minv, const mat Mbg, size_t first_atoms, size_t l_max)
{
  size_t i_atoms_1, i_atoms_2;
  size_t l1, l2;
  int m1, m2;

  int iaux;

  int iev2;
  int iod2;
  int odd1;

  real *ptr_1, *ptr_2, *ptr_end;

  mat Maux_a = NULL, Maux_b = NULL;
  mat UL = NULL, UR = NULL, LL = NULL, LR = NULL;

  /* Extract UL (upper left) from Mbg and allocate Maux_a, Maux_b. */
  iaux = first_atoms * (l_max + 1)*(l_max + 1);
  UL = matextract(UL, Mbg, 1, iaux, 1, iaux);

  iaux = first_atoms * (l_max + 1) * (l_max + 2)/2;
  Maux_a = matalloc( Maux_a, iaux, iaux, NUM_COMPLEX);
  iaux = first_atoms * (l_max + 1) * l_max/2;
  Maux_b = matalloc( Maux_b, iaux, iaux, NUM_COMPLEX);
 
  /* Loop over (l1,m1),(l2,m2): Set up  Maux_a and Maux_b.
   *
   * - UL separates into two blocks with even (l1+m1), (l2+m2) and odd
   *   (l1+m1), (l2+m2) which are stored in Maux_a and Maux_b, respectively.
   */
  for(i_atoms_1 = 0, iev2 = 1, iod2 = 1, ptr_1 = UL->rel+1, ptr_2 = UL->iel+1;
      i_atoms_1 < first_atoms; i_atoms_1 ++)
  {
    for(l1 = 0; l1 <= l_max; l1 ++)
    {
      for(m1 = -l1; m1 <= l1; m1 ++)
      {
        odd1 = ODD(l1+m1);
        for(i_atoms_2 = 0; i_atoms_2 < first_atoms; i_atoms_2 ++)
        {
          for(l2 = 0; l2 <= l_max; l2 ++)
          {
            for(m2 = -l2; m2 <= l2; m2 ++, ptr_1 ++, ptr_2 ++)
            {
              if(ODD(l2+m2))
              {
                if(odd1 != 0)
                {
                  Maux_b->rel[iod2] = *ptr_1;
                  Maux_b->iel[iod2] = *ptr_2;
                  iod2++;
                }
              }
              else
              {
                if(odd1 == 0)
                {
                  Maux_a->rel[iev2] = *ptr_1;
                  Maux_a->iel[iev2] = *ptr_2;
                  iev2++;
                }
              } /* else */
            } /* m2 */
          } /* l2 */
        } /* i_atoms_2 */
      } /* m1 */
    } /* l1 */
  } /* i_atoms_1 */
 
  /* Matrix inversion: (Maux_a/od)^-1 */
  Maux_a = matinv(Maux_a, Maux_a);
  Maux_b = matinv(Maux_b, Maux_b);
 
  /* Copy (Maux_a)^-1 and (Maux_b)^-1 back into UL in the natural order. */
  for(i_atoms_1 = 0, iev2 = 1, iod2 = 1, ptr_1 = UL->rel+1, ptr_2 = UL->iel+1;
      i_atoms_1 < first_atoms; i_atoms_1 ++)
  {
    for(l1 = 0; l1 <= l_max; l1 ++)
    {
      for(m1 = -l1; m1 <= l1; m1 ++)
      {
        odd1 = ODD(l1+m1);
        for(i_atoms_2 = 0; i_atoms_2 < first_atoms; i_atoms_2 ++)
        {
          for(l2 = 0; l2 <= l_max; l2 ++)
          {
            for(m2 = -l2; m2 <= l2; m2 ++, ptr_1 ++, ptr_2 ++)
            {
              if(ODD(l2+m2))
              {
                if(odd1 != 0 )
                {
                  *ptr_1 = Maux_b->rel[iod2];
                  *ptr_2 = Maux_b->iel[iod2];
                  iod2++;
                }
                else
                {
                  *ptr_1 = 0.;
                  *ptr_2 = 0.;
                }
              }
              else
              {
                if(odd1 == 0)
                {
                  *ptr_1 = Maux_a->rel[iev2];
                  *ptr_2 = Maux_a->iel[iev2];
                  iev2++;
                }
                else
                {
                  *ptr_1 = 0.;
                  *ptr_2 = 0.;
                }
              } /* else */
            } /* m2 */
          } /* l2 */
        } /* i_atoms_2 */
      } /* m1 */
    } /* l1 */
  } /* i_atoms_1 */

  /* If all the atoms are in the same plane (i.e. first_atoms * (l_max + 1)^2
   * is equal to the dimension of Mbg) the inversion is already complete.
   * => Return here.
   */
  iaux = first_atoms * (l_max + 1)*(l_max + 1);
  if(iaux == Mbg->cols)
  {
    CONTROL_MSG(CONTROL, "All atoms are in the same plane.\n");

    matfree(Maux_a);
    matfree(Maux_b);

    Minv = matcopy(Minv, UL);
    matfree(UL);

    return(Minv);
  }

  /* Calculate submatrices of the inverse of Mbg according to:
   *
   * Mbg = (UL UR)  Minv = (Mbg)^-1 = (P Q)
   *       (LL LR)                    (R S)
   *
   * where:
   *       S = (LR - (LL*UL^-1)*UR )^-1
   *       R = -S*(LL*UL^-1)
   *       Q = -(UL^-1)*UR * S
   *       P = (UL^-1) + (UL^-1)*UR * S * (LL*UL^-1)
   */

  /* First extract UR (upper right), LL (lower left), and LR (lower right)
   * from Mbg, then allocate Maux_a, Maux_b. */
  iaux = first_atoms * (l_max + 1)*(l_max + 1);
  UR = matextract(UR, Mbg, 1, iaux, iaux+1, Mbg->cols);
  LL = matextract(LL, Mbg, iaux+1, Mbg->rows, 1, iaux);
  LR = matextract(LR, Mbg, iaux+1, Mbg->rows, iaux+1, Mbg->cols);

  CONTROL_MSG(CONTROL, "\n\tUL(%d x %d) UR(%d x %d)\n"
              "\t\tLL(%d x %d) LR(%d x %d)\n",
                  UL->rows, UL->cols, UR->rows, UR->cols,
                  LL->rows, LL->cols, LR->rows, LR->cols);

  /* Maux_a = (LL*UL^-1)
   * Maux_b = (LL*UL^-1)*UR ) = Maux_a * UR
   */
  Maux_a = matmul(Maux_a, LL, UL);
  Maux_b = matmul(Maux_b, Maux_a, UR);

  /* Maux_b = -(LR - (LL*UL^-1)*UR) = Maux_b - LR (first real then imag. part)
   * LR = Maux_b^-1 = -S
   */
  iaux = LR->cols * LR->rows;
  for(ptr_1 = LR->rel+1, ptr_2 = Maux_b->rel+1, ptr_end = LR->rel+iaux;
      ptr_1 <= ptr_end; ptr_1 ++, ptr_2 ++)
  {
    *ptr_2 -= *ptr_1;
  }
   
  for(ptr_1 = LR->iel+1, ptr_2 = Maux_b->iel+1, ptr_end = LR->iel+iaux;
      ptr_1 <= ptr_end; ptr_1 ++, ptr_2 ++)
  {
    *ptr_2 -= *ptr_1;
  }

  LR = matinv(LR, Maux_b);

  /* Maux_b = (UL^-1)*UR
   *   R -> LL = - S * (LL*UL^-1) = LR * Maux_a
   *   Q -> UR = - (UL^-1)*UR * S = Maux_b * LR
   */
  Maux_b = matmul(Maux_b, UL, UR);

  LL = matmul(LL, LR, Maux_a);
  UR = matmul(UR, Maux_b, LR);

  /* P -> UL = (UL^-1) + (UL^-1)*UR * S * (LL*UL^-1)
   *         = UL - Maux_b * LL
   */
  Maux_b = matmul(Maux_b, Maux_b, LL);
 
  iaux = UL->cols * UL->rows;
  for(ptr_1 = UL->rel+1, ptr_2 = Maux_b->rel+1, ptr_end = UL->rel+iaux;
      ptr_1 <= ptr_end; ptr_1 ++, ptr_2 ++)
  {
    *ptr_1 -= *ptr_2;
  }
  
  for(ptr_1 = UL->iel+1, ptr_2 = Maux_b->iel+1, ptr_end = UL->iel+iaux;
      ptr_1 <= ptr_end; ptr_1 ++, ptr_2 ++)
 { 
    *ptr_1 -= *ptr_2;
 }

  /* S -> LR = -LR */
  iaux = LR->cols * LR->rows;
  for(ptr_1 = LR->rel+1, ptr_2 = LR->iel+1, ptr_end = LR->rel+iaux;
      ptr_1 <= ptr_end; ptr_1 ++, ptr_2 ++)
  {
    *ptr_1 = -*ptr_1;
    *ptr_2 = -*ptr_2;
  }

  matfree(Maux_a);
  matfree(Maux_b);

  /* Insert UL (upper left), UR (upper right), LL (lower left), and
   * LR (lower right) into Minv. */
  Minv = matalloc(Minv, Mbg->rows, Mbg->cols, NUM_COMPLEX);

  iaux = first_atoms * (l_max + 1)*(l_max + 1) + 1;
  Minv = matins(Minv, UL, 1, 1);
  Minv = matins(Minv, UR, 1, iaux);
  Minv = matins(Minv, LL, iaux, 1);
  Minv = matins(Minv, LR, iaux, iaux);

  /* clean up */
  matfree(UL);
  matfree(UR);
  matfree(LL);
  matfree(LR);

  return(Minv);
} /* end of function ms_partinv */
