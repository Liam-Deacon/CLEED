/*********************************************************************
 *                       CLEED_CORE.H
 *
 *  Core library entry points for CLEED.
 *********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef CLEED_CORE_H
#define CLEED_CORE_H

#include <stddef.h>

/**
 * @brief In-memory LEED curve representation.
 */
typedef struct cleed_curve {
  size_t n;
  double *E;
  double *I;
} cleed_curve;

/**
 * @brief Allocate a curve with space for n samples.
 *
 * @param curve Output curve struct.
 * @param n Number of samples.
 * @return 0 on success, non-zero on failure.
 */
int cleed_curve_alloc(cleed_curve *curve, size_t n);

/**
 * @brief Free curve storage and reset fields to zero.
 */
void cleed_curve_free(cleed_curve *curve);

/**
 * @brief Copy a curve into a destination curve (allocates storage).
 *
 * @param dst Destination curve.
 * @param src Source curve.
 * @return 0 on success, non-zero on failure.
 */
int cleed_curve_copy(cleed_curve *dst, const cleed_curve *src);

/**
 * @brief Linearly interpolate intensity at the given energy.
 *
 * If the energy is out of bounds, the nearest endpoint is returned.
 *
 * @param curve Input curve.
 * @param energy Energy to sample.
 * @return Interpolated intensity, or NaN if curve is invalid.
 */
double cleed_curve_interp(const cleed_curve *curve, double energy);

/**
 * @brief Return the core library version string.
 */
const char *cleed_core_version(void);

#endif /* CLEED_CORE_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
