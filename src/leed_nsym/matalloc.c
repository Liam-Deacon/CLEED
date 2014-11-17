/*********************************************************************
 *                        MATALLOC.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/11.08.94 - Diagonal matrices
 *   GH/15.08.94 - set all matrix elements to zero.
 *   GH/26.08.94 - num_type has a different meaning: num_type + mat_type.
 *   GH/20.01.95 - default blk_type = BLK_SINGLE
 *********************************************************************/

/*! \file
 *
 * Implements matalloc() function to allocate memory for a matrix.
 */

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include "mat.h"

/*!
 * Allocates memory for a \p cols by \p rows dimensional matrix \p M of
 * type \p num_type . All matrix elements are set to zero.
 *
 * \param[out] M Pointer to the matrix allocated in memory.
 * \param rows Number of rows to assign.
 * \param cols Number of columns to assign.
 * \param num_type Type of matrix elements ( #NUM_REAL or #NUM_COMPLEX )
 * and type of matrix ( #MAT_NORMAL , #MAT_DIAG , etc.). Input as:
 * \code mat_type | num_type \endcode .
 * \return Pointer to the allocated matrix.
 * \retval \c NULL if unable to allocate memory for matrix and #EXIT_ON_ERROR
 * is not defined.
 */
mat matalloc(mat M, size_t rows, size_t cols, int num_type)
{
  int mat_type;
  size_t no_of_elts;
  real *ptr, *ptr_end;

  #ifdef CONTROL
  fprintf(STDCTR, "(matalloc): enter function rows=%u cols=%u num_type=%d\n",
          rows, cols, num_type);
  #endif

  /* Check the validity of the pointer M and cols/rows first */
  if(matcheck(M) < 0)
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (matalloc): Invalid pointer \n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(NULL);
    #endif
  }

  if( (cols < 1) || (rows < 1) )
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (matalloc): Invalid number of cols/rows \n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(NULL);
    #endif
  }

  /* Find the correct matrix type (high byte of num_type) and number
   * types (low byte of num_type).
   */
  mat_type = num_type & MAT_MASK;

  if(mat_type == 0)
  {
    if(cols != rows) mat_type = MAT_NORMAL;
    else
    {
      if(cols != 1)  mat_type = MAT_SQUARE;
      else           mat_type = MAT_SCALAR;
    }
  }
  else if( (mat_type == MAT_DIAG) && (cols != rows) )
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (matalloc): different numbers of cols ");
    fprintf(STDERR, "and rows are incompatible with diagonal matrix type\n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(NULL);
    #endif
  }

  num_type = num_type & NUM_MASK;

  #ifdef CONTROL_X
  fprintf(STDCTR, "(matalloc): num_type: %x mat_type: %x \n",
          num_type, mat_type);
  #endif

  /* If M already points to the right matrix type, only reset matrix elements */
  if( (matcheck(M) > 0) &&
      (M->cols == cols) &&
      (M->rows == rows) &&
      (M->num_type == num_type) &&
      (M->mat_type == mat_type) )
  {
    if(M->num_type == NUM_COMPLEX)
    {
      /* M->mat_type < MAT_DIAG means square, normal or scalar */
      if(M->mat_type < MAT_DIAG)
      {
        ptr_end = M->rel + M->rows*M->cols;
        for(ptr = M->rel; ptr <= ptr_end; ptr ++) *ptr = 0.;

        ptr_end = M->iel + M->rows*M->cols;
        for(ptr = M->iel; ptr <= ptr_end; ptr ++) *ptr = 0.;
      }
      if(M->mat_type == MAT_DIAG)
      {
        ptr_end = M->rel + M->cols;
        for(ptr = M->rel; ptr <= ptr_end; ptr ++) *ptr = 0.;

        ptr_end = M->iel + M->cols;
        for(ptr = M->iel; ptr <= ptr_end; ptr ++) *ptr = 0.;
      }
    } /* NUM_COMPLEX */

    if(M->num_type == NUM_REAL)
    {
      if(M->mat_type < MAT_DIAG)
      {
        ptr_end = M->rel + M->rows*M->cols;
        for(ptr = M->rel; ptr <= ptr_end; ptr ++) *ptr = 0.;
      }
      if(M->mat_type == MAT_DIAG)
      {
        ptr_end = M->rel + M->cols;
        for(ptr = M->rel; ptr <= ptr_end; ptr ++) *ptr = 0.;
      }
    } /* NUM_REAL */

    /* Set magic number and return */
    M->mag_no = MATRIX;
    return(M);

  } /* reset existing matrix */

  /* If not: allocate new memory
   * - if M == NULL: allocate a new matrix header, blk_type = BLK_SINGLE
   * - if M != NULL: update the old header, use former blk_type.
   */
  if(M == NULL)
  {
    #ifdef CONTROL_X
    fprintf(STDCTR, "(matalloc): create new matrix structure\n");
    #endif

    M = ( mat )malloc(sizeof(struct mat_str));
    M->blk_type = BLK_SINGLE;
  }
  else  /* M != NULL */
  {
    #ifdef CONTROL_X
    fprintf(STDCTR, "(matalloc): reuse old matrix structure\n");
    #endif

    if (M->iel != NULL) free(M->iel);
    if (M->rel != NULL) free(M->rel);
  }

  /* assign matrix member values */
  M->cols = cols;
  M->rows = rows;
  M->num_type = num_type;
  M->mat_type = mat_type;

  if (M->mat_type == MAT_DIAG) no_of_elts = M->cols+1;
  else                         no_of_elts = M->rows * M->cols + 1;

  switch(num_type)
  {
    case(NUM_REAL):
    {
      #ifdef CONTROL_X
      fprintf(STDCTR, "(matalloc): allocate %d real matrix elements\n",
              no_of_elts);
      #endif

      M->iel = NULL;
      M->rel = (real*)calloc( no_of_elts, sizeof(real));

      if (M->rel == NULL)
      {
        free(M);

        #ifdef ERROR
        fprintf(STDERR, "*** error (matalloc) allocation error\n");
        #endif

        #ifdef EXIT_ON_ERROR
        exit(1);
        #else
        return(NULL);
        #endif
      }
      break;
    } /* NUM_REAL */

    case(NUM_COMPLEX):
    {
      #ifdef CONTROL_X
      fprintf(STDCTR, "(matalloc): allocate 2 * %d complex matrix elements\n",
              no_of_elts);
      #endif

      M->rel = (real*)calloc(no_of_elts, sizeof(real));
      M->iel = (real*)calloc(no_of_elts, sizeof(real));

      if( (M->rel == NULL) || (M->iel == NULL) )
      {
        if (M->rel != NULL) free(M->rel);
        if (M->iel != NULL) free(M->iel);
        free(M);

        #ifdef ERROR
        fprintf(STDERR, "*** error (matalloc) allocation error\n");
        #endif

        #ifdef EXIT_ON_ERROR
        exit(1);
        #else
        return(NULL);
        #endif
      }
      break;
    } /* NUM_COMPLEX */

    default: /* unknown number type */
    {
      #ifdef ERROR
      fprintf(STDERR, "*** error (matalloc) wrong number type: %d\n",
              num_type);
      #endif

      #ifdef EXIT_ON_ERROR
      exit(1);
      #else
      return(NULL);
      #endif
    }
  } /* switch */

  /* Set magic number and return */
  #ifdef CONTROL_X
  fprintf(STDCTR, "(matalloc): set magic number and return\n\n");
  #endif

  M->mag_no = MATRIX;
  return(M);
} /* end of function matalloc */
