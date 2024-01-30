#ifndef CLEED_MATRIX_GSL_H
#define CLEED_MATRIX_GSL_H

#include "cleed_real.h"
#include "mat_def.h"
#include "cc_attribute.h"

#ifdef CLEED_REAL_IS_DOUBLE
#include "_cleed_matrix_gsl_double.h"
#else
#include "_cleed_matrix_gsl_float.h"
#endif


#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
namespace cleed {
#endif

#define CLEED_REAL_MAT2CBLAS(Mx,cblas_mx)     mat2gsl((Mx),(void*)(cblas_mx),NUM_REAL)
#define CLEED_COMPLEX_MAT2CBLAS(Mx,cblas_mx)  mat2gsl((Mx),(void*)(cblas_mx),NUM_COMPLEX)

#define CLEED_REAL_CBLAS2MAT(cblas_mx,Mx)     gsl2mat((const void*)(cblas_mx),NUM_REAL,(Mx),NUM_REAL)
#define CLEED_COMPLEX_CBLAS2MAT(cblas_mx,Mx)  gsl2mat((const void*)(cblas_mx),NUM_COMPLEX,(Mx),NUM_COMPLEX)

/* CLEED <-> GSL utility functions (should only compiled if USE_GSL is enabled) */
__attribute__ ((nonnull(1)))
int gsl2mat(const void *gsl_mx_ptr, mat_enum gsl_type, mat Mx, mat_enum mat_type);

__attribute__ ((nonnull(1)))
int mat2gsl(const mat Mx, void *gsl_mx_ptr, mat_enum gsl_type);

__attribute__ ((nonnull(1)))
int gsl2cblas(const void *gsl_mx_ptr, mat_enum gsl_type, real *cblas_mx, mat_enum cblas_type);

__attribute__ ((nonnull(1)))
int cblas2gsl(const real *cblas_mx, mat_enum cblas_type, void *gsl_mx_ptr, mat_enum gsl_type, size_t cols, size_t rows);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* namespace cleed */
#endif

#endif /* CLEED_MATRIX_GSL_H */