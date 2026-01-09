/*********************************************************************
 *                       SEARCH_FUNC.H
 *
 *  Public function prototypes for SEARCH (csearch).
 *
 *  Historical versions used non-redistributable third-party routines for
 *  optimisation and for vector/matrix helpers. Those implementations
 *  have been removed; the remaining interfaces are provided by
 *  GPL-compatible, in-tree implementations.
 *********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef SEARCH_FUNC_H
#define SEARCH_FUNC_H

#include <stdio.h>
// cppcheck-suppress missingIncludeSystem
#include <stdint.h>

/**
 * @file search_func.h
 * @brief Public function prototypes for SEARCH (csearch).
 *
 * The refactored implementations preserve the historical SEARCH calling
 * conventions, most notably the use of 1-based vectors/matrices:
 * - simplex vertices: `p[1..ndim+1][1..ndim]`
 * - function values:  `y[1..ndim+1]`
 * - vectors:          `v[1..n]`
 *
 * These interfaces are used by the SEARCH driver routines (`sr_sa`,
 * `sr_sx`, `sr_po`, `sr_er`) and utilities.
 */

/** @name Optimisers (derivative-free) */
///@{

/**
 * @brief Nelder–Mead downhill simplex minimiser.
 *
 * @param p Simplex vertices (`[1..ndim+1][1..ndim]`), updated in-place.
 * @param y Function values at `p` (`1..ndim+1`), updated in-place.
 * @param ndim Dimensionality of the parameter vector.
 * @param ftol Termination tolerance (absolute difference between best/worst).
 * @param funk Objective function.
 * @param nfunk In/out evaluation counter (legacy API).
 * @return 0 on success, non-zero on failure.
 */
int sr_amoeba(real **p, real *y, int ndim, real ftol, real (*funk)(real *), int *nfunk);

typedef real (*sr_amebsa_func)(real *);

/**
 * @brief Configuration for @ref sr_amebsa.
 *
 * Grouping these inputs avoids "too many parameters" warnings and keeps
 * the public API stable while allowing internal evolution.
 */
typedef struct sr_amebsa_cfg {
  real ftol;
  sr_amebsa_func funk;
  real temptr;
} sr_amebsa_cfg;

/**
 * @brief Accessor for the objective function pointer in @ref sr_amebsa_cfg.
 *
 * This is used to keep the `sr_amebsa_cfg` layout explicit while avoiding
 * "unused member" warnings in some static analysis configurations.
 *
 * @param cfg Configuration pointer.
 * @return Objective function pointer (may be NULL if cfg is NULL).
 */
static inline sr_amebsa_func sr_amebsa_cfg_get_funk(const sr_amebsa_cfg *cfg)
{
  return cfg ? cfg->funk : NULL;
}

/**
 * @brief Simulated annealing wrapper around the simplex method.
 *
 * @param p Simplex vertices (`[1..ndim+1][1..ndim]`), updated in-place.
 * @param y Function values at `p` (`1..ndim+1`), updated in-place.
 * @param ndim Dimensionality of the parameter vector.
 * @param pb Output best point (`1..ndim`).
 * @param yb Output best function value.
 * @param cfg Annealing + tolerance configuration.
 * @param iter In/out evaluation budget (input) and used evaluations (output).
 * @return 0 on success, non-zero on failure.
 */
int sr_amebsa(real **p, real *y, int ndim, real *pb, real *yb,
              const sr_amebsa_cfg *cfg, int *iter);

/**
 * @brief Powell direction-set minimiser.
 *
 * @param p In/out point vector (`1..n`).
 * @param xi In/out direction matrix (`[1..n][1..n]`), columns are directions.
 * @param n Dimensionality of the parameter vector.
 * @param ftol Termination tolerance.
 * @param iter In/out iteration budget (legacy API).
 * @param fret Output best function value.
 * @param func Objective function.
 * @return 0 on success, non-zero on failure.
 */
int sr_powell(real *p, real **xi, int n, real ftol, int *iter, real *fret,
              real (*func)(real *));
///@}

/**
 * @brief Default configuration values for PSO (Clerc's constriction).
 */
#define SR_PSO_DEFAULT_INERTIA  ((real)0.729)
#define SR_PSO_DEFAULT_C1       ((real)1.49445)
#define SR_PSO_DEFAULT_C2       ((real)1.49445)

/**
 * @brief Configuration for particle swarm optimisation (PSO).
 */
typedef struct sr_pso_cfg {
  // cppcheck-suppress unusedStructMember
  int swarm_size; /**< Number of particles (default: 10*ndim or 10, range: >0) */
  // cppcheck-suppress unusedStructMember
  int max_iters;  /**< Maximum iterations (default: 200, range: >0) */
  // cppcheck-suppress unusedStructMember
  int max_evals;  /**< Maximum evaluations (default: 10000, range: >0) */
  real inertia;   /**< Inertia weight (default: 0.729, range: 0.0-1.0) */
  real c1;        /**< Cognitive coefficient (default: 1.49445, range: >0.0) */
  real c2;        /**< Social coefficient (default: 1.49445, range: >0.0) */
  real v_max;     /**< Maximum velocity (default: dpos or 1.0, range: >0.0) */
  // cppcheck-suppress unusedStructMember
  uint64_t seed;  /**< RNG seed (default: 0/random, range: any) */
} sr_pso_cfg;

/**
 * @brief Initialise PSO defaults based on dimensionality and dpos.
 *
 * This function initializes a configuration structure with default values
 * suitable for most problems. It modifies the `cfg` structure in-place.
 *
 * @param cfg Pointer to the configuration structure to initialise. If NULL,
 *            the function does nothing.
 * @param ndim Dimensionality of the parameter vector (must be > 0).
 * @param dpos Initial search range (step size) for parameters. Defaults to
 *             1.0 if <= 0.0. Used to set `v_max`.
 *
 * Default values set:
 * - swarm_size: max(10, 10 * ndim)
 * - max_iters: 0 (defer to optimiser default)
 * - max_evals: 0 (defer to optimiser default)
 * - inertia: 0.729
 * - c1 (cognitive): 1.49445
 * - c2 (social): 1.49445
 * - v_max: dpos (or 1.0)
 * - seed: 0
 *
 * This function has no return value.
 */
void sr_pso_cfg_init(sr_pso_cfg *cfg, int ndim, real dpos);

/**
 * @brief Particle swarm optimisation (PSO) minimiser.
 *
 * @param cfg Configuration (may be NULL for defaults).
 * @param ndim Dimensionality of the parameter vector.
 * @param func Objective function.
 * @param best Output best point (`1..ndim`).
 * @param best_val Output best function value.
 * @param evals In/out evaluation counter (may be NULL).
 * @return 0 on success, non-zero on failure.
 */
int sr_pso_optimize(const sr_pso_cfg *cfg, int ndim, real (*func)(const real *),
                    real *best, real *best_val, int *evals);

/* Drivers */
void sr_sa(int ndim, real dpos, const char *bak_file, const char *log_file);
void sr_sx(int ndim, real dpos, const char *bak_file, const char *log_file);
void sr_po(int ndim, const char *bak_file, const char *log_file);
void sr_pso(int ndim, real dpos, const char *bak_file, const char *log_file);
void sr_er(int ndim, real dpos, const char *bak_file, const char *log_file);

/* file input|output */
real sr_ckgeo(real *);
int  sr_ckrot(struct sratom_str *, struct search_str *);
real sr_evalrf(real *);
int  sr_mkinp(real *, int, char *);
int  sr_rdinp(const char *);
int  sr_rdver(const char *, real *, real **, int);

/* debye temperature */
real leed_inp_debye_temp(real , real , real );

/* help */
void search_usage(FILE * );
void search_info();

#endif /* SEARCH_FUNC_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
