/*********************************************************************
 *													cleed_linalg.h
 *
 *  Copyright (C) 2013-2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/04.02.2015 - creation
 *********************************************************************/

/*!
 * \file
 * \author Liam Deacon
 *
 * Adaptor factory design pattern for providing linear algebra methods such as
 * LU decomposition, matrix inversion, etc. regardless of actual linear algebra
 * back-end used.
 */

#ifndef CLEED_LINALG_H
#define CLEED_LINALG_H

#include "cleed_vector.h"
#include "cleed_matrix.h"
#include "cleed_permutation.h"
#include "cleed_blas.h"

#if USE_GSL
# include <gsl/gsl_linalg.h>
#endif

#if USE_MKL
# include <mkl_lapacke.h>
#elif USE_CLAPACK /* Netlib's F2C interface for LAPACK */
# include <clapack.h>
#elif USE_LAPACKE /* C interface to LAPACK */
# include <lapacke.h>
#elif USE_ATLAS /* Automatically Tuned Linear Algebra Software */
# include <clapack.h>
#else
# include "clapack.h" /* default is Netlib's CLAPACK interface */
# if (!USE_GSL)
#   warning "No linear algebra back-end selected (either LAPACK or GSL) - using native routines."
# endif
#endif

/********************************************************************
 * Linear algebra interface for LU decomposition and matrix inversion
 ********************************************************************/
#define ILAENV ilaenv_
#if (USE_CLAPACK) || (USE_ATLAS)
# define SGETRF(m,n,a,lda,ipiv,info)                                          \
  (info) = clapack_sgetrf(CblasRowMajor, (m), (n), (a), (lda), (ipiv) )
# define DGETRF(m,n,a,lda,ipiv,info)                                          \
  (info) = clapack_dgetrf(CblasRowMajor, (m), (n), (a), (lda), (ipiv) )
# define CGETRF(m,n,a,lda,ipiv,info)                                          \
  (info) = clapack_cgetrf(CblasRowMajor, (m), (n), (a), (lda), (ipiv) )
# define ZGETRF(m,n,a,lda,ipiv,info)                                          \
  (info) = clapack_zgetrf(CblasRowMajor, (m), (n), (a), (lda), (ipiv) )
# define SGETRI(n,a,lda,ipiv,info)                                            \
  (info) = clapack_sgetri(CblasRowMajor), (n), (a), (lda), (ipiv) )
# define DGETRI(n,a,lda,ipiv,info)                                            \
  (info) = clapack_dgetri(CblasRowMajor), (n), (a), (lda), (ipiv) )
# define CGETRI(n,a,lda,ipiv,info)                                            \
  (info) = clapack_cgetri(CblasRowMajor), (n), (a), (lda), (ipiv) )
# define ZGETRI(n,a,lda,ipiv,info)                                            \
  (info) = clapack_zgetri(CblasRowMajor), (n), (a), (lda), (ipiv) )
#elif (USE_LAPACKE)
# define SGETRF(m,n,a,lda,ipiv,info)                                          \
  LAPACKE_sgetrf(CblasRowMajor, &(m), &(n), (a), &(lda), (ipiv), &(info) )
# define DGETRF(m,n,a,lda,ipiv,info)                                          \
  LAPACKE_dgetrf(CblasRowMajor, &(m), &(n), (a), &(lda), (ipiv), &(info) )
# define CGETRF(m,n,a,lda,ipiv,info)                                          \
  LAPACKE_cgetrf(CblasRowMajor, &(m), &(n), (a), &(lda), (ipiv), &(info) )
# define ZGETRF(m,n,a,lda,ipiv,info)                                          \
  LAPACKE_zgetrf(CblasRowMajor, &(m), &(n), (a), &(lda), (ipiv), &(info) )
# define SGETRI(n,a,lda,ipiv,info)                                            \
  LAPACKE_sgetri(CblasRowMajor), &(n), (a), &(lda), (ipiv), &(info) )
# define DGETRI(n,a,lda,ipiv,info)                                            \
  LAPACKE_dgetri(CblasRowMajor), &(n), (a), &(lda), (ipiv), &(info) )
# define CGETRI(n,a,lda,ipiv,info)                                            \
  LAPACKE_cgetri(CblasRowMajor), &(n), (a), &(lda), (ipiv), &(info) )
# define ZGETRI(n,a,lda,ipiv,info)                                            \
  LAPACKE_zgetri(CblasRowMajor), &(n), (a), &(lda), (ipiv), &(info) )
#else /* FORTRAN style lapack */
# define SGETRF(m,n,a,lda,ipiv,info) sgetrfac_(m,n,a,lda,ipiv,&(info))
# define DGETRF(m,n,a,lda,ipiv,info) dgetrfac_(m,n,a,lda,ipiv,&(info))
# define CGETRF(m,n,a,lda,ipiv,info) cgetrfac_(m,n,a,lda,ipiv,&(info))
# define ZGETRF(m,n,a,lda,ipiv,info) zgetrfac_(m,n,a,lda,ipiv,&(info))
# define SGETRI(n,a,lda,ipiv,info)   sgetri_(n,a,lda,ipiv,&(info))
# define DGETRI(n,a,lda,ipiv,info)   dgetri_(n,a,lda,ipiv,&(info))
# define CGETRI(n,a,lda,ipiv,info)   cgetri_(n,a,lda,ipiv,&(info))
# define ZGETRI(n,a,lda,ipiv,info)   zgetri_(n,a,lda,ipiv,&(info))
#endif

/* Produce LAPACK function macros appropriate to package and precision of real type */
#ifdef CLEED_REAL_IS_FLOAT
# define CLEED_REAL_ILAENV(P,N,M)    ILAENV((P), "SGETRI", " ", (N), (M), (M), (M));
# define CLEED_COMPLEX_ILAENV(P,N,M) ILAENV((P), "CGETRI", " ", (N), (M), (M), (M));
# define CLEED_REAL_GETRF(m,n,a,lda,ipiv,info)    SGETRF(m,n,a,lda,ipiv,info)
# define CLEED_REAL_GETRI(n,a,lda,ipiv,info)      SGETRI(n,a,lda,ipiv,info)
# define CLEED_COMPLEX_GETRF(m,n,a,lda,ipiv,info) CGETRF(m,n,a,lda,ipiv,info)
# define CLEED_COMPLEX_GETRI(n,a,lda,ipiv,info)   CGETRI(n,a,lda,ipiv,info)
#else
# define CLEED_REAL_ILAENV(P,N,M)    ILAENV((P), "DGETRI", " ", (N), (M), (M), (M))
# define CLEED_COMPLEX_ILAENV(P,N,M) ILAENV((P), "ZGETRI", " ", (N), (M), (M), (M))
# define CLEED_REAL_GETRF(m,n,a,lda,ipiv,info)    DGETRF(m,n,a,lda,ipiv,info)
# define CLEED_REAL_GETRI(n,a,lda,ipiv,info)      DGETRI(n,a,lda,ipiv,info)
# define CLEED_COMPLEX_GETRF(m,n,a,lda,ipiv,info) ZGETRF(m,n,a,lda,ipiv,info)
# define CLEED_COMPLEX_GETRI(n,a,lda,ipiv,info)   ZGETRI(n,a,lda,ipiv,info)
#endif

#ifdef __cplusplus /* if this is a C++ compiler then use C linkage */
namespace cleed {
#endif /* __cplusplus */

/*!
 * \define CLEED_REAL_LU_DECOMPOSITION
 * Macro that performs LU decomposition for a real matrix.
 *
 * \define CLEED_COMPLEX_LU_DECOMPOSITION
 * Macro that performs LU decomposition for a complex matrix.
 *
 * \define CLEED_MATRIX_REAL_INVERSION
 * Macro for performing real matrix inversion.
 *
 * \define CLEED_MATRIX_COMPLEX_INVERSION
 * Macro for performing complex matrix inversion.
 *
 */

#if ( USE_CBLAS || USE_MKL || USE_LAPACK )

# define CLEED_REAL_LU_DECOMPOSITION(a, perm, n, info)                        \
  do { CLEED_REAL_GETRF( n, n, a, n, perm, info ); } while (0)

# define CLEED_COMPLEX_LU_DECOMPOSITION(a, perm, n, info)                     \
do { CLEED_COMPLEX_GETRF( n, n, a, n, perm, info ); } while (0)

# define CLEED_MATRIX_REAL_INVERSION(a, n, perm, a_1, info)               \
  do {                                                                        \
      const int p1 = 1, m1 = -1;                                              \
      int nb = CLEED_REAL_ILAENV(&p1, &(n), &m1);                             \
      int lwork = (n)*nb;                                                     \
      cleed_matrix *work = cleed_matrix_alloc((n), nb);                       \
      CLEED_REAL_GETRI( (n), (a), (perm), work, &lwork, (info));              \
      cleed_matrix_free(work);                                                \
} while(0);

# define CLEED_MATRIX_COMPLEX_INVERSION(a, n, perm, a_1, info)            \
  do {                                                                        \
    const int p1 = 1, m1 = -1;                                                \
    int nb = CLEED_COMPLEX_ILAENV(&p1, &(n), &m1);                            \
    int lwork = (n)*nb;                                                       \
    cleed_matrix_complex *work = cleed_matrix_complex_alloc((n), nb);         \
    CLEED_COMPLEX_GETRI( n, a, perm, work, &lwork, info);             \
    cleed_matrix_complex_free(work);                                      \
} while(0);

/*!
 * Performs LU decomposition on matrix \p a .
 *
 * \param[in,out] a Pointer to matrix to perform LU decomposition.
 * \param[in,out] n Pointer to the number of columns in \p a .
 * \param[in,out] perm Pointer to permutation vector.
 *
 * \return Error code from lapack's getrf function.
 */
__attribute__ ((nonnull))
static inline int
cleed_linalg_LU_decomp(cleed_matrix *a,
                       size_t *n,
                       cleed_permutation *perm)
{
  int info; CLEED_REAL_GETRF( n, n, a, n, perm, info ); return (info);
}

/*!
 * Performs LU decomposition on complex matrix \p a .
 *
 * \param[in,out] a Pointer to complex matrix to perform LU decomposition.
 * \param[in,out] n Pointer to the number of columns in \p a .
 * \param[in,out] perm Pointer to permutation vector.
 *
 * \return Error code from lapack's getrf function.
 */
__attribute__ ((nonnull))
static inline int
cleed_linalg_complex_LU_decomp(cleed_matrix_complex *a,
                                size_t *n,
                                cleed_permutation *perm)
{
  int info; CLEED_COMPLEX_GETRF( n, n, a, n, perm, info ); return (info);
}

/*!
 * Performs matrix inversion.
 *
 * \param[in] a Pointer to input matrix (from LU decomposition).
 * \param[in,out] a_1 Pointer to inverse matrix.
 * \param[in] perm Pointer to permutation vector (from LU decomposition).
 * \param n Number of columns in \p a and \p a_1
 *
 * \return C style return code.
 */
__attribute__ ((nonnull))
static inline int
cleed_linalg_invert(cleed_matrix *a,
                    cleed_matrix *a_1,
                    cleed_permutation *perm,
                    size_t n)
{
  cleed_matrix *tmp = a_1;
  int info = 0;
  const int p1 = 1, m1 = -1;
  int nb = CLEED_REAL_ILAENV(&p1, &(n), &m1);
  int lwork = (n)*nb;
  cleed_matrix *work = cleed_matrix_alloc((n), nb);

  a_1 = a; a = tmp; /* switch pointers */
  CLEED_REAL_GETRI( n, a_1, perm, work, &lwork, &info);
  cleed_matrix_free(work);

  return (info);
}

/*!
 * Performs complex matrix inversion.
 *
 * \param[in] a Pointer to input matrix (from LU decomposition).
 * \param[in,out] a_1 Pointer to inverse matrix.
 * \param[in] perm Pointer to permutation vector (from LU decomposition).
 * \param n Number of columns in \p a and \p a_1
 *
 * \return C style return code.
 */
__attribute__ ((nonnull))
static int
cleed_linalg_complex_invert(cleed_matrix_complex *a,
                            cleed_matrix_complex *a_1,
                            cleed_permutation *perm,
                            size_t *n)
{
  cleed_matrix_complex *tmp = a_1;
  const int p1 = 1, m1 = -1;
  int nb = CLEED_COMPLEX_ILAENV(&p1, &(n), &m1);
  int lwork = (n)*nb;
  cleed_matrix_complex *work = cleed_matrix_complex_alloc((n), nb);

  a_1 = a; a = tmp; /* switch pointers */
  CLEED_COMPLEX_GETRI( n, a_1, perm, work, &lwork, info);
  cleed_matrix_complex_free(work);

  return (info);
}

#elif USE_GSL

# define CLEED_REAL_LU_DECOMPOSITION(a, perm, s, info)                        \
(info = gsl_linalg_LU_decomp ((a), (perm), &s))

# define CLEED_COMPLEX_LU_DECOMPOSITION(a, perm, s, info)                     \
(info = gsl_linalg_complex_LU_decomp ((a), (perm), &s))

# define CLEED_MATRIX_REAL_INVERSION(a, perm, a_1, info)                  \
(info = gsl_linalg_LU_invert ((a), (perm), (a_1)))

# define CLEED_MATRIX_COMPLEX_INVERSION(a, perm, a_1, info)               \
((info) = gsl_linalg_complex_LU_invert ((a), (perm), (a_1)))

/*!
 * Performs LU decomposition on matrix \p a .
 *
 * \param[in,out] a Pointer to matrix to perform LU decomposition.
 * \param[in,out] n Pointer to the number of columns in \p a .
 * \param[in,out] perm Pointer to permutation vector.
 *
 * \return Error code from lapack's getrf function.
 */
__attribute__ ((nonnull))
static inline int
cleed_linalg_LU_decomp(cleed_matrix *a,
                       size_t *n,
                       cleed_permutation *perm)
{
  int s; return (gsl_linalg_LU_decomp(a, perm, &s)); *(n) = *(n)*1;
}

/*!
 * Performs LU decomposition on complex matrix \p a .
 *
 * \param[in,out] a Pointer to complex matrix to perform LU decomposition.
 * \param[in,out] n Pointer to the number of columns in \p a .
 * \param[in,out] perm Pointer to permutation vector.
 *
 * \return Error code from lapack's getrf function.
 */
__attribute__ ((nonnull))
static inline int
cleed_linalg_complex_LU_decomp(cleed_matrix_complex *a,
                               size_t *n,
                               cleed_permutation *perm)
{
  int s; return (gsl_linalg_complex_LU_decomp(a, perm, &s)); *(n) = *(n)*1;
}

/*!
 * Performs matrix inversion.
 *
 * \param[in] a Pointer to input matrix (from LU decomposition).
 * \param[in,out] a_1 Pointer to inverse matrix.
 * \param[in] perm Pointer to permutation vector (from LU decomposition).
 * \param n Number of columns in \p a and \p a_1
 *
 * \return C style return code.
 */
__attribute__ ((nonnull))
static inline int
cleed_linalg_invert(gsl_matrix *a,
                    gsl_matrix *a_1,
                    size_t *n)
{
  return (gsl_linalg_LU_invert(a, perm, a_1)); *(n) = *(n)*1;
}

/*!
 * Performs complex matrix inversion.
 *
 * \param[in] a Pointer to input matrix (from LU decomposition).
 * \param[in,out] a_1 Pointer to inverse matrix.
 * \param[in] perm Pointer to permutation vector (from LU decomposition).
 * \param n Number of columns in \p a and \p a_1
 *
 * \return C style return code.
 */
__attribute__ ((nonnull))
static int
cleed_linalg_complex_invert(gsl_matrix_complex *a,
                            gsl_matrix_complex *a_1,
                            size_t *n)
{
  return (gsl_linalg_complex_LU_invert(a, perm, a_1)); *(n) = *(n)*1;
}

#else /* USE_NATIVE */

/*!
 * Performs LU decomposition on matrix \p a .
 *
 * \param[in,out] a Pointer to matrix to perform LU decomposition.
 * \param[in,out] n Pointer to the number of columns in \p a .
 * \param[in,out] perm Pointer to permutation vector.
 *
 * \return Error code from lapack's getrf function.
 */
__attribute__ ((nonnull))
static inline int
cleed_linalg_LU_decomp(cleed_matrix *a,
                       size_t *n,
                       cleed_permutation *perm)
{
  return (r_ludcmp( a->rel, perm, a->cols )); *(n) = *(n)*1;
}

/*!
 * Performs LU decomposition on complex matrix \p a .
 *
 * \param[in,out] a Pointer to complex matrix to perform LU decomposition.
 * \param[in,out] n Pointer to the number of columns in \p a .
 * \param[in,out] perm Pointer to permutation vector.
 *
 * \return Error code from c_ludcmp() .
 */
__attribute__ ((nonnull))
static inline int
cleed_linalg_complex_LU_decomp(cleed_matrix_complex *a,
                               size_t *n,
                               cleed_permutation *perm)
{
  return (c_ludcmp( a->rel, a->iel, perm, a->cols )); *(n) = *(n)*1;
}

/*!
 * Performs matrix inversion.
 *
 * \param[in] a Pointer to input matrix (from LU decomposition).
 * \param[in,out] a_1 Pointer to inverse matrix.
 * \param[in] perm Pointer to permutation vector (from LU decomposition).
 * \param n Number of columns in \p a and \p a_1
 *
 * \return C style return code.
 */
__attribute__ ((nonnull))
static inline int
cleed_linalg_invert(cleed_matrix *a,
                    cleed_matrix *a_1,
                    cleed_permutation *perm,
                    size_t *n)
{
  return (r_luinv( a_1->rel, a_1->iel, perm, a->cols ));  *(n) = *(n)*1;
}

/*!
 * Performs complex matrix inversion.
 *
 * \param[in] a Pointer to input matrix (from LU decomposition).
 * \param[in,out] a_1 Pointer to inverse matrix.
 * \param[in] perm Pointer to permutation vector (from LU decomposition).
 * \param n Number of columns in \p a and \p a_1
 *
 * \return C style return code.
 */
__attribute__ ((nonnull))
static int
cleed_linalg_complex_invert(cleed_matrix_complex *a,
                            cleed_matrix_complex *a_1,
                            cleed_permutation *perm,
                            size_t *n)
{
  return (c_luinv( a_1->rel, a_1->iel, a->rel, a->iel, perm, a->cols )); *(n) = *(n)*1;
}

# define CLEED_REAL_LU_DECOMPOSITION(a, perm, n, info)                        \
do { ((info) = r_ludcmp((a)->rel, &(perm), (a)->cols)); } while(0)

# define CLEED_COMPLEX_LU_DECOMPOSITION(a, perm, n, info)                     \
do { ((info) = c_ludcmp((a)->rel, (a)->iel, &(perm), (a)->cols)); } while(0)

# define CLEED_MATRIX_REAL_INVERSION(a, n, perm, a_1, info)               \
do { ((info) = r_luinv((a_1)->rel, (a_1)->iel, &(perm), (a)->cols)); } while(0)

# define CLEED_MATRIX_COMPLEX_INVERSION(a, n, perm, a_1, info)            \
do  { ((info) = c_luinv((a_1)->rel, (a_1)->iel,                       \
                  (a)->rel, (a)->iel, &(perm), (a)->cols)); } while(0)

#endif


#ifdef __cplusplus /* if this is a C++ compiler then use C linkage */
}
#endif /* __cplusplus */

#endif /* CLEED_LINALG_H */
