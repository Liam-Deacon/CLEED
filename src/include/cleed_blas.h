/*********************************************************************
 *													cleed_blas.h
 *
 *  Copyright (C) 2013-2015 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/05.02.2015 - creation
 *********************************************************************/

/*!
 * \file
 * \author Liam Deacon
 *
 * Provides an adapter design pattern to allow matrix multiplication using
 * either CBLAS, GSL or native back-ends.
 */

#ifndef CLEED_BLAS_H
#define CLEED_BLAS_H

#include <string.h>
#include "cleed_real.h"
#include "cleed_matrix.h"

#if (!__GNUC__)
# define __attribute__(x) /* empty to disable GCC attribute extension */
#endif

#ifdef __cplusplus /* if this is a C++ compiler then use C linkage */
extern "C" {
#endif

#if USE_MKL
# include <mkl_blas.h>
#elif USE_CBLAS
# include <cblas.h>
#elif USE_GSL
# include <gsl/gsl_blas.h>
#else
# warning "No BLAS package selected - using native routines."
#endif

#if ( USE_CBLAS ) || ( USE_MKL ) /* CBLAS */

# ifdef CLEED_REAL_IS_DOUBLE

#   define CLEED_REAL_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr) \
cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,                        \
            (m1_rows), (m2_cols), (m1_cols),                                  \
            (alpha), (double*)(m1), (m1_cols),                                \
            (double*)(m2), (m2_cols),                                         \
            (beta), (double*)(mr), (m2_cols))

#   define CLEED_COMPLEX_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr)       \
cblas_zgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,                        \
            (m1_rows), (m2_cols), (m1_cols),                                  \
            (alpha), (double*)(m1), (m1_cols),                                \
            (double*)(m2), (m2_cols),                                         \
            (beta), (double*)(mr), (m2_cols))

# else

#   define CLEED_REAL_CBLAS_GEMM(alpha,m1,m1_cols,m2,m2_cols,beta,mr)         \
cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,                        \
            (m1_rows), (m2_cols), (m1_cols),                                  \
            (alpha), (float*)(m1), (m1_cols),                                 \
            (float*)(m2), (m2_cols),                                          \
            (beta), (float*)(mr), (m2_cols))

#   define CLEED_COMPLEX_CBLAS_GEMM(alpha,m1,m1_cols,m2,m2_cols,beta,mr)      \
cblas_cgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,                        \
            (m1_rows), (m2_cols), (m1_cols),                                  \
            (alpha), (float*)(m1), (m1_cols),                                 \
            (float*)(m2), (m2_cols),                                          \
            (beta), (float*)(mr), (m2_cols))

# endif /* sizeof(real) */

#elif ( USE_GSL )

# define CLEED_REAL_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr)    \
gsl_blas_sgemm(CblasNoTrans, CblasNoTrans, (alpha), (m1), (m2), (beta), (mr))

# define CLEED_COMPLEX_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr) \
gsl_blas_cgemm(CblasNoTrans, CblasNoTrans, (alpha), (m1), (m2), (beta), (mr))

#else /* native interface */

# define CLEED_REAL_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr)    cleed_cblas_gemm((m1),(m2),(mr))
# define CLEED_COMPLEX_CBLAS_GEMM(alpha,m1,m1_rows,m1_cols,m2,m2_cols,beta,mr) cleed_complex_cblas_gemm((m1),(m2),(mr))

__attribute__ ((unused, nonnull))
static void
cleed_cblas_gemm(const cleed_matrix *m1,
                 const cleed_matrix *m2,
                       cleed_matrix *mr)
{
  register real rsum;
  register size_t i, i_c2, i_r1, j_m1, j_m2;

  CONTROL_MSG(CONTROL, "entering real loop\n");

  for(i=1, i_r1 = 1; i_r1 <= (m1->rows * m1->cols); i_r1 += m1->cols)
    for( i_c2 = 1; i_c2 <= m2->cols; i_c2 ++, i++)
    {
      rsum = 0;
      for ( j_m1 = i_r1, j_m2 = i_c2;
            j_m1 < (i_r1 + m1->cols);
            j_m1++, j_m2 += m2->cols )
      { rsum += m1->rel[j_m1] * m2->rel[j_m2]; }

      mr->rel[i] = rsum;
    }
}

__attribute__ ((unused, nonnull))
static void
cleed_complex_cblas_gemm(const cleed_matrix_complex *m1,
                         const cleed_matrix_complex *m2,
                               cleed_matrix_complex *mr)
{
  register real rsum, isum;
  register size_t i, i_1, i_2, j, j_1, j_2, i_c2, i_r1;

  CONTROL_MSG(CONTROL, "entering complex loop\n");

  if(( m1->num_type == NUM_COMPLEX ) && ( m2->num_type == NUM_COMPLEX ) )
  {
    CONTROL_MSG(CONTROL, "M1 and M2 are complex \n");
    for( i=1, j=1, i_r1=1; i_r1 <= (m1->rows * m1->cols); i_r1 += m1->cols )
      for( i_c2=1; i_c2 <= m2->cols; i_c2++, i++, j++ )
      {
        rsum = 0; isum = 0;
        for ( i_1=i_r1, i_2=i_c2, j_1 = i_r1, j_2 = i_c2;
            i_1 < i_r1 + m1->cols;
            i_1++, i_2 += m2->cols, j_1++, j_2 += m2->cols)
        {
          rsum += (m1->rel[i_1] * m2->rel[i_2]) - (m1->iel[j_1] * m2->iel[j_2]);
          isum += (m1->rel[i_1] * m2->iel[j_2]) + (m1->iel[j_1] * m2->iel[i_2]);
        }
        mr->rel[i] = rsum; mr->iel[j] = isum;
    }
  } /* if both m1 & m2 are complex */

  /* Only m1 is complex */
  else if ( m1->num_type == NUM_COMPLEX)
  {
    CONTROL_MSG(CONTROL, "only M1 is complex \n");
    for( i=1, j=1, i_r1 = 1; i_r1 <= (m1->rows * m1->cols); i_r1 += m1->cols)
      for( i_c2 = 1; i_c2 <= m2->cols; i_c2 ++, i++, j++)
      {
        rsum = 0; isum = 0;
        for ( i_1 = i_r1, j_1 = i_r1, i_2 = i_c2;
              i_1 < i_r1 + m1->cols;
              i_1++, i_2 += m2->cols, j_1++)
        {
          rsum += m1->rel[i_1] * m2->rel[i_2];
          isum += m1->iel[j_1] * m2->rel[i_2];
        }
        mr->rel[i] = rsum; mr->iel[j] = isum;
      } /* for ptrr */
  }  /* m1 is complex */

  /* Only m2 is complex */
  else
  {
    CONTROL_MSG(CONTROL, "only M2 is complex \n");
    for( i=1, j=1, i_r1 = 1; i_r1 <= (m1->rows * m1->cols); i_r1 += m1->cols)
      for( i_c2 = 1; i_c2 <= m2->cols; i_c2 ++, i++, j++)
      {
        rsum = 0; isum = 0;
        for (i_1 = i_r1, i_2 = i_c2, j_2 = i_c2;
             i_1 < i_r1 + m1->cols;
             i_1++, i_2 += m2->cols, j_2 += m2->cols)
        {
          rsum += m1->rel[i_1] * m2->rel[i_2];
          isum += mr->rel[i_1] * m2->iel[j_2];
        }
        mr->rel[i] = rsum; mr->iel[j] = isum;
      } /* for ptrr */
    } /* m2 is complex */
}

#endif /* end USE_GSL */

#ifdef __cplusplus /* if this is a C++ compiler then use C linkage */
} /* namespace cleed */
#endif /* __cplusplus */

#ifdef __attribute__
# undef __attribute__
#endif

#endif /* CLEED_BLAS_H */
