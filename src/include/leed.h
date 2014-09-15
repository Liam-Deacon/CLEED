/*********************************************************************
GH/07.10.95

include file for basic quantum mechanical functions
 - type definitions
 - constant values
*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef LEED_H
#define LEED_H

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

/*********************************************************************
END
*********************************************************************/
#endif /* LEED_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif