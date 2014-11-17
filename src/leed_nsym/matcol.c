/*********************************************************************
 *                      MATCOL.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
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
 * Contains matcol() function for extracting a single column from a matrix.
 */

#include <math.h>  
#include <stdio.h>
#include <stdlib.h>
#include "mat.h"

/*!
 * Extracts a single column (col_num) from a matrix M.
 *
 * \param[out] col Pointer to the output column, will be created, if \c NULL .
 * \param[in] M Pointer to the input matrix.
 * \param col_num number of column.
 * \return Pointer to the output column.
 * \retval \c NULL if function is unsuccessful and #EXIT_ON_ERROR is not defined.
 */
mat matcol(const mat M, size_t col_num)
{
  real *ptr_r, *ptr_M, *ptr_end;
  mat col = NULL;
  
  /* Check input matrix and column number */

  /* check validity of the input matrices M1 and M2 */
  if (matcheck(M) < 1)
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (matcol): invalid input matrix\n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(NULL);
    #endif
  }

  if ( (col_num > M->cols) || (col_num < 1) )
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (matcol): invalid column number\n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(NULL);
    #endif
  }

  /* Extract column */
  col = matalloc(col, M->rows, 1, M->num_type);

  if ( (M->mat_type == MAT_NORMAL) || (M->mat_type == MAT_SQUARE) )
  {
    switch(M->num_type)
    {
      case(NUM_REAL):
      {
        for (ptr_M = M->rel + col_num,
             ptr_r = col->rel + 1, ptr_end = M->rel + M->rows*M->cols;
             ptr_M <= ptr_end; ptr_r ++, ptr_M += M->cols)
        {
          *ptr_r = *ptr_M;
        }

        break;
      } /* case REAL */

      case(NUM_COMPLEX):
      {
        /* first real part */
        for (ptr_M = M->rel + col_num,
             ptr_r = col->rel + 1, ptr_end = M->rel + M->rows*M->cols;
             ptr_M <= ptr_end; ptr_r ++, ptr_M += M->cols)
        {
          *ptr_r = *ptr_M;
        }

        /* now imaginary part */
        for (ptr_M = M->iel + col_num,
             ptr_r = col->iel + 1, ptr_end = M->iel + M->rows*M->cols;
             ptr_M <= ptr_end; ptr_r ++, ptr_M += M->cols)
        {
          *ptr_r = *ptr_M;
        }

        break;
      } /* case COMPLEX */
    } /* switch */
  } /* matrix type is not diagonal */
  else if (M->mat_type == MAT_DIAG)
  {
    switch(M->num_type)
    {
      case(NUM_REAL):
      {
        for ( ptr_r = col->rel + 1, ptr_end = ptr_r + M->rows;
              ptr_r <= ptr_end; ptr_r ++)
        {
          *ptr_r = 0.;
        }
        *(col->rel + col_num) = *(M->rel + col_num);

        break;
      } /* case REAL */

      case(NUM_COMPLEX):
      {
        /* first real part */
        for ( ptr_r = col->rel + 1, ptr_end = ptr_r + M->rows;
              ptr_r <= ptr_end; ptr_r ++)
        {
          *ptr_r = 0.;
        }
        *(col->rel + col_num) = *(M->rel + col_num);

        /* now imaginary part */
        for ( ptr_r = col->iel + 1, ptr_end = ptr_r + M->rows;
              ptr_r <= ptr_end; ptr_r ++)
        {
          *ptr_r = 0.;
        }
        *(col->iel + col_num) = *(M->iel + col_num);

        break;
      } /* case COMPLEX */
    } /* switch */
  } /* matrix type is diagonal */
  else /* neither square nor normal, nor diagonal matrix */
  {
    /* matrix type is not implemented! */
    #ifdef ERROR
    fprintf(STDERR, "*** error (matcol): matrix type 0x%x not implemented\n",
            M->mat_type);
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    matfree(col);
    return(NULL);
    #endif
  }

  return(col);
}  /* end of function matcol */
