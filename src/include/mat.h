/**
 * @file mat.h
 * @brief Master include file for matrix operations.
 *
 * @defgroup matrix_ops Matrix Operations
 * @brief Matrix and vector operations for LEED calculations.
 *
 * The MAT module provides:
 * - Complex and real matrix storage
 * - Matrix arithmetic (multiplication, inversion, etc.)
 * - Special matrix types (diagonal, square)
 *
 * ## Matrix Conventions
 *
 * - 1-based indexing (element (1,1) is the first element)
 * - Row-major storage
 * - Complex matrices store real and imaginary parts separately
 *
 * ## Access Macros
 *
 * Use RMATEL(m,n,Mat) and IMATEL(m,n,Mat) to access elements:
 * @code
 * real val = RMATEL(1, 1, myMatrix);  // Get real part of (1,1)
 * IMATEL(2, 3, myMatrix) = 0.5;       // Set imaginary part of (2,3)
 * @endcode
 */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef MAT_H
#define MAT_H

/**
 * @addtogroup matrix_ops
 * @{
 */

/*********************************************************************
 Include type definitions and constant values and functions for MAT
*********************************************************************/


typedef long long long_int;
#define  LONG_INT_IS_LONG_LONG

#define  REAL_IS_DOUBLE
#include "real.h"
#include "mat_def.h"
#include "mat_func.h"

/*********************************************************************
Other (basic) includes
*********************************************************************/

#include "gh_stddef.h"
#include "cpl.h"

/** @} */ /* end of matrix_ops group */

#endif /* MAT_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif