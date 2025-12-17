/*********************************************************************
 *                       SR_ALLOC.H
 *
 *  GPL-3.0-or-later
 *
 *  Small allocation helpers for SEARCH code that historically used
 *  1-based vector/matrix helpers. These helpers allocate
 *  0-based storage with an extra leading element so existing SEARCH
 *  code can keep using 1-based indices (i.e. indices 1..N).
 *********************************************************************/

#ifndef SR_ALLOC_H
#define SR_ALLOC_H

// cppcheck-suppress missingIncludeSystem
#include <stddef.h>

#include "real.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Allocates an array usable with indices 0..length (1..length are typical). */
real *sr_alloc_vector(size_t length);
void sr_free_vector(real *v);

/* Allocates a matrix usable with indices [0..rows][0..cols] (1-based typical). */
real **sr_alloc_matrix(size_t rows, size_t cols);
void sr_free_matrix(real **m);

#ifdef __cplusplus
}
#endif

#endif /* SR_ALLOC_H */
