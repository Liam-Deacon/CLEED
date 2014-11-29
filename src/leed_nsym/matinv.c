/*********************************************************************
 *                           MATINV.H
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/08.06.1994 - Creation
 *   GH/20.07.1995 - Change call of function c_luinv
 * MGJF/18.07.2014 - Workaround:
 *                   replace Numerical Recipes inversion by LAPACK routines.
 *   LD/29.11.2014 - Added selection of several linear algebra backends via
 *                   macro definitions in mat_aux.h
 *********************************************************************/

/*! \file
 *
 * Implements LU decomposition and matrix inversion function matinv()
 *
 * If using LAPACK as a backend then the original Numerical Recipes routines
 * have been replaced with a CBLAS one, which gives massive speed gains over
 * the original textbook version (LAPACK code based on code kindly contributed
 * by Michael Fink <Michael.Fink@uibk.ac.at>).
 *
 * If using GSL as a linear algebra backend then the routine have been replaced
 * by those found in the GNU Scientific Library, however, they may not be as
 * optimised as the LAPACK routines.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "real.h"
#include "mat.h"
#include "mat_aux.h"
#include "cblas_aux.h"

#ifdef LAPACK_VENDOR
# if LAPACK_VENDOR == CLAPACK /* Netlib's F2C interface for LAPACK */
#   include <clapack.h>
# elif LAPACK_VENDOR == LAPACKE /* C interface to LAPACK */
#   include <lapacke.h>
# elif LAPACK_VENDOR == ATLAS /* Automatically Tuned Linear Algebra Software */
#   include <clapack.h>
# endif
#else
# include "clapack.h" /* default is Netlib's CLAPACK interface */
#endif


/*!
 * Inverts a matrix by LU decomposition (real and complex).
 *
 * \param[out] A_1 Pointer to the inverse matrix.
 * \param[in] A Pointer to the matrix to be inverted.
 * \return Pointer to the inverse matrix.
 * \retval \c NULL if failed.
 */
mat matinv(mat A_1, const mat A)
{
  int i, n, info;

  int *ipiv;

  mat Alu = NULL;

  /* check input matrix */

  /* check type of input matrix */
  if ( (A->mat_type != MAT_SQUARE) && (A->mat_type != MAT_DIAG) )
  {
    ERROR_MSG("improper input matrix type (not square or diagonal)\n");
    ERROR_RETURN(NULL);
  }
  /* check dimensions of input matrix A */
  else if (A->cols != A->rows)
  {
    ERROR_MSG("dimensions of input matrix do not match\n");
    ERROR_RETURN(NULL);
  }
  n = A->cols;

  /* Backup input matrix A */
  Alu = matcopy(Alu, A); /* Copy A into temporary storage Alu */

  /* allocate memory for inverse matrix output */
  A_1 = matalloc(A_1, n, n, A->num_type);

  /* DIAGONAL MATRIX */

  /* If the matrix is diagonal, the inversion is trivial
   * (but currently not implemented)
   */
  if (A->mat_type == MAT_DIAG)
  {
    if (A_1 != NULL) matfree(A_1);

    ERROR_MSG("inversion of diagonal matrices is not implemented\n");
    matfree(Alu);
    ERROR_RETURN(NULL);
  }

  if ( sizeof(real) != sizeof(float) && sizeof(real) != sizeof(double) )
  {
    ERROR_MSG("unexpected sizeof(real)=%lu\n", sizeof(real));
    matfree(Alu);
    ERROR_RETURN(NULL);
  }

  /* NON-DIAGONAL MATRIX */

  /* Create lapack and result matrices */
  ipiv = (int *)calloc( (n+1), sizeof(int));

  switch(A->num_type)
  {
    /* Real matrix (NUM_REAL) */
    case (NUM_REAL):
    {
      REAL_MATRIX a = REAL_MATRIX_ALLOC(n, n);
      REAL_MAT2CBLAS(a, Alu);

      /* do LU decomposition and matrix inversion */
      REAL_LU_DECOMPOSITION(a, n, ipiv, info);
      if (info != 0)
      {
        ERROR_MSG("LU decomposition failed with code %i\n", info);
        REAL_MATRIX_FREE(a);
        matfree(Alu);
        free(ipiv);
        ERROR_RETURN(NULL);
      }

      /* now perform the LU back-substitution */
      REAL_MATRIX a_1 = REAL_MATRIX_ALLOC(n, n);

      REAL_MATRIX_INVERSION(a, n, ipiv, a_1, info);
      if (info != 0)
      {
        ERROR_MSG("real matrix inversion failed with code %i\n", info);
        REAL_MATRIX_FREE(a);
        REAL_MATRIX_FREE(a_1);
        matfree(Alu);
        free(ipiv);
        ERROR_RETURN(NULL);
      }

      /* produce pivot vector information */
      #if CONTROL
      CONTROL_MSG(CONTROL, "ipiv: ");
      for (i=0; i<n; i++) fprintf(STDCTR, " %d", ipiv[i]);
      fprintf(STDCTR, "\n");
      #endif

      /* Finalise */
      REAL_CBLAS2MAT(A_1, a_1);
      REAL_MATRIX_FREE(a_1);
      REAL_MATRIX_FREE(a);

      break;
    }  /* REAL */

    /* Complex matrix (NUM_COMPLEX) */
    case (NUM_COMPLEX):
    {
      COMPLEX_MATRIX a = COMPLEX_MATRIX_ALLOC(n, n);
      COMPLEX_MAT2CBLAS(a, Alu);

      /* do LU decomposition and matrix inversion */
      COMPLEX_LU_DECOMPOSITION(a, n, ipiv, info);
      if (info != 0)
      {
        ERROR_MSG("LU decomposition failed with code %i\n", info);
        COMPLEX_MATRIX_FREE(a);
        matfree(Alu);
        free(ipiv);
        ERROR_RETURN(NULL);
      }

      /* now perform the LU back-substitution */
      COMPLEX_MATRIX a_1 = COMPLEX_MATRIX_ALLOC(n, n);

      COMPLEX_MATRIX_INVERSION(a, n, ipiv, a_1, info);
      if (info != 0)
      {
        ERROR_MSG("complex matrix inversion failed with code %i\n", info);
        COMPLEX_MATRIX_FREE(a);
        COMPLEX_MATRIX_FREE(a_1);
        matfree(Alu);
        free(ipiv);
        ERROR_RETURN(NULL);
      }

      /* produce pivot vector information */
      #if CONTROL
      CONTROL_MSG(CONTROL, "ipiv: ");
      for (i=0; i<n; i++) fprintf(STDCTR, " %d", ipiv[i]);
      fprintf(STDCTR, "\n");
      #endif

      /* Finalise */
      COMPLEX_CBLAS2MAT(A_1, a_1);
      COMPLEX_MATRIX_FREE(a_1);
      COMPLEX_MATRIX_FREE(a);
      break;
    } /* COMPLEX */

  } /* switch(A->num_type) */

  matfree(Alu);
  free(ipiv);

  return(A_1);
}
