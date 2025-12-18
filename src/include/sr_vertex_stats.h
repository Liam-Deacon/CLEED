/*********************************************************************
 *                     SR_VERTEX_STATS.H
 *
 *  GPL-3.0-or-later
 *
 *  Statistics over SEARCH vertex files (1-based arrays).
 *********************************************************************/

#ifndef SR_VERTEX_STATS_H
#define SR_VERTEX_STATS_H

#include "real.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file sr_vertex_stats.h
 * @brief Statistics over SEARCH simplex vertex arrays (1-based indexing).
 *
 * The legacy SEARCH codebase uses 1-based vectors/matrices throughout.
 * These helpers keep the invariants explicit and provide small, testable
 * building blocks for tools such as `verstat`.
 *
 * All arrays are 1-based:
 * - `y`: indices `1..mpar`
 * - `p`: indices `[1..mpar][1..ndim]`
 * - output parameter vectors: indices `1..ndim`
 */

/**
 * @brief Compute per-simplex min/max statistics for y and each coordinate of p.
 *
 * @param y Function values per vertex (`1..mpar`).
 * @param p Vertex coordinates (`[1..mpar][1..ndim]`).
 * @param ndim Number of parameters (dimensions).
 * @param mpar Number of simplex vertices (`ndim+1` in typical SEARCH usage).
 * @param out_min_y Optional output: minimum y value.
 * @param out_max_y Optional output: maximum y value.
 * @param out_min_p Optional output: per-dimension minimum coordinate (`1..ndim`).
 * @param out_max_p Optional output: per-dimension maximum coordinate (`1..ndim`).
 */
void sr_vertex_minmax(const real *y, real **p, int ndim, int mpar,
                      real *out_min_y, real *out_max_y,
                      real *out_min_p, real *out_max_p);

/**
 * @brief Compute per-simplex averages for y and each coordinate of p.
 *
 * @param y Function values per vertex (`1..mpar`).
 * @param p Vertex coordinates (`[1..mpar][1..ndim]`).
 * @param ndim Number of parameters (dimensions).
 * @param mpar Number of simplex vertices (`ndim+1` in typical SEARCH usage).
 * @param out_avg_y Optional output: average y value.
 * @param out_avg_p Optional output: per-dimension average coordinate (`1..ndim`).
 */
void sr_vertex_avg(const real *y, real **p, int ndim, int mpar,
                   real *out_avg_y, real *out_avg_p);

/**
 * @brief Compute per-simplex standard deviations for y and each coordinate of p.
 *
 * This function takes the already-computed averages to avoid re-walking the
 * arrays when multiple statistics are needed.
 *
 * @param y Function values per vertex (`1..mpar`).
 * @param p Vertex coordinates (`[1..mpar][1..ndim]`).
 * @param ndim Number of parameters (dimensions).
 * @param mpar Number of simplex vertices (`ndim+1` in typical SEARCH usage).
 * @param avg_y Precomputed average y value.
 * @param avg_p Precomputed per-dimension average coordinates (`1..ndim`).
 * @param out_dev_y Optional output: standard deviation of y.
 * @param out_dev_p Optional output: per-dimension standard deviation (`1..ndim`).
 */
void sr_vertex_dev(const real *y, real **p, int ndim, int mpar,
                   real avg_y, const real *avg_p,
                   real *out_dev_y, real *out_dev_p);

#ifdef __cplusplus
}
#endif

#endif /* SR_VERTEX_STATS_H */
