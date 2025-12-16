/*********************************************************************
GH/15.08.95

include file for SEARCH program.
 - type definitions
 - constant values
 - function prototypes
*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef SEARCH_H
#define SEARCH_H

/*********************************************************************
 Other (basic) includes
*********************************************************************/

#include "gh_stddef.h"

#define REAL_IS_DOUBLE
#include "real.h"
#include "leed_def.h"


/*********************************************************************
 Include type definitions and constant values and functions for SEARCH
*********************************************************************/

#if !defined(USE_GSL) && !defined(_USE_GSL)
  /* numerical receipes */
  #include "nrr.h"              
#else
  /* GNU Scientific Library (open source) */
  #include <gsl/gsl_vector.h>
  #include <gsl/gsl_matrix.h>
  #include <gsl/gsl_multimin.h>
#endif

#include "search_def.h"
#include "search_func.h"
#include "search_ver.h"

extern struct sratom_str *sr_atoms;
extern struct search_str *sr_search;
extern char *sr_project;

/*********************************************************************
 End of include file 
*********************************************************************/

#endif /* SEARCH_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
