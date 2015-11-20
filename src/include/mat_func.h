/*********************************************************************
 *                           MAT_FUNC.H
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1994.02.10 - creation
 *********************************************************************/

/*!
 * \file
 * \brief header for matrix operations function prototypes
 */
#ifndef MAT_FUNC_H
#define MAT_FUNC_H

#if !__GNUC__
#define __attribute__(x) /* empty */
#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include <stdio.h>
#include "cleed_real.h"
#include "mat_def.h"

/* modulus of a matrix */
__attribute__((nonnull)) real matabs(const mat);

/* allocate matrix memory  in file matalloc.c */
mat matalloc(mat, size_t, size_t, mat_enum);

/* allocate array of matrices in file matarralloc.c */
mat matarralloc(mat, size_t);

/* free array of matrices in file matarrfree. c*/
int matarrfree(mat);

/* check validity of pointer */
int matcheck(const mat);

/*  extract a column from a matrix */
__attribute__((nonnull, returns_nonnull)) mat matcol(const mat, size_t);

/* transpose and conjugate a matrix */
__attribute__((nonnull, returns_nonnull)) mat matconj(mat, const mat);

/* Copy matrices */
__attribute__((nonnull, returns_nonnull)) mat matcopy(mat M_dst, const mat M_src);

/* difference between matrices */
__attribute__((nonnull)) real matdiff(const mat, const mat);

/* Extract a submatrix from a larger one */
__attribute__((nonnull, returns_nonnull))
mat matextract(mat, const mat, size_t, size_t, size_t, size_t);

/* Free memory allocated for a matrix */
__attribute__((nonnull)) int matfree(mat);

/* copy one matrix into another */
__attribute__((nonnull, returns_nonnull))
mat matins(mat, const mat, size_t, size_t);

/* matrix inversion in file matinv.c */
__attribute__((nonnull, returns_nonnull)) mat matinv(mat, const mat);
mat matinv_old(mat, mat);

/* matrix multiplication in file matmul.c */
__attribute__((nonnull, returns_nonnull)) mat matmul(mat, mat, mat);

/* convert order */
__attribute__((nonnull)) void matnattovht(const mat, size_t, size_t);
void matline(const mat, size_t, size_t, size_t, size_t);

/*  extract a row from a matrix */
__attribute__((nonnull, returns_nonnull)) mat matrow(mat, mat, size_t);

/* matrix multiplication with complex number in file matscal.c */
__attribute__((nonnull, returns_nonnull))
mat matscal(mat, const mat, real, real);

/* print a matrix in file matshow.c */
__attribute__((nonnull)) void matshow(const mat);

/* print the modulus of a matrix in file matshow.c */
__attribute__((nonnull)) void matshowabs(const mat);

/* print the parameters of a matrix in file matshowpar.c */
__attribute__((nonnull)) void matshowpar(const mat);

/* square modulus of a matrix */
__attribute__((nonnull, returns_nonnull)) mat matsqmod(mat, const mat);

/* trace of a matrix */
__attribute__((nonnull)) int mattrace(mat, const mat);

/* transpose a matrix */
__attribute__((nonnull, returns_nonnull)) mat mattrans(mat, const mat);

/*********************************************************************
 * read and write (<stdio.h> used for type definition of FILE)
 *********************************************************************/
int matwrite(const mat, FILE *);
__attribute__((nonnull, returns_nonnull)) mat matread(mat, FILE *);
__attribute__((nonnull, returns_nonnull)) mat matrdlm(mat, int, const char *);

/*********************************************************************
 * lower level functions
 *********************************************************************/
/* LU decomposition in file ludcmp.c */
__attribute__((nonnull))
int ludcmp(cleed_real *, int *, size_t);

/* LU decomposition (real) in file matrlu.c */
__attribute__((nonnull))
int r_ludcmp(cleed_real *, size_t *, size_t);

__attribute__((nonnull))
int r_luinv(cleed_real *, const cleed_real *, size_t *, size_t);

__attribute__((nonnull, returns_nonnull))
real *r_lubksb(const cleed_real *, const size_t *, cleed_real *, size_t);

/* LU decomposition (complex) in file matclu.c */
__attribute__((nonnull))
int c_ludcmp(cleed_real *, cleed_real *, size_t *, size_t);

__attribute__((nonnull))
int c_luinv(cleed_real *, cleed_real *, cleed_real *,
			cleed_real *, size_t *, size_t);

__attribute__((nonnull))
int c_lubksb(cleed_real *, cleed_real *, size_t *,
			 cleed_real *, cleed_real *, size_t);

/* matrix multiplication for square mat. in file matrm.c */
__attribute__((nonnull, returns_nonnull))
real *r_sqmul(cleed_real *, cleed_real *, cleed_real *, size_t);

/* convert row major ordering FORTRAN <-> C */
__attribute__((nonnull(1)))
int row2col_order(const cleed_real *row_major,
				cleed_real *col_major, size_t rows, size_t cols);

__attribute__((nonnull(1)))
int col2row_order(const cleed_real *col_major,
				cleed_real *row_major, size_t rows, size_t cols);

/* utility functions */
static inline const char *strmtype(mat_enum matrix_type) {
  switch (matrix_type)
  {
    case(NUM_REAL): return ("real\0"); break;
    case(NUM_IMAG): return ("imaginary\0"); break;
    case(NUM_COMPLEX): return ("complex\0"); break;
    case(NUM_MASK): return ("mask\0"); break;
  }
  return ("invalid\0");
}

/*!
 * Access real matrix element \$f (i,j) \$f of matrix Mat.
 *
 * \param i Row number
 * \param j Column number
 * \param Mat Pointer to CLEED matrix
 *
 * \note \p m and \p n must be positive integers, \p Mat must be of type #mat.
 * \note \p Mat is stored in row-major order.
 */
static inline cleed_real *rmatel(size_t i, size_t j, mat Mat)
{
  return (Mat->rel + (i-1) * Mat->cols + j);
}
/*!
 * Access imaginary matrix element \f$ (i,j) \f$ of matrix Mat.
 *
 * \param i Row number
 * \param j Column number
 * \param Mat Pointer to CLEED matrix
 *
 * \note \p m and \p n must be positive integers, \p Mat must be of type #mat.
 * \note \p Mat is stored in row-major order.
 */
static inline cleed_real *imatel(size_t i, size_t j, mat Mat)
{
  return (Mat->iel + ((i-1) * Mat->cols + j));
}

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#ifdef __attribute__
# undef __attribute__
#endif

#endif /* MAT_FUNC_H */
