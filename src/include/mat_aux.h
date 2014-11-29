/*********************************************************************
 *													mat_aux.h
 *
 *  Copyright (C) 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
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
 * \file mat_aux.h
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


#ifndef __mat_aux_h__
#define __mat_aux_h__

/*****************************************************
 * Define useful Preprocessor MACROS for matrix types
 *****************************************************/

/*!
 * \define REAL_MATRIX_ALLOC(rows,cols)
 * Allocates memory for a real matrix using either CBLAS, GSL or native
 * interface.
 *
 * \define COMPLEX_MATRIX_ALLOC(rows,cols)
 * Allocates memory for a complex matrix using either CBLAS, GSL or native
 * interface.
 *
 * \define REAL_MATRIX_FREE(a)
 * Frees a real matrix from memory using either CBLAS, GSL or native interface.
 *
 * \define COMPLEX_MATRIX_FREE(a)
 * Frees a complex matrix from memory using either CBLAS, GSL or native interface.
 *
 * \define COMPLEX(z,r,i)
 * Allocates the complex number \c z with real and imaginary parts.
 *
 * \define REAL_MAT2CBLAS(Mx,cblas_mx)
 * Converts from native real matrix to CBLAS format (which will be the GSL
 * implementation if \c USE_GSL is defined).
 *
 * \define COMPLEX_MAT2CBLAS(Mx,cblas_mx)
 * Converts from native complex matrix to CBLAS format (which will be the GSL
 * implementation if \c USE_GSL is defined).
 *
 * \define REAL_CBLAS2MAT(cblas_mx,Mx)
 * Converts from CBLAS real matrix (may be GSL implementation if \c USE_GSL is
 * defined) to the native matrix type.
 *
 * \define COMPLEX_CBLAS2MAT(cblas_mx,Mx)
 * Converts from CBLAS complex matrix (may be GSL implementation if \c USE_GSL
 * is defined) to the native matrix type.
 *
 * \define REAL_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr)
 * Macro for performing matrix multiplication involving real matrices
 * (backend dependent) using either CBLAS, GSL or native implementation.
 *
 * \define COMPLEX_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr)
 * Macro for performing matrix multiplication involving complex matrices
 * (backend dependent) using either CBLAS, GSL or native implementation.
 */

#if defined( USE_GSL )

#ifdef REAL_IS_DOUBLE

#define REAL_MATRIX                     gsl_matrix
#define COMPLEX_MATRIX                  gsl_matrix_complex

#define REAL_MATRIX_ALLOC(rows,cols)    gsl_matrix_calloc((rows), (cols))
#define COMPLEX_MATRIX_ALLOC(rows,cols) gsl_matrix_complex_calloc((rows), (cols))

#define REAL_MATRIX_FREE(a)             gsl_matrix_free((a))
#define COMPLEX_MATRIX_FREE(a)          gsl_matrix_complex_free((a))

#define REAL_MAT2CBLAS(Mx,cblas_mx)     mat2gsl((cblas_mx),NULL,NUM_REAL,(Mx))
#define COMPLEX_MAT2CBLAS(Mx,cblas_mx)  mat2gsl(NULL,(cblas_mx),NUM_COMPLEX,(Mx))

#define REAL_CBLAS2MAT(cblas_mx,Mx)     gsl2mat((cblas_mx),NULL,NUM_REAL,(Mx))
#define COMPLEX_CBLAS2MAT(cblas_mx,Mx)  gsl2mat(NULL,(cblas_mx),NUM_COMPLEX,(Mx))

#define REAL_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr)          \
gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, (alpha), (m1), (m2), (beta), (mr))

#define COMPLEX_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr)       \
gsl_blas_zgemm(CblasNoTrans, CblasNoTrans, (alpha), (m1), (m2), (beta), (mr))

#define COMPLEX(z,r,i) gsl_complex (z)={.dat[0]=(double)(r),.dat[1]=(double)(i)}

#else

#define REAL_MATRIX                     gsl_matrix_float
#define COMPLEX_MATRIX                  gsl_matrix_complex_float

#define REAL_MATRIX_ALLOC(rows,cols)    gsl_matrix_float_calloc((rows),(cols))
#define COMPLEX_MATRIX_ALLOC(rows,cols) gsl_matrix_complex_float_calloc((rows),(cols))

#define REAL_MATRIX_FREE(a)             gsl_matrix_float_free((a))
#define COMPLEX_MATRIX_FREE(a)          gsl_matrix_complex_float_free((a))

#define REAL_MAT2CBLAS(Mx,cblas_mx)     mat2gsl((cblas_mx),NULL,NUM_REAL,(Mx))
#define COMPLEX_MAT2CBLAS(Mx,cblas_mx)  mat2gsl(NULL,(cblas_mx),NUM_COMPLEX,(Mx))

#define REAL_CBLAS2MAT(cblas_mx,Mx)     gsl2mat((cblas_mx),NULL,NUM_REAL,(Mx))
#define COMPLEX_CBLAS2MAT(cblas_mx,Mx)  gsl2mat(NULL,(cblas_mx),NUM_COMPLEX,(Mx))

#define REAL_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr)          \
gsl_blas_sgemm(CblasNoTrans, CblasNoTrans, (alpha), (m1), (m2), (beta), (mr))

#define COMPLEX_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr)       \
gsl_blas_cgemm(CblasNoTrans, CblasNoTrans, (alpha), (m1), (m2), (beta), (mr))

#define COMPLEX(z,r,i)                                                        \
gsl_complex_float (z) = {.dat[0]=(float)(r), .dat[1]=(float)(i)}

#endif /* sizeof(real) */

#elif defined( USE_CBLAS ) || defined( USE_MKL ) /* CBLAS */

#define REAL_MATRIX                     real
#define COMPLEX_MATRIX                  real

#define REAL_MATRIX_ALLOC(rows,cols)    (real*)calloc((cols)*(rows)*sizeof(real))
#define COMPLEX_MATRIX_ALLOC(rows,cols) (real*)calloc(2*(cols)*(rows)*sizeof(real))

#define REAL_MATRIX_FREE(a)             free((a))
#define COMPLEX_MATRIX_FREE(a)          free((a))

#define REAL_MAT2CBLAS(Mx,cblas_mx)     mat2cblas((cblas_mx),NUM_REAL,(Mx))
#define COMPLEX_MAT2CBLAS(Mx,cblas_mx)  mat2cblas((cblas_mx),NUM_COMPLEX,(Mx))

#define REAL_CBLAS2MAT(cblas_mx,Mx)     cblas2mat((Mx),(cblas_mx))
#define COMPLEX_CBLAS2MAT(cblas_mx,Mx)  cblas2mat((Mx),(cblas_mx))

#define COMPLEX(z,i,r)                  real z[2] = {(real)(i), (real)(r)}

#ifdef REAL_IS_DOUBLE

#define REAL_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr)          \
cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,                        \
            (m1_rows), (m2_cols), (m1_cols),                                  \
            (alpha), (double*)(m1), (m1_cols),                                \
            (double*)(m2), (m2_cols),                                         \
            (beta), (double*)(mr), (m2_cols))

#define COMPLEX_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr)       \
cblas_zgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,                        \
            (m1_rows), (m2_cols), (m1_cols),                                  \
            (alpha), (double*)(m1), (m1_cols),                                \
            (double*)(m2), (m2_cols),                                         \
            (beta), (double*)(mr), (m2_cols))

#else

#define REAL_CBLAS_GEMM(alpha,m1,m1_cols,m2,m2_cols,beta,mr)                  \
cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,                        \
            (m1_rows), (m2_cols), (m1_cols),                                  \
            (alpha), (float*)(m1), (m1_cols),                                 \
            (float*)(m2), (m2_cols),                                          \
            (beta), (float*)(mr), (m2_cols))

#define COMPLEX_CBLAS_GEMM(alpha,m1,m1_cols,m2,m2_cols,beta,mr)               \
cblas_cgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,                        \
            (m1_rows), (m2_cols), (m1_cols),                                  \
            (alpha), (float*)(m1), (m1_cols),                                 \
            (float*)(m2), (m2_cols),                                          \
            (beta), (float*)(mr), (m2_cols))

#endif /* sizeof(real) */

#else /* native interface */

#define REAL_MATRIX                     mat
#define COMPLEX_MATRIX                  mat

#define REAL_MATRIX_ALLOC(rows,cols)    matalloc(NULL,(rows),(cols),NUM_REAL)
#define COMPLEX_MATRIX_ALLOC(rows,cols) matalloc(NULL,(rows),(cols),NUM_COMPLEX)

#define REAL_MATRIX_FREE(a)             matfree(a)
#define COMPLEX_MATRIX_FREE(a)          matfree(a)

#define REAL_MAT2CBLAS(Mx,cblas_mx)     matcopy(cblas_mx, Mx)
#define COMPLEX_MAT2CBLAS(Mx,cblas_mx)  matcopy(cblas_mx, Mx)

#define REAL_CBLAS2MAT(cblas_mx,Mx)     matcopy(Mx, cblas_mx)
#define COMPLEX_CBLAS2MAT(cblas_mx,Mx)  matcopy(Mx, cblas_mx)

#define REAL_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr)          \
do {                                                                          \
  register real rsum;                                                         \
  register real *ptr1, *ptr2, *ptr_end, *ptrr;                                \
                                                                              \
  size_t i;                                                                   \
  size_t i_c2, i_r1;                                                          \
                                                                              \
  size_t i_cr1 = (m1)->rows * (m1)->cols;                                     \
  size_t i_cr2 = (m2)->rows * (m2)->cols;                                     \
                                                                              \
  CONTROL_MSG(CONTROL, "entering real loop\n");                               \
                                                                              \
  for( ptrr = (mr)->rel + 1, i_r1 = 1; i_r1 <= i_cr1; i_r1 += (m1)->cols)     \
    for( i_c2 = 1; i_c2 <= (m2)->cols; i_c2 ++, ptrr ++)                      \
    {                                                                         \
      rsum = 0;                                                               \
      for (ptr1 = (m1)->rel + i_r1, ptr2 = (m2)->rel + i_c2,                  \
           ptr_end = (m1)->rel + i_r1 + (m1)->cols;                           \
           ptr1 < ptr_end; ptr1 ++, ptr2 += (m2)->cols)                       \
      { rsum += *ptr1 * *ptr2; }                                              \
                                                                              \
      *ptrr = rsum;                                                           \
    }                                                                         \
} while(0) /* native real matrix multiplication */

#define COMPLEX_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr)       \
do {                                                                          \
  register real rsum, isum;                                                   \
  register real *ptri1, *ptri2, *ptr_end;                                     \
  register real *ptrr1, *ptrr2;                                               \
  register real *ptrr, *ptri;                                                 \
                                                                              \
  size_t i;                                                                   \
  size_t i_c2, i_r1;                                                          \
                                                                              \
  size_t i_cr1 = (m1)->rows * (m1)->cols;                                     \
  size_t i_cr2 = (m2)->rows * (m2)->cols;                                     \
                                                                              \
  CONTROL_MSG(CONTROL, "entering complex loop\n");                            \
                                                                              \
  if(( (m1)->num_type == NUM_COMPLEX ) && ( (m2)->num_type == NUM_COMPLEX ) ) \
  {                                                                           \
    CONTROL_MSG(CONTROL, "M1 and M2 are complex \n");                         \
                                                                              \
    for( ptrr = (mr)->rel + 1, ptri = (mr)->iel + 1, i_r1 = 1;                \
       i_r1 <= i_cr1; i_r1 += (m1)->cols)                                     \
    for( i_c2 = 1; i_c2 <= (m2)->cols; i_c2 ++, ptrr ++, ptri ++)             \
    {                                                                         \
      rsum = 0; isum = 0;                                                     \
      for (ptrr1 = (m1)->rel + i_r1, ptrr2 = (m2)->rel + i_c2,                \
           ptri1 = (m1)->iel + i_r1, ptri2 = (m2)->iel + i_c2,                \
           ptr_end = (m1)->rel + i_r1 + (m1)->cols;                           \
           ptrr1 < ptr_end;                                                   \
           ptrr1 ++, ptrr2 += (m2)->cols, ptri1 ++, ptri2 += (m2)->cols)      \
      {                                                                       \
        rsum += (*ptrr1 * *ptrr2) - (*ptri1 * *ptri2);                        \
        isum += (*ptrr1 * *ptri2) + (*ptri1 * *ptrr2);                        \
      }                                                                       \
      *ptrr = rsum; *ptri = isum;                                             \
    }                                                                         \
  } /* if both m1 & m2 are complex */                                         \
                                                                              \
  /* Only m1 is complex */                                                    \
  else if ( (m1)->num_type == NUM_COMPLEX)                                    \
  {                                                                           \
    CONTROL_MSG(CONTROL, "only M1 is complex \n");                            \
                                                                              \
    for( ptrr = (mr)->rel + 1, ptri = (mr)->iel + 1, i_r1 = 1;                \
         i_r1 <= i_cr1; i_r1 += (m1)->cols)                                   \
      for( i_c2 = 1; i_c2 <= (m2)->cols; i_c2 ++, ptrr ++, ptri ++)           \
      {                                                                       \
        rsum = 0; isum = 0;                                                   \
        for (ptrr1 = (m1)->rel + i_r1, ptri1 = (m1)->iel + i_r1,              \
             ptrr2 = (m2)->rel + i_c2,                                        \
             ptr_end = (m1)->rel + i_r1 + (m1)->cols;                         \
             ptrr1 < ptr_end;                                                 \
             ptrr1 ++, ptrr2 += (m2)->cols, ptri1 ++)                         \
        {                                                                     \
          rsum += *ptrr1 * *ptrr2;                                            \
          isum += *ptri1 * *ptrr2;                                            \
        }                                                                     \
        *ptrr = rsum; *ptri = isum;                                           \
      } /* for ptrr */                                                        \
  }  /* m1 is complex */                                                      \
                                                                              \
  /* Only m2 is complex */                                                    \
  else                                                                        \
  {                                                                           \
    CONTROL_MSG(CONTROL, "only M2 is complex \n");                            \
                                                                              \
    for( ptrr = (mr)->rel + 1, ptri = (mr)->iel + 1, i_r1 = 1;                \
         i_r1 <= i_cr1; i_r1 += (m1)->cols)                                   \
      for( i_c2 = 1; i_c2 <= (m2)->cols; i_c2 ++, ptrr ++, ptri ++)           \
      {                                                                       \
        rsum = 0; isum = 0;                                                   \
        for (ptrr1 = (m1)->rel + i_r1, ptrr2 = (m2)->rel + i_c2,              \
             ptri2 = (m2)->iel + i_c2,                                        \
             ptr_end = (m1)->rel + i_r1 + (m1)->cols;                         \
             ptrr1 < ptr_end;                                                 \
             ptrr1 ++, ptrr2 += (m2)->cols, ptri2 += (m2)->cols)              \
        {                                                                     \
          rsum += *ptrr1 * *ptrr2;                                            \
          isum += *ptrr1 * *ptri2;                                            \
        }                                                                     \
        *ptrr = rsum; *ptri = isum;                                           \
      } /* for ptrr */                                                        \
    } /* m2 is complex */                                                     \
} while(0) /* native complex matrix multiplication */

#define COMPLEX(z,r,i) real z = 0.

#endif /* end USE_GSL */

/********************************************************************
 * Linear algebra interface for LU decomposition and matrix inversion
 ********************************************************************/
#define ILAENV ilaenv_
#if LAPACK_VENDOR == CLAPACK || LAPACK_VENDOR == ATLAS
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
#elif LAPACK_VENDOR == LAPACKE
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
#else
# define SGETRF(m,n,a,lda,ipiv,info) sgetrf_(m,n,a,lda,ipiv,&(info))
# define DGETRF(m,n,a,lda,ipiv,info) dgetrf_(m,n,a,lda,ipiv,&(info))
# define CGETRF(m,n,a,lda,ipiv,info) cgetrf_(m,n,a,lda,ipiv,&(info))
# define ZGETRF(m,n,a,lda,ipiv,info) zgetrf_(m,n,a,lda,ipiv,&(info))
# define SGETRI(n,a,lda,ipiv,info)   sgetri_(n,a,lda,ipiv,&(info))
# define DGETRI(n,a,lda,ipiv,info)   dgetri_(n,a,lda,ipiv,&(info))
# define CGETRI(n,a,lda,ipiv,info)   cgetri_(n,a,lda,ipiv,&(info))
# define ZGETRI(n,a,lda,ipiv,info)   zgetri_(n,a,lda,ipiv,&(info))
#endif

#ifdef REAL_IS_FLOAT
# define REAL_ILAENV(P,N,M)    ILAENV((P), "SGETRI", " ", (N), (M), (M), (M));
# define COMPLEX_ILAENV(P,N,M) ILAENV((P), "CGETRI", " ", (N), (M), (M), (M));
# define REAL_GETRF(m,n,a,lda,ipiv,info)    SGETRF(m,n,a,lda,ipiv,info)
# define REAL_GETRI(n,a,lda,ipiv,info)      SGETRI(n,a,lda,ipiv,info)
# define COMPLEX_GETRF(m,n,a,lda,ipiv,info) CGETRF(m,n,a,lda,ipiv,info)
# define COMPLEX_GETRI(n,a,lda,ipiv,info)   CGETRI(n,a,lda,ipiv,info)
#else
# define REAL_ILAENV(P,N,M)    ILAENV((P), "DGETRI", " ", (N), (M), (M), (M))
# define COMPLEX_ILAENV(P,N,M) ILAENV((P), "ZGETRI", " ", (N), (M), (M), (M))
# define REAL_GETRF(m,n,a,lda,ipiv,info)    DGETRF(m,n,a,lda,ipiv,info)
# define REAL_GETRI(n,a,lda,ipiv,info)      DGETRI(n,a,lda,ipiv,info)
# define COMPLEX_GETRF(m,n,a,lda,ipiv,info) ZGETRF(m,n,a,lda,ipiv,info)
# define COMPLEX_GETRI(n,a,lda,ipiv,info)   ZGETRI(n,a,lda,ipiv,info)
#endif

/*!
 * \define REAL_LU_DECOMPOSITION
 * Macro that performs LU decomposition for a real matrix.
 *
 * \define COMPLEX_LU_DECOMPOSITION
 * Macro that performs LU decomposition for a complex matrix.
 *
 * \define REAL_MATRIX_INVERSION
 * Macro for performing real matrix inversion.
 *
 * \define COMPLEX_MATRIX_INVERSION
 * Macro for performing complex matrix inversion.
 *
 */

#if USE_GSL
#define REAL_LU_DECOMPOSITION(m, perm, s, info)                               \
(info = gsl_linalg_LU_decomp (m, perm, &s))

#define COMPLEX_LU_DECOMPOSITION(m, perm, s, info)                            \
(info = gsl_linalg_complex_LU_decomp (m, perm, &s))

#define REAL_MATRIX_INVERSION(m, perm, inverse, info)                         \
(info = gsl_linalg_LU_invert (m, perm, inverse))

#define COMPLEX_MATRIX_INVERSION(m, perm, inverse, info)                      \
((info) = gsl_linalg_complex_LU_invert (m, perm, inverse))

#elif ( USE_CBLAS || USE_MKL || USE_LAPACK )

#define REAL_LU_DECOMPOSITION(a, perm, n, info)                               \
  REAL_GETRF( &(n), &(n), (a), &(n), (perm), (info) );

#define COMPLEX_LU_DECOMPOSITION(a, perm, n, info)                            \
  COMPLEX_GETRF( &(n), &(n), (a), &(n), &(perm), (info) );

#define REAL_MATRIX_INVERSION(a, n, perm, inverse, info)                      \
  do {                                                                        \
      const int p1 = 1, m1 = -1;                                              \
      int nb = REAL_ILAENV(&p1, &(n), &m1);                                   \
      int lwork = (n)*nb;                                                     \
      REAL_MATRIX work = REAL_MATRIX_ALLOC((n), nb);                          \
      REAL_GETRI( (n), (a), (perm), work, &lwork, (info));                    \
      REAL_MATRIX_FREE(work);                                                 \
} while(0);

#define COMPLEX_MATRIX_INVERSION(a, n, perm, inverse, info)                   \
  do {                                                                        \
    const int p1 = 1, m1 = -1;                                                \
    int nb = COMPLEX_ILAENV(&p1, &(n), &m1);                                  \
    int lwork = (n)*nb;                                                       \
    COMPLEX_MATRIX work = COMPLEX_MATRIX_ALLOC((n), nb);                      \
    COMPLEX_GETRI( (n), (a), (perm), work, &lwork, (info));                   \
    COMPLEX_MATRIX_FREE(work);                                                \
} while(0);

#else /* USE_NATIVE */

#define REAL_LU_DECOMPOSITION(m, perm, n, info)                               \
((info) = r_ludcmp((m)->rel, perm, (m)->cols))

#define COMPLEX_LU_DECOMPOSITION(m, perm, n, info)                            \
((info) = c_ludcmp((m)->rel, (m)->iel, perm, (m)->cols))

#define REAL_MATRIX_INVERSION(m, n, perm, inverse, info)                      \
((info) = r_luinv ((inverse)->rel, (inverse)->iel,                            \
                  (m)->rel, (m)->iel, (perm), (m)->cols))

#define COMPLEX_MATRIX_INVERSION(m, n, perm, inverse, info)                   \
((info) = c_luinv((inverse)->rel, (inverse)->iel,                             \
                  (m)->rel, (m)->iel, (perm), (m)->cols))

#endif


#ifdef __cplusplus__ /* if this is a C++ compiler then use C linkage */
extern C {
#endif /* __cplusplus__ */

/* includes */


/* defines */


/* enums, structs and typedefs */


/* functions */



#ifdef __cplusplus__ /* if this is a C++ compiler then use C linkage */
namespace cleed {

/* classes */

} /* namespace cleed */

}
#endif /* __cplusplus__ */

#endif /* __mat_aux_h__ */
