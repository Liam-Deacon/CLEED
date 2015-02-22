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

#include "cleed_linalg.h"

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
  cleed_permutation *ipiv = NULL;

  int info = 0;
  size_t n = 0;

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
    ERROR_MSG("unexpected sizeof(real)=%u\n", sizeof(real));
    matfree(Alu);
    ERROR_RETURN(NULL);
  }

  /* NON-DIAGONAL MATRIX */

  /* Create lapack and result matrices */
  ipiv = cleed_permutation_alloc(n);

  switch(A->num_type)
  {
    /* Real matrix (NUM_REAL) */
    case (NUM_REAL):
    {
      cleed_matrix *a = cleed_matrix_alloc(n, n);
      CLEED_REAL_MAT2CBLAS(a, Alu);

      /* do LU decomposition and matrix inversion */
      info = cleed_linalg_LU_decomp(a, &n, ipiv);
      if (info != 0)
      {
        ERROR_MSG("LU decomposition failed with code %i\n", info);
        cleed_matrix_free(a);
        matfree(Alu);
        cleed_permutation_free(ipiv);
        ERROR_RETURN(NULL);
      }

      /* now perform the LU back-substitution */
      cleed_matrix *a_1 = cleed_matrix_alloc(n, n);

      info = cleed_linalg_invert(a, a_1, ipiv, &n);
      if (info != 0)
      {
        ERROR_MSG("real matrix inversion failed with code %i\n", info);
        cleed_matrix_free(a);
        cleed_matrix_free(a_1);
        matfree(Alu);
        cleed_permutation_free(ipiv);
        ERROR_RETURN(NULL);
      }

      /* Finalise */
      CLEED_REAL_CBLAS2MAT(A_1, a_1);
      cleed_matrix_free(a_1);
      cleed_matrix_free(a);

      break;
    }  /* REAL */

    /* Complex matrix (NUM_COMPLEX) */
    case (NUM_COMPLEX):
    {
      cleed_matrix_complex *a = cleed_matrix_complex_alloc(n, n);
      CLEED_COMPLEX_MAT2CBLAS(Alu, a);

      /* do LU decomposition and matrix inversion */
      info = cleed_linalg_complex_LU_decomp(a, &n, ipiv);
      if (info != 0)
      {
        ERROR_MSG("LU decomposition failed with code %i\n", info);
        cleed_matrix_complex_free(a);
        matfree(Alu);
        cleed_permutation_free(ipiv);
        ERROR_RETURN(NULL);
      }

      /* now perform the LU back-substitution */
      cleed_matrix_complex *a_1 = cleed_matrix_complex_alloc(n, n);

      if ( (info = cleed_linalg_complex_invert(a, a_1, ipiv, &n)) != 0 )
      {
        ERROR_MSG("complex matrix inversion failed with code %i\n", info);
        cleed_matrix_complex_free(a);
        cleed_matrix_complex_free(a_1);
        matfree(Alu);
        cleed_permutation_free(ipiv);
        ERROR_RETURN(NULL);
      }

      /* Finalise */
      CLEED_COMPLEX_CBLAS2MAT(A_1, a_1);
      cleed_matrix_complex_free(a_1);
      cleed_matrix_complex_free(a);
      break;
    } /* CLEED_COMPLEX */


    case(NUM_IMAG): case(NUM_MASK): default:
      ERROR_MSG("Unsupported matrix data type (%s)\n", strmtype(A->num_type));
      break;

  } /* switch(A->num_type) */

  /* produce pivot vector information */
#if CONTROL
  CONTROL_MSG(CONTROL, "ipiv: ");
  cleed_permutation_fprintf(STDOUT, ipiv, n);
  fprintf(STDCTR, "\n");
#endif

  /* clean up */
  matfree(Alu);
  cleed_permutation_free(ipiv);

  return(A_1);
}
