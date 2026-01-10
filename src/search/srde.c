/***********************************************************************
 * @file srde.c
 * @brief Differential Evolution (DE) optimizer implementation.
 *
 * This file implements the DE/rand/1/bin variant of the Differential
 * Evolution algorithm for global optimization. DE is a population-based
 * stochastic optimizer that evolves candidate solutions using vector
 * differences.
 *
 * @par Algorithm Reference:
 * Storn, R., & Price, K. (1997). Differential Evolution – A Simple and
 * Efficient Heuristic for Global Optimization over Continuous Spaces.
 * Journal of Global Optimization, 11, 341–359.
 * https://doi.org/10.1023/A:1008202821328
 *
 * @author CLEED Development Team
 * @date 2025
 ***********************************************************************/

// cppcheck-suppress missingIncludeSystem
#include <stdio.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>

#include "search.h"
#include "sr_alloc.h"
#include "sr_rng.h"

typedef struct sr_de_state {
  sr_rng rng;
  int pop;
  int ndim;
  int max_iters;
  int max_evals;
  real weight;
  real crossover;
  real span;
  real conv_tol;
  sr_de_conv_mode conv_mode;
  real **pop_vec;
  real *scores;
  real *trial;
} sr_de_state;

typedef struct sr_de_selection_ctx {
  real **pop_vec;
  real *scores;
  real *best;
  real *best_val;
  int ndim;
} sr_de_selection_ctx;

/*===========================================================================*/
/* Internal helper functions                                                 */
/*===========================================================================*/

/**
 * @brief Compute default population size based on problem dimensionality.
 *
 * Uses the heuristic of 10 individuals per dimension, with a minimum of 20.
 *
 * @param ndim Number of dimensions in the optimization problem.
 * @return Recommended population size.
 */
static int sr_de_default_population(int ndim)
{
  const int base = 10 * ndim;
  return (base < 20) ? 20 : base;
}

/**
 * @brief Generate a random value uniformly distributed in [-span, +span].
 *
 * @param rng  Pointer to the random number generator state.
 * @param span Half-width of the uniform distribution.
 * @return Random value in the range [-span, +span].
 */
static real sr_de_rand_span(sr_rng *rng, real span)
{
  const real u = (real)sr_rng_uniform01(rng);
  return (u * (real)2.0 - (real)1.0) * span;
}

static real sr_de_abs(real value)
{
  return (value < (real)0.0) ? -value : value;
}

static int sr_de_skip_excludes(int skip, int pop)
{
  if (skip < 1) return 0;
  if (skip > pop) return 0;
  return 1;
}

static int sr_de_pick_available(int pop, int skip)
{
  int available = pop;
  if (sr_de_skip_excludes(skip, pop)) {
    available -= 1;
  }
  return available;
}

static int sr_de_validate_pick_inputs(const sr_rng *rng, int pop, int skip,
                                      const int *a, const int *b, const int *c)
{
  if (!rng) return -1;
  if (!a) return -1;
  if (!b) return -1;
  if (!c) return -1;
  if (pop < 4) return -1;
  if (sr_de_pick_available(pop, skip) < 3) return -1;

  return 0;
}

/**
 * @brief Select a random index for DE mutation, excluding specified indices.
 *
 * Selects an index from [1, pop] that does not equal skip, exclude1, or exclude2.
 *
 * @param rng      Pointer to the random number generator state.
 * @param pop      Population size.
 * @param skip     Index to exclude (typically the current target vector).
 * @param exclude1 First additional index to exclude.
 * @param exclude2 Second additional index to exclude.
 * @return Randomly selected index not matching any excluded values.
 */
static int sr_de_pick_index(sr_rng *rng, int pop, int skip, int exclude1,
                            int exclude2)
{
  int idx = 0;

  do {
    idx = 1 + (int)(sr_rng_uniform01(rng) * pop);
  } while (idx == skip || idx == exclude1 || idx == exclude2);

  return idx;
}

/**
 * @brief Select three distinct random indices for DE mutation.
 *
 * Selects indices a, b, c from [1, pop] such that none equal skip or
 * each other. This is used to form the DE/rand/1 mutant vector.
 *
 * @param rng  Pointer to the random number generator state.
 * @param pop  Population size.
 * @param skip Index to exclude (typically the current target vector).
 * @param a    Output: first randomly selected index.
 * @param b    Output: second randomly selected index.
 * @param c    Output: third randomly selected index.
 * @return 0 on success, -1 on invalid parameters.
 */
static int sr_de_pick_indices(sr_rng *rng, int pop, int skip,
                              int *a, int *b, int *c)
{
  if (sr_de_validate_pick_inputs(rng, pop, skip, a, b, c) != 0) return -1;

  *a = sr_de_pick_index(rng, pop, skip, 0, 0);
  *b = sr_de_pick_index(rng, pop, skip, *a, 0);
  *c = sr_de_pick_index(rng, pop, skip, *a, *b);

  return 0;
}

/**
 * @brief Update the global best solution if a new best is found.
 *
 * @param trial     Trial vector that may be the new best.
 * @param trial_val Objective value of the trial vector.
 * @param best      Current best solution vector (updated if improved).
 * @param best_val  Current best objective value (updated if improved).
 * @param ndim      Number of dimensions.
 */
static void sr_de_update_best(const real *trial, real trial_val,
                              real *best, real *best_val, int ndim)
{
  if (trial_val < *best_val) {
    *best_val = trial_val;
    for (int j = 1; j <= ndim; j++) {
      best[j] = trial[j];
    }
  }
}

/**
 * @brief Copy a vector from source to destination.
 *
 * @param dest Destination vector (1-indexed).
 * @param src  Source vector (1-indexed).
 * @param ndim Number of dimensions.
 */
static void sr_de_copy_vector(real *dest, const real *src, int ndim)
{
  for (int j = 1; j <= ndim; j++) {
    dest[j] = src[j];
  }
}

/**
 * @brief Fill a vector with random values within the search span.
 *
 * Initializes a vector with random values in [-span, +span] for each
 * dimension using the random state from the DE state.
 *
 * @param state Pointer to the DE state containing RNG and parameters.
 * @param dest  Output vector to fill with random values (1-indexed).
 */
static void sr_de_fill_vector(sr_de_state *state, real *dest)
{
  const int ndim = state->ndim;
  const real span = state->span;

  for (int j = 1; j <= ndim; j++) {
    dest[j] = sr_de_rand_span(&state->rng, span);
  }
}

static real sr_de_eval_vector(real (*func)(const real *), const real *vec,
                              int *evals)
{
  const real value = (*func)(vec);
  if (evals) (*evals)++;
  return value;
}

static int sr_de_init_population(sr_de_state *state, real *best, real *best_val,
                                 real (*func)(const real *), int *evals)
{
  if (!state || !best || !best_val || !func) return -1;

  const int pop = state->pop;
  const int ndim = state->ndim;

  sr_de_fill_vector(state, state->pop_vec[1]);
  state->scores[1] = sr_de_eval_vector(func, state->pop_vec[1], evals);
  *best_val = state->scores[1];
  sr_de_copy_vector(best, state->pop_vec[1], ndim);

  for (int i = 2; i <= pop; i++) {
    sr_de_fill_vector(state, state->pop_vec[i]);
    state->scores[i] = sr_de_eval_vector(func, state->pop_vec[i], evals);

    if (state->scores[i] < *best_val) {
      *best_val = state->scores[i];
      sr_de_copy_vector(best, state->pop_vec[i], ndim);
    }
  }

  return 0;
}

/**
 * @brief Create a trial vector using DE/rand/1/bin crossover.
 *
 * Generates a trial vector by combining the mutant vector (formed from
 * three random individuals) with the target vector using binomial crossover.
 *
 * @param state  Pointer to the DE state containing population and parameters.
 * @param target Index of the target vector.
 * @param a      Index of first mutant base vector.
 * @param b      Index of second mutant base vector.
 * @param c      Index of third mutant base vector.
 */
static void sr_de_create_trial(sr_de_state *state, int target,
                               int a, int b, int c)
{
  if (!state) return;

  sr_rng *rng = &state->rng;
  real **pop_vec = state->pop_vec;
  const int ndim = state->ndim;
  const real weight = state->weight;
  const real cr = state->crossover;
  real *trial = state->trial;

  int j_rand = 1 + (int)(sr_rng_uniform01(rng) * ndim);
  if (j_rand < 1) j_rand = 1;
  if (j_rand > ndim) j_rand = ndim;

  for (int j = 1; j <= ndim; j++) {
    const real r = (real)sr_rng_uniform01(rng);
    if (r < cr || j == j_rand) {
      /* Mutant: v = x_a + F * (x_b - x_c) */
      trial[j] = pop_vec[a][j] + weight * (pop_vec[b][j] - pop_vec[c][j]);
    } else {
      /* Inherit from target */
      trial[j] = pop_vec[target][j];
    }
  }
}

/**
 * @brief Perform selection: replace target if trial is at least as good.
 *
 * @param ctx       Selection context (population, scores, best trackers).
 * @param target    Index of the target vector.
 * @param trial     Trial vector.
 * @param trial_val Objective value of the trial vector.
 */
static void sr_de_selection(sr_de_selection_ctx *ctx, int target,
                            const real *trial, real trial_val)
{
  if (!ctx || !trial) return;

  if (trial_val <= ctx->scores[target]) {
    ctx->scores[target] = trial_val;
    sr_de_copy_vector(ctx->pop_vec[target], trial, ctx->ndim);
    sr_de_update_best(trial, trial_val, ctx->best, ctx->best_val, ctx->ndim);
  }
}

/**
 * @brief Free DE working memory.
 *
 * @param pop_vec Population matrix.
 * @param scores  Scores vector.
 * @param trial   Trial vector.
 */
static void sr_de_free_memory(real **pop_vec, real *scores, real *trial)
{
  sr_free_matrix(pop_vec);
  sr_free_vector(scores);
  sr_free_vector(trial);
}

/**
 * @brief Allocate DE working memory.
 *
 * @param pop     Population size.
 * @param ndim    Number of dimensions.
 * @param pop_vec Output: population matrix.
 * @param scores  Output: scores vector.
 * @param trial   Output: trial vector.
 * @return 0 on success, -1 on allocation failure.
 */
static int sr_de_alloc_memory(int pop, int ndim,
                              real ***pop_vec, real **scores, real **trial)
{
  *pop_vec = sr_alloc_matrix((size_t)pop, (size_t)ndim);
  *scores = sr_alloc_vector((size_t)pop);
  *trial = sr_alloc_vector((size_t)ndim);

  if (!*pop_vec || !*scores || !*trial) {
    sr_de_free_memory(*pop_vec, *scores, *trial);
    return -1;
  }
  return 0;
}

/**
 * @brief Validate and apply default configuration values.
 *
 * @param cfg      Input configuration (may be NULL for defaults).
 * @param defaults Output: configuration with defaults applied.
 * @param ndim     Number of dimensions.
 * @return Pointer to the effective configuration.
 */
static const sr_de_cfg *sr_de_get_effective_cfg(const sr_de_cfg *cfg,
                                                 sr_de_cfg *defaults, int ndim)
{
  if (!cfg) {
    sr_de_cfg_init(defaults, ndim, (real)1.0);
    return defaults;
  }
  return cfg;
}

static int sr_de_resolve_population(const sr_de_cfg *src, int ndim, int *out)
{
  if (!out || ndim <= 0) return -1;

  *out = (src->population > 0) ? src->population
                               : sr_de_default_population(ndim);
  if (*out < 4) return -1;

  return 0;
}

static int sr_de_resolve_limit(int value, int default_value)
{
  return (value > 0) ? value : default_value;
}

static real sr_de_resolve_weight(real weight)
{
  if (weight <= (real)0.0) return (real)0.8;
  if (weight > (real)2.0) return (real)2.0;
  return weight;
}

static real sr_de_resolve_crossover(real crossover)
{
  if (crossover < (real)0.0) return (real)0.0;
  if (crossover > (real)1.0) return (real)1.0;
  return crossover;
}

static real sr_de_resolve_span(real span)
{
  return (span > (real)0.0) ? span : (real)1.0;
}

static real sr_de_resolve_conv_tol(real conv_tol)
{
  return (conv_tol > (real)0.0) ? conv_tol : (real)R_TOLERANCE;
}

static sr_de_conv_mode sr_de_resolve_conv_mode(sr_de_conv_mode mode)
{
  if (mode == SR_DE_CONV_NONE || mode == SR_DE_CONV_ABS) {
    return mode;
  }
  return SR_DE_CONV_NONNEG;
}

static int sr_de_resolve_cfg(const sr_de_cfg *cfg, int ndim,
                             sr_de_cfg *resolved)
{
  if (!resolved || ndim <= 0) return -1;

  sr_de_cfg defaults;
  const sr_de_cfg *src = sr_de_get_effective_cfg(cfg, &defaults, ndim);

  if (sr_de_resolve_population(src, ndim, &resolved->population) != 0) {
    return -1;
  }

  resolved->max_iters = sr_de_resolve_limit(src->max_iters, MAX_ITER_DE);
  resolved->max_evals = sr_de_resolve_limit(src->max_evals, MAX_EVAL_DE);
  resolved->weight = sr_de_resolve_weight(src->weight);
  resolved->crossover = sr_de_resolve_crossover(src->crossover);
  resolved->init_span = sr_de_resolve_span(src->init_span);
  resolved->conv_tol = sr_de_resolve_conv_tol(src->conv_tol);
  resolved->conv_mode = sr_de_resolve_conv_mode(src->conv_mode);
  resolved->seed = src->seed;

  return 0;
}

static int sr_de_evolve_member(sr_de_state *state, int target,
                               sr_de_selection_ctx *select_ctx,
                               real (*func)(const real *), int *evals)
{
  int a = 0;
  int b = 0;
  int c = 0;

  if (sr_de_pick_indices(&state->rng, state->pop, target, &a, &b, &c) != 0) {
    return -1;
  }

  sr_de_create_trial(state, target, a, b, c);

  const real trial_val = sr_de_eval_vector(func, state->trial, evals);
  sr_de_selection(select_ctx, target, state->trial, trial_val);

  return 0;
}

static int sr_de_run_generation_inputs_ok(const sr_de_state *state,
                                          real (*func)(const real *),
                                          const real *best,
                                          const real *best_val,
                                          const int *evals)
{
  if (!state || !func || !best || !best_val || !evals) return 0;
  return 1;
}

static int sr_de_run_generation(sr_de_state *state, real (*func)(const real *),
                                real *best, real *best_val, int *evals)
{
  if (!sr_de_run_generation_inputs_ok(state, func, best, best_val, evals)) {
    return -1;
  }

  sr_de_selection_ctx select_ctx;
  select_ctx.pop_vec = state->pop_vec;
  select_ctx.scores = state->scores;
  select_ctx.best = best;
  select_ctx.best_val = best_val;
  select_ctx.ndim = state->ndim;

  for (int i = 1; i <= state->pop; i++) {
    if (*evals >= state->max_evals) {
      break;
    }
    if (sr_de_evolve_member(state, i, &select_ctx, func, evals) != 0) {
      return -1;
    }
  }

  return 0;
}

static int sr_de_should_stop(const sr_de_state *state, real best_val)
{
  if (!state || state->conv_mode == SR_DE_CONV_NONE) return 0;
  if (state->conv_tol <= (real)0.0) return 0;

  if (state->conv_mode == SR_DE_CONV_ABS) {
    return sr_de_abs(best_val) <= state->conv_tol;
  }

  return best_val >= (real)0.0 && best_val <= state->conv_tol;
}

static int sr_de_run_inputs_ok(const sr_de_state *state,
                               real (*func)(const real *),
                               const real *best, const real *best_val)
{
  if (!state || !func || !best || !best_val) return 0;
  return 1;
}

static int sr_de_run_step(sr_de_state *state, real (*func)(const real *),
                          real *best, real *best_val, int *evals, int *stop)
{
  if (!stop) return -1;

  if (*evals >= state->max_evals) {
    *stop = 1;
    return 0;
  }

  if (sr_de_run_generation(state, func, best, best_val, evals) != 0) {
    return -1;
  }

  *stop = sr_de_should_stop(state, *best_val);
  return 0;
}

static int sr_de_run(sr_de_state *state, real (*func)(const real *), real *best,
                     real *best_val, int *evals)
{
  if (!sr_de_run_inputs_ok(state, func, best, best_val)) return -1;

  int local_evals = 0;
  if (sr_de_init_population(state, best, best_val, func, &local_evals) != 0) {
    return -1;
  }

  for (int iter = 0; iter < state->max_iters; iter++) {
    int stop = 0;
    if (sr_de_run_step(state, func, best, best_val, &local_evals, &stop) != 0) {
      return -1;
    }
    if (stop) break;
  }

  if (evals) {
    *evals = local_evals;
  }

  return 0;
}

/*===========================================================================*/
/* Public API                                                                */
/*===========================================================================*/

/**
 * @brief Initialize a DE configuration structure with sensible defaults.
 *
 * Sets up default hyperparameters suitable for LEED optimization:
 * - Population: 10 × ndim (minimum 20)
 * - Differential weight (F): 0.8 (clamped to (0, 2])
 * - Crossover probability (CR): 0.9 (clamped to [0, 1])
 * - Initial span: dpos or 1.0
 * - Convergence tolerance: R_TOLERANCE (mode: non-negative only)
 *
 * @param cfg   Pointer to configuration structure to initialize.
 * @param ndim  Number of dimensions in the optimization problem.
 * @param dpos  Initial displacement (used for initial span).
 */
void sr_de_cfg_init(sr_de_cfg *cfg, int ndim, real dpos)
{
  if (!cfg) return;
  cfg->population = sr_de_default_population(ndim);
  cfg->max_iters = 0;
  cfg->max_evals = 0;
  cfg->weight = (real)0.8;
  cfg->crossover = (real)0.9;
  cfg->init_span = (dpos > 0.0) ? dpos : (real)1.0;
  cfg->conv_tol = (real)R_TOLERANCE;
  cfg->conv_mode = SR_DE_CONV_NONNEG;
  cfg->seed = 0;
}

int sr_de_optimize(const sr_de_cfg *cfg, int ndim, real (*func)(const real *),
                   real *best, real *best_val, int *evals)
{
  if (!func || !best || !best_val || ndim <= 0) {
    return -1;
  }

  sr_de_cfg effective_cfg;
  if (sr_de_resolve_cfg(cfg, ndim, &effective_cfg) != 0) {
    return -1;
  }

  /* Initialize RNG */
  sr_de_state state;
  state.pop = effective_cfg.population;
  state.ndim = ndim;
  state.max_iters = effective_cfg.max_iters;
  state.max_evals = effective_cfg.max_evals;
  state.weight = effective_cfg.weight;
  state.crossover = effective_cfg.crossover;
  state.span = effective_cfg.init_span;
  state.conv_tol = effective_cfg.conv_tol;
  state.conv_mode = effective_cfg.conv_mode;
  state.pop_vec = NULL;
  state.scores = NULL;
  state.trial = NULL;
  sr_rng_seed(&state.rng, (uint64_t)effective_cfg.seed);

  /* Allocate working memory */
  if (sr_de_alloc_memory(state.pop, state.ndim, &state.pop_vec,
                         &state.scores, &state.trial) != 0) {
    return -1;
  }

  if (sr_de_run(&state, func, best, best_val, evals) != 0) {
    sr_de_free_memory(state.pop_vec, state.scores, state.trial);
    return -1;
  }

  sr_de_free_memory(state.pop_vec, state.scores, state.trial);
  return 0;
}

/**
 * @brief Log DE optimization results to a file stream.
 *
 * @param log_stream Output file stream.
 * @param ndim       Number of dimensions.
 * @param evals      Total function evaluations performed.
 * @param best       Best solution vector found.
 * @param best_val   Best objective value found.
 */
static void sr_de_log_results(FILE *log_stream, int ndim, int evals,
                              const real *best, real best_val)
{
  fprintf(log_stream, "\n=> No. of function evaluations in sr_de: %3d\n", evals);
  fprintf(log_stream, "=> Best parameter set:\n");
  for (int j = 1; j <= ndim; j++) {
    fprintf(log_stream, "%.6f ", (double)best[j]);
  }
  fprintf(log_stream, "\n");
  fprintf(log_stream, "=> Best function value:\n");
  fprintf(log_stream, "rmin = %.6f\n", (double)best_val);
}

static const char *sr_de_conv_mode_label(sr_de_conv_mode mode)
{
  switch (mode) {
    case SR_DE_CONV_NONE:
      return "none";
    case SR_DE_CONV_ABS:
      return "abs";
    case SR_DE_CONV_NONNEG:
    default:
      return "nonneg";
  }
}

/**
 * @brief Log DE configuration parameters to a file stream.
 *
 * @param log_stream Output file stream.
 * @param cfg        DE configuration.
 */
static void sr_de_log_config(FILE *log_stream, const sr_de_cfg *cfg)
{
  fprintf(log_stream, "=> DIFFERENTIAL EVOLUTION:\n");
  fprintf(log_stream,
          "=> pop=%d weight=%.3f cr=%.3f span=%.3f conv_tol=%.6f conv_mode=%s\n",
          cfg->population, (double)cfg->weight, (double)cfg->crossover,
          (double)cfg->init_span, (double)cfg->conv_tol,
          sr_de_conv_mode_label(cfg->conv_mode));
}

/**
 * @brief Build DE configuration from global variables.
 *
 * Constructs a sr_de_cfg structure by reading global configuration
 * variables that may have been set via CLI or environment.
 *
 * @param cfg   Output: configuration structure.
 * @param ndim  Number of dimensions.
 * @param dpos  Initial displacement.
 */
static void sr_de_build_config(sr_de_cfg *cfg, int ndim, real dpos)
{
  sr_de_cfg_init(cfg, ndim, dpos);

  if (sr_de_population > 0) {
    cfg->population = sr_de_population;
  }
  if (sr_de_weight > 0.0) {
    cfg->weight = sr_de_weight;
  }
  if (sr_de_crossover > 0.0) {
    cfg->crossover = sr_de_crossover;
  }
  if (sr_de_init_span > 0.0) {
    cfg->init_span = sr_de_init_span;
  }
  cfg->max_iters = sr_de_iter_limit;
  cfg->max_evals = sr_de_eval_limit;

  if (sa_idum != 0) {
    cfg->seed = (uint64_t)(sa_idum < 0 ? -sa_idum : sa_idum);
  }
}

/**
 * @brief DE optimizer entry point for the CLEED search subsystem.
 *
 * This is the main driver function called by the optimizer registry.
 * It configures DE from global settings, runs the optimization, and
 * logs results to the specified log file.
 *
 * @param ndim     Number of search dimensions.
 * @param dpos     Initial displacement for parameter variations.
 * @param bak_file Backup file path (unused, for API compatibility).
 * @param log_file Log file path for recording optimization progress.
 */
void sr_de(int ndim, real dpos, const char *bak_file, const char *log_file)
{
  (void)bak_file;

  /* Build configuration from globals */
  sr_de_cfg cfg;
  sr_de_build_config(&cfg, ndim, dpos);
  sr_de_cfg effective_cfg;
  if (sr_de_resolve_cfg(&cfg, ndim, &effective_cfg) != 0) {
    fprintf(STDERR, "*** error (sr_de): invalid configuration\n");
    exit(1);
  }

  /* Allocate result vector */
  real *best = sr_alloc_vector((size_t)ndim);
  if (!best) {
    fprintf(STDERR, "*** error (sr_de): allocation failure\n");
    exit(1);
  }

  /* Log configuration */
  FILE *log_stream = fopen(log_file, "a");
  if (log_stream == NULL) {
    sr_free_vector(best);
    OPEN_ERROR(log_file);
    exit(1);  /* Ensure exit if OPEN_ERROR doesn't */
  }
  sr_de_log_config(log_stream, &effective_cfg);
  fclose(log_stream);

  /* Run optimization */
  real best_val = 0.0;
  int evals = 0;
  if (sr_de_optimize(&effective_cfg, ndim, sr_evalrf, best, &best_val, &evals) != 0) {
    sr_free_vector(best);
    fprintf(STDERR, "*** error (sr_de): optimisation failed\n");
    exit(1);
  }

  /* Log results */
  log_stream = fopen(log_file, "a");
  if (log_stream == NULL) {
    sr_free_vector(best);
    OPEN_ERROR(log_file);
    exit(1);  /* Ensure exit if OPEN_ERROR doesn't */
  }
  sr_de_log_results(log_stream, ndim, evals, best, best_val);
  fclose(log_stream);

  sr_free_vector(best);
}
