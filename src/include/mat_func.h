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

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include <stdio.h>
#include "real.h"

/* modulus of a matrix */
real matabs(const mat);

/* allocate matrix memory  in file matalloc.c */
mat matalloc(mat, size_t, size_t, int);

/* allocate array of matrices in file matarralloc.c */
mat matarralloc(mat, size_t);

/* free array of matrices in file matarrfree. c*/
int matarrfree(mat);

/* check validity of pointer */
int matcheck(const mat);

/*  extract a column from a matrix */
mat matcol(const mat, size_t);

/* transpose and conjugate a matrix */
mat matconj(mat, const mat);

/* Copy matrices */
mat matcopy(mat, const mat);

/* difference between matrices */
real matdiff(const mat, const mat);

/* Extract a submatrix from a larger one */
mat matextract(mat, const mat, size_t, size_t, size_t, size_t);

/* Free memory allocated for a matrix */
int matfree(mat);

/* copy one matrix into another */
mat matins(mat, const mat, size_t, size_t);

/* matrix inversion in file matinv.c */
mat matinv(mat, const mat);
mat matinv_old(mat, mat);

/* matrix multiplication in file matmul.c */
mat matmul(mat, mat, mat);

/* convert order */
void matnattovht(const mat, size_t, size_t);
void matline(const mat, int, size_t, size_t, size_t);

/*  extract a row from a matrix */
mat matrow(mat, mat, int); 

/* matrix multiplication with complex number in file matscal.c */
mat matscal(mat, const mat, real, real);

/* print a matrix in file matshow.c */
void matshow(const mat);

/* print the modulus of a matrix in file matshow.c */
void matshowabs(const mat);

/* print the parameters of a matrix in file matshowpar.c */
void matshowpar(const mat);

/* square modulus of a matrix */
mat matsqmod(mat, const mat);

/* trace of a matrix */
int mattrace(mat, const mat);

/* transpose a matrix */
mat mattrans(mat, const mat);

/*********************************************************************
 * read and write (<stdio.h> used for type definition of FILE)
 *********************************************************************/
int matwrite(const mat, FILE *);
mat matread(mat, FILE *);
mat matrdlm(mat, int, const char *);

/*********************************************************************
 * lower level functions
 *********************************************************************/
/* LU decomposition in file ludcmp.c */
int ludcmp(real *, int *, size_t);

/* LU decomposition (real) in file matrlu.c */
int r_ludcmp(real *, size_t *, size_t);
int r_luinv(real *, const real *, size_t *, size_t);
real *r_lubksb(const real *, const size_t *, real *, size_t);

/* LU decomposition (complex) in file matclu.c */
int c_ludcmp(real *, real *, int *, int);
int c_luinv(real *, real *, real *, real *, int *, int);
int c_lubksb(real *, real *, int *, real *, real *, int);

/* matrix multiplication for square mat. in file matrm.c */
real *r_sqmul(real *, real *, real *, size_t);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* MAT_FUNC_H */
