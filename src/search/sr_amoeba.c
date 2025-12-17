/*********************************************************************
 *                       SR_AMOEBA.C
 *
 *  GPL-3.0-or-later
 *
 *  Open implementation of a Nelder–Mead (downhill simplex) minimiser.
 *
 *  The API matches legacy SEARCH code conventions:
 *  - simplex vertices are stored in p[1..ndim+1][1..ndim]
 *  - function values are stored in y[1..ndim+1]
 *********************************************************************/

// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdio.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>
// cppcheck-suppress missingIncludeSystem
#include <string.h>
// cppcheck-suppress missingIncludeSystem
#include <time.h>

#include "copy_file.h"
#include "search.h"

#ifndef MAX_ITER_AMOEBA
#define MAX_ITER_AMOEBA 2000
#endif

static int sr_write_vertex_file(const real **p, const real *y, int ndim)
{
  if (sr_project == NULL || sr_project[0] == '\0') return 0;

  char ver_file[STRSZ];
  char vbk_file[STRSZ];

  /* Best-effort backup of the previous vertex file. */
  (void)snprintf(ver_file, sizeof(ver_file), "%s.ver", sr_project);
  (void)snprintf(vbk_file, sizeof(vbk_file), "%s.vbk", sr_project);

  (void)copy_file(ver_file, vbk_file);

  FILE *fp = fopen(ver_file, "w");
  if (fp == NULL) return -1;

  int mpts = ndim + 1;
  fprintf(fp, "%d %d %s\n", ndim, mpts, sr_project);
  for (int i = 1; i <= mpts; i++) {
    fprintf(fp, "%e ", (double)y[i]);
    for (int j = 1; j <= ndim; j++) {
      fprintf(fp, "%e ", (double)p[i][j]);
    }
    fputc('\n', fp);
  }

  time_t now = time(NULL);
  const struct tm *tm_info = localtime(&now);
  if (tm_info != NULL) {
    char timebuf[64];
    if (strftime(timebuf, sizeof(timebuf), "%c", tm_info) > 0) {
      fprintf(fp, "%s\n", timebuf);
    }
  }

  fclose(fp);
  return 0;
}

static void sr_copy_point(real *dst, const real *src, int ndim)
{
  for (int j = 1; j <= ndim; j++) dst[j] = src[j];
}

static void sr_centroid_excluding(const real **p, real *centroid, int ndim, int exclude_index)
{
  int mpts = ndim + 1;
  for (int j = 1; j <= ndim; j++) centroid[j] = 0.0;

  for (int i = 1; i <= mpts; i++) {
    if (i == exclude_index) continue;
    for (int j = 1; j <= ndim; j++) centroid[j] += p[i][j];
  }

  /* There are ndim points when excluding one vertex. */
  for (int j = 1; j <= ndim; j++) centroid[j] /= (real)ndim;
}

static void sr_simplex_extremes(const real *y, int ndim, int *ilo, int *ihi, int *inhi)
{
  int mpts = ndim + 1;

  *ilo = 1;
  *ihi = (y[1] > y[2]) ? 1 : 2;
  *inhi = (y[1] > y[2]) ? 2 : 1;

  for (int i = 1; i <= mpts; i++) {
    if (y[i] < y[*ilo]) *ilo = i;
    if (y[i] > y[*ihi]) {
      *inhi = *ihi;
      *ihi = i;
    } else if (i != *ihi && y[i] > y[*inhi]) {
      *inhi = i;
    }
  }
}

typedef struct sr_amoeba_ctx {
  int ndim;
  int mpts;
  real alpha;
  real gamma;
  real rho;
  real sigma;
  real ftol;
  real (*funk)(real *);
  int *nfunk;
  real **p;
  real *y;
  real *centroid;
  real *trial;
  real *trial2;
} sr_amoeba_ctx;

static real sr_amoeba_eval(sr_amoeba_ctx *ctx, real *x)
{
  real v = ctx->funk(x);
  (*ctx->nfunk)++;
  return v;
}

static int sr_amoeba_converged(const sr_amoeba_ctx *ctx, int ilo, int ihi)
{
  return (fabs((double)(ctx->y[ihi] - ctx->y[ilo])) < (double)ctx->ftol) ? 1 : 0;
}

static void sr_amoeba_accept(sr_amoeba_ctx *ctx, int ihi, const real *x, real fx)
{
  sr_copy_point(ctx->p[ihi], x, ctx->ndim);
  ctx->y[ihi] = fx;
}

static real sr_amoeba_reflect(sr_amoeba_ctx *ctx, int ihi)
{
  for (int j = 1; j <= ctx->ndim; j++) {
    ctx->trial[j] = ctx->centroid[j] + ctx->alpha * (ctx->centroid[j] - ctx->p[ihi][j]);
  }
  return sr_amoeba_eval(ctx, ctx->trial);
}

static void sr_amoeba_expand_or_reflect(sr_amoeba_ctx *ctx, int ihi, real fr)
{
  for (int j = 1; j <= ctx->ndim; j++) {
    ctx->trial2[j] = ctx->centroid[j] + ctx->gamma * (ctx->trial[j] - ctx->centroid[j]);
  }
  real fe = sr_amoeba_eval(ctx, ctx->trial2);

  if (fe < fr) {
    sr_amoeba_accept(ctx, ihi, ctx->trial2, fe);
  } else {
    sr_amoeba_accept(ctx, ihi, ctx->trial, fr);
  }
}

static void sr_amoeba_shrink(sr_amoeba_ctx *ctx, int ilo)
{
  for (int i = 1; i <= ctx->mpts; i++) {
    if (i == ilo) continue;
    for (int j = 1; j <= ctx->ndim; j++) {
      ctx->p[i][j] = ctx->p[ilo][j] + ctx->sigma * (ctx->p[i][j] - ctx->p[ilo][j]);
      ctx->trial2[j] = ctx->p[i][j];
    }
    ctx->y[i] = sr_amoeba_eval(ctx, ctx->trial2);
  }
}

static void sr_amoeba_contract_or_shrink(sr_amoeba_ctx *ctx, int ilo, int ihi, real fr)
{
  int outside = (fr < ctx->y[ihi]);
  if (outside) {
    for (int j = 1; j <= ctx->ndim; j++) {
      ctx->trial2[j] = ctx->centroid[j] + ctx->rho * (ctx->trial[j] - ctx->centroid[j]);
    }
  } else {
    for (int j = 1; j <= ctx->ndim; j++) {
      ctx->trial2[j] = ctx->centroid[j] + ctx->rho * (ctx->p[ihi][j] - ctx->centroid[j]);
    }
  }

  real fc = sr_amoeba_eval(ctx, ctx->trial2);

  if (fc < ctx->y[ihi]) {
    sr_amoeba_accept(ctx, ihi, ctx->trial2, fc);
    return;
  }

  sr_amoeba_shrink(ctx, ilo);
}

static int sr_amoeba_step(sr_amoeba_ctx *ctx)
{
  int ilo = 1, ihi = 1, inhi = 1;
  sr_simplex_extremes(ctx->y, ctx->ndim, &ilo, &ihi, &inhi);

  if (sr_amoeba_converged(ctx, ilo, ihi)) return 1;
  if (*ctx->nfunk >= MAX_ITER_AMOEBA) return -2;

  sr_centroid_excluding((const real **)ctx->p, ctx->centroid, ctx->ndim, ihi);
  real fr = sr_amoeba_reflect(ctx, ihi);

  if (fr < ctx->y[ilo]) {
    sr_amoeba_expand_or_reflect(ctx, ihi, fr);
  } else if (fr < ctx->y[inhi]) {
    sr_amoeba_accept(ctx, ihi, ctx->trial, fr);
  } else {
    sr_amoeba_contract_or_shrink(ctx, ilo, ihi, fr);
  }

  (void)sr_write_vertex_file((const real **)ctx->p, ctx->y, ctx->ndim);
  return 0;
}

int sr_amoeba(real **p, real *y, int ndim, real ftol, real (*funk)(real *), int *nfunk)
{
  if (p == NULL) return -1;
  if (y == NULL) return -1;
  if (ndim <= 0) return -1;
  if (funk == NULL) return -1;
  if (nfunk == NULL) return -1;

  sr_amoeba_ctx ctx;
  ctx.ndim = ndim;
  ctx.mpts = ndim + 1;
  ctx.alpha = (real)1.0;
  ctx.gamma = (real)2.0;
  ctx.rho = (real)0.5;
  ctx.sigma = (real)0.5;
  ctx.ftol = ftol;
  ctx.funk = funk;
  ctx.nfunk = nfunk;
  ctx.p = p;
  ctx.y = y;
  ctx.centroid = NULL;
  ctx.trial = NULL;
  ctx.trial2 = NULL;

  *nfunk = 0;

  ctx.centroid = (real *)calloc((size_t)ndim + 1, sizeof(real));
  ctx.trial = (real *)calloc((size_t)ndim + 1, sizeof(real));
  ctx.trial2 = (real *)calloc((size_t)ndim + 1, sizeof(real));
  if (ctx.centroid == NULL || ctx.trial == NULL || ctx.trial2 == NULL) {
    free(ctx.centroid);
    free(ctx.trial);
    free(ctx.trial2);
    return -1;
  }

  int rc = 0;
  for (;;) {
    rc = sr_amoeba_step(&ctx);
    if (rc != 0) break;
  }

  free(ctx.centroid);
  free(ctx.trial);
  free(ctx.trial2);

  return (rc == 1) ? 0 : rc;
}
