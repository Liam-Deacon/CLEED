/*********************************************************************
 *                       SR_ALLOC.C
 *
 *  GPL-3.0-or-later
 *********************************************************************/

/**
 * @file sr_alloc.c
 * @brief Implementation of allocation helpers for legacy 1-based SEARCH arrays.
 *
 * These helpers intentionally keep the allocation strategy simple and portable
 * (calloc + contiguous storage) so that tests can rely on deterministic
 * zero-initialised buffers.
 */

// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>

#include "sr_alloc.h"

real *sr_alloc_vector(size_t length)
{
  /* +1 so callers can use 1..length without pointer tricks. */
  return (real *)calloc(length + 1, sizeof(real));
}

void sr_free_vector(real *v)
{
  free(v);
}

real **sr_alloc_matrix(size_t rows, size_t cols)
{
  /* +1 so callers can use 1..rows and 1..cols. */
  size_t nrows = rows + 1;
  size_t ncols = cols + 1;

  real **m = (real **)calloc(nrows, sizeof(real *));
  if (m == NULL) return NULL;

  real *data = (real *)calloc(nrows * ncols, sizeof(real));
  if (data == NULL) {
    free(m);
    return NULL;
  }

  for (size_t i = 0; i < nrows; i++) {
    m[i] = data + i * ncols;
  }

  return m;
}

void sr_free_matrix(real **m)
{
  if (m == NULL) return;
  free(m[0]);
  free(m);
}
