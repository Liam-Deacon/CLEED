/*********************************************************************
 *                        MATABS.C
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
 * Implements matabs() function to calculate modulus of a matrix
 * (sum of moduli of elements).
 */

#include <math.h>  
#include <stdio.h>
#include <stdlib.h>

#include "mat.h"

/*!
 * Calculates the modulus of matrix \p M , i.e. the sum of the moduli of the
 * matrix elements.
 *
 * \param[in] M Pointer to input matrix.
 * \return modulus of \p M
 * \retval -1. if function was unsuccessful and #EXIT_ON_ERROR not defined.
 */
real matabs(const mat M)
{
  real mabs;
  real *ptr_r, *ptr_i, *ptr_end;
  long int nn;
  
  /* Check input matrix */
  
  /* check validity of the input matrices M */
  if (matcheck(M) < 0)
  {
    ERROR_MSG("invalid input matrix\n");
    ERROR_RETURN(-1.);
  }

  /* Calculate modulus for valid matrix types */
  mabs = 0.;

  if ( (M->mat_type == MAT_NORMAL) || (M->mat_type == MAT_SQUARE) )
  {
    nn = M->cols * M->cols;
    switch(M->num_type)
    {
      case(NUM_REAL):
      {
        for (ptr_r = M->rel + 1, ptr_end = M->rel + nn;
             ptr_r <= ptr_end; ptr_r ++)
        {
          mabs += R_fabs(*ptr_r);
        }
        break;
      } /* case REAL */

      case(NUM_COMPLEX):
      {
        for (ptr_r = M->rel + 1, ptr_i = M->iel + 1, ptr_end = M->rel + nn;
             ptr_r <= ptr_end; ptr_r ++, ptr_i ++)
        {
          mabs += R_cabs(*ptr_r, *ptr_i);
        }
        break;
      } /* case COMPLEX */
    } /* switch */
  } /* matrix type is not diagonal */
  else  if (M->mat_type == MAT_DIAG)
  {
    nn = M->cols;
    switch(M->num_type)
    {
      case(NUM_REAL):
      {
        for (ptr_r = M->rel + 1, ptr_end = M->rel + nn;
             ptr_r <= ptr_end; ptr_r ++)
        {
          mabs += R_fabs(*ptr_r);
        }
        break;
      } /* case REAL */

      case(NUM_COMPLEX):
      {
        for (ptr_r = M->rel + 1, ptr_i = M->iel + 1, ptr_end = M->rel + nn;
             ptr_r <= ptr_end; ptr_r ++, ptr_i ++)
        {
          mabs += R_cabs(*ptr_r, *ptr_i);
        }
        break;
      } /* case COMPLEX */
    } /* switch */
  } /* diag. matrix */
  else /* not a valid matrix type */
  {
    ERROR_MSG("%d not a valid matrix type\n",
            M->mat_type);
    ERROR_RETURN(-1.);
  }

  return(mabs);
}  /* end of function matabs */
