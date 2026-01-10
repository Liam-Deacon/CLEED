/**
 * @file leed.h
 * @brief Master include file for LEED quantum mechanical calculations.
 *
 * @defgroup leed_core LEED Core
 * @brief Low-Energy Electron Diffraction calculation framework.
 *
 * The LEED module provides quantum mechanical calculations for simulating
 * I-V curves (intensity vs. energy) of diffracted electron beams from
 * crystalline surfaces.
 *
 * ## Key Components
 *
 * - @ref leed_crystal "Crystal Structure" - Lattice and layer definitions
 * - @ref leed_scattering "Scattering Theory" - Phase shifts, T-matrices
 * - @ref leed_beams "Beam Handling" - Reciprocal space beam management
 * - @ref leed_energy "Energy Loop" - Energy-dependent calculation control
 *
 * ## Physical Background
 *
 * LEED calculations use multiple scattering theory to compute the
 * intensities of diffracted beams. The key physical inputs are:
 * - Atomic positions and thermal vibration amplitudes
 * - Atomic scattering phase shifts (energy-dependent)
 * - Inner potential (real and imaginary parts)
 *
 * @see Van Hove & Tong, "Surface Crystallography by LEED" (1979)
 * @see Pendry, "Low Energy Electron Diffraction" (1974)
 */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef LEED_H
#define LEED_H

/**
 * @addtogroup leed_core
 * @{
 */

/*********************************************************************
Basic includes
*********************************************************************/

#define REAL_IS_DOUBLE
#include "real.h"
#include "gh_stddef.h"
#include "mat.h"

/*********************************************************************
include type definitions and constant values from "leed_def.h"
*********************************************************************/
#include "cpl.h"
#include "qm.h"

#include "leed_def.h"
#include "leed_func.h"

/** @} */ /* end of leed_core group */

/*********************************************************************
END
*********************************************************************/
#endif /* LEED_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif