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

/**
 * @file sr_alloc.h
 * @brief Allocation helpers for legacy 1-based SEARCH arrays.
 *
 * SEARCH historically uses 1-based indexing for vectors and matrices.
 * These helpers allocate a leading "dummy" element so the call sites
 * can keep using indices `1..N` without pointer arithmetic tricks.
 */

/**
 * @brief Allocate a 1-based vector.
 *
 * The returned buffer is valid for indices `0..length` (with `1..length`
 * being the conventional SEARCH range).
 *
 * @param length Desired 1-based logical length (N).
 * @return Allocated vector or NULL on allocation failure.
 */
real *sr_alloc_vector(size_t length);

/**
 * @brief Free a vector allocated by @ref sr_alloc_vector.
 *
 * @param v Vector pointer (may be NULL).
 */
void sr_free_vector(real *v);

/**
 * @brief Allocate a 1-based matrix.
 *
 * The returned matrix supports indices `[0..rows][0..cols]` (with
 * `[1..rows][1..cols]` being the conventional SEARCH range).
 *
 * The storage is a single contiguous block, so `m[0]` points to the
 * start of the underlying element data.
 *
 * @param rows Desired 1-based logical row count.
 * @param cols Desired 1-based logical column count.
 * @return Allocated matrix or NULL on allocation failure.
 */
real **sr_alloc_matrix(size_t rows, size_t cols);

/**
 * @brief Free a matrix allocated by @ref sr_alloc_matrix.
 *
 * @param m Matrix pointer (may be NULL).
 */
void sr_free_matrix(real **m);

#ifdef __cplusplus
}
#endif

#endif /* SR_ALLOC_H */
