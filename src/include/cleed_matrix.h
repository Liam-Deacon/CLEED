/*********************************************************************
 *                          cleed_matrix.h
 *
 *  Copyright (C) 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/28.11.2014 - creation
 *********************************************************************/

/*!
 * \file
 * \author Liam Deacon
 *
 * Configuration header for choosing suitable linear algebra backend for
 * matrix calculations.
 *
 * \warning There is extensive use of macros to enable a single interface to
 * the linear algebra backends e.g. CBLAS, LAPACK, ATLAS, MKL, GSL, etc.
 * These macros are purposefully type unsafe to facilitate the single
 * interface and should be used with caution (and a large amount of scrutiny!).
 */


#ifndef CLEED_MATRIX_H
#define CLEED_MATRIX_H

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "cc_attribute.h"
#include "cleed_real.h"
#include "cleed_complex.h"
#include "cleed_basic_matrix.h"
#include "mat.h"

/*****************************************************
 * Define useful Preprocessor MACROS for matrix types
 *****************************************************/

/*!
 * \def CLEED_MATRIX_REAL_ALLOC(rows,cols)
 * Allocates memory for a real matrix using either CBLAS, GSL or native
 * interface.
 *
 * \def CLEED_MATRIX_COMPLEX_ALLOC(rows,cols)
 * Allocates memory for a complex matrix using either CBLAS, GSL or native
 * interface.
 *
 * \def CLEED_MATRIX_REAL_FREE(a)
 * Frees a real matrix from memory using either CBLAS, GSL or native interface.
 *
 * \def CLEED_MATRIX_COMPLEX_FREE(a)
 * Frees a complex matrix from memory using either CBLAS, GSL or native interface.
 *
 * \def CLEED_COMPLEX(z,r,i)
 * Allocates the complex number \c z with real and imaginary parts.
 *
 * \def CLEED_REAL_MAT2CBLAS(Mx,cblas_mx)
 * Converts from native real matrix to CBLAS format (which will be the GSL
 * implementation if \c USE_GSL is defined).
 *
 * \def CLEED_COMPLEX_MAT2CBLAS(Mx,cblas_mx)
 * Converts from native complex matrix to CBLAS format (which will be the GSL
 * implementation if \c USE_GSL is defined).
 *
 * \def CLEED_REAL_CBLAS2MAT(cblas_mx,Mx)
 * Converts from CBLAS real matrix (may be GSL implementation if \c USE_GSL is
 * defined) to the native matrix type.
 *
 * \def CLEED_COMPLEX_CBLAS2MAT(cblas_mx,Mx)
 * Converts from CBLAS complex matrix (may be GSL implementation if \c USE_GSL
 * is defined) to the native matrix type.
 *
 * \def CLEED_REAL_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr)
 * Macro for performing matrix multiplication involving real matrices
 * (backend dependent) using either CBLAS, GSL or native implementation.
 *
 * \def CLEED_COMPLEX_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr)
 * Macro for performing matrix multiplication involving complex matrices
 * (backend dependent) using either CBLAS, GSL or native implementation.
 */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
namespace cleed {
#endif

/* CLEED <-> CBLAS utility functions (always compiled) */
__attribute__ ((nonnull(1))) int mat2cblas(real *cblas_mx, mat_enum cblas_num, mat Mx);
__attribute__ ((nonnull(1))) int cblas2mat(mat Mx, const real *cblas_mx);
__attribute__ ((nonnull(1))) int mat2blas(real *blas_mx, mat_enum blas_num, const mat Mx);


#if USE_CBLAS || USE_MKL || USE_LAPACK /* perferred interface */

typedef cleed_basic_matrix cleed_matrix;
typedef cleed_basic_matrix_complex cleed_matrix_complex;

# define CLEED_MATRIX_COPY(a, b, m, n)              cleed_basic_matrix_copy((a),(b),(m),(n))
# define CLEED_MATRIX_GET(a, i, j, m, n)            cleed_basic_matrix_get((a),(i),(j),(n))
# define CLEED_MATRIX_SET(a, i, j, m, n, x)         cleed_basic_matrix_set((a),(i),(j),(n),(x))

# define CLEED_MATRIX_COMPLEX_COPY(a, b, m, n)      cleed_matrix_complex_copy((a),(b),(m),(n))
# define CLEED_MATRIX_COMPLEX_GET(a, i, j, m, n)    cleed_matrix_complex_get((a),(i),(j),(m),(n))
# define CLEED_MATRIX_COMPLEX_SET(a, i, j, m, n, x) cleed_matrix_complex_set((a),(i),(j),(m),(n),(x))

# define CLEED_REAL_MAT2CBLAS(Mx,cblas_mx)     mat2cblas((cblas_mx),NUM_REAL,(Mx))
# define CLEED_COMPLEX_MAT2CBLAS(Mx,cblas_mx)  mat2cblas((cblas_mx),NUM_COMPLEX,(Mx))

# define CLEED_REAL_CBLAS2MAT(cblas_mx,Mx)     cblas2mat((Mx),(cblas_mx))
# define CLEED_COMPLEX_CBLAS2MAT(cblas_mx,Mx)  cblas2mat((Mx),(cblas_mx))

__attribute__ ((returns_nonnull, alloc_size(1,2)))
static inline cleed_matrix *cleed_matrix_alloc(size_t rows, size_t cols)
{
  return (cleed_basic_matrix_alloc(rows, cols));
}

__attribute__((nonnull))
static inline int cleed_matrix_copy(cleed_matrix *dst, const cleed_matrix *src, size_t rows, size_t cols)
{
  return (cleed_basic_matrix_copy(dst, src, rows, cols));
}

__attribute__((nonnull))
static inline void cleed_matrix_free(cleed_matrix *a)
{
  cleed_basic_matrix_free(a);
}

__attribute__((nonnull))
static inline cleed_real cleed_matrix_get(cleed_matrix *a, size_t i, size_t j, size_t cols)
{
  return (cleed_basic_matrix_get(a, i, j, cols));
}

__attribute__((nonnull))
static inline void cleed_matrix_set(cleed_matrix *a, size_t i, size_t j, size_t rows,  size_t cols, cleed_real value)
{
  cleed_basic_matrix_set(a, i, j, cols, value);
}

__attribute__((returns_nonnull, alloc_size(1,2)))
static inline cleed_matrix_complex *cleed_matrix_complex_alloc(size_t rows, size_t cols)
{
  return (cleed_basic_matrix_complex_alloc(rows, cols));
}

__attribute__((nonnull))
static inline int cleed_matrix_complex_copy(cleed_matrix_complex *dst, const cleed_matrix_complex *src, size_t rows, size_t cols)
{
  return (cleed_basic_matrix_complex_copy(dst, src, rows, cols));
}

__attribute__((nonnull))
static inline void cleed_matrix_complex_free(cleed_matrix_complex *a)
{
  cleed_basic_matrix_complex_free(a);
}

__attribute__((nonnull))
static inline cleed_complex* cleed_matrix_complex_get(cleed_matrix_complex *a, size_t i, size_t j, size_t rows, size_t cols)
{
  return &a[2*(i*cols + j)];
}

__attribute__((nonnull))
static inline void cleed_matrix_complex_set(cleed_matrix_complex *a, size_t i, size_t j, size_t rows, size_t cols, cleed_complex z)
{
  a[2*(i*cols + j)] = z[0];
  a[2*(i*cols + j) + 1] = z[1];
}

#elif USE_GSL == 1
# include "_cleed_matrix_gsl.h"
#else /* USE_NATIVE */

typedef struct mat_str cleed_matrix;
typedef struct mat_str cleed_matrix_complex;

# define CLEED_MATRIX_COPY(a, b, m, n)       matcopy((a),(b))
# define CLEED_MATRIX_GET(a, i, j, m, n)     (*rmatel((i),(j),(a)))
# define CLEED_MATRIX_SET(a, i, j, m, n, x)  do { (*rmatel((i),(j),(a))) = (x); } while (0)

# define CLEED_MATRIX_COMPLEX_COPY(a, b, m, n)      matcopy((a),(b))
# define CLEED_MATRIX_COMPLEX_GET(a, i, j, m, n)    (*imatel((i),(j),(a)))
# define CLEED_MATRIX_COMPLEX_SET(a, i, j, m, n, z) do { (*imatel((i),(j),(a))) = (z) } while (0)

# define CLEED_REAL_MAT2CBLAS(Mx,cblas_mx)     matcopy(cblas_mx, Mx)
# define CLEED_COMPLEX_MAT2CBLAS(Mx,cblas_mx)  matcopy(cblas_mx, Mx)

# define CLEED_REAL_CBLAS2MAT(cblas_mx,Mx)     matcopy(Mx, cblas_mx)
# define CLEED_COMPLEX_CBLAS2MAT(cblas_mx,Mx)  matcopy(Mx, cblas_mx)

__attribute__((alloc_size(1,2)))
static inline cleed_matrix *cleed_matrix_alloc(size_t rows, size_t cols)
{
  return (matalloc(NULL, rows, cols, NUM_REAL));
}

__attribute__((nonnull))
static inline int cleed_matrix_copy(cleed_matrix *dst, const cleed_matrix *src, size_t rows, size_t cols)
{
  return ((matcopy((mat) dst, (const mat)src) != NULL) ? 0 : 1);
}

__attribute__((nonnull))
static inline void cleed_matrix_free(cleed_matrix *a)
{
  matfree(a);
}

__attribute__((nonnull))
static inline cleed_real cleed_matrix_get(
    const cleed_matrix *m,
    size_t i,
    size_t j)
{
  return (RMATEL(i,j,m));
}

__attribute__((nonnull))
static inline void cleed_matrix_set(
    cleed_matrix *m,
    size_t i,
    size_t j,
    cleed_real x)
{
  m->rel[(i-1) * m->cols + j] = x;
}

__attribute__((alloc_size(1,2)))
static inline cleed_matrix_complex *cleed_matrix_complex_alloc(size_t rows, size_t cols)
{
  return (matalloc(NULL, rows, cols, NUM_COMPLEX));
}

__attribute__((nonnull))
static inline int cleed_matrix_complex_copy(cleed_matrix_complex *dst, const cleed_matrix_complex *src)
{
  return ((matcopy((mat)dst, (const mat)src) != NULL) ? 0 : -1);
}

__attribute__((nonnull))
static inline void cleed_matrix_complex_free(cleed_matrix_complex *m)
{
  matfree(m);
}

__attribute__((nonnull))
static inline cleed_real cleed_matrix_complex_get(const cleed_matrix *m, size_t i, size_t j)
{
  return (IMATEL(i,j,m));
}

__attribute__((nonnull))
static inline void cleed_matrix_complex_set(cleed_matrix *m, size_t i, size_t j, cleed_complex x)                                  \
{
  m->rel[(i-1) * m->cols + j] = x[0];
  m->iel[(i-1) * m->cols + j] = x[1];
}

#endif


#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* namespace cleed */
#endif

#ifdef __attribute__
# undef __attribute__
#endif

#endif /* CLEED_MATRIX_H */
