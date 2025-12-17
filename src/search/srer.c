/***********************************************************************
GH/21.09.02
 File contains:

  sr_er(int ndim, real dpos, char *bak_file, char *log_file)
 Determine error bars for all parameters.

 Modified:
GH/21.09.02 - copy from srsx

***********************************************************************/

// cppcheck-suppress missingIncludeSystem
#include <stdio.h>
// cppcheck-suppress missingIncludeSystem
#include <string.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>
// cppcheck-suppress missingIncludeSystem
#include <math.h>
#include "search.h"
#include "sr_alloc.h"
#include "sr_parse.h"

extern char *sr_project;

/**********************************************************************/

typedef struct sr_er_ctx {
  int ndim;
  real dpos;
  real y0;
  real pref;
  real rtol;
  real rr;
  real *x;
  real *x0;
} sr_er_ctx;

typedef struct sr_er_arrays {
  real *y;
  real *del;
  real *err;
} sr_er_arrays;

static void sr_er_set_displacement(sr_er_ctx *ctx, int i_par, real delta)
{
  for (int j = 1; j <= ctx->ndim; j++)
  {
    ctx->x[j] = ctx->x0[j];
  }
  ctx->x[i_par] = ctx->x0[i_par] + delta;
}

static int sr_er_find_delta(sr_er_ctx *ctx, int i_par, real *out_y, real *out_delta)
{
  real delta = ctx->dpos;
  real rdel = 1.0;
  real y = ctx->y0;

  while ((rdel < 0.75) || (rdel > 1.25))
  {
    if (rdel < ctx->rtol)
    {
      if (rdel < 0.0)
      {
#ifdef ERROR
        fprintf(STDERR, "*** error (sr_er): minimum not found (%d)\n", i_par);
#endif
        return -1;
      }

#ifdef WARNING
      fprintf(STDWAR,
              "* warning (sr_er): minimum R factor independent of par. %d\n",
              i_par);
#endif
      rdel = 1.0;
    }
    else
    {
      // cppcheck-suppress suspiciousFloatingPointCast
      delta = delta / R_sqrt(rdel);
      sr_er_set_displacement(ctx, i_par, delta);

#ifdef CONTROL
      fprintf(STDCTR, "(sr_er): Calculate function for parameter (%d)\n", i_par);
#endif

      y = sr_evalrf(ctx->x);
      // cppcheck-suppress suspiciousFloatingPointCast
      rdel = R_fabs(y - ctx->y0) / ctx->pref;
    }
  }

  *out_y = y;
  *out_delta = delta;
  return 0;
}

static FILE *sr_er_open_log_append(const char *log_file)
{
  FILE *log_stream = fopen(log_file, "a");
  if (log_stream == NULL) {
    OPEN_ERROR(log_file);
  }
  return log_stream;
}

static int sr_er_alloc_buffers(sr_er_ctx *ctx, sr_er_arrays *a, int ndim)
{
  ctx->x = sr_alloc_vector((size_t)ndim);
  ctx->x0 = sr_alloc_vector((size_t)ndim);
  a->y = sr_alloc_vector((size_t)ndim);
  a->del = sr_alloc_vector((size_t)ndim);
  a->err = sr_alloc_vector((size_t)ndim);
  return (ctx->x != NULL && ctx->x0 != NULL && a->y != NULL && a->del != NULL && a->err != NULL) ? 0 : -1;
}

static void sr_er_free_buffers(sr_er_ctx *ctx, sr_er_arrays *a)
{
  sr_free_vector(ctx->x);
  sr_free_vector(ctx->x0);
  sr_free_vector(a->y);
  sr_free_vector(a->del);
  sr_free_vector(a->err);
}

static void sr_er_init_minimum(sr_er_ctx *ctx)
{
  for (int i_par = 1; i_par <= ctx->ndim; i_par++) {
    ctx->x0[i_par] = 0.0;
  }
  ctx->y0 = sr_evalrf(ctx->x0);
}

static void sr_er_read_rr(sr_er_ctx *ctx, const char *log_file)
{
  char dum_path[STRSZ];
  snprintf(dum_path, sizeof(dum_path), "%s.dum", sr_project);

  real rfac = 0.0;
  ctx->rr = 0.0;
  if (sr_read_two_reals_from_file(dum_path, &rfac, &ctx->rr) != 0)
  {
    FILE *log_stream = sr_er_open_log_append(log_file);
    fprintf(log_stream, "*** error while reading output from %s\n",
            getenv("CSEARCH_RFAC"));
    fclose(log_stream);
    exit(1);
  }

  ctx->pref = ctx->y0 * ctx->rr;
  ctx->rtol = R_TOLERANCE / ctx->rr;

#ifdef CONTROL
  fprintf(STDCTR, "(sr_er): rfac = %.4f, rr = %.4f\n", rfac, ctx->rr);
#endif
}

static void sr_er_compute_displacements(sr_er_ctx *ctx, sr_er_arrays *a)
{
  for (int i_par = 1; i_par <= ctx->ndim; i_par++)
  {
    real yi = 0.0;
    real di = 0.0;
    if (sr_er_find_delta(ctx, i_par, &yi, &di) != 0)
    {
      exit(1);
    }
    a->y[i_par] = yi;
    a->del[i_par] = di;
  }
}

static void sr_er_abs_deltas(sr_er_arrays *a, int ndim)
{
  for (int i_par = 1; i_par <= ndim; i_par++)
  {
    // cppcheck-suppress suspiciousFloatingPointCast
    a->del[i_par] = R_fabs(a->del[i_par]);
  }
}

static void sr_er_write_error_bar_line(FILE *log_stream, const sr_er_ctx *ctx, sr_er_arrays *a,
                                       int i_par, real pref)
{
  real dr = a->y[i_par] - ctx->y0;
  if (dr < 0.0)
  {
#ifdef WARNING
    fprintf(STDWAR, "* warning (sr_er): not at minimum: dr = %.4f\n", dr);
#endif
    // cppcheck-suppress suspiciousFloatingPointCast
    dr = R_fabs(dr);
  }

  if (!IS_EQUAL_REAL(dr, 0.0))
  {
    // cppcheck-suppress suspiciousFloatingPointCast
    a->err[i_par] = R_sqrt(pref / dr) * a->del[i_par];
    fprintf(log_stream, "%2d: del R = %.4f; del par = %.4f\n",
            i_par, dr, a->err[i_par]);
  }
  else
  {
    a->err[i_par] = -1.0;
    fprintf(log_stream, "%2d: del R = %.4f; del par ** undefined **\n",
            i_par, dr);
  }
}

void sr_er(int ndim, real dpos, const char *bak_file, const char *log_file)
{
  (void)bak_file;

  sr_er_ctx ctx;
  ctx.ndim = ndim;
  ctx.dpos = dpos;
  ctx.x = NULL;
  ctx.x0 = NULL;

/***********************************************************************
  write title to log file
***********************************************************************/

  FILE *log_stream = sr_er_open_log_append(log_file);
  fprintf(log_stream, "=> DETERMINE ERROR BARS:\n\n");

/***********************************************************************
  allocate memory and preset variables
***********************************************************************/

  sr_er_arrays a;
  a.y = NULL;
  a.del = NULL;
  a.err = NULL;

  if (sr_er_alloc_buffers(&ctx, &a, ndim) != 0)
  {
    sr_er_free_buffers(&ctx, &a);
    fprintf(STDERR, "*** error (sr_er): allocation failure\n");
    exit(1);
  }

/***********************************************************************
  Calculate R factor for minimum.
***********************************************************************/

  fprintf(log_stream, "=> displace parameters from minimum:\n");
  fclose(log_stream);

  sr_er_init_minimum(&ctx);

/***********************************************************************
  Read R factor value and RR factor for minimum
***********************************************************************/

  sr_er_read_rr(&ctx, log_file);

/***********************************************************************
  Calculate R factors for displacements
***********************************************************************/

  sr_er_compute_displacements(&ctx, &a);


/***********************************************************************
  Calculate error bars and write to log file
***********************************************************************/

  real pref = a.y[1] * ctx.rr;
  sr_er_abs_deltas(&a, ndim);

  log_stream = sr_er_open_log_append(log_file);

  fprintf(log_stream, "\n=> ERROR BARS:\n\n");
  for (int i_par = 1; i_par <= ndim; i_par++)
  {
    sr_er_write_error_bar_line(log_stream, &ctx, &a, i_par, pref);
  }

  fclose(log_stream);

  sr_er_free_buffers(&ctx, &a);
} /* end of function sr_er */

/***********************************************************************/
