/*********************************************************************
 *                      mataux.c
 *
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*! \file
 *
 * Contains row2col_order() and col2row_order() functions for converting
 * between C row-major ordering and FORTRAN column-major ordering for
 * storage of matrix elements.
 *
 * These functions are intended to be useful with the low-level BLAS and
 * LAPACK FORTRAN libraries, however they may not needed if using native C
 * (rather than F2C'd) versions such as CBLAS or LAPACKE.
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cleed_real.h"

/*!
 * Converts a row-major ordered C matrix to a column-major ordered FORTRAN
 * matrix. It is intended for use with the \b FORTRAN library versions of
 * BLAS and LAPACK.
 *
 * \param[in] row_major Pointer to flattened matrix stored in row-major order.
 * \param[out] col_major Pointer to flattened matrix stored in column-major
 * order (can be passed in as \c NULL as memory will be allocated if needed).
 * \param rows Number of rows in the \p row_major & \p col_major matrices.
 * \param cols Number of columns in the \p row_major & \p col_major matrices.
 * \return C style return code indicating function success.
 * \retval 0 if function is successful.
 * \retval -1 if the input matrix is \c NULL
 * \retval -2 if memory cannot be allocated for the output matrix.
 */
int row2col_order(const real *row_major, real *col_major, size_t rows, size_t cols)
{
  size_t i, j;


  if (row_major == NULL) return(-1); /* cannot proceed as 1st arg is NULL */

  if (col_major == NULL) col_major = (real*) malloc(rows*cols*sizeof(real));

  if (col_major == NULL)  return(-2); /* cannot allocate memory */

  for (i = 0; i < cols; ++i)
    for (j = 0; j < rows; ++j)
      col_major[ i*rows + j ] = row_major[ j*rows + i ];

  return(0);

}

/*!
 * Converts a column-major ordered FORTRAN matrix to a row-major ordered C
 * matrix. It is intended for use with the \b FORTRAN library versions of
 * BLAS and LAPACK.
 *
 * \param[in] col_major Pointer to flattened matrix stored in column-major order.
 * \param[out] row_major Pointer to flattened matrix stored in row-major
 * order (can be passed in as \c NULL as memory will be allocated if needed).
 * \param rows Number of rows in the \p row_major & \p col_major matrices.
 * \param cols Number of columns in the \p row_major & \p col_major matrices.
 * \return C style return code indicating function success.
 * \retval 0 if function is successful.
 * \retval -1 if the input matrix is \c NULL
 * \retval -2 if memory cannot be allocated for the output matrix.
 */
int col2row_order(const real *col_major, real *row_major, size_t rows, size_t cols)
{
  size_t i, j;


  if (col_major == NULL) return(-1); /* cannot proceed as 1st arg is NULL */

  if (row_major == NULL) row_major = (real*) calloc(rows*cols, sizeof(real));

  if (row_major == NULL) return(-2); /* cannot allocate memory */

  for ( i = 0; i < cols; ++i)
    for ( j = 0; j < rows; ++j)
      row_major[ j*rows + i ] = col_major[ i*rows + j ];

  return(0);
}
