/*********************************************************************
GH/30.08.97

include file for R factor program.
 - type definitions
 - constant values
 - function prototypes
*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef RFAC_H
#define RFAC_H

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

/*********************************************************************
 End of include file 
*********************************************************************/

#endif /* RFAC_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif