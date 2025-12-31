/***********************************************************************
 *                        SRPSO.C
 *
 *  Particle Swarm Optimisation (PSO) driver + core routine.
 ***********************************************************************/

// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdio.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>
// cppcheck-suppress missingIncludeSystem
#include <string.h>

#include "search.h"
#include "sr_alloc.h"
#include "sr_rng.h"

static int sr_pso_default_swarm_size(int ndim)
{
  const int base = 5 * ndim;
  return (base < 10) ? 10 : base;
}

void sr_pso_cfg_init(sr_pso_cfg *cfg, int ndim, real dpos)
{
  if (!cfg) return;
  cfg->swarm_size = sr_pso_default_swarm_size(ndim);
  cfg->max_iters = 0;
  cfg->max_evals = 0;
  cfg->inertia = (real)0.72;
  cfg->c1 = (real)1.49;
  cfg->c2 = (real)1.49;
  cfg->v_max = (dpos > 0.0) ? dpos : (real)1.0;
  cfg->seed = 0;
}

static real sr_pso_rand_span(sr_rng *rng, real span)
{
  const real u = (real)sr_rng_uniform01(rng);
  return (u * (real)2.0 - (real)1.0) * span;
}

static int sr_pso_init_swarm(sr_rng *rng, int swarm, int ndim, real span,
                             real **pos, real **vel, real **pbest,
                             real *pbest_val, real *gbest, real *gbest_val,
                             real (*func)(real *), int *evals)
{
  if (!rng || !pos || !vel || !pbest || !pbest_val || !gbest || !gbest_val || !func) {
    return -1;
  }

  *gbest_val = 0.0;
  for (int i = 1; i <= swarm; i++) {
    for (int j = 1; j <= ndim; j++) {
      pos[i][j] = sr_pso_rand_span(rng, span);
      vel[i][j] = sr_pso_rand_span(rng, span);
    }

    pbest_val[i] = (*func)(pos[i]);
    if (evals) (*evals)++;

    for (int j = 1; j <= ndim; j++) {
      pbest[i][j] = pos[i][j];
    }

    if (i == 1 || pbest_val[i] < *gbest_val) {
      *gbest_val = pbest_val[i];
      for (int j = 1; j <= ndim; j++) {
        gbest[j] = pos[i][j];
      }
    }
  }

  return 0;
}

int sr_pso_optimize(const sr_pso_cfg *cfg, int ndim, real (*func)(real *),
                    real *best, real *best_val, int *evals)
{
  if (!cfg || !func || !best || !best_val || ndim <= 0) {
    return -1;
  }

  const int swarm = (cfg->swarm_size > 0) ? cfg->swarm_size : sr_pso_default_swarm_size(ndim);
  const int max_iters = (cfg->max_iters > 0) ? cfg->max_iters : MAX_ITER_PSO;
  const int max_evals = (cfg->max_evals > 0) ? cfg->max_evals : MAX_EVAL_PSO;
  const real inertia = cfg->inertia;
  const real c1 = cfg->c1;
  const real c2 = cfg->c2;
  const real v_max = (cfg->v_max > 0.0) ? cfg->v_max : (real)1.0;

  sr_rng rng;
  const unsigned long long seed = (cfg->seed > 0) ? cfg->seed : 1ULL;
  sr_rng_seed(&rng, (uint64_t)seed);

  real **pos = sr_alloc_matrix((size_t)swarm, (size_t)ndim);
  real **vel = sr_alloc_matrix((size_t)swarm, (size_t)ndim);
  real **pbest = sr_alloc_matrix((size_t)swarm, (size_t)ndim);
  real *pbest_val = sr_alloc_vector((size_t)swarm);
  real *gbest = sr_alloc_vector((size_t)ndim);

  if (!pos || !vel || !pbest || !pbest_val || !gbest) {
    sr_free_matrix(pos);
    sr_free_matrix(vel);
    sr_free_matrix(pbest);
    sr_free_vector(pbest_val);
    sr_free_vector(gbest);
    return -1;
  }

  int local_evals = 0;
  if (sr_pso_init_swarm(&rng, swarm, ndim, v_max, pos, vel, pbest, pbest_val,
                        gbest, best_val, func, &local_evals) != 0) {
    sr_free_matrix(pos);
    sr_free_matrix(vel);
    sr_free_matrix(pbest);
    sr_free_vector(pbest_val);
    sr_free_vector(gbest);
    return -1;
  }

  int iter = 0;
  while (iter < max_iters && local_evals < max_evals) {
    iter++;
    for (int i = 1; i <= swarm; i++) {
      for (int j = 1; j <= ndim; j++) {
        const real r1 = (real)sr_rng_uniform01(&rng);
        const real r2 = (real)sr_rng_uniform01(&rng);
        real v = inertia * vel[i][j];
        v += c1 * r1 * (pbest[i][j] - pos[i][j]);
        v += c2 * r2 * (gbest[j] - pos[i][j]);

        if (v > v_max) v = v_max;
        if (v < -v_max) v = -v_max;

        vel[i][j] = v;
        pos[i][j] += v;
      }

      const real f = (*func)(pos[i]);
      local_evals++;

      if (f < pbest_val[i]) {
        pbest_val[i] = f;
        for (int j = 1; j <= ndim; j++) {
          pbest[i][j] = pos[i][j];
        }
        if (f < *best_val) {
          *best_val = f;
          for (int j = 1; j <= ndim; j++) {
            gbest[j] = pos[i][j];
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

  for (int j = 1; j <= ndim; j++) {
    best[j] = gbest[j];
  }

  if (evals) *evals = local_evals;

  sr_free_matrix(pos);
  sr_free_matrix(vel);
  sr_free_matrix(pbest);
  sr_free_vector(pbest_val);
  sr_free_vector(gbest);

  return 0;
}

static void sr_pso_log_results(FILE *log_stream, int ndim, int evals,
                               const real *best, real best_val)
{
  fprintf(log_stream, "\n=> No. of function evaluations in sr_pso: %3d\n", evals);
  fprintf(log_stream, "=> Best parameter set:\n");
  for (int j = 1; j <= ndim; j++) {
    fprintf(log_stream, "%.6f ", (double)best[j]);
  }
  fprintf(log_stream, "\n");
  fprintf(log_stream, "=> Best function value:\n");
  fprintf(log_stream, "rmin = %.6f\n", (double)best_val);
}

void sr_pso(int ndim, real dpos, const char *bak_file, const char *log_file)
{
  (void)bak_file;

  sr_pso_cfg cfg;
  sr_pso_cfg_init(&cfg, ndim, dpos);
  if (sr_pso_swarm_size > 0) {
    cfg.swarm_size = sr_pso_swarm_size;
  }
  if (sr_pso_inertia > 0.0) {
    cfg.inertia = sr_pso_inertia;
  }
  if (sr_pso_c1 > 0.0) {
    cfg.c1 = sr_pso_c1;
  }
  if (sr_pso_c2 > 0.0) {
    cfg.c2 = sr_pso_c2;
  }
  if (sr_pso_vmax > 0.0) {
    cfg.v_max = sr_pso_vmax;
  }
  cfg.max_iters = sr_pso_iter_limit;
  cfg.max_evals = sr_pso_eval_limit;

  if (sa_idum < 0) {
    cfg.seed = (unsigned long long)(-sa_idum);
  } else if (sa_idum > 0) {
    cfg.seed = (unsigned long long)sa_idum;
  }

  real *best = sr_alloc_vector((size_t)ndim);
  if (!best) {
    fprintf(STDERR, "*** error (sr_pso): allocation failure\n");
    exit(1);
  }

  real best_val = 0.0;
  int evals = 0;

  FILE *log_stream = fopen(log_file, "a");
  if (log_stream == NULL) {
    sr_free_vector(best);
    OPEN_ERROR(log_file);
  }
  fprintf(log_stream, "=> PARTICLE SWARM OPTIMISATION:\n");
  fprintf(log_stream, "=> swarm=%d inertia=%.3f c1=%.3f c2=%.3f vmax=%.3f\n",
          cfg.swarm_size, (double)cfg.inertia, (double)cfg.c1, (double)cfg.c2,
          (double)cfg.v_max);
  fclose(log_stream);

  if (sr_pso_optimize(&cfg, ndim, sr_evalrf, best, &best_val, &evals) != 0) {
    sr_free_vector(best);
    fprintf(STDERR, "*** error (sr_pso): optimisation failed\n");
    exit(1);
  }

  log_stream = fopen(log_file, "a");
  if (log_stream == NULL) {
    sr_free_vector(best);
    OPEN_ERROR(log_file);
  }
  sr_pso_log_results(log_stream, ndim, evals, best, best_val);
  fclose(log_stream);

  sr_free_vector(best);
}
