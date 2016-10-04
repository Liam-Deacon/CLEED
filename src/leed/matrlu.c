/*********************************************************************
 *                        MATRLU.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/20.07.97 - include output (inv) in the parameter list in order
 *                 to avoid unfreed memory allocation.
 *   GH/22.09.00 - include malloc at the top of file
 *********************************************************************/

/*! \file
 *
 * Implements r_ludcmp() , r_luinv() and r_lubksb() for performing LU
 * decomposition.
 */

#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include <errno.h>

#include "mat.h"

#define TINY 1.0e-20;

/*!
 * Performs LU decomposition (first step of matrix inversion).
 *
 * This routine is used in combination with lubksb() to solve linear equations.
 *
 * Given a NxN matrix A this routine replaces it by the LU decomposition
 * of a row-wise permutation of itself. A is output in LU arrangement
 * (Num. Rec. eq. 2.3.14 );
 *
 * \param[in,out] a Pointer to matrix to be inverted (will be modified).
 * \param[out] indx Vector which records the row permutation affected by the
 * partial pivoting (must be allocated with the correct size).
 * \param n Dimension of the matrix,
 * \return +/- 1 depending on whether the number of row interchanges was even
 * or odd, respectively.
 * \retval 0 if function failed.
 * \see lubksb()
 */
int r_ludcmp(real *a, size_t *indx, size_t n)
{
  int d=1;

  size_t i_r, i_c;
  size_t i_max=1;

  real big, dum, sum;
  real *vv = NULL;              /* vv stores the implicit scaling of each row */
  real *ptr1, *ptr2, *ptr_end;  /* used as counters in the innermost loops */

  if ((vv = (real *)calloc((n + 1), sizeof(real))) == NULL)
  {
    ERROR_MSG("could not allocate memory for 'vv'\n");
    exit(ENOMEM);
  }

  /* get implicit scaling information */
  for (i_r = 1; i_r <= n; i_r ++ )  /* loop over rows */
  {
    big = 0.0;
    /* loop over cols */
    for ( ptr1 = a + (i_r - 1)*n + 1, ptr_end = a + (i_r -1)*n + n;
          ptr1 <= ptr_end; ptr1 ++)
    {
      if ((dum = fabs(*ptr1)) > big) big = dum;
    }

    if (IS_EQUAL_REAL(big, 0.0))
    {
      ERROR_MSG("Singular matrix \n");
      return(0);
    }
    vv[i_r]=1.0/big;
  }
  for (i_c = 1; i_c <= n; i_c++)  /* loop over columns in Crout's method */
  {                               /* NR eq 2.3.12 except for i_r == i_c */
    for (i_r = 1; i_r < i_c; i_r ++)
    {
      sum = *(a + (i_r - 1)*n + i_c);
   
      for (ptr1 = a + (i_r - 1)*n + 1,
           ptr2 = a + i_c,
           ptr_end  = a + (i_r - 1)*n + i_r;
           ptr1 < ptr_end;
           ptr1 ++, ptr2 += n)
      {
        sum -= (*ptr1) * (*ptr2);
      }
           
      *(a + (i_r - 1)*n + i_c) = sum;

    } /* for i_r */

    /* Initialize for the search for largest pivot element
     * This is i_r = i_c for NR eq 2.3.12 and
     * i_r = i_c +1, ..., n for NR eq 2.3.13
     */
    big = 0.0;
    for (i_r = i_c; i_r <= n; i_r ++)
    {
      sum = *(a + (i_r - 1)*n + i_c);
 
      for (ptr1 = a + (i_r - 1)*n + 1,
           ptr2 = a + i_c,
           ptr_end  = a + (i_r - 1)*n + i_c;
           ptr1 < ptr_end; ptr1 ++, ptr2 += n)
      {
        sum -= (*ptr1) * (*ptr2);
      }

      *(a + (i_r - 1)*n + i_c) = sum;

      /* Figure of merit for the pivot: is it better than the best so far? */
      if ( ( dum = vv[i_r]*fabs(sum) ) >= big)
      {
        big = dum;
        i_max = i_r;
      }

    } /* for i_r */

    if (i_c != i_max)      /* Do we need to interchange rows? */
    {
      for (ptr1 = a + (i_max - 1)*n + 1,
           ptr2 = a + (i_c  - 1)*n + 1,
           ptr_end  = a +  i_max*n;
           ptr1 <= ptr_end; ptr1 ++, ptr2 ++)
      {
        dum = *ptr1;
        *ptr1 = *ptr2;
        *ptr2 = dum;
      }
      d *= -1;              /* change the parity of d */
      vv[i_max] = vv[i_c];  /* Also interchange scale factor */

    } /* if i_c != i_max */
    indx[i_c] = i_max;

    /* If the pivot element is zero the matrix is singular (at least to the
     * precision of the algorithm). For some applications on singular matrices
     * it is desirable to substitute TINY to zero.
     */
    if ( *(a + (i_c - 1)*n + i_c) == 0.0 ) *(a + (i_c - 1)*n + i_c) = TINY;

    /* Now, finally, divide by the pivot element. */
    if (i_c != n)
    {
      dum=1.0/(*(a + (i_c - 1)*n + i_c));

      for (ptr1 = a + i_c*n + i_c, ptr_end = a + n*n + i_c;
           ptr1 < ptr_end; ptr1 += n)
      {
        *ptr1 *= dum;
      }
    } /* if i_c != n */

  } /* for i_c */

  free(vv);

  return(d);
} /* end of function r_ludcmp */

#undef TINY

/*!
 * Solves the set of n linear equations A*A_1 = I.
 *
 *  Here A is input but not as the matrix A but rather as its LU decomposition
 * determined by the routine ludcmp() . \p indx is input as the permutation
 * vector returned by ludcmp() . b is input as the right hand side vector b,
 * and returns with the solution vector X. A and index are not modified by
 * this routine and can be left in place for successive calls with
 * different right-hand sides b. This routine takes into account the
 * possibility that b will begin with many zero elements, so it is
 * efficient for use in matrix inversion.
 *
 * \param[out] inv Pointer to the inverse matrix.
 * \param[in] lu Pointer to the matrix to be inverted (LU decomposition as
 * returned by r_ludcmp()
 * \param[in,out] indx Vector which records the row permutation as returned
 * by ludcmp()
 * \param n Dimension of matrix.
 * \return Normal C convention integer code representing function success.
 * \retval -1 if function was unsuccessful.
 */
int r_luinv(real *inv, const real *lu, size_t *indx, size_t n)
{
  size_t i, j;
  size_t i_c, i_i, i_r;

  real sum;

  /* First, preset inv as nxn identity matrix. */
  for (i = 1; i <= n*n; i++)
  {
    inv[i] = 0.;
  }
  for (i = 1; i <= n*n; i += n+1)
  {
    inv[i] = 1.0;
  }

  /* Backsubstitution starts here. */
  for (i_c = 1; i_c <= n; i_c ++)     /* loop over columns of inv */
  {
    i_i = 0;
    /* loop through columns of inv (i.e. over rows) */
    for (i_r = 1; i_r <= n; i_r ++)
    {
      sum = inv[(indx[i_r] - 1)*n + i_c];
      inv[(indx[i_r] - 1)*n + i_c] = inv[(i_r - 1)*n + i_c];

      if (i_i)
      {
        for (i = (i_r - 1)*n + i_i, j = (i_i - 1)*n + i_c;
             i < (i_r - 1)*n + i_r;
             i ++, j += n )
        {
          sum -= (lu[i]) * (inv[j]);
        }
      }
      else if ( ! IS_EQUAL_REAL(sum, 0.0)) i_i = i_r;

      inv[(i_r - 1)*n + i_c] = sum;
    }

    for (i_r = n; i_r >= 1; i_r --)
    {
      sum = inv[(i_r - 1)*n + i_c];
      for (i = (i_r - 1)*n + i_r + 1, j = i_r*n + i_c;
           i <= (i_r - 1)*n + n;
           i++, j += n )
      {
        sum -= (lu[i]) * (inv[j]);
      }

      inv[(i_r - 1)*n + i_c] = sum/(lu[(i_r - 1)*n + i_r]);
    }

  } /* for i_c */

 return(0);
} /* end of function r_luinv */

/*!
 * Solves the set of n linear equations A*X = b.
 *
 * Here A is input - not as the matrix A, but rather as its LU decomposition
 * determined by the routine ludcmp() . \p indx is input as the permutation
 * vector returned by ludcmp. b is input as the right hand side vector b, and returns
 * with the solution vector X. A and index are not modified by this routine
 * and can be left in place for successive calls with different right-hand
 * sides b. This routine takes into account the possibility that b will begin
 * with many zero elements, so it is efficient for the use in matrix inversion.
 *
 * \param[in] lu Pointer to the matrix to be inverted by LU decomposition as
 * returned by r_ludcmp()
 * \param[in] indx Pointer to the vector which records the row permutation as
 * returned by ludcmp()
 * \param[in, out] br Pointer to the vector to be multiplied with the inverted
 * matrix.
 * \param n Dimension of the matrix.
 * \return Pointer to the result.
 * \retval \c NULL if the function was unsuccessful.
 */
real *r_lubksb(const real *lu, const size_t *indx, real *br, size_t n)
{
  size_t i, j;
  size_t i_c, i_i, i_r;

  real sum;

  for (i_c = 1; i_c <= n; i_c ++)     /* loop over columns of br */
  {
    i_i = 0;
    /* loop through columns of br (i.e. over rows) */
    for (i_r = 1; i_r <= n; i_r ++)
    {
      sum = br[(indx[i_r] - 1)*n + i_c];
      br[(indx[i_r] - 1)*n + i_c] = br[(i_r - 1)*n + i_c];

      if (i_i)
      {
        for (i = (i_r - 1)*n + i_i, j = (i_i - 1)*n + i_c;
             i < (i_r - 1)*n + i_r; i ++, j += n )
        {
          sum -= (lu[i]) * (br[j]);
        }
      }
      else if ( ! IS_EQUAL_REAL(sum, 0.0)) i_i = i_r;

      br[(i_r - 1)*n + i_c] = sum;
    }

    for (i_r = n; i_r >= 1; i_r --)
    {
      sum = br[(i_r - 1)*n + i_c];
      for (i = (i_r - 1)*n + i_r + 1, j = i_r*n + i_c;
           i <= (i_r - 1)*n + n;
           i ++, j += n )
      {
        sum -= (lu[i]) * (br[j]);
      }

      br[(i_r - 1)*n + i_c] = sum/(lu[(i_r - 1)*n + i_r]);
    }

  } /* for i_c */

  return(br);
} /* end of function r_lubksb */
