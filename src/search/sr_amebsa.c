/*********************************************************************
 *                       SR_AMEBSA.C
 *
 *  GPL-3.0-or-later
 *
 *  Lightweight simulated annealing wrapper around the simplex method.
 *
 *  Legacy SEARCH conventions:
 *  - simplex vertices are stored in p[1..ndim+1][1..ndim]
 *  - function values are stored in y[1..ndim+1]
 *  - best point is returned in pb[1..ndim] and *yb
 *********************************************************************/

// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>

#include "search.h"
#include "sr_rng.h"

/* The SEARCH driver keeps a global seed. */
extern long sa_idum;

static void sr_copy_point(real *dst, const real *src, int ndim)
{
  for (int j = 1; j <= ndim; j++) dst[j] = src[j];
}

static void sr_simplex_extremes(const real *y, int ndim, int *ilo, int *ihi)
{
  int mpts = ndim + 1;
  *ilo = 1;
  *ihi = 1;
  for (int i = 2; i <= mpts; i++) {
    if (y[i] < y[*ilo]) *ilo = i;
    if (y[i] > y[*ihi]) *ihi = i;
  }
}

static void sr_centroid_excluding(const real **p, real *centroid, int ndim, int exclude_index)
{
  int mpts = ndim + 1;
  for (int j = 1; j <= ndim; j++) centroid[j] = 0.0;
  for (int i = 1; i <= mpts; i++) {
    if (i == exclude_index) continue;
    for (int j = 1; j <= ndim; j++) centroid[j] += p[i][j];
  }
  for (int j = 1; j <= ndim; j++) centroid[j] /= (real)ndim;
}

static int sr_accept_move(sr_rng *rng, real current, real proposed, real temp)
{
  if (proposed <= current) return 1;
  if (temp <= (real)0.0) return 0;
  double u = sr_rng_uniform01(rng);
  double prob = exp(-(double)(proposed - current) / (double)temp);
  return u < prob;
}

typedef struct sr_amebsa_ctx {
  int ndim;
  real ftol;
  real temptr;
  sr_amebsa_func funk;
  real **p;
  real *y;
  real *pb;
  real *yb;
  int *iter_io;
  uint64_t seed;
  sr_rng rng;
  real *centroid;
  real *trial;
  int budget;
  int used;
} sr_amebsa_ctx;

static int sr_amebsa_validate_inputs(real **p, real *y, int ndim, real *pb, real *yb,
                                     const sr_amebsa_cfg *cfg, int *iter)
{
  if (p == NULL || y == NULL || pb == NULL || yb == NULL || cfg == NULL || iter == NULL || ndim <= 0) {
    return -1;
  }
  return 0;
}

static uint64_t sr_amebsa_seed_from_idum(long idum)
{
  uint64_t seed = (idum < 0) ? (uint64_t)(-idum) : (uint64_t)idum;
  return (seed == 0) ? 1 : seed;
}

static int sr_amebsa_alloc(sr_amebsa_ctx *ctx)
{
  ctx->centroid = (real *)calloc((size_t)ctx->ndim + 1, sizeof(real));
  ctx->trial = (real *)calloc((size_t)ctx->ndim + 1, sizeof(real));
  if (ctx->centroid == NULL || ctx->trial == NULL) return -1;
  return 0;
}

static void sr_amebsa_free(sr_amebsa_ctx *ctx)
{
  free(ctx->centroid);
  free(ctx->trial);
  ctx->centroid = NULL;
  ctx->trial = NULL;
}

static void sr_amebsa_init_best(sr_amebsa_ctx *ctx)
{
  int ilo = 1, ihi = 1;
  sr_simplex_extremes(ctx->y, ctx->ndim, &ilo, &ihi);
  sr_copy_point(ctx->pb, ctx->p[ilo], ctx->ndim);
  *ctx->yb = ctx->y[ilo];
}

static int sr_amebsa_step(sr_amebsa_ctx *ctx)
{
  int ilo = 1, ihi = 1;
  sr_simplex_extremes(ctx->y, ctx->ndim, &ilo, &ihi);

  if (fabs((double)(ctx->y[ihi] - ctx->y[ilo])) < (double)ctx->ftol) return 1;

  sr_centroid_excluding((const real **)ctx->p, ctx->centroid, ctx->ndim, ihi);

  for (int j = 1; j <= ctx->ndim; j++) {
    double jitter = (sr_rng_uniform01(&ctx->rng) - 0.5) * 2.0;
    ctx->trial[j] = ctx->centroid[j] + (real)1.0 * (ctx->centroid[j] - ctx->p[ihi][j]) +
                    (real)(ctx->temptr * (real)0.05) * (real)jitter;
  }

  real fr = (*ctx->funk)(ctx->trial);
  ctx->used++;

  if (!sr_accept_move(&ctx->rng, ctx->y[ihi], fr, ctx->temptr)) return 0;

  sr_copy_point(ctx->p[ihi], ctx->trial, ctx->ndim);
  ctx->y[ihi] = fr;
  if (fr < *ctx->yb) {
    sr_copy_point(ctx->pb, ctx->trial, ctx->ndim);
    *ctx->yb = fr;
  }

  return 0;
}

static int sr_amebsa_init_ctx(sr_amebsa_ctx *ctx, real **p, real *y, int ndim, real *pb, real *yb,
                              const sr_amebsa_cfg *cfg, int *iter)
{
  if (ctx == NULL) return -1;
  if (sr_amebsa_validate_inputs(p, y, ndim, pb, yb, cfg, iter) != 0) return -1;

  ctx->ndim = ndim;
  ctx->ftol = cfg->ftol;
  ctx->temptr = cfg->temptr;
  ctx->funk = sr_amebsa_cfg_get_funk(cfg);
  ctx->p = p;
  ctx->y = y;
  ctx->pb = pb;
  ctx->yb = yb;
  ctx->iter_io = iter;
  ctx->centroid = NULL;
  ctx->trial = NULL;
  ctx->used = 0;

  if (ctx->funk == NULL) return -1;

  ctx->budget = *iter;
  if (ctx->budget < 0) ctx->budget = 0;

  if (sr_amebsa_alloc(ctx) != 0) return -1;

  ctx->seed = sr_amebsa_seed_from_idum(sa_idum);
  sr_rng_seed(&ctx->rng, ctx->seed);
  sr_amebsa_init_best(ctx);
  return 0;
}

static void sr_amebsa_run(sr_amebsa_ctx *ctx)
{
  for (int k = 0; k < ctx->budget; k++) {
    int rc = sr_amebsa_step(ctx);
    if (rc != 0) break;
  }
}

static void sr_amebsa_finalize(sr_amebsa_ctx *ctx)
{
  *ctx->iter_io = ctx->used;
  sa_idum = (long)(ctx->seed + (uint64_t)ctx->used + 1);
  sr_amebsa_init_best(ctx);
}

int sr_amebsa(real **p, real *y, int ndim, real *pb, real *yb,
              const sr_amebsa_cfg *cfg, int *iter)
{
  sr_amebsa_ctx ctx;
  if (sr_amebsa_init_ctx(&ctx, p, y, ndim, pb, yb, cfg, iter) != 0) {
    sr_amebsa_free(&ctx);
    return -1;
  }

  sr_amebsa_run(&ctx);
  sr_amebsa_finalize(&ctx);
  sr_amebsa_free(&ctx);
  return 0;
}
