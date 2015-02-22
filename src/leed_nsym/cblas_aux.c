/*********************************************************************
 *                             CBLAS_AUX.C
 *
 *  Copyright 2014 Michael Fink <Michael.Fink@uibk.ac.at>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 * MGJF/18.07.2014 - Creation
**********************************************************************/

/*! \file
 *  \author Michael Fink <Michael.Fink@uibk.ac.at>
 *
 * Contains mat2blas() , blas2mat() and info_check() functions for converting
 * matrices between the CLEED native and CBLAS style matrices.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mat.h"

/*!
 * Returns an error message if \p info does not equal zero.
 *
 * \param routine Name of the routine for output.
 * \param info The value to check - most likely as the return code from a
 * function call.
 */
void info_check(const char *routine, const int info)
{
  if ( info != 0 )
  {
    ERROR_MSG("(%s) -> routine failed, info = %d\n", routine, info);
    exit(1);
  }
}

/*!
 * Converts a CLEED matrix \p Mx into a CBLAS matrix \p cblas_mx . \p cblas_num
 *
 * \param[out] cblas_mx Pointer to CBLAS matrix.
 * \param cblas_num Indicates type of matrix: either \c NUM_REAL or
 * \c NUM_COMPLEX .
 * \param[in] Mx Pointer to CLEED matrix.
 * \return C style return code indicating function success.
 * \retval -2 if \p cblas_mx is \c NULL and #EXIT_ON_ERROR is not defined.
 * \retval -1 if \p Mx is \c NULL and #EXIT_ON_ERROR is not defined.
 * \retval 0 if successful.
 * \retval 1 if the operation type is invalid and #EXIT_ON_ERROR is not defined.
 */
int mat2cblas ( real *cblas_mx, mat_enum cblas_num, mat Mx )
{

  size_t i, incre;
  real *cblas_px;
  real *ptrx, *ptix;

  if (Mx == NULL)
  {
    ERROR_MSG("input matrix is NULL\n");
    ERROR_RETURN(-1);
  }

  /* we permit real<-real, complex<-real, complex<-complex */
  if ( (cblas_num == NUM_REAL) && (Mx->num_type == NUM_REAL) ) incre = 1;
  else if ( cblas_num == NUM_COMPLEX ) incre = 2;
  else
  {
    ERROR_MSG("invalid operand type: %d %d\n", cblas_num, Mx->num_type);
    ERROR_RETURN(1);
  }

  /* check that output matrix has been allocated memory */
  if (cblas_mx == NULL)
    cblas_mx = (real*)calloc(incre*Mx->rows*Mx->cols, sizeof(real));
  if (cblas_mx == NULL)
  {
    ERROR_MSG("cannot allocate memory for output matrix\n");
    ERROR_RETURN(-2);
  }

  /* input data into output matrix */
  if (Mx->num_type == NUM_REAL)
  {
    for ( i = 1 , cblas_px = cblas_mx , ptrx = Mx->rel+1 ;
          i <= Mx->rows * Mx->cols ;
          i++ , cblas_px += incre , ptrx++ )
    {
      *cblas_px = *ptrx;
      if ( cblas_num == NUM_COMPLEX ) *(cblas_px+1) = 0;
    }

  }
  else if (Mx->num_type == NUM_COMPLEX)
  {
    for ( i = 1 , cblas_px = cblas_mx , ptrx = Mx->rel+1 , ptix = Mx->iel+1 ;
          i <= Mx->rows * Mx->cols ;
          i++ , cblas_px += incre , ptrx++ , ptix++ )
    {
      *cblas_px = *ptrx;
      *(cblas_px+1) = *ptix;
    }

  }
  else
  {
    ERROR_MSG("invalid operand type: %d\n", Mx->num_type);
    ERROR_RETURN(1);
  }
  return(0);
}

/*!
 * Converts a CBLAS matrix \p cblas_mx to a CLEED matrix \p Mx .
 *
 * \param[out] Mx Pointer to CLEED matrix (must be allocated memory prior to
 * entry into function).
 * \param[in] cblas_mx Pointer to CBLAS matrix.
 * \return C style return code indicating function success.
 * \retval -1 if \p cblas_mx is \c NULL and #EXIT_ON_ERROR is not defined.
 * \retval 0 if successful.
 * \retval 1 if the operation type is invalid and #EXIT_ON_ERROR is not defined.
 */
int cblas2mat ( mat Mx, const real *cblas_mx )
{
  /* silently assume cblas same type as Mx */
  size_t i, j;

  if (cblas_mx == NULL)
  {
    ERROR_MSG("input matrix is NULL\n");
    ERROR_RETURN(-1);
  }

  if (Mx->num_type == NUM_REAL)
  {
    /* use faster memcpy method rather than element-wise assignment */
    memcpy((void*)Mx->rel+1, (const void*)cblas_mx,
            sizeof(real)*Mx->rows*Mx->cols);
    //for ( i=1; i <= Mx->rows * Mx->cols; i++) Mx->rel[i] = cblas_mx[i-1];

  }
  else if (Mx->num_type == NUM_COMPLEX)
  {
    for ( i=1, j=0; i <= Mx->rows * Mx->cols ; i++, j+=2)
    {
      Mx->rel[i] = cblas_mx[j];
      Mx->iel[i] = cblas_mx[j+1];
    }

  }
  else
  {
    ERROR_MSG("invalid operand type: %d\n", Mx->num_type);
    ERROR_RETURN(1);
  }
  return(0);
}

/*!
 * Converts a CLEED matrix \p Mx into a FORTRAN BLAS matrix \p blas_mx . \p blas_num
 *
 * \param[out] blas_mx Pointer to CBLAS matrix.
 * \param blas_num Indicates type of matrix: either \c NUM_REAL or
 * \c NUM_COMPLEX .
 * \param[in] Mx Pointer to CLEED matrix.
 * \return C style return code indicating function success.
 * \retval -2 if \p blas_mx is \c NULL and #EXIT_ON_ERROR is not defined.
 * \retval -1 if \p Mx is \c NULL and #EXIT_ON_ERROR is not defined.
 * \retval 0 if successful.
 * \retval 1 if the operation type is invalid and #EXIT_ON_ERROR is not defined.
 *
 * \todo implement conversion taking into account complex types and the starting
 * indexing for the Fortran BLAS.
 */
int mat2blas ( real *blas_mx, mat_enum blas_num, const mat Mx )
{

  size_t i, j, incre;
  real *blas_px;
  real *ptrx, *ptix;

  if (Mx == NULL)
  {
    ERROR_MSG("input matrix is NULL\n");
    ERROR_RETURN(-1);
  }

  /* we permit real<-real, complex<-real, complex<-complex */
  if ( (blas_num == NUM_REAL) && (Mx->num_type == NUM_REAL) ) incre = 1;
  else if ( blas_num == NUM_COMPLEX ) incre = 2;
  else
  {
    ERROR_MSG("invalid operand type: %d %d\n", blas_num, Mx->num_type);
    ERROR_RETURN(1);
  }

  /* check that output matrix has been allocated memory */
  if (blas_mx == NULL)
  {
    blas_mx = (real*)calloc(incre*Mx->rows*Mx->cols, sizeof(real));
  }
  if (blas_mx == NULL)
  {
    ERROR_MSG("cannot allocate memory for output matrix\n");
    ERROR_RETURN(-2);
  }

  /* input data into output matrix */
  if (Mx->num_type == NUM_REAL)
  {
    for (i = 0; i < Mx->cols; ++i)
      for (j = 0; j < Mx->rows; ++j)
      {
        blas_mx[ i * Mx->rows + j ] = Mx->rel[ j * Mx->rows + i ];
      }

    if ( blas_num == NUM_COMPLEX )
    {
      for ( i = 1; i <= Mx->rows * Mx->cols; i+=incre)
      {
        blas_mx[i-1] = Mx->rel[i];
        blas_mx[i] = 0.;
      }
    }
    else /* just assign real part */
    {
      /* use faster memcpy method rather than element-wise assignment */
      memcpy((void*)blas_mx, (const void*)Mx->rel+1,
                sizeof(real)*Mx->rows*Mx->cols);
      //for ( i=1; i <= Mx->rows * Mx->cols; i+=incre ) blas_mx[i-1] = Mx->rel[i];
    }
  }
  else if (Mx->num_type == NUM_COMPLEX)
  {
    for ( i = 1 , blas_px = blas_mx;
          i <= Mx->rows * Mx->cols ;
          i++ , blas_px += incre , ptrx++ , ptix++ )
    {
      blas_mx[i-1] = Mx->rel[i];
      blas_mx[i] = Mx->iel[i];
    }

  }
  else
  {
    ERROR_MSG("invalid operand type: %d\n", Mx->num_type);
    ERROR_RETURN(1);
  }
  return(0);
}

/*!
 * Converts a FOTRTAN BLAS matrix \p blas_mx to a CLEED matrix \p Mx .
 *
 * \param[out] Mx Pointer to CLEED matrix (must be allocated memory prior to
 * entry into function).
 * \param[in] blas_mx Pointer to FORTRAN BLAS matrix.
 * \return C style return code indicating function success.
 * \retval -1 if \p blas_mx is \c NULL and #EXIT_ON_ERROR is not defined.
 * \retval 0 if successful.
 * \retval 1 if the operation type is invalid and #EXIT_ON_ERROR is not defined.
 */
int blas2mat ( mat Mx, real *blas_mx )
{
  /* silently assume blas same type as Mx */
  size_t i, j;
  size_t cols, rows;

  if (blas_mx == NULL)
  {
    ERROR_MSG("input matrix is NULL\n");
    ERROR_RETURN(-1);
  }

  cols = Mx->cols;
  rows = Mx->rows;

  if (Mx->num_type == NUM_REAL)
  {
    for ( i=0 ; i < cols ; i++ )
      for (j=0 ; j < rows ; j++ )
        Mx->rel[ j*rows + i ] = blas_mx[ i*rows + j ];
  }
  else if (Mx->num_type == NUM_COMPLEX)
  {
    for ( i=0 ; i < cols ; i++ )
      for (j=0 ; j < rows ; j++ )
      {
        /*!TODO: complex Fortran to C conversion */
        Mx->rel[ j*rows + i ] = blas_mx[ i*rows + j ];
        Mx->iel[ j*rows + i ] = blas_mx[ i*rows + j ];
      }

  }
  else
  {
    ERROR_MSG("invalid operand type: %d\n", Mx->num_type);
    ERROR_RETURN(1);
  }
  return(0);
}

