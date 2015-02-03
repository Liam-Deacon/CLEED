/*********************************************************************
 *                             GSL_AUX.C
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/21.11.2014 - Creation
**********************************************************************/

/*! \file
 *  \author Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 * Contains mat2gsl(), gsl2mat() and gsl2blas() and blas2gsl() functions
 * for converting matrices between native, GSL and CBLAS style matrices.
 */

#include <gsl/gsl_complex.h>
#include <gsl/gsl_matrix.h>
#include "mat.h"

/*!
 * Converts a GSL matrix into a native CLEED matrix. The GSL matrix can be
 * either real (\p gsl_mx ) or complex (\p gsl_mx_cmplx ) and the output
 * matrix is controlled with \p mm_type .
 *
 * \param[out] gsl_mx Pointer to a real GSL matrix. If this matrix has already
 * been allocated it will be first freed then reallocated memory.
 * \param[out] gsl_mx_cmplx Pointer to a complex GSL matrix. If this matrix
 * has already been allocated it will be first freed then reallocated memory.
 * \param mx_type Desired output type for GSL matrix. Can be either \c
 * NUM_REAL to allocate \p gsl_mx or \c NUM_COMPLEX to allocate \p gsl_mx_cmplx .
 * \param[in] Mx Pointer to native matrix type (may be \c NULL ).
 * \note If the input matrix is \c NULL for the desired output type (e.g.
 * \c NUM_REAL ), then the output matrix will be returned as \c NULL .
 *
 * \return C style return code indicating function success.
 * \retval 0 if function was successful.
 * \retval 1 if operation type defined by \p gsl_num is invalid
 * and #EXIT_ON_ERROR is not defined.
 */
int mat2gsl(gsl_matrix *gsl_mx, gsl_matrix_complex *gsl_mx_cmplx,
            int gsl_num, mat Mx)
{
  size_t i;
  size_t i_row, i_col;
  real *ptrx, *ptix;
  gsl_complex z;

  /* we permit real<-real, complex<-real, complex<-complex */
  if ( (gsl_num == NUM_REAL) && (Mx->num_type == NUM_REAL) ) i = 1;
  else if ( gsl_num == NUM_COMPLEX ) i = 2;
  else
  {
    ERROR_MSG("invalid operand type: %d %d\n", gsl_num, Mx->num_type);
    ERROR_RETURN(1);
  }

  switch (Mx->num_type)
  {
    case NUM_REAL:
    {
      /* allocate memory */
      if (gsl_mx != NULL) gsl_matrix_free(gsl_mx);
      gsl_mx = gsl_matrix_calloc(Mx->rows, Mx->cols);

      for ( i = 1 , ptrx = Mx->rel+1 ;
            i <= Mx->rows * Mx->cols ;
            i++ , ptrx++)
      {
        i_row = ((i-1) % Mx->rows);
        i_col = (i - i_row) / Mx->rows;
        gsl_matrix_set(gsl_mx, i_row+1, i_col+1, (double)*ptrx);
      }

      break;
    }

    case NUM_COMPLEX:
    {
      /* allocate memory */
      if (gsl_mx_cmplx != NULL) gsl_matrix_complex_free(gsl_mx_cmplx);
      gsl_mx_cmplx = gsl_matrix_complex_calloc(Mx->rows, Mx->cols);

      for ( i = 1 , ptrx = Mx->rel+1 , ptix = Mx->iel+1 ;
            i <= Mx->rows * Mx->cols ;
            i++ , ptrx++ , ptix++ )
      {
        z.dat[0] = (double)*ptrx;
        z.dat[1] = (double)*ptix;
        i_row = ((i-1) % Mx->rows);
        i_col = (i - i_row) / Mx->rows;
        gsl_matrix_complex_set(gsl_mx_cmplx, i_row, i_col, z);
      }
      break;
    }

  }
  return(0);
}

/*!
 * Converts a GSL matrix into a native CLEED matrix. The GSL matrix can be
 * either real (\p gsl_mx ) or complex (\p gsl_mx_cmplx ) and the output
 * matrix is controlled with \p mm_type .
 *
 * \param[in] gsl_mx Pointer to a real GSL matrix (may be \c NULL ).
 * \param[in] gsl_mx_cmplx Pointer to a complex GSL matrix (may be \c NULL ).
 * \param mx_type Desired output type for \p Mx . Can be either \c NUM_REAL or
 * \c NUM_COMPLEX .
 * \param[out] Mx Pointer to native matrix type. If this matrix has already
 * been allocated it will be first freed then reallocated memory.
 * \note If the input matrix is \c NULL for the desired output type (e.g.
 * \c NUM_REAL ) then \p Mx will be returned as \c NULL .
 *
 * \return C style return code indicating function success.
 * \retval -1 if \p Mx is \c NULL and #EXIT_ON_ERROR is not defined.
 * \retval 0 if function was successful.
 * \retval 1 if operation type defined by \p mat_type is invalid or both
 * input matrices are \c NULL and #EXIT_ON_ERROR is not defined.
 *
 */
int gsl2mat(gsl_matrix *gsl_mx, gsl_matrix_complex *gsl_mx_cmplx,
             int mat_type, mat Mx)
{
  size_t i, incr, i_row, i_col;
  size_t nrows, ncols;
  real *ptrx, *ptix, *gsl_ptr;
  double *gsl_px;
  int gsl_type = NUM_REAL;

  if (Mx != NULL) matfree(Mx); /* free old memory */

  switch(mat_type)
  {
    case NUM_REAL:
    {
      if (gsl_mx != NULL)
      {
        nrows = gsl_mx->size1;
        ncols = gsl_mx->size2;
        gsl_ptr = gsl_mx->data;
        incr = 1;
      }
      else if (gsl_mx_cmplx != NULL)
      {
        nrows = gsl_mx_cmplx->size1;
        ncols = gsl_mx_cmplx->size2;
        gsl_ptr = gsl_mx->data;
        incr = 2;
      }
      else /* invalid operation */
      {
        ERROR_MSG("both input matrices are NULL or "
                  "invalid operation type %d->%d\n", gsl_type, mat_type);
        ERROR_RETURN(1);
      }

      matalloc(Mx, gsl_mx->size1, gsl_mx->size2, NUM_REAL);

      /* populate matrix data */
      for ( i = 1 , gsl_px = gsl_mx->data, ptrx = Mx->rel+1 ;
            i <= Mx->rows * Mx->cols ;
            i++ , gsl_px++, ptrx++ )
      {
        *ptrx = *gsl_px;
      }
      return(0);
      break;
    }

    case NUM_COMPLEX:
    {
      if (gsl_mx_cmplx != NULL)
      {
        matalloc(Mx, gsl_mx->size1, gsl_mx->size2, NUM_COMPLEX);

        /* populate complex matrix data */
        for ( i = 1 , gsl_px = gsl_mx_cmplx->data,
              ptrx = Mx->rel+1, ptix = Mx->iel+1 ;
              i <= Mx->rows * Mx->cols ;
              i++ , gsl_px+=2, ptrx++)
        {
          *ptrx = *gsl_px;
          *ptix = *(gsl_px+1);
        }
      }
      return(0);
      break;
    }
  }

  Mx = NULL; /* because input matrix is NULL */
  return(-1);
}

/*!
 * Converts a CBLAS matrix into a GSL matrix.
 *
 * \param[in] cblas_mx Pointer to array of real data for CBLAS (can be \c NULL ).
 * \param[in] cblas_mx_cmplx Pointer to array of complex data for CBLAS
 * (can be \c NULL ).
 * \param[out] gsl_mx Pointer to ordinary GSL matrix.
 * \param[out] gsl_mx_cmplx Pointer to GSL complex matrix.
 * \param cblas_type The output matrix type for CBLAS. Note this can be either
 * \c NUM_REAL or \c NUM_COMPLEX if \p cblas_mx_cmplx is \c !NULL and
 * \p cblas_mx is \c !NULL , otherwise it can only be \c NUM_REAL for either
 * \p cblas_mx .
 * \param cols Number of columns in CBLAS matrix.
 * \param rows Number of rows in CBLAS matrix.
 *
 * \return C style return code indicating function success.
 * \retval -1 if both input matrices are \c NULL and
 * #EXIT_ON_ERROR is not defined.
 * \retval 0 if function was successful.
 * \retval 1 if operation type defined by \p gsl_num is invalid
 * and #EXIT_ON_ERROR is not defined.
 */
int cblas2gsl(real *cblas_mx, real *cblas_mx_cmplx,
               void *gsl_mx, void *gsl_mx_cmplx,
               int gsl_type, size_t cols, size_t rows)
{
  size_t i, i_row, i_col;
  real *cblas_ptrx, *cblas_ptix, *cblas_ptr;
  gsl_complex z;
  int cblas_type = NUM_REAL;

  if (cblas_mx != NULL && cblas_mx_cmplx != NULL)
  {
    cblas_type = NUM_COMPLEX;
  }

  /* check which matrix to read from if output is real */
  if (cblas_mx == NULL && cblas_mx_cmplx != NULL)
  {
    cblas_ptr = cblas_mx_cmplx;
  }
  else if (cblas_mx != NULL && cblas_mx_cmplx == NULL)
  {
    cblas_ptr = cblas_mx;
  }
  else
  {
    ERROR_MSG("both cblas input matrices are NULL\n");
    ERROR_RETURN(-1);
  }

  if (gsl_type == NUM_COMPLEX && cblas_type == NUM_REAL)
  {
    ERROR_MSG("invalid operation type %d->%d\n", cblas_type, gsl_type);
    ERROR_RETURN(1);
  }

  switch (gsl_type)
  {
    case NUM_REAL:
    {
      if (gsl_mx != NULL) gsl_matrix_free(gsl_mx);
      gsl_mx = (gsl_matrix *) gsl_matrix_calloc(rows, cols);

      for (i=0, cblas_ptrx = cblas_ptr ;
           i <= rows*cols ;
           i++, cblas_ptrx++ )
      {
        i_row = ((i-1) % rows);
        i_col = (i - i_row) / rows;
        gsl_matrix_set(gsl_mx, i_row, i_col, (double)*cblas_ptrx);
      }
      break;
    }

    case NUM_COMPLEX:
    {
      if (gsl_mx_cmplx != NULL) gsl_matrix_complex_free(gsl_mx_cmplx);
      gsl_mx_cmplx = (gsl_matrix *) gsl_matrix_complex_calloc(rows, cols);

      for (i=0, cblas_ptrx = cblas_mx, cblas_ptix = cblas_mx_cmplx ;
           i <= rows*cols ;
           i++, cblas_ptrx++, cblas_ptix++ )
      {
        i_row = ((i-1) % rows);
        i_col = (i - i_row) / rows;
        z.dat[0] = (double)*cblas_ptrx;
        z.dat[1] = (double)*cblas_ptix;
        gsl_matrix_complex_set(gsl_mx, i_row, i_col, z);
      }
      break;
    }
  }
  return(0);
}

/*!
 * Converts a GSL matrix into its CBLAS counterpart.
 *
 * \param[out] cblas_mx Pointer to array of real data for CBLAS.
 * \param[out] cblas_mx_cmplx Pointer to array of complex data for CBLAS.
 * \param[in] gsl_mx Pointer to ordinary GSL matrix (can be \c NULL ).
 * \param[in] gsl_mx_cmplx Pointer to GSL complex matrix (can be \c NULL ).
 * \param cblas_type The output matrix type for CBLAS. Note this can be either
 * \c NUM_REAL or \c NUM_COMPLEX if \p gsl_mx_cmplx is \c !NULL and \p gsl_mx
 * is \c NULL , otherwise it can only be \c NUM_REAL for either \p gsl_mx or
 * \c gsl_mx_cmplx . In this latter case \p gsl_mx must be \c NULL to only
 * extract the real part of \p gsl_mx_cmplx .
 * \return C style return code indicating function success.
 * \retval -3 if \p cblas_mx_cmplx is \c NULL and #EXIT_ON_ERROR is not defined.
 * \retval -2 if \p cblas_mx is \c NULL and #EXIT_ON_ERROR is not defined.
 * \retval -1 if the input matrix is \c NULL and #EXIT_ON_ERROR is not defined.
 * \retval 0 if successful.
 * \retval 1 if the operation type is invalid or both input matrices are
 * \c NULL and #EXIT_ON_ERROR is not defined.
 */
int gsl2cblas(real *cblas_mx, real *cblas_mx_cmplx,
               gsl_matrix *gsl_mx, gsl_matrix_complex *gsl_mx_cmplx,
               int cblas_type)
{
  size_t i, incr;
  size_t *gsl_px, *ptrx, *ptix, *gsl_ptr;
  size_t nrows, ncols;
  int gsl_type;

  switch(cblas_type)
  {
    case NUM_REAL:
    {
      if (gsl_mx != NULL)
      {
        nrows = gsl_mx->size1;
        ncols = gsl_mx->size2;
        gsl_ptr = gsl_mx->data;
        gsl_type = NUM_REAL;
        incr = 1;
      }
      else if (gsl_mx_cmplx != NULL)
      {
        nrows = gsl_mx_cmplx->size1;
        ncols = gsl_mx_cmplx->size2;
        gsl_ptr = gsl_mx->data;
        gsl_type = NUM_REAL;
        incr = 2;
      }
      else /* invalid operation */
      {
        ERROR_MSG("both GSL matrices are NULL or "
                  "invalid operation type %d->%d\n", gsl_type, cblas_type);
        ERROR_RETURN(1);
      }

      /* allocate memory */
      if (cblas_mx != NULL) free(cblas_mx);
      if ( (cblas_mx = calloc(nrows * ncols, sizeof(real))) == NULL )
      {
        ERROR_MSG("unable to allocate memory for cblas output matrix\n");
        ERROR_RETURN(-2);
      }

      /* populate matrix data */
      for ( i = 0 , gsl_px = gsl_ptr, ptrx = cblas_mx ;
            i <= gsl_mx->size1 * gsl_mx->size2 ;
            i++ , gsl_px+=incr, ptrx++ )
      {
        *ptrx = *gsl_px;
      }

      break;
    }

    case NUM_COMPLEX:
    {
      if (gsl_mx_cmplx == NULL)
      {
        ERROR_MSG("GSL complex input matrix is NULL\n");
        ERROR_RETURN(-1);
      }

      /* allocate memory */
      if (cblas_mx != NULL) free(cblas_mx);
      cblas_mx = (real *)calloc(gsl_mx_cmplx->size1 * gsl_mx_cmplx->size2,
                                sizeof(real));
      if (cblas_mx == NULL )
      {
        ERROR_MSG("unable to allocate memory for cblas real output matrix\n");
        ERROR_RETURN(-2);
      }

      if (cblas_mx_cmplx != NULL) free(cblas_mx_cmplx);
      cblas_mx_cmplx = (real *) calloc(gsl_mx_cmplx->size1 * gsl_mx_cmplx->size2,
                                       sizeof(real));
      if (cblas_mx_cmplx == NULL)
      {
        free(cblas_mx);

        ERROR_MSG("unable to allocate memory for cblas complex output matrix\n");
        ERROR_RETURN(-3);
      }

      /* populate matrix data */
      for ( i = 0 , gsl_px = gsl_mx_cmplx->data,
            ptrx = cblas_mx, ptix = cblas_mx_cmplx ;
            i <= gsl_mx_cmplx->size1 * gsl_mx_cmplx->size2 ;
            i++ , gsl_px+=2, ptrx++, ptix++ )
      {
        *ptrx = *gsl_px;
        *ptix = *(gsl_px+1);
      }

      break;
    }
  }
  return(0);
}
