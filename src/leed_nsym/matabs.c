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
  size_t nn;
  
  /* Check input matrix */
  
  /* check validity of the input matrices M */
  if (matcheck(M) < 0)
  {
    ERROR_MSG("invalid input matrix\n");
    ERROR_RETURN(-1.);
  }

  /* Calculate modulus for valid matrix types */
  mabs = 0.;

  switch( M->mat_type )
  {
    case(MAT_NORMAL): case(MAT_SQUARE):
    {
      nn = M->cols * M->cols;
      switch(M->num_type)
      {
        case(NUM_REAL):
        {
          for (ptr_r = M->rel + 1, ptr_end = M->rel + nn;
              ptr_r <= ptr_end; ptr_r ++)
          {
            mabs += cleed_real_fabs(*ptr_r);
          }
          break;
        } /* case REAL */

        case(NUM_COMPLEX):
        {
          for (ptr_r = M->rel + 1, ptr_i = M->iel + 1, ptr_end = M->rel + nn;
              ptr_r <= ptr_end; ptr_r ++, ptr_i ++)
          {
            mabs += cleed_real_cabs(*ptr_r, *ptr_i);
          }
          break;
        } /* case CLEED_COMPLEX */

        case(NUM_IMAG): case(NUM_MASK): default:
        {
          ERROR_MSG("Unsupported matrix data type (%s)\n", strmtype(M->num_type));
          ERROR_RETURN(-1.);
          break;
        } /* default case */

      } /* switch (M->num_type) */

      break;

    } /* matrix type is not diagonal */

    case(MAT_DIAG):
    {
      nn = M->cols;
      switch(M->num_type)
      {
        case(NUM_REAL):
        {
          for (ptr_r = M->rel + 1, ptr_end = M->rel + nn;
              ptr_r <= ptr_end; ptr_r ++)
          {
            mabs += cleed_real_fabs(*ptr_r);
          }
          break;
        } /* case REAL */

        case(NUM_COMPLEX):
        {
          for (ptr_r = M->rel + 1, ptr_i = M->iel + 1, ptr_end = M->rel + nn;
              ptr_r <= ptr_end; ptr_r ++, ptr_i ++)
          {
            mabs += cleed_real_cabs(*ptr_r, *ptr_i);
          }
          break;
        } /* case CLEED_COMPLEX */

        case(NUM_IMAG): case(NUM_MASK): default:
        {
          ERROR_MSG("Unsupported matrix data type (%s)\n", strmtype(M->num_type));
          ERROR_RETURN(-1.);
          break;
        } /* default case */

      } /* switch (M->num_type) */

      break;

    } /* MAT_DIAG */

    default:/* not a valid matrix type */
    {
      ERROR_MSG("%d not a valid matrix type\n",
          M->mat_type);
      ERROR_RETURN(-1.);
      break;
    }

  } /* switch (M->mat_type) */

  return(mabs);
}  /* end of function matabs */
