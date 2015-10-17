/*********************************************************************
 *                             GSL_AUX.C
 *
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
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
 *  \author Liam Deacon <liam.m.deacon@gmail.com>
 *
 * Contains mat2gsl(), gsl2mat() and gsl2blas() and blas2gsl() functions
 * for converting matrices between native, GSL and CBLAS style matrices.
 */

#include <gsl/gsl_complex.h>
#include <gsl/gsl_matrix.h>
#include <stdbool.h>
#include <string.h>
#include "mat.h"

/*!
 * Converts a GSL matrix into a native CLEED matrix. The GSL matrix can be
 * either real or complex and the output matrix is controlled with \p gsl_type .
 *
 * \param[out] gsl_mx_ptr Pointer to a real GSL matrix. If this matrix has already
 * been allocated it will be first freed then reallocated memory.
 * \param mx_type Desired output type for GSL matrix. Can be either \c
 * NUM_REAL or \c NUM_COMPLEX .
 * \param[in] Mx Pointer to native matrix type (may be \c NULL ).
 * \note If the input matrix is \c NULL for the desired output type (e.g.
 * \c NUM_REAL ), then the output matrix will be returned as \c NULL .
 *
 * \return C style return code indicating function success.
 * \retval 0 if function was successful.
 * \retval 1 if operation type defined by \p gsl_num is invalid
 * and #EXIT_ON_ERROR is not defined.
 */
int mat2gsl(const mat Mx, void *gsl_mx_ptr, mat_enum gsl_type)
{
  size_t i, j;

  gsl_matrix *gsl_mx = NULL;
  gsl_matrix_complex *gsl_mx_cmplx = NULL;

  bool shortcut = (sizeof(real) == sizeof(double)) ? true : false;

  /* cast gsl_mx_ptr to correct type of matrix */
  if (gsl_type == NUM_REAL)
    gsl_mx = (gsl_matrix *) gsl_mx_ptr;
  else if (gsl_type == NUM_COMPLEX)
    gsl_mx_cmplx = (gsl_matrix_complex *) gsl_mx_ptr;
  else
  {
    ERROR_MSG("Incorrect value for gsl matrix type (neither real or complex)\n");
    ERROR_RETURN(1);
  }

  /* we permit real<-real, real<-complex, complex<-real, complex<-complex */
  if ( !((gsl_type == NUM_REAL || gsl_type == NUM_COMPLEX) &&
       (Mx->num_type == NUM_REAL || Mx->num_type == NUM_COMPLEX)) )
  {
    ERROR_MSG("invalid operand type: %d %d\n", gsl_type, Mx->num_type);
    ERROR_RETURN(1);
  }

  switch (Mx->num_type)
  {
    case (NUM_REAL):
    {
      /* allocate memory for real gsl matrix */
      if (gsl_mx != NULL) gsl_matrix_free(gsl_mx);
      gsl_mx = (gsl_matrix *) gsl_matrix_calloc(Mx->rows, Mx->cols);

      if (shortcut == true)
      {
        memcpy((void*)gsl_mx->data, (const void*)Mx->rel+1,
               sizeof(real)*(Mx->rows * Mx->cols));
      }
      else
      {
        for ( i=0; i < Mx->rows * Mx->cols; i++ )
        {
          gsl_mx->data[i] = (double)Mx->rel[i+1];
          /*
          // slower method
          i_row = ((i-1) % Mx->rows);
          i_col = (i - i_row) / Mx->rows;
          gsl_matrix_set(gsl_mx, i_row+1, i_col+1, (double)Mx->rel[i]);
          */
        }
      }
      break;
    } /* NUM_REAL */

    case (NUM_COMPLEX):
    {
      /* allocate memory for complex gsl_matrix */
      if (gsl_mx_cmplx != NULL) gsl_matrix_complex_free(gsl_mx_cmplx);
      gsl_mx_cmplx = (gsl_matrix_complex *)
                        gsl_matrix_complex_calloc(Mx->rows, Mx->cols);

      for ( i=0, j=0; i < Mx->rows * Mx->cols; i++, j+=2 )
      {
        // faster - but potentially dangerous: assumes data is 2 x cols x rows
        gsl_mx_cmplx->data[j] = Mx->rel[i+1];
        gsl_mx_cmplx->data[j+1] = Mx->iel[i+1];
        /*
        // slow assignment method:
        z.dat[0] = (double)*ptrx;
        z.dat[1] = (double)*ptix;
        i_row = ((i-1) % Mx->rows);
        i_col = (i - i_row) / Mx->rows;
        gsl_matrix_complex_set(gsl_mx_cmplx, i_row, i_col, z);
        */
      }
      break;
    } /* NUM_COMPLEX */

    case (NUM_IMAG): case (NUM_MASK): default:
    {
      ERROR_MSG("Unsupported matrix data type (%s)\n", strmtype(Mx->num_type));
      ERROR_RETURN(-1);
      break;
    } /* default case */

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
int gsl2mat(const void *gsl_mx_ptr, mat_enum gsl_type, mat Mx, mat_enum mat_type)
{
  size_t i, j, incr;
  size_t nrows, ncols;
  const double *gsl_data_ptr = NULL;

  const gsl_matrix *gsl_mx = NULL;
  const gsl_matrix_complex *gsl_mx_cmplx = NULL;

  bool shortcut = (sizeof(real) == sizeof(double)) ? true : false;

  /* cast gsl_mx_ptr to correct type of matrix */
  if (gsl_type == NUM_REAL)
    gsl_mx = (const gsl_matrix *) gsl_mx_ptr;
  else if (gsl_type == NUM_COMPLEX)
    gsl_mx_cmplx = (const gsl_matrix_complex *) gsl_mx_ptr;
  else
  {
    ERROR_MSG("Incorrect value for gsl matrix type (neither real nor complex)\n");
    ERROR_RETURN(1);
  }

  /* determine parameters */
  if (gsl_mx != NULL)
  {
    nrows = gsl_mx->size1;
    ncols = gsl_mx->size2;
    gsl_data_ptr = (const double *) gsl_mx->data;
    incr = 1;
  }
  else if (gsl_mx_cmplx != NULL)
  {
    nrows = gsl_mx_cmplx->size1;
    ncols = gsl_mx_cmplx->size2;
    gsl_data_ptr = (const double *) gsl_mx_cmplx->data;
    incr = 2;
  }
  else /* invalid operation */
  {
    ERROR_MSG("both input matrices are NULL or invalid operation type %s->%s\n",
              strmtype(gsl_type), strmtype(mat_type));
    ERROR_RETURN(1);
  }

  if (Mx != NULL) matfree(Mx); /* free old memory */

  switch(mat_type)
  {
    case NUM_REAL:
    {
      matalloc(Mx, nrows, ncols, NUM_REAL);

      /* populate matrix data */
      if (shortcut == true && incr == 1)
      {
        memcpy((void*)Mx->rel+1, (const void*)gsl_data_ptr,
                  sizeof(real)*nrows*ncols);
      }
      else
      {
        for ( i=0, j=0; i < nrows*ncols; i++ , j+=incr )
        {
          Mx->rel[i+1] = gsl_data_ptr[j];
        }
      }

      return(0);
      break;
    } /* NUM_REAL */

    case (NUM_COMPLEX):
    {
      if (gsl_mx_cmplx != NULL)
      {
        matalloc(Mx, nrows, ncols, NUM_COMPLEX);

        /* populate complex matrix data */
        for ( i=0, j=0; i < nrows*ncols; i++, j+=incr )
        {
          Mx->rel[i+1] = gsl_data_ptr[j];
          Mx->iel[i+1] = gsl_data_ptr[j+1];
        }
      }
      return(0);
      break;
    } /* NUM_COMPLEX */

    case NUM_IMAG: case NUM_MASK:
    default:
    {
      ERROR_MSG("CLEED matrix type (%s) not supported\n", strmtype(mat_type));
      break;
    } /* default */

  } /* case (Mx->num_type) */

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
int cblas2gsl(const real *cblas_mx, mat_enum cblas_type, void *gsl_mx_ptr,
               mat_enum gsl_type, size_t cols, size_t rows)
{
  size_t i;

  gsl_matrix *gsl_mx = NULL;
  gsl_matrix_complex *gsl_mx_cmplx = NULL;

  bool shortcut = (sizeof(real) == sizeof(double)) ? true : false;

  /* cast gsl_mx_ptr to correct type of matrix */
  if (gsl_type == NUM_REAL || gsl_type == NUM_IMAG)
    gsl_mx = (gsl_matrix *) gsl_mx_ptr;
  else if (gsl_type == NUM_COMPLEX)
    gsl_mx_cmplx = (gsl_matrix_complex *) gsl_mx_ptr;
  else
  {
    ERROR_MSG("Invalid GSL matrix type (%s)\n", strmtype(gsl_type));
    ERROR_RETURN(1);
  }

  if (cblas_mx == NULL)
  {
    ERROR_MSG("CBLAS input matrix is NULL\n");
    ERROR_RETURN(-1);
  }

  if (gsl_type == NUM_COMPLEX && cblas_type == NUM_REAL)
  {
    ERROR_MSG("invalid CBLAS to GSL operation type %s->%s\n",
        strmtype(cblas_type), strmtype(gsl_type));
    ERROR_RETURN(1);
  }

  switch (gsl_type)
  {
    case NUM_REAL:
    {
      if (gsl_mx != NULL) gsl_matrix_free(gsl_mx);
      gsl_mx = (gsl_matrix *) gsl_matrix_calloc(rows, cols);

      if (shortcut == true)
      {
        memcpy((void*)gsl_mx->data, (const void*)cblas_mx,
                sizeof(real)*rows*cols);
      }
      else
      {
        for (i=0; i < rows*cols ; i++)
        {
          gsl_mx->data[i] = (double)cblas_mx[i];
          /*
          // slower assignment method:
          i_row = ((i-1) % rows);
          i_col = (i - i_row) / rows;
          gsl_matrix_set(gsl_mx, i_row, i_col, (double)*cblas_ptrx);
          */
        }
      }
      break;
    }

    case NUM_COMPLEX:
    {
      if (gsl_mx_cmplx != NULL) gsl_matrix_complex_free(gsl_mx_cmplx);
      gsl_mx_cmplx = (gsl_matrix_complex *) gsl_matrix_complex_calloc(rows, cols);

      for ( i=0; i < rows*cols; i++ )
      {
        // faster assignment method - assumes data size is 2 x cols x rows
        gsl_mx_cmplx->data[i] = cblas_mx[i];
        gsl_mx_cmplx->data[i+1] = (double)cblas_mx[i+1];

        /*
        // slow assignment method
        i_row = ((i-1) % rows);
        i_col = (i - i_row) / rows;
        z.dat[0] = (double)*cblas_ptrx;
        z.dat[1] = (double)*cblas_ptix;
        gsl_matrix_complex_set(gsl_mx, i_row, i_col, z);
        */
      }
      break;
    }
    default:
      ERROR_MSG("GSL output matrix type not supported (%s)\n", strmtype(gsl_type));
      ERROR_RETURN(-1);
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
int gsl2cblas(const void *gsl_mx_ptr, mat_enum gsl_type, real *cblas_mx, mat_enum cblas_type)
{
  size_t i, j, incr;
  size_t nrows, ncols;

  const gsl_matrix *gsl_mx = NULL;
  const gsl_matrix_complex *gsl_mx_cmplx = NULL;

  bool shortcut = (sizeof(real) == sizeof(double)) ? true : false;

  /* cast gsl_mx_ptr to correct type of matrix */
  if (gsl_type == NUM_REAL || gsl_type == NUM_IMAG)
    gsl_mx = (const gsl_matrix *) gsl_mx_ptr;
  else if (gsl_type == NUM_COMPLEX)
    gsl_mx_cmplx = (const gsl_matrix_complex *) gsl_mx_ptr;
  else
  {
    ERROR_MSG("Incorrect gsl matrix type (%s)\n", strmtype(gsl_type));
    ERROR_RETURN(1);
  }

  /* set loop parameters */
  if (gsl_mx != NULL)
  {
    nrows = gsl_mx->size1;
    ncols = gsl_mx->size2;
    incr = 1;
  }
  else if (gsl_mx_cmplx != NULL)
  {
    nrows = gsl_mx_cmplx->size1;
    ncols = gsl_mx_cmplx->size2;
    incr = 2;
  }
  else /* invalid operation */
  {
    ERROR_MSG("GSL input matrix is NULL\n");
    ERROR_RETURN(1);
  }

  switch(cblas_type)
  {
    case NUM_REAL:
    {
      /* allocate memory */
      if (cblas_mx != NULL) free(cblas_mx);
      if ( (cblas_mx = calloc(nrows * ncols, sizeof(real))) == NULL )
      {
        ERROR_MSG("unable to allocate memory for cblas output matrix\n");
        ERROR_RETURN(-2);
      }

      /* populate matrix data */
      if (shortcut == true)
      {
        memcpy((void*)cblas_mx, (const void*)gsl_mx->data, sizeof(real) * nrows * ncols);
      }
      else
      {
        /* copy element by element */
        for ( i=0, j=0; i < nrows * ncols; i++, j+=incr)
          cblas_mx[i] = (real) gsl_mx->data[j];
      }
      break;
    }

    case NUM_COMPLEX:
    {
      /* allocate memory */
      if (cblas_mx != NULL) free(cblas_mx);
      cblas_mx = (real *)calloc(nrows * ncols * 2, sizeof(real));
      if (cblas_mx == NULL )
      {
        ERROR_MSG("unable to allocate memory for cblas complex output matrix\n");
        ERROR_RETURN(-2);
      }

      if (shortcut == true)
        memcpy((void*)cblas_mx, (const void*)gsl_mx_cmplx->data, sizeof(real) * nrows * ncols * 2);
      else
      {
        /* populate matrix data */
        for ( i=0; i <= nrows * ncols; i+=2 )
        {
          cblas_mx[i] = (real)gsl_mx_cmplx->data[i];
          cblas_mx[i+1] = (real)gsl_mx_cmplx->data[i+1];
        }
      }

      break;
    }
    default:
      ERROR_MSG("CBLAS output matrix type not supported (%s)\n",
        strmtype(cblas_type));
      ERROR_RETURN(-1);
  }
  return(0);
}
