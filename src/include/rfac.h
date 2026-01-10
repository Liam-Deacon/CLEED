/**
 * @file rfac.h
 * @brief Master include file for R-factor calculations.
 *
 * @defgroup rfac_core R-Factor Core
 * @brief R-factor calculation framework for LEED-IV curve comparison.
 *
 * The RFAC module provides various reliability factors (R-factors) for
 * quantitative comparison of theoretical and experimental I-V curves.
 *
 * ## Available R-Factors
 *
 * | R-Factor | Description |
 * |----------|-------------|
 * | R1 | Integral of absolute difference |
 * | R2 | Integral of squared difference |
 * | Rp | Pendry R-factor (logarithmic derivative) |
 * | Rb | Zanazzi-Jona R-factor |
 * | Rg | Goodness-of-fit R-factor |
 *
 * ## References
 *
 * @see Pendry, J. Phys. C 13, 937 (1980) for Rp derivation
 * @see Zanazzi & Jona, Surf. Sci. 62, 61 (1977) for Rb
 */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef RFAC_H
#define RFAC_H

/**
 * @addtogroup rfac_core
 * @{
 */

/*********************************************************************
 Other (basic) includes
*********************************************************************/

#include "gh_stddef.h"
#define REAL_IS_FLOAT    /* should be float for DEC alpha */
#include "real.h"

/*********************************************************************
 Include type definitions and constant values and functions for RFAC
*********************************************************************/

#include "rfac_def.h"
#include "rfac_func.h"
#include "rfac_ver.h"

/** @} */ /* end of rfac_core group */

/*********************************************************************
 End of include file 
*********************************************************************/

#endif /* RFAC_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif