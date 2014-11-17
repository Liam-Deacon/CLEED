/*********************************************************************
 *                        MATCLU.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/20.07.97 - include output (inv_r/i) in the parameter list in
 *                 order to avoid unfreed memory allocation.
 * GH/22.09.00 - include malloc.h at the top of file
 *********************************************************************/

/*! \file
 *
 * Implements c_ludcmp() , c_luinv() and c_lubksb() functions for
 * performing LU decomposition.
 *
 * \note The routines contained in this file are modified from
 * Numerical Recipes in C.
 */

#include <math.h>
#include <stdio.h>
#include <malloc.h>

#include "mat.h"

#define TINY 1.0e-20;

#define CAB2RI(a,b)  (a)*(a) + (b)*(b)

/*!
 * Perform LU decomposition (first step of matrix inversion) for a
 * complex matrix \f$ \textbf{A} = A_r + i A_i \f$ .
 *
 * Given a \f$ N \times N \f$ matrix \f$ \textbf{A} \f$ this routine replaces
 * it by the LU decomposition of a row-wise permutation of itself.
 * \f$ \textbf{A} \f$ is output in LU arrangement (NR eq. 2.3.14 )
 *
 * This routine is used in combination with lubksb() to solve linear equations.
 *
 * \param ar Pointer to the real part of the matrix to be inverted.
 * \param ai Pointer to the imaginary part of the matrix to be inverted.
 * \param indx Pointer to vector which records the row permutation effected
 * by the partial pivoting (must be pre-allocated with the correct size).
 * \param n Dimension of matrix \p ar and \p ai
 * \return +/- 1 depending on whether the number of row interchanges was even
 * or odd, respectively.
 * \retval 0 if the function is unsuccessful.
 * \see lubksb()
 */
int c_ludcmp(real *ar, real *ai, size_t *indx, size_t n)
{
  int d;

  int i_r, i_c;
  int i_max;

  real big, dum, sumr, sumi;
  real *vv;                  /* vv stores the implicit scaling of each row */
  real *ptrr1, *ptrr2;       /* used as counters in the innermost loops */
  real *ptri1, *ptri2;       /* used as counters in the innermost loops */
  real *ptr_end;             /* used as counters in the innermost loops */

  vv = (real *)malloc( (n+1) * sizeof(real) );
  d = i_max = 1;            /* initialise */

  /* get implicit scaling information */
  for (i_r = 1; i_r <= n; i_r ++ )  /* loop over rows */
  {
    big = 0.0;

    /* loop over cols */
    for ( ptrr1 = ar + (i_r - 1)*n + 1, ptri1 = ai + (i_r - 1)*n + 1,
          ptr_end = ar + (i_r -1)*n + n;
          ptrr1 <= ptr_end; ptrr1 ++, ptri1 ++)
    {
      if ((dum = R_cabs(*ptrr1, *ptri1)) > big) big = dum;
    }
    if (IS_EQUAL_REAL(big, 0.0))
    {
      #ifdef ERROR
      fprintf(STDERR, "*** error (ludcmp): Singular matrix \n");
      #endif

      return(0);
    }
    vv[i_r]=1.0/big;
  }
  for (i_c = 1; i_c <= n; i_c++)  /* loop over columns in Crout's method */
  {                               /* NR eq 2.3.12 except for i_r == i_c */
    for (i_r = 1; i_r < i_c; i_r ++)
    {
      sumr = *(ar + (i_r - 1)*n + i_c);
      sumi = *(ai + (i_r - 1)*n + i_c);
   
      for (ptrr1 = ar + (i_r - 1)*n + 1, ptri1 = ai + (i_r - 1)*n + 1,
           ptrr2 = ar + i_c, ptri2 = ai + i_c,
           ptr_end  = ar + (i_r - 1)*n + i_r ;
           ptrr1 < ptr_end; ptrr1 ++, ptrr2 += n, ptri1 ++, ptri2 += n)
      {
        sumr -= (*ptrr1 * *ptrr2) - (*ptri1 * *ptri2);
        sumi -= (*ptrr1 * *ptri2) + (*ptri1 * *ptrr2);
      }
           
      *(ar + (i_r - 1)*n + i_c) = sumr;
      *(ai + (i_r - 1)*n + i_c) = sumi;
    } /* for i_r */

    /* Initialize for the search for largest pivot element
     * This is i_r = i_c for NR eq 2.3.12 and
     * i_r = i_c +1, ..., n for NR eq 2.3.13
     */
    big = 0.0;
    for (i_r = i_c; i_r <= n; i_r ++)
    {
      sumr = *(ar + (i_r - 1)*n + i_c);
      sumi = *(ai + (i_r - 1)*n + i_c);

      for (ptrr1 = ar + (i_r - 1)*n + 1, ptri1 = ai + (i_r - 1)*n + 1,
           ptrr2 = ar + i_c, ptri2 = ai + i_c,
           ptr_end  = ar + (i_r - 1)*n + i_c;
           ptrr1 < ptr_end; ptrr1 ++, ptrr2 += n, ptri1 ++, ptri2 += n)
      {
        sumr -= (*ptrr1 * *ptrr2) - (*ptri1 * *ptri2);
        sumi -= (*ptrr1 * *ptri2) + (*ptri1 * *ptrr2);
      }

      *(ar + (i_r - 1)*n + i_c) = sumr;
      *(ai + (i_r - 1)*n + i_c) = sumi;

      /* Figure of merit for the pivot: is it better than the best so far? */
      if ( ( dum = vv[i_r] * R_cabs(sumr,sumi) )  >= big)
      {
        big = dum;
        i_max = i_r;
      }
    } /* for i_r */

    if (i_c != i_max)      /* Do we need to interchange rows? */
    {
      for (ptrr1 = ar + (i_max - 1)*n + 1, ptri1 = ai + (i_max - 1)*n + 1,
           ptrr2 = ar + (i_c  - 1)*n + 1,  ptri2 = ai + (i_c  - 1)*n + 1,
           ptr_end  = ar +  i_max*n;
           ptrr1 <= ptr_end; ptrr1 ++, ptrr2 ++, ptri1 ++, ptri2 ++)
      {
        dum = *ptrr1; *ptrr1 = *ptrr2; *ptrr2 = dum;
        dum = *ptri1; *ptri1 = *ptri2; *ptri2 = dum;
      }

      d = -d;             /* change the parity of d */
      vv[i_max] = vv[i_c];  /* Also interchange scale factor */
    }  /* if i_c != i_max */
    indx[i_c]=i_max;

    /* If the pivot element is zero the matrix is singular (at least to the
     * precision of the algorithm). For some applications on singular matrices
     * it is desirable to substitute TINY to zero.
     */
    if( IS_EQUAL_REAL(*(ar + (i_c - 1)*n + i_c), 0.0) &&
        IS_EQUAL_REAL(*(ai + (i_c - 1)*n + i_c), 0.0))
    {
      *(ar + (i_c - 1)*n + i_c) = TINY;     /* set only a small real part */
    }

    /* Now, finally, divide by the pivot element. */
    if (i_c != n)
    {
      /* sum = 1/a(i_c, i_c) */
      dum = CAB2RI( *(ar + (i_c - 1)*n + i_c), *(ai + (i_c - 1)*n + i_c) );
      sumr =   (*(ar + (i_c - 1)*n + i_c)) / dum;
      sumi = - (*(ai + (i_c - 1)*n + i_c)) / dum;

      for (ptrr1 = ar + i_c*n + i_c, ptri1 = ai + i_c*n + i_c,
           ptr_end = ar + n*n + i_c;
           ptrr1 < ptr_end; ptrr1 += n, ptri1 += n)
      {
        dum    = (*ptrr1 * sumr) - (*ptri1 * sumi);
        *ptri1 = (*ptrr1 * sumi) + (*ptri1 * sumr);
        *ptrr1 = dum;
      }
    } /* if i_c != n */
  } /* for i_c */

  free(vv);

  return(d);
} /* end of function c_ludcmp */

#undef TINY


/*!
 * Solves the set of n linear equations A*X = b for a complex matrix A.
 *
 * Here A is input but not as the matrix A but rather as its LU decomposition
 * \code lu_r + i*lu_i \endcode determined by the c_ludcmp() routine.
 *
 * \param[out] inv_r Pointer to the real result of the matrix inversion.
 * \param[out] inv_i Pointer to the imaginary result of the matrix inversion.
 * \param[in] lu_r Pointer to the real part of the LU decomposition.
 * \param[in] lu_i Pointer to the imaginary part of the LU decomposition.
 * \param indx Pointer to the permutation vector returned by c_ludcmp()
 * \param n Dimension of matrix \p inv_r and \p inv_i
 * \return C return code indicating function success.
 * \retval 0 if successful.
 * \retval -1 if unsuccessful.
 *
 * \warning \p inv_r and \p inv_i must already be allocated enough memory
 * \code (n*n + 1) \endcode before entering the function.
 *
 * \note \p lu_r and \p lu_iu are not modified by this routine and can be
 * reused for successive calls for solutions of the equation system for
 * with different right-hand sides b.
 *
 * \see c_ludcmp()
 */
int c_luinv(real *inv_r, real *inv_i,
            real *lu_r,  real *lu_i, size_t *indx, size_t n)
{
  int i_c, i_i, i_r;

  real sumr, sumi, dum;

  real *ptrr1, *ptrr2, *ptr_end;  /* pointers used in innermost loops */
  real *ptri1, *ptri2;            /* pointers used in innermost loops */

  /* First, create (n x n) identity matrix (will be output). */
  for (ptrr1 = inv_r + 1, ptrr2 = inv_i + 1, ptr_end = inv_r + n*n;
       ptrr1 <= ptr_end; ptrr1 ++, ptrr2 ++)
  {
    *ptrr1 = *ptrr2 = 0.;
  }
  for (ptrr1 = inv_r + 1, ptr_end = inv_r + n*n;
       ptrr1 <= ptr_end; ptrr1 += n+1)
  {
    *ptrr1 = 1.0;
  }

  /* Back-substitution starts here. */
  for (i_c = 1; i_c <= n; i_c ++)     /* loop over columns of Minv */
  {
    i_i = 0;

    /* When i_i is set to a positive value, it will become the index of the
     * first non-vanishing element of b. We now do the forward substitution
     * (eq. 2.3.6 in NR).
     * The only new wrinkle is to unscramble the permutation as we go.
     */

    /* loop through columns of Minv (i.e. over rows) */
    for (i_r = 1; i_r <= n; i_r ++)
    {
      sumr = *(inv_r + (indx[i_r] - 1)*n + i_c);
      sumi = *(inv_i + (indx[i_r] - 1)*n + i_c);

      *(inv_r + (indx[i_r] - 1)*n + i_c) = *(inv_r + (i_r - 1)*n + i_c);
      *(inv_i + (indx[i_r] - 1)*n + i_c) = *(inv_i + (i_r - 1)*n + i_c);

      if (i_i)
      {
        for (ptrr1 = lu_r + (i_r - 1)*n + i_i, ptri1 = lu_i + (i_r - 1)*n + i_i,
             ptrr2 = inv_r+ (i_i - 1)*n + i_c, ptri2 = inv_i+ (i_i - 1)*n + i_c,
             ptr_end = lu_r + (i_r - 1)*n + i_r;
             ptrr1 < ptr_end; ptrr1 ++, ptrr2 += n, ptri1 ++, ptri2 += n)
        {
          sumr -= (*ptrr1 * *ptrr2) - (*ptri1 * *ptri2);
          sumi -= (*ptrr1 * *ptri2) + (*ptri1 * *ptrr2);
        }
      }
      else if( ! IS_EQUAL_REAL(sumr, 0.0) ||
               ! IS_EQUAL_REAL(sumi, 0.0) ) i_i = i_r;
      /* A non-zero element was encountered, from now on we will have to do the
       * sums in the loop above.
       */
      *(inv_r + (i_r - 1)*n + i_c) = sumr;
      *(inv_i + (i_r - 1)*n + i_c) = sumi;
    }

    /* back-substitution (eq. 2.3.7 in NR) */
    for (i_r = n; i_r >= 1; i_r --)
    {
      sumr = *(inv_r + (i_r - 1)*n + i_c);
      sumi = *(inv_i + (i_r - 1)*n + i_c);

      for (ptrr1 = lu_r  + (i_r - 1)*n + i_r + 1,
           ptri1 = lu_i  + (i_r - 1)*n + i_r + 1,
           ptrr2 = inv_r + i_r*n + i_c, ptri2 = inv_i + i_r*n + i_c,
           ptr_end = lu_r + (i_r - 1)*n + n;
           ptrr1 <= ptr_end; ptrr1 ++, ptrr2 += n, ptri1 ++, ptri2 += n )
      {
        sumr -= (*ptrr1 * *ptrr2) - (*ptri1 * *ptri2);
        sumi -= (*ptrr1 * *ptri2) + (*ptri1 * *ptrr2);
      }

      /* Store a component of the solution vector x: b = sum/a(i,i) */
      dum = CAB2RI(*(lu_r + (i_r - 1)*n + i_r), *(lu_i + (i_r - 1)*n + i_r));
      *(inv_r + (i_r - 1)*n + i_c) = (sumr* *(lu_r + (i_r-1)*n + i_r) +
                                      sumi* *(lu_i + (i_r-1)*n + i_r))/dum;
      *(inv_i + (i_r - 1)*n + i_c) = (sumi* *(lu_r + (i_r-1)*n + i_r) -
                                      sumr* *(lu_i + (i_r-1)*n + i_r))/dum;
    }
  } /* for i_c */

  return(0);
} /* end of function c_luinv */


/*!
 * Solves the set of n linear equations A*x = b for a complex matrix A.
 *
 * Here A is input but not as the matrix A but rather as its LU decomposition
 * determined by the routine ludcmp.
 *
 * b is input as the right hand side complex vector b, and will be
 * overwritten by the solution vector X.
 *
 * A and index are not modified by this routine and can be left in place for
 * successive calls with different right-hand sides b.
 *
 * This routine takes into account the possibility that \p br and \p bi
 * will begin with many zero elements, so it is efficient for
 * use in matrix inversion.
 *
 * \param lur Pointer to the real part of the matrix to be inverted.
 * \param lui Pointer to the imaginary part of the matrix to be inverted.
 * \param indx Pointer to the vector which records row permutation as
 * returned by the ludcmp() routine.
 * \param br Pointer to the vector (length \p n ) to be multiplied from the
 * left hand side with the real part of the inverted matrix.
 * \param bi Pointer to the vector (length \p n ) to be multiplied from the
 * left hand side with the imaginary part of the inverted matrix.
 * \param n Dimension of matrices \p lur and \p lui
 * \return C style return code indicating function success.
 * \retval 0 if successful.
 * \retval -1 if unsuccessful.
 */
int c_lubksb(real *lur, real *lui, size_t *indx, real *br,  real *bi, size_t n)
{
  int i_i, i_r;

  real sumr, sumi, dum;
  real *ptrr1, *ptrr2, *ptr_end;  /* pointers used in innermost loops */
  real *ptri1, *ptri2;            /* pointers used in innermost loops */

  i_i = 0;
  /* When i_i is set to a positive value, it will become the index of the first
   * non-vanishing element of b. We now do the forward substitution (eq. 2.3.6
   * in NR). The only new wrinkle is to unscramble the permutation as we go.
   */

  for (i_r = 1; i_r <= n; i_r ++) /* loop through b */
  {
    sumr = *(br + (indx[i_r] - 1)*n + 1);
    sumi = *(bi + (indx[i_r] - 1)*n + 1);

    *(br + (indx[i_r] - 1)*n + 1) = *(br + (i_r - 1)*n + 1);
    *(bi + (indx[i_r] - 1)*n + 1) = *(bi + (i_r - 1)*n + 1);

    if (i_i)
    {
      for (ptrr1 = lur  + (i_r - 1)*n + i_i, ptri1 = lui  + (i_r - 1)*n + i_i,
           ptrr2 = br + (i_i - 1)*n + 1, ptri2 = bi + (i_i - 1)*n + 1,
           ptr_end = lur + (i_r - 1)*n + i_r;
           ptrr1 < ptr_end; ptrr1 ++, ptrr2 += n, ptri1 ++, ptri2 += n)
      {
        sumr -= (*ptrr1 * *ptrr2) - (*ptri1 * *ptri2);
        sumi -= (*ptrr1 * *ptri2) + (*ptri1 * *ptrr2);
      }
    }
    else if( ! IS_EQUAL_REAL(sumr, 0.0) || ! IS_EQUAL_REAL(sumi, 0.0) )
    {
      i_i = i_r;
    }

    /* A non-zero element was encountered, from now on we will have to do the
     * sums in the loop above.
     */
    *(br + (i_r - 1)*n + 1) = sumr;
    *(bi + (i_r - 1)*n + 1) = sumi;
  }

  /* back-substitution (eq. 2.3.7 in NR) */
  for (i_r = n; i_r >= 1; i_r --)
  {
    sumr = *(br + (i_r - 1)*n + 1);
    sumi = *(bi + (i_r - 1)*n + 1);

    for (ptrr1 = lur  + (i_r - 1)*n + i_r + 1,
         ptri1 = lui  + (i_r - 1)*n + i_r + 1,
         ptrr2 = br + i_r*n + 1, ptri2 = bi + i_r*n + 1,
         ptr_end = lur + (i_r - 1)*n + n;
         ptrr1 <= ptr_end; ptrr1 ++, ptrr2 += n, ptri1 ++, ptri2 += n )
    {
      sumr -= (*ptrr1 * *ptrr2) - (*ptri1 * *ptri2);
      sumi -= (*ptrr1 * *ptri2) + (*ptri1 * *ptrr2);
    }

    /* Store a component of the solution vector x: b = sum/a(i,i) */
    dum = CAB2RI(*(lur + (i_r - 1)*n + i_r), *(lui + (i_r - 1)*n + i_r));
    *(br + (i_r - 1)*n + 1) = (sumr* *(lur + (i_r-1)*n + i_r) +
                               sumi* *(lui + (i_r-1)*n + i_r))/dum;
    *(bi + (i_r - 1)*n + 1) = (sumi* *(lur + (i_r-1)*n + i_r) -
                               sumr* *(lui + (i_r-1)*n + i_r))/dum;
  }

  return(0);
} /* end of function c_lubksb */
