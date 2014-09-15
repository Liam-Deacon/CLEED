/*********************************************************************
GH/17.08.95

include file for matrix operations:

*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef MAT_H
#define MAT_H

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

#endif /* MAT_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif