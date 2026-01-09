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
#include "search_optimizer.h"
#include "sr_alloc.h"
#include "sr_rng.h"

static int sr_pso_default_swarm_size(int ndim)
{
  const int base = 10 * ndim;
  return (base < 10) ? 10 : base;
}

void sr_pso_cfg_init(sr_pso_cfg *cfg, int ndim, real dpos)
{
  if (!cfg) return;
  cfg->swarm_size = sr_pso_default_swarm_size(ndim);
  cfg->max_iters = 0;
  cfg->max_evals = 0;
  cfg->inertia = SR_PSO_DEFAULT_INERTIA;
  cfg->c1 = SR_PSO_DEFAULT_C1;
  cfg->c2 = SR_PSO_DEFAULT_C2;
  cfg->v_max = (dpos > 0.0) ? dpos : (real)1.0;
  cfg->seed = 0;
}

static real sr_pso_rand_span(sr_rng *rng, real span)
{
  const real u = (real)sr_rng_uniform01(rng);
  return (u * (real)2.0 - (real)1.0) * span;
}

typedef struct sr_pso_swarm {
  int size;
  int ndim;
  real **pos;
  real **vel;
  real **pbest;
  real *pbest_val;
  real *gbest;
} sr_pso_swarm;

static void sr_pso_init_particle(sr_pso_swarm *s, int i, sr_rng *rng, real span,
                                 real (*func)(real *), real *gbest_val, int *evals)
{
  for (int j = 1; j <= s->ndim; j++) {
    s->pos[i][j] = sr_pso_rand_span(rng, span);
    s->vel[i][j] = sr_pso_rand_span(rng, span);
  }

  s->pbest_val[i] = (*func)(s->pos[i]);
  if (evals) (*evals)++;

  for (int j = 1; j <= s->ndim; j++) {
    s->pbest[i][j] = s->pos[i][j];
  }

  if (i == 1 || s->pbest_val[i] < *gbest_val) {
    *gbest_val = s->pbest_val[i];
    for (int j = 1; j <= s->ndim; j++) {
      s->gbest[j] = s->pos[i][j];
    }
  }
}

static int sr_pso_init_swarm(sr_rng *rng, real span, sr_pso_swarm *s,
                             real *gbest_val, real (*func)(real *), int *evals)
{
  if (!rng || !s || !gbest_val || !func) {
    return -1;
  }

  *gbest_val = 0.0;
  for (int i = 1; i <= s->size; i++) {
    sr_pso_init_particle(s, i, rng, span, func, gbest_val, evals);
  }

  return 0;
}

static void sr_pso_free_swarm(sr_pso_swarm *s)
{
  if (!s) return;
  sr_free_matrix(s->pos);
  sr_free_matrix(s->vel);
  sr_free_matrix(s->pbest);
  sr_free_vector(s->pbest_val);
  sr_free_vector(s->gbest);
}

static int sr_pso_alloc_swarm(sr_pso_swarm *s, int swarm, int ndim)
{
  s->size = swarm;
  s->ndim = ndim;
  s->pos = NULL;
  s->vel = NULL;
  s->pbest = NULL;
  s->pbest_val = NULL;
  s->gbest = NULL;

  s->pos = sr_alloc_matrix((size_t)swarm, (size_t)ndim);
  s->vel = sr_alloc_matrix((size_t)swarm, (size_t)ndim);
  s->pbest = sr_alloc_matrix((size_t)swarm, (size_t)ndim);
  s->pbest_val = sr_alloc_vector((size_t)swarm);
  s->gbest = sr_alloc_vector((size_t)ndim);

  if (!s->pos || !s->vel || !s->pbest || !s->pbest_val || !s->gbest) {
    sr_pso_free_swarm(s);
    return -1;
  }
  return 0;
}

static void sr_pso_update_particle(const sr_pso_swarm *s, int i, sr_rng *rng,
                                   real inertia, real c1, real c2, real v_max)
{
  for (int j = 1; j <= s->ndim; j++) {
    const real r1 = (real)sr_rng_uniform01(rng);
    const real r2 = (real)sr_rng_uniform01(rng);
    real v = inertia * s->vel[i][j];
    v += c1 * r1 * (s->pbest[i][j] - s->pos[i][j]);
    v += c2 * r2 * (s->gbest[j] - s->pos[i][j]);

    if (v > v_max) v = v_max;
    if (v < -v_max) v = -v_max;

    s->vel[i][j] = v;
    s->pos[i][j] += v;
  }
}

static void sr_pso_update_best(sr_pso_swarm *s, int i, real f, real *best_val)
{
  if (f < s->pbest_val[i]) {
    s->pbest_val[i] = f;
    for (int j = 1; j <= s->ndim; j++) s->pbest[i][j] = s->pos[i][j];
    if (f < *best_val) {
      *best_val = f;
      for (int j = 1; j <= s->ndim; j++) s->gbest[j] = s->pos[i][j];
    }
  }
}

static void sr_pso_iterate_swarm(sr_pso_swarm *s, const sr_pso_cfg *cfg,
                                 sr_rng *rng, real (*func)(real *),
                                 real *best_val, int *local_evals, int max_evals)
{
  for (int i = 1; i <= s->size && *local_evals < max_evals; i++) {
    sr_pso_update_particle(s, i, rng, cfg->inertia, cfg->c1, cfg->c2, cfg->v_max);
    const real f = (*func)(s->pos[i]);
    (*local_evals)++;
    sr_pso_update_best(s, i, f, best_val);
  }
}

int sr_pso_optimize(const sr_pso_cfg *cfg, int ndim, real (*func)(real *),
                    real *best, real *best_val, int *evals)
{
  sr_pso_cfg defaults;
  if (!func || !best || !best_val || ndim <= 0) return -1;
  if (!cfg) {
    sr_pso_cfg_init(&defaults, ndim, (real)1.0);
    cfg = &defaults;
  }

  const int swarm_size = (cfg->swarm_size > 0) ? cfg->swarm_size : sr_pso_default_swarm_size(ndim);
  const int max_iters = (cfg->max_iters > 0) ? cfg->max_iters : MAX_ITER_PSO;
  const int max_evals = (cfg->max_evals > 0) ? cfg->max_evals : MAX_EVAL_PSO;
  const real v_max = (cfg->v_max > 0.0) ? cfg->v_max : (real)1.0;

  sr_rng rng;
  sr_rng_seed(&rng, (cfg->seed > 0) ? cfg->seed : 1ULL);

  sr_pso_swarm s;
  if (sr_pso_alloc_swarm(&s, swarm_size, ndim) != 0) return -1;

  int local_evals = 0;
  if (sr_pso_init_swarm(&rng, v_max, &s, best_val, func, &local_evals) != 0) {
    sr_pso_free_swarm(&s);
    return -1;
  }

  for (int iter = 1; iter <= max_iters && local_evals < max_evals; iter++) {
    sr_pso_iterate_swarm(&s, cfg, &rng, func, best_val, &local_evals, max_evals);
    if (*best_val <= R_TOLERANCE) break;
  }

  for (int j = 1; j <= ndim; j++) best[j] = s.gbest[j];
  if (evals) *evals = local_evals;
  sr_pso_free_swarm(&s);
  return 0;
}

static void sr_pso_get_config(sr_pso_cfg *cfg, int ndim, real dpos)
{
  sr_pso_cfg_init(cfg, ndim, dpos);
  if (sr_pso_swarm_size > 0) cfg->swarm_size = sr_pso_swarm_size;
  if (sr_pso_inertia > 0.0) cfg->inertia = sr_pso_inertia;
  if (sr_pso_c1 > 0.0) cfg->c1 = sr_pso_c1;
  if (sr_pso_c2 > 0.0) cfg->c2 = sr_pso_c2;
  if (sr_pso_vmax > 0.0) cfg->v_max = sr_pso_vmax;
  cfg->max_iters = sr_pso_iter_limit;
  cfg->max_evals = sr_pso_eval_limit;
  cfg->seed = sr_optimizer_get_seed();
}

static void sr_pso_log_start(const char *log_file, const sr_pso_cfg *cfg)
{
  FILE *log_stream = fopen(log_file, "a");
  if (log_stream) {
    fprintf(log_stream, "=> PARTICLE SWARM OPTIMISATION:\n");
    fprintf(log_stream, "=> swarm=%d inertia=%.3f c1=%.3f c2=%.3f vmax=%.3f\n",
            cfg->swarm_size, (double)cfg->inertia, (double)cfg->c1, (double)cfg->c2,
            (double)cfg->v_max);
    fclose(log_stream);
  }
}

void sr_pso(int ndim, real dpos, const char *bak_file, const char *log_file)
{
  sr_pso_cfg cfg;
  sr_pso_get_config(&cfg, ndim, dpos);
  sr_pso_log_start(log_file, &cfg);
  (void)bak_file;

  real *best = sr_alloc_vector((size_t)ndim);
  if (!best) {
    fprintf(STDERR, "*** error (sr_pso): allocation failure\n");
    exit(1);
  }

  real best_val = 0.0;
  int evals = 0;

  if (sr_pso_optimize(&cfg, ndim, sr_evalrf, best, &best_val, &evals) != 0) {
    sr_free_vector(best);
    fprintf(STDERR, "*** error (sr_pso): optimisation failed\n");
    exit(1);
  }

  FILE *log_stream = fopen(log_file, "a");
  if (log_stream) {
    sr_optimizer_log_results(log_stream, "sr_pso", ndim, evals, best, best_val);
    fclose(log_stream);
  } else {
    OPEN_ERROR(log_file);
  }

  sr_free_vector(best);
}
