#ifndef CLEED_BASIC_MATRIX_H
#define CLEED_BASIC_MATRIX_H

#include <string.h>

#include "cc_attribute.h"
#include "cleed_real.h"
#include "cleed_complex.h"

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
namespace cleed {
#endif

typedef cleed_real cleed_basic_matrix;           /*!< A simple row-major matrix */
typedef cleed_real cleed_basic_matrix_complex;   /*!< A simple row-major complex matrix */

/* outline cleed_basic_matrix functions for a common simple matrix interface */
# define CLEED_BASIC_MATRIX_GET(a, i, j, m, n)      ((a)[(i)*(n) + (j)])
# define CLEED_BASIC_MATRIX_SET(a, i, j, m, n, x)   do { (a)[(i)*(n) + (j)] = (x); } while (0)

__attribute__((alloc_size(1,2), returns_nonnull))
static inline cleed_basic_matrix *cleed_basic_matrix_alloc(size_t rows, size_t cols)
{
  return (cleed_basic_matrix *)calloc(rows*cols + 1, sizeof(cleed_basic_matrix));
}

__attribute__ ((nonnull))
static inline int cleed_basic_matrix_copy(cleed_basic_matrix *dst, const cleed_basic_matrix *src, size_t rows, size_t cols)
{
#if (! __GNUC__)
  if (src == NULL) { ERROR_MSG("source matrix is NULL\n"); return (-1); }
  if (dst == NULL) { ERROR_MSG("destination matrix is NULL\n"); return (-1); }
#endif
  memcpy(dst, src, sizeof(cleed_basic_matrix) * rows *cols);
  return (0);
}

static inline void cleed_basic_matrix_free(cleed_basic_matrix *m)
{
  if (m != NULL) { free(m); }
  m = NULL;
}

__attribute__ ((nonnull))
static inline cleed_real cleed_basic_matrix_get(cleed_basic_matrix *m, size_t i, size_t j, size_t cols)
{
  return (m[i*cols + j]);
}

__attribute__ ((nonnull))
static inline void cleed_basic_matrix_set(cleed_basic_matrix *m, size_t i, size_t j, size_t cols, cleed_real x)
{
  m[i*cols + j] = x;
}

/* same for a basic complex matrix */
__attribute__((alloc_size(1,2), returns_nonnull))
static inline cleed_basic_matrix_complex *cleed_basic_matrix_complex_alloc(size_t rows, size_t cols)
{
  return ( (cleed_basic_matrix_complex *) calloc( rows*cols*2 + 1, sizeof(cleed_basic_matrix) ) );
}

__attribute__ ((nonnull(1,2)))
static inline int cleed_basic_matrix_complex_copy(cleed_basic_matrix_complex *dst, const cleed_basic_matrix_complex *src, size_t rows, size_t cols)
{
#if (! __GNUC__)
  if (src == NULL) { ERROR_MSG("source matrix is NULL\n"); return (-1); }
  if (dst == NULL) { ERROR_MSG("destination matrix is NULL\n"); return (-1); }
#endif
  memcpy(dst, src, sizeof(cleed_basic_matrix_complex)*rows*cols*2);
  return (0);
}

__attribute__ ((nonnull))
static inline void cleed_basic_matrix_complex_free(cleed_basic_matrix_complex *m)
{
  if (m != NULL) { free(m); }
  m = NULL;
}

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* namespace cleed */
#endif

#endif /* CLEED_BASIC_MATRIX_H */