/***********************************************************************
 *                        SRDE.C
 *
 *  Differential Evolution (DE) driver + core routine.
 ***********************************************************************/

// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdio.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>

#include "search.h"
#include "sr_alloc.h"
#include "sr_rng.h"

static int sr_de_default_population(int ndim)
{
  const int base = 10 * ndim;
  return (base < 20) ? 20 : base;
}

void sr_de_cfg_init(sr_de_cfg *cfg, int ndim, real dpos)
{
  if (!cfg) return;
  cfg->population = sr_de_default_population(ndim);
  cfg->max_iters = 0;
  cfg->max_evals = 0;
  cfg->weight = (real)0.8;
  cfg->crossover = (real)0.9;
  cfg->init_span = (dpos > 0.0) ? dpos : (real)1.0;
  cfg->seed = 0;
}

static real sr_de_rand_span(sr_rng *rng, real span)
{
  const real u = (real)sr_rng_uniform01(rng);
  return (u * (real)2.0 - (real)1.0) * span;
}

static int sr_de_pick_indices(sr_rng *rng, int pop, int skip,
                              int *a, int *b, int *c)
{
  if (!rng || !a || !b || !c || pop < 4) return -1;

  do {
    *a = 1 + (int)(sr_rng_uniform01(rng) * pop);
  } while (*a == skip);

  do {
    *b = 1 + (int)(sr_rng_uniform01(rng) * pop);
  } while (*b == skip || *b == *a);

  do {
    *c = 1 + (int)(sr_rng_uniform01(rng) * pop);
  } while (*c == skip || *c == *a || *c == *b);

  return 0;
}

static int sr_de_init_population(sr_rng *rng, int pop, int ndim, real span,
                                 real **pop_vec, real *scores, real *best,
                                 real *best_val, real (*func)(real *), int *evals)
{
  if (!rng || !pop_vec || !scores || !best || !best_val || !func) return -1;

  *best_val = 0.0;
  for (int i = 1; i <= pop; i++) {
    for (int j = 1; j <= ndim; j++) {
      pop_vec[i][j] = sr_de_rand_span(rng, span);
    }
    scores[i] = (*func)(pop_vec[i]);
    if (evals) (*evals)++;

    if (i == 1 || scores[i] < *best_val) {
      *best_val = scores[i];
      for (int j = 1; j <= ndim; j++) {
        best[j] = pop_vec[i][j];
      }
    }
  }

  return 0;
}

int sr_de_optimize(const sr_de_cfg *cfg, int ndim, real (*func)(real *),
                   real *best, real *best_val, int *evals)
{
  if (!cfg || !func || !best || !best_val || ndim <= 0) {
    return -1;
  }

  int pop = (cfg->population > 0) ? cfg->population : sr_de_default_population(ndim);
  if (pop < 4) {
    return -1;
  }

  const int max_iters = (cfg->max_iters > 0) ? cfg->max_iters : MAX_ITER_DE;
  const int max_evals = (cfg->max_evals > 0) ? cfg->max_evals : MAX_EVAL_DE;
  real weight = (cfg->weight > 0.0) ? cfg->weight : (real)0.8;
  real cr = (cfg->crossover > 0.0) ? cfg->crossover : (real)0.9;
  if (cr > (real)1.0) {
    cr = (real)1.0;
  }
  const real span = (cfg->init_span > 0.0) ? cfg->init_span : (real)1.0;

  sr_rng rng;
  const unsigned long long seed = (cfg->seed > 0) ? cfg->seed : 1ULL;
  sr_rng_seed(&rng, (uint64_t)seed);

  real **pop_vec = sr_alloc_matrix((size_t)pop, (size_t)ndim);
  real *scores = sr_alloc_vector((size_t)pop);
  real *trial = sr_alloc_vector((size_t)ndim);

  if (!pop_vec || !scores || !trial) {
    sr_free_matrix(pop_vec);
    sr_free_vector(scores);
    sr_free_vector(trial);
    return -1;
  }

  int local_evals = 0;
  if (sr_de_init_population(&rng, pop, ndim, span, pop_vec, scores, best,
                            best_val, func, &local_evals) != 0) {
    sr_free_matrix(pop_vec);
    sr_free_vector(scores);
    sr_free_vector(trial);
    return -1;
  }

  int iter = 0;
  while (iter < max_iters && local_evals < max_evals) {
    iter++;
    for (int i = 1; i <= pop; i++) {
      int a = 0;
      int b = 0;
      int c = 0;
      int j_rand = 1 + (int)(sr_rng_uniform01(&rng) * ndim);
      if (j_rand < 1) j_rand = 1;

      if (sr_de_pick_indices(&rng, pop, i, &a, &b, &c) != 0) {
        sr_free_matrix(pop_vec);
        sr_free_vector(scores);
        sr_free_vector(trial);
        return -1;
      }

      for (int j = 1; j <= ndim; j++) {
        const real r = (real)sr_rng_uniform01(&rng);
        if (r < cr || j == j_rand) {
          trial[j] = pop_vec[a][j] + weight * (pop_vec[b][j] - pop_vec[c][j]);
        } else {
          trial[j] = pop_vec[i][j];
        }
      }

      const real f = (*func)(trial);
      local_evals++;

      if (f <= scores[i]) {
        scores[i] = f;
        for (int j = 1; j <= ndim; j++) {
          pop_vec[i][j] = trial[j];
        }
        if (f < *best_val) {
          *best_val = f;
          for (int j = 1; j <= ndim; j++) {
            best[j] = trial[j];
          }
        }
      }

      if (local_evals >= max_evals) {
        break;
      }
    }

    if (*best_val <= R_TOLERANCE) {
      break;
    }
  }

  if (evals) *evals = local_evals;

  sr_free_matrix(pop_vec);
  sr_free_vector(scores);
  sr_free_vector(trial);

  return 0;
}

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

void sr_de(int ndim, real dpos, const char *bak_file, const char *log_file)
{
  (void)bak_file;

  sr_de_cfg cfg;
  sr_de_cfg_init(&cfg, ndim, dpos);
  if (sr_de_population > 0) {
    cfg.population = sr_de_population;
  }
  if (sr_de_weight > 0.0) {
    cfg.weight = sr_de_weight;
  }
  if (sr_de_crossover > 0.0) {
    cfg.crossover = sr_de_crossover;
  }
  if (sr_de_init_span > 0.0) {
    cfg.init_span = sr_de_init_span;
  }
  cfg.max_iters = sr_de_iter_limit;
  cfg.max_evals = sr_de_eval_limit;

  if (sa_idum < 0) {
    cfg.seed = (unsigned long long)(-sa_idum);
  } else if (sa_idum > 0) {
    cfg.seed = (unsigned long long)sa_idum;
  }

  real *best = sr_alloc_vector((size_t)ndim);
  if (!best) {
    fprintf(STDERR, "*** error (sr_de): allocation failure\n");
    exit(1);
  }

  real best_val = 0.0;
  int evals = 0;

  FILE *log_stream = fopen(log_file, "a");
  if (log_stream == NULL) {
    sr_free_vector(best);
    OPEN_ERROR(log_file);
  }
  fprintf(log_stream, "=> DIFFERENTIAL EVOLUTION:\n");
  fprintf(log_stream, "=> pop=%d weight=%.3f cr=%.3f span=%.3f\n",
          cfg.population, (double)cfg.weight, (double)cfg.crossover,
          (double)cfg.init_span);
  fclose(log_stream);

  if (sr_de_optimize(&cfg, ndim, sr_evalrf, best, &best_val, &evals) != 0) {
    sr_free_vector(best);
    fprintf(STDERR, "*** error (sr_de): optimisation failed\n");
    exit(1);
  }

  log_stream = fopen(log_file, "a");
  if (log_stream == NULL) {
    sr_free_vector(best);
    OPEN_ERROR(log_file);
  }
  sr_de_log_results(log_stream, ndim, evals, best, best_val);
  fclose(log_stream);

  sr_free_vector(best);
}
