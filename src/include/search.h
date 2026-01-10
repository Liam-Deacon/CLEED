/**
 * @file search.h
 * @brief Master include file for SEARCH (csearch).
 *
 * @defgroup search_core SEARCH Core
 * @brief Core SEARCH optimization framework for LEED-IV analysis.
 *
 * The SEARCH module provides derivative-free optimization algorithms
 * for finding optimal surface structures by minimizing R-factors between
 * theoretical and experimental LEED I-V curves.
 *
 * ## Available Algorithms
 *
 * | Algorithm | Code | Description |
 * |-----------|------|-------------|
 * | Downhill Simplex | `sx` | Nelder-Mead method, good for local refinement |
 * | Powell's Method | `po` | Direction-set method, efficient for smooth landscapes |
 * | Simulated Annealing | `sa` | Stochastic global search, escapes local minima |
 * | Genetic Algorithm | `ga` | Population-based evolutionary search |
 *
 * ## Key Components
 *
 * - @ref search_algos "Optimization Algorithms" - Core minimizers
 * - @ref search_config "Configuration" - Parameter structures
 * - @ref search_eval "Evaluation" - R-factor computation interface
 * - @ref search_rng "Random Number Generation" - Deterministic RNG
 *
 * @see Pendry, J. Phys. C 13, 937 (1980) for Pendry R-factor
 * @see Storn & Price, J. Global Optim. 11, 341 (1997) for DE algorithm
 */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef SEARCH_H
#define SEARCH_H

/**
 * @addtogroup search_core
 * @{
 */

/*********************************************************************
 Other (basic) includes
*********************************************************************/

#include "gh_stddef.h"
// cppcheck-suppress missingIncludeSystem
#include <stdint.h>

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

/** @brief Global atom array used by SEARCH routines. */
extern struct sratom_str *sr_atoms;

/** @brief Global search configuration structure. */
extern struct search_str *sr_search;

/** @brief Project name/identifier for file naming. */
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
extern uint64_t sa_idum;

/** @} */ /* end of search_core group */

/*********************************************************************
 End of include file 
*********************************************************************/

#endif /* SEARCH_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
