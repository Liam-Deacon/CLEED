/*********************************************************************
 *                        MATROW.C
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/23.01.95 - diagonal matrices are implemented.
 *********************************************************************/

/*! \file
 *
 * Extracts a single row from a matrix using the matrow() function.
 */

#include <math.h>  
#include <stdio.h>
#include <stdlib.h>
#include "mat.h"

/*!
 * Extracts a single row (\p row_num ) from a matrix \p M .
 *
 * \param[out] row Pointer to the output matrix (will be created if \c NULL )
 * \param[in] M Input matrix.
 * \param row_num Row number.
 * \return Pointer to the extracted row matrix.
 * \retval \c NULL if failed and #EXIT_ON_ERROR not defined.
 */
mat matrow(mat row, mat M, int row_num)
{
  real *ptr_r, *ptr_M, *ptr_end;
  
  /* Check input matrix and row number */

  /* check validity of the input matrices M1 and M2 */
  if (matcheck(M) < 1)
  {
    ERROR_MSG("invalid input matrix\n");
    ERROR_RETURN(NULL);
  }

  if ( (row_num > M->rows) || (row_num < 1) )
  {
    ERROR_MSG("invalid row number\n");
    ERROR_RETURN(NULL);
  }

  /* Extract row */
  row = matalloc(row, 1, M->cols, M->num_type);

  if ( (M->mat_type == MAT_NORMAL) || (M->mat_type == MAT_SQUARE) )
  {
    switch(M->num_type)
    {
      case(NUM_REAL):
      {
        for (ptr_M = M->rel + M->cols*(row_num - 1) + 1,
             ptr_r = row->rel + 1, ptr_end = ptr_M + M->cols;
             ptr_M <= ptr_end; ptr_r ++, ptr_M ++)
        {
          *ptr_r = *ptr_M;
        }

        break;
      } /* case REAL */

      case(NUM_COMPLEX):
      {
        /* first real part */
        for (ptr_M = M->rel + M->cols*(row_num - 1) + 1,
             ptr_r = row->rel + 1, ptr_end = ptr_M + M->cols;
             ptr_M <= ptr_end; ptr_r ++, ptr_M ++)
        {
          *ptr_r = *ptr_M;
        }

        /* now imaginary part */
        for (ptr_M = M->iel + M->cols*(row_num - 1) + 1,
             ptr_r = row->iel + 1, ptr_end = ptr_M + M->cols;
             ptr_M <= ptr_end; ptr_r ++, ptr_M ++)
        {
          *ptr_r = *ptr_M;
        }

        break;
      } /* case CLEED_COMPLEX */
    }   /* switch */
  }     /* matrix type is not diagonal */

  else if (M->mat_type == MAT_DIAG)
  {
    switch(M->num_type)
    {
      case(NUM_REAL):
      {
        for ( ptr_r = row->rel + 1, ptr_end = ptr_r + M->cols;
              ptr_r <= ptr_end; ptr_r ++)
        {
          *ptr_r = 0.;
        }

        *(row->rel + row_num) = *(M->rel + row_num);
        break;
      } /* case REAL */

      case(NUM_COMPLEX):
      {
        /* first real part */
        for ( ptr_r = row->rel + 1, ptr_end = ptr_r + M->cols;
              ptr_r <= ptr_end; ptr_r ++)
        {
          *ptr_r = 0.;
        }

        *(row->rel + row_num) = *(M->rel + row_num);

        /* now imaginary part */
        for ( ptr_r = row->iel + 1, ptr_end = ptr_r + M->cols;
              ptr_r <= ptr_end; ptr_r ++)
        {
          *ptr_r = 0.;
        }

        *(row->iel + row_num) = *(M->iel + row_num);

        break;
      } /* case CLEED_COMPLEX */
    }   /* switch */
  }     /* matrix type is diagonal */

  else /* neither square nor normal, nor diagonal matrix */
  {
    /* matrix type is not implemented! */
    ERROR_MSG("matrix type 0x%x not implemented\n", M->mat_type);
    matfree(row);
    ERROR_RETURN(NULL);
  }

  return(row);
}  /* end of function matrow */
