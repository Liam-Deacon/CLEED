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
 * @brief Configuration for particle swarm optimisation (PSO).
 */
typedef struct sr_pso_cfg {
  int swarm_size;
  int max_iters;
  int max_evals;
  real inertia;
  real c1;
  real c2;
  real v_max;
  unsigned long long seed;
} sr_pso_cfg;

/**
 * @brief Initialise PSO defaults based on dimensionality and dpos.
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
int sr_pso_optimize(const sr_pso_cfg *cfg, int ndim, real (*func)(real *),
                    real *best, real *best_val, int *evals);

/**
 * @brief Configuration for differential evolution (DE).
 */
typedef struct sr_de_cfg {
  int population;
  int max_iters;
  int max_evals;
  real weight;
  real crossover;
  real init_span;
  unsigned long long seed;
} sr_de_cfg;

/**
 * @brief Initialise DE defaults based on dimensionality and dpos.
 */
void sr_de_cfg_init(sr_de_cfg *cfg, int ndim, real dpos);

/**
 * @brief Differential evolution (DE) minimiser.
 *
 * @param cfg Configuration (may be NULL for defaults).
 * @param ndim Dimensionality of the parameter vector.
 * @param func Objective function.
 * @param best Output best point (`1..ndim`).
 * @param best_val Output best function value.
 * @param evals In/out evaluation counter (may be NULL).
 * @return 0 on success, non-zero on failure.
 */
int sr_de_optimize(const sr_de_cfg *cfg, int ndim, real (*func)(real *),
                   real *best, real *best_val, int *evals);

/* Drivers */
void sr_sa(int ndim, real dpos, const char *bak_file, const char *log_file);
void sr_sx(int ndim, real dpos, const char *bak_file, const char *log_file);
void sr_po(int ndim, const char *bak_file, const char *log_file);
void sr_pso(int ndim, real dpos, const char *bak_file, const char *log_file);
void sr_de(int ndim, real dpos, const char *bak_file, const char *log_file);
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
