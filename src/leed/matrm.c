/*********************************************************************
 *                          MATRM.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/05.06.94
 *********************************************************************/

/*! \file
 *
 * Implements r_sqmul() function for matrix multiplication of square matrices.
 */

#include <math.h>
#include "mat.h"

/*!
 * Perform matrix multiplication for square matrices.
 *
 * \param res pointer to an array of results
 * \param a Pointer to matrix @b A
 * \param b Pointer to matrix @b B
 * \param n The dimension of the matrices @b A and @b B
 * \return pointer to @p res
 * @note enough memory must be allocated for @p res
 */
real *r_sqmul(real *res, real *a, real *b, size_t n)
{
  long unsigned int nn = n*n;
  long unsigned int i_r1, i_c2;

  real *ptrr, *ptr_end;
  register real *ptr1, *ptr2;

  CONTROL_MSG(CONTROL, "entering real loop\n");

  for(ptrr = res + 1, i_r1 = 1; i_r1 < nn; i_r1 += n)
  {
    for(i_c2 = 1; i_c2 <= n; ptrr ++, i_c2 ++ )
    {

      *ptrr = 0.;
      for(ptr1 = a + i_r1, ptr2 = b + i_c2, ptr_end = a + i_r1 + n;
          ptr1 < ptr_end ; ptr1 ++, ptr2 += n)
      {
        *ptrr += *ptr1 * *ptr2;
      }
    }
  }

  return(res);
}
