/*********************************************************************
 *                        MATTRACE.C
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/07.06.94 - Creation
 *********************************************************************/

/*! \file
 *
 * Implements matrix trace for a square matrix
 */

#include <math.h>  
#include <stdio.h>

#include "mat.h"


/*!
 * Calculates the trace \f$ \text{Tr} \f$ of a square matrix \p M
 *
 * For a square matrix \f$ \textbf{M} \f$ the trace is the sum of
 * the diagonal elements:
 *
 * \f[ \text{Tr}(\textbf{M}) = \sum_{i=1}^n a_{ii} \f]
 *
 * \param[out] tra Pointer to the trace
 * \param[in] M Pointer to the input matrix.
 * \return Integer code for success.
 * \retval 0 if the function is successful.
 * \retval -1 if the function is unsuccessful.
 */
int mattrace(mat tra, const mat M)
{
  real *ptr, *ptr_end;
  size_t nn;
  
  if ((M == NULL) || (M->mat_type != MAT_SQUARE) )
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (mattrace): matrix has the wrong format\n");
    #endif

    return(-1);
  }
  else
  {

    tra->num_type = M->num_type;
    tra->mat_type = MAT_SCALAR;
    tra->cols = 1;
    tra->rows = 1;

    nn = M->cols * M->cols;

    switch(M->num_type)
    {
      case(NUM_REAL):
      {
        *(tra->rel) = 0.;
        for (ptr = M->rel + 1, ptr_end = M->rel + nn; ptr <= ptr_end; ptr++ )
        {
          *(tra->rel) += *ptr;
        }
        break;
      } /* case REAL */

      case(NUM_COMPLEX):
      {
        *(tra->rel) = 0.;
        *(tra->iel) = 0.;
        for (ptr = M->rel + 1, ptr_end = M->rel + nn; ptr <= ptr_end; ptr++ )
        {
          *(tra->rel) += *ptr;
        }

        for (ptr = M->iel + 1, ptr_end = M->iel + nn; ptr <= ptr_end; ptr++ )
        {
          *(tra->rel) += *ptr;
        }
        break;
      } /* case COMPLEX */
    }  /* switch */
  }   /* else */

  return(0);
}  /* end of function mattrace */
