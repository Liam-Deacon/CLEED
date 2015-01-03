/*********************************************************************
 *                        MATDIFF.C
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/18.01.95 - diagonal matrices are not implemented!
 *********************************************************************/

/*! \file
 *
 * Implements matdiff() function to compute the difference between two matrices.
 */

#include <math.h>  
#include <stdio.h>
#include <stdlib.h>
#include "mat.h"

/*!
 * Calculates the difference between two matrices, i.e. the sum of the moduli
 * of the difference between the matrix elements of \p M1 and \p M2 .
 *
 * \param[in] M1 Pointer to first matrix for comparison.
 * \param[in] M2 Pointer to second matrix for comparison.
 * \return The sum of the moduli of the difference between the matrix elements.
 * \retval -1. if function was unsuccessful.
 */
real matdiff(const mat M1, const mat M2)
{
  real diff;
  real *ptr_1, *ptr_2, *ptr_end;
  long int nn;
  
  /* Check input matrix */
  
  /* check validity of the input matrices M1 and M2 */
  if ( (matcheck(M1) < 1) || (matcheck(M2) < 1) )
  {
    ERROR_MSG("invalid input matrix\n");
    ERROR_RETURN(-1.);
  }

  if ((M1->cols != M2->cols) || (M1->rows != M2->rows) )
  {
    ERROR_MSG("matrix dimensions do not match\n");
    ERROR_RETURN(-1.);
  }

  /* Calculate difference for valid matrix types */
  diff = 0.;

  if ( ( (M1->mat_type == MAT_NORMAL) || (M1->mat_type == MAT_SQUARE) ) &&
       ( (M1->mat_type == MAT_NORMAL) || (M1->mat_type == MAT_SQUARE) ) )
  {
    nn = M1->cols * M1->cols;
    switch(M1->num_type)
    {
      case(NUM_REAL):
      {
        for (ptr_1 = M1->rel + 1, ptr_2 = M2->rel + 1, ptr_end = M1->rel + nn;
             ptr_1 <= ptr_end; ptr_1 ++, ptr_2 ++)
        {
          diff += R_fabs(*ptr_1 - *ptr_2);
        }
        break;
      } /* case REAL */

      case(NUM_COMPLEX):
      {
        /* first calculate difference of real part */
        for (ptr_1 = M1->rel + 1, ptr_2 = M2->rel + 1, ptr_end = M1->rel + nn;
             ptr_1 <= ptr_end; ptr_1 ++, ptr_2 ++)
        {
          diff += R_fabs(*ptr_1 - *ptr_2);
        }

        /* add difference of imaginary part */
        for (ptr_1 = M1->iel + 1, ptr_2 = M2->iel + 1, ptr_end = M1->iel + nn;
             ptr_1 <= ptr_end; ptr_1 ++, ptr_2 ++)
        {
          diff += R_fabs(*ptr_1 - *ptr_2);
        }
        break;
      } /* case COMPLEX */
    } /* switch */
  } /* matrix type is not diagonal */
  else /* one matrix is diagonal */
  {
    /* diagonal matrices are not implemented! */
    ERROR_MSG("diagonal matrices not implemented\n");
    ERROR_RETURN(-1.);
  }

  return(diff);
} /* end of function matdiff */
