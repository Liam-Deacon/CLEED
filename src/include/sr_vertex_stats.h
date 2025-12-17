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

/*
 * All arrays are 1-based:
 * - y: 1..mpar
 * - p: 1..mpar, 1..ndim
 * - out_*_p: 1..ndim
 *
 * The y statistics are returned via out_*_y.
 */
void sr_vertex_minmax(const real *y, real **p, int ndim, int mpar,
                      real *out_min_y, real *out_max_y,
                      real *out_min_p, real *out_max_p);

void sr_vertex_avg(const real *y, real **p, int ndim, int mpar,
                   real *out_avg_y, real *out_avg_p);

void sr_vertex_dev(const real *y, real **p, int ndim, int mpar,
                   real avg_y, const real *avg_p,
                   real *out_dev_y, real *out_dev_p);

#ifdef __cplusplus
}
#endif

#endif /* SR_VERTEX_STATS_H */

