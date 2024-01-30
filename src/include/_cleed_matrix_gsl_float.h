#ifndef CLEED_MATRIX_GSL_FLOAT_H
#define CLEED_MATRIX_GSL_FLOAT_H

#include "cleed_real.h"
#include "cleed_complex.h"
#include <gsl/gsl_matrix.h>

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
namespace cleed {
#endif

typedef gsl_matrix_float cleed_matrix;
typedef gsl_matrix_complex_float cleed_matrix_complex;

#define CLEED_MATRIX_COPY(a, b, m, n)       gsl_matrix_float_memcopy((a),(b))
#define CLEED_MATRIX_GET(a, i, j, m, n)     gsl_matrix_float_get((a),(i),(j))
#define CLEED_MATRIX_SET(a, i, j, m, n, x)  gsl_matrix_float_set((a),(i),(j),(x))

#define CLEED_MATRIX_COMPLEX_COPY(a, b, m, n)      gsl_matrix_complex_float_memcopy((a),(b))
#define CLEED_MATRIX_COMPLEX_GET(a, i, j, m, n)   &gsl_matrix_complex_float_get((a),(i),(j)).dat
#define CLEED_MATRIX_COMPLEX_SET(a, i, j, m, n, z) gsl_matrix_complex_float_set((a),(i),(j),(z))

/* define inline functions for dealing with cleed_matrix type */
__attribute__((returns_nonnull, alloc_size(1,2)))
static inline cleed_matrix *cleed_matrix_alloc(size_t rows, size_t cols)
{
  return gsl_matrix_float_calloc(rows, cols);
}

__attribute__((nonnull))
static inline int cleed_matrix_copy(cleed_matrix *dst, const cleed_matrix *src, size_t rows, size_t cols)
{
  return gsl_matrix_float_memcpy(dst, src);
}

__attribute__((nonnull))
static inline void cleed_matrix_free(cleed_matrix *a)
{
  gsl_matrix_float_free(a);
}

__attribute__((nonnull))
static inline cleed_real cleed_matrix_get(const cleed_matrix *a, size_t i, size_t j)
{
  return gsl_matrix_float_get(a, i, j);
}

__attribute__((nonnull))
static inline void cleed_matrix_set(cleed_matrix *a, size_t i, size_t j, cleed_real x)
{
  gsl_matrix_float_set(a, i, j, x);
}

/* define inline functions for dealing with cleed_matrix_complex type */
__attribute__((returns_nonnull, alloc_size(1,2)))
static inline cleed_matrix_complex *cleed_matrix_complex_alloc(size_t rows, size_t cols)
{
  return gsl_matrix_complex_float_calloc(rows, cols);
}

__attribute__((nonnull))
static inline int cleed_matrix_complex_copy(cleed_matrix_complex *dst, const cleed_matrix_complex *src)
{
  return gsl_matrix_complex_float_memcpy(dst, src);
}

__attribute__((nonnull))
static inline void cleed_matrix_complex_free(cleed_matrix_complex *a)
{
  gsl_matrix_complex_float_free(a);
}

__attribute__((nonnull))
static inline cleed_complex cleed_matrix_complex_get(const cleed_matrix_complex *a, size_t i, size_t j)
{
  return gsl_matrix_complex_float_get(a, i, j);
}

__attribute__((nonnull))
static inline void cleed_matrix_complex_set(cleed_matrix_complex *a, size_t i, size_t j, cleed_complex x)
{
  gsl_matrix_complex_float_set(a, i, j, x);
}

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* namespace cleed */
#endif

#endif /* CLEED_MATRIX_GSL_FLOAT_H */