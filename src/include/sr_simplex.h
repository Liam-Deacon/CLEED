/*********************************************************************
 *                       SR_SIMPLEX.H
 *
 *  GPL-3.0-or-later
 *********************************************************************/

/**
 * @file sr_simplex.h
 * @brief Shared helpers for SEARCH simplex data structures and drivers.
 *
 * SEARCH uses a historical convention of 1-based indexing for vectors and
 * matrices. These helpers preserve that contract and provide small reusable
 * building blocks used by multiple optimiser implementations and driver
 * routines.
 */

#ifndef SR_SIMPLEX_H
#define SR_SIMPLEX_H

#include "search.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Copy one 1-based point vector into another.
 *
 * @param dst Destination vector (1..ndim).
 * @param src Source vector (1..ndim).
 * @param ndim Dimensionality of the point.
 */
void sr_simplex_copy_point(real *dst, const real *src, int ndim);

/**
 * @brief Compute the centroid of all simplex vertices excluding one.
 *
 * @param p Simplex vertices `p[1..ndim+1][1..ndim]`.
 * @param centroid Output vector `centroid[1..ndim]`.
 * @param ndim Dimensionality of the problem.
 * @param exclude_index Index of the vertex to exclude (1..ndim+1).
 */
void sr_simplex_centroid_excluding(const real **p, real *centroid, int ndim, int exclude_index);

/**
 * @brief Find the lowest/highest/2nd-highest vertices by objective value.
 *
 * @param y Objective values `y[1..ndim+1]`.
 * @param ndim Dimensionality of the problem.
 * @param ilo Out index of lowest value.
 * @param ihi Out index of highest value.
 * @param inhi Out index of second-highest value.
 */
void sr_simplex_extremes(const real *y, int ndim, int *ilo, int *ihi, int *inhi);

/**
 * @brief Working buffers for simplex drivers.
 *
 * The buffers are allocated with @ref sr_alloc_vector and @ref sr_alloc_matrix
 * and are suitable for SEARCH optimiser APIs that require 1-based arrays.
 */
typedef struct sr_simplex_buffers {
  int ndim;
  int mpar;
  real *x;    /**< scratch vector (1..ndim) */
  real *y;    /**< objective values (1..mpar) */
  real **p;   /**< simplex vertices (1..mpar, 1..ndim) */
} sr_simplex_buffers;

/**
 * @brief Allocate buffers for a simplex driver.
 *
 * @param b Buffer struct to fill.
 * @param ndim Dimensionality of the problem.
 * @return 0 on success, non-zero on allocation failure.
 */
int sr_simplex_buffers_alloc(sr_simplex_buffers *b, int ndim);

/**
 * @brief Free buffers allocated by @ref sr_simplex_buffers_alloc.
 *
 * @param b Buffer struct to free (fields are reset to NULL/0).
 */
void sr_simplex_buffers_free(sr_simplex_buffers *b);

/**
 * @brief Build an axis-aligned initial simplex around the origin.
 *
 * The simplex is:
 * - vertex 1: all zeros
 * - vertex (j+1): coordinate j displaced by @p dpos
 *
 * @param b Allocated simplex buffers.
 * @param dpos Initial displacement for axis-aligned vertices.
 * @param func Objective function to evaluate at each vertex.
 * @return 0 on success, non-zero on invalid inputs.
 */
int sr_simplex_build_initial(sr_simplex_buffers *b, real dpos, real (*func)(real *));

/**
 * @brief Populate simplex buffers by reading a vertex file via sr_rdver().
 *
 * @param b Allocated simplex buffers.
 * @param bak_file Vertex file path.
 * @return 0 on success, non-zero on failure.
 */
int sr_simplex_read_vertex(sr_simplex_buffers *b, const char *bak_file);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

