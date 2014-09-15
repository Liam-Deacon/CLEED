/*********************************************************************
GH/30.08.97

include file for R factor program CRFAC.
 - type definitions
 - constant values
 - function prototypes
*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef CRFAC_H
#define CRFAC_H

/*********************************************************************
 Other (basic) includes
*********************************************************************/

#include "gh_stddef.h"

#define REAL_IS_FLOAT            /* should be float for DEC alpha */
#include "real.h"

/*********************************************************************
 Include type definitions and constant values and functions for CRFAC
*********************************************************************/

#include "crfac_def.h"
#include "crfac_func.h"

/*********************************************************************
 End of include file 
*********************************************************************/

#endif /* CRFAC_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif