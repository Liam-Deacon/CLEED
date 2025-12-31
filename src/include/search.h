/*********************************************************************
 *                       SEARCH.H
 *
 *  Master include file for SEARCH (csearch).
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

/* Optional GNU Scientific Library integration (not required for core SEARCH). */
#if defined(USE_GSL) || defined(_USE_GSL)
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_vector.h>
#endif

#include "search_def.h"
#include "search_func.h"
#include "search_ver.h"

extern struct sratom_str *sr_atoms;
extern struct search_str *sr_search;
extern char *sr_project;
extern int sr_amoeba_eval_limit;
extern int sr_powell_iter_limit;
extern int sr_sa_iter_limit;
extern int sr_pso_eval_limit;
extern int sr_pso_iter_limit;
extern int sr_pso_swarm_size;
extern real sr_pso_inertia;
extern real sr_pso_c1;
extern real sr_pso_c2;
extern real sr_pso_vmax;
extern int sr_de_eval_limit;
extern int sr_de_iter_limit;
extern int sr_de_population;
extern real sr_de_weight;
extern real sr_de_crossover;
extern real sr_de_init_span;
extern long sa_idum;

/*********************************************************************
 End of include file 
*********************************************************************/

#endif /* SEARCH_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
