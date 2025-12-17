/*********************************************************************
 *                       SR_POWELL.C
 *
 *  GPL-3.0-or-later
 *
 *  Open implementation of Powell's direction-set method using a
 *  derivative-free 1D line minimiser (bracketing + Brent).
 *
 *  Legacy SEARCH conventions:
 *  - vectors are expected to be usable with indices 1..n
 *  - direction matrix xi is expected as xi[1..n][1..n] (columns = directions)
 *********************************************************************/

// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>

#include "search.h"
#include "sr_alloc.h"

#ifndef MAX_ITER_POWELL
#define MAX_ITER_POWELL 100
#endif

typedef struct sr_line_ctx {
  int n;
  real *p0;         /* base point (1..n) */
  real *dir;        /* direction (1..n) */
  real *tmp;        /* scratch (1..n) */
  real (*func)(real *);   /* objective */
} sr_line_ctx;

static real sr_line_eval(real alpha, void *vctx)
{
  sr_line_ctx *ctx = (sr_line_ctx *)vctx;
  for (int j = 1; j <= ctx->n; j++) {
    ctx->tmp[j] = ctx->p0[j] + alpha * ctx->dir[j];
  }
  return (*ctx->func)(ctx->tmp);
}

static void sr_swap_real(real *a, real *b)
{
  real t = *a;
  *a = *b;
  *b = t;
}

static int sr_bracket_minimum(real *a, real *b, real *c,
                              real *fa, real *fb, real *fc,
                              sr_line_ctx *ctx)
{
  /* Simple forward/backward bracketing with expansion. */
  const real golden = (real)1.618033988749895;
  const int max_expand = 64;

  *a = 0.0;
  *b = 1.0;
  *fa = sr_line_eval(*a, ctx);
  *fb = sr_line_eval(*b, ctx);

  /* Ensure we move downhill from a to b. */
  if (*fb > *fa) {
    *b = -1.0;
    *fb = sr_line_eval(*b, ctx);
    if (*fb > *fa) {
      /* Fall back to a small step around zero. */
      *b = 0.1;
      *fb = sr_line_eval(*b, ctx);
      if (*fb > *fa) {
        *b = -0.1;
        *fb = sr_line_eval(*b, ctx);
      }
    }
  }

  *c = *b + golden * (*b - *a);
  *fc = sr_line_eval(*c, ctx);

  int k = 0;
  while (*fb > *fc && k++ < max_expand) {
    *a = *b;
    *fa = *fb;
    *b = *c;
    *fb = *fc;
    *c = *b + golden * (*b - *a);
    *fc = sr_line_eval(*c, ctx);
  }

  return (k < max_expand) ? 0 : -1;
}

typedef struct sr_brent_state {
  real a;
  real b;
  real x;
  real w;
  real v;
  real fx;
  real fw;
  real fv;
  real d;
  real e;
} sr_brent_state;

static void sr_brent_init(sr_brent_state *s, real ax, real bx, real cx, sr_line_ctx *ctx, int *evals)
{
  s->a = (ax < cx) ? ax : cx;
  s->b = (ax < cx) ? cx : ax;
  s->x = bx;
  s->w = bx;
  s->v = bx;
  s->fx = sr_line_eval(s->x, ctx);
  s->fw = s->fx;
  s->fv = s->fx;
  s->d = 0.0;
  s->e = 0.0;
  if (evals) *evals += 1;
}

static int sr_brent_is_converged(const sr_brent_state *s, real m, real tol2)
{
  return ((real)fabs((double)(s->x - m)) <= (tol2 - (s->b - s->a) * (real)0.5)) ? 1 : 0;
}

static void sr_brent_golden_step(sr_brent_state *s, real m, real cgold)
{
  s->e = (s->x >= m) ? (s->a - s->x) : (s->b - s->x);
  s->d = cgold * s->e;
}

static int sr_brent_parabolic_step(sr_brent_state *s, real m, real tol1, real tol2)
{
  real r = (s->x - s->w) * (s->fx - s->fv);
  real q = (s->x - s->v) * (s->fx - s->fw);
  real p = (s->x - s->v) * q - (s->x - s->w) * r;
  q = (real)2.0 * (q - r);
  if (q > 0.0) p = -p;
  q = (real)fabs((double)q);

  real etmp = s->e;
  s->e = s->d;

  if ((real)fabs((double)p) >= (real)fabs((double)((real)0.5 * q * etmp))) return -1;
  if (p <= q * (s->a - s->x)) return -1;
  if (p >= q * (s->b - s->x)) return -1;

  s->d = p / q;
  real u = s->x + s->d;
  if (u - s->a < tol2) {
    s->d = (s->x < m) ? tol1 : -tol1;
  }
  if (s->b - u < tol2) {
    s->d = (s->x < m) ? tol1 : -tol1;
  }
  return 0;
}

static void sr_brent_choose_step(sr_brent_state *s, real m, real tol1, real tol2, real cgold)
{
  if ((real)fabs((double)s->e) <= tol1) {
    sr_brent_golden_step(s, m, cgold);
    return;
  }

  if (sr_brent_parabolic_step(s, m, tol1, tol2) != 0) {
    sr_brent_golden_step(s, m, cgold);
  }
}

static real sr_brent_propose_u(const sr_brent_state *s, real tol1)
{
  real d = s->d;
  if ((real)fabs((double)d) >= tol1) return s->x + d;
  return s->x + ((d > 0) ? tol1 : -tol1);
}

static void sr_brent_update_better(sr_brent_state *s, real u, real fu)
{
  if (u >= s->x) s->a = s->x;
  else s->b = s->x;

  s->v = s->w;
  s->fv = s->fw;
  s->w = s->x;
  s->fw = s->fx;
  s->x = u;
  s->fx = fu;
}

static void sr_brent_update_worse(sr_brent_state *s, real u, real fu)
{
  if (u < s->x) s->a = u;
  else s->b = u;

  if (fu <= s->fw) {
    s->v = s->w;
    s->fv = s->fw;
    s->w = u;
    s->fw = fu;
    return;
  }

  if (s->w == s->x) {
    s->w = u;
    s->fw = fu;
    return;
  }

  if (fu <= s->fv) {
    s->v = u;
    s->fv = fu;
    return;
  }

  if (s->v == s->x) {
    s->v = u;
    s->fv = fu;
    return;
  }

  if (s->v == s->w) {
    s->v = u;
    s->fv = fu;
  }
}

static void sr_brent_update(sr_brent_state *s, real u, real fu)
{
  if (fu <= s->fx) {
    sr_brent_update_better(s, u, fu);
    return;
  }
  sr_brent_update_worse(s, u, fu);
}

static real sr_brent_minimise(real ax, real bx, real cx,
                              sr_line_ctx *ctx, real tol,
                              real *xmin, int *evals)
{
  /*
   * Brent's method (derivative-free) on a bracket [ax,cx] with a point bx.
   * This implementation is written from the algorithm description.
   */
  const int itmax = 100;
  const real cgold = (real)0.3819660112501051; /* 1 - 1/phi */
  const real zeps = (real)1e-12;

  sr_brent_state s;
  sr_brent_init(&s, ax, bx, cx, ctx, evals);

  for (int iter = 0; iter < itmax; iter++) {
    real m = (s.a + s.b) * (real)0.5;
    real tol1 = tol * (real)fabs((double)s.x) + zeps;
    real tol2 = (real)2.0 * tol1;

    if (sr_brent_is_converged(&s, m, tol2)) {
      if (xmin) *xmin = s.x;
      return s.fx;
    }

    sr_brent_choose_step(&s, m, tol1, tol2, cgold);
    real u = sr_brent_propose_u(&s, tol1);
    real fu = sr_line_eval(u, ctx);
    if (evals) *evals += 1;

    sr_brent_update(&s, u, fu);
  }

  if (xmin) *xmin = s.x;
  return s.fx;
}

static int sr_linmin(real *p, real *dir, int n, real *fret, real (*func)(real *))
{
  sr_line_ctx ctx;
  ctx.n = n;
  ctx.p0 = p;
  ctx.dir = dir;
  ctx.func = func;
  ctx.tmp = sr_alloc_vector((size_t)n);
  if (ctx.tmp == NULL) return -1;

  real a, b, c, fa, fb, fc;
  if (sr_bracket_minimum(&a, &b, &c, &fa, &fb, &fc, &ctx) != 0) {
    sr_free_vector(ctx.tmp);
    return -2;
  }

  int evals = 0;
  real xmin = 0.0;
  real fmin = sr_brent_minimise(a, b, c, &ctx, (real)1e-6, &xmin, &evals);

  for (int j = 1; j <= n; j++) {
    p[j] = p[j] + xmin * dir[j];
  }

  if (fret) *fret = fmin;

  sr_free_vector(ctx.tmp);
  return 0;
}

static int sr_powell_alloc_work(int n, real **out_p_prev, real **out_p_extrap, real **out_dir)
{
  real *p_prev = sr_alloc_vector((size_t)n);
  real *p_extrap = sr_alloc_vector((size_t)n);
  real *dir = sr_alloc_vector((size_t)n);
  if (p_prev == NULL || p_extrap == NULL || dir == NULL) {
    sr_free_vector(p_prev);
    sr_free_vector(p_extrap);
    sr_free_vector(dir);
    return -1;
  }
  *out_p_prev = p_prev;
  *out_p_extrap = p_extrap;
  *out_dir = dir;
  return 0;
}

static void sr_powell_free_work(real *p_prev, real *p_extrap, real *dir)
{
  sr_free_vector(dir);
  sr_free_vector(p_extrap);
  sr_free_vector(p_prev);
}

static int sr_powell_minimise_all_directions(real *p, real **xi, int n, real *fret, real (*func)(real *),
                                             real *dir, int *out_best_dir)
{
  real biggest_drop = 0.0;
  int best_dir = 1;

  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= n; j++) dir[j] = xi[j][i];
    real f_before = *fret;
    if (sr_linmin(p, dir, n, fret, func) != 0) return -2;

    real drop = (real)fabs((double)(f_before - *fret));
    if (drop > biggest_drop) {
      biggest_drop = drop;
      best_dir = i;
    }
  }

  *out_best_dir = best_dir;
  return 0;
}

static int sr_powell_converged(real f_start, real f_end, real ftol)
{
  return ((real)fabs((double)(f_start - f_end)) <= (real)0.5 * ftol) ? 1 : 0;
}

static void sr_powell_extrapolate(real *p, int n, real *p_prev, real *p_extrap, real *dir)
{
  for (int j = 1; j <= n; j++) {
    p_extrap[j] = (real)2.0 * p[j] - p_prev[j];
    dir[j] = p[j] - p_prev[j];
    p_prev[j] = p[j];
  }
}

typedef struct sr_powell_update_ctx {
  real *p;
  real **xi;
  int n;
  real *fret;
  real (*func)(real *);
  real *dir;
} sr_powell_update_ctx;

static void sr_powell_maybe_update_direction(sr_powell_update_ctx *ctx, int best_dir,
                                             real f_start, real f_extrap)
{
  if (f_extrap >= f_start) return;

  real f_saved = *ctx->fret;
  if (sr_linmin(ctx->p, ctx->dir, ctx->n, ctx->fret, ctx->func) != 0) return;
  if (*ctx->fret >= f_saved) return;

  for (int j = 1; j <= ctx->n; j++) ctx->xi[j][best_dir] = ctx->dir[j];
}

typedef struct sr_powell_run_ctx {
  real *p;
  real **xi;
  int n;
  real ftol;
  real *fret;
  real (*func)(real *);
  real *p_prev;
  real *p_extrap;
  real *dir;
} sr_powell_run_ctx;

static void sr_powell_copy_prev(sr_powell_run_ctx *ctx)
{
  for (int j = 1; j <= ctx->n; j++) ctx->p_prev[j] = ctx->p[j];
}

static int sr_powell_iteration(sr_powell_run_ctx *ctx)
{
  real f_start = *ctx->fret;
  int best_dir = 1;

  if (sr_powell_minimise_all_directions(ctx->p, ctx->xi, ctx->n, ctx->fret, ctx->func, ctx->dir, &best_dir) != 0) {
    return -2;
  }

  if (sr_powell_converged(f_start, *ctx->fret, ctx->ftol)) return 1;

  sr_powell_extrapolate(ctx->p, ctx->n, ctx->p_prev, ctx->p_extrap, ctx->dir);
  real f_extrap = (*ctx->func)(ctx->p_extrap);

  sr_powell_update_ctx update_ctx;
  update_ctx.p = ctx->p;
  update_ctx.xi = ctx->xi;
  update_ctx.n = ctx->n;
  update_ctx.fret = ctx->fret;
  update_ctx.func = ctx->func;
  update_ctx.dir = ctx->dir;
  sr_powell_maybe_update_direction(&update_ctx, best_dir, f_start, f_extrap);

  return 0;
}

static int sr_powell_run(sr_powell_run_ctx *ctx, int *iter)
{
  for (*iter = 1; *iter <= MAX_ITER_POWELL; (*iter)++) {
    int rc = sr_powell_iteration(ctx);
    if (rc == 0) continue;
    return (rc == 1) ? 0 : rc;
  }
  return -3;
}

int sr_powell(real *p, real **xi, int n, real ftol, int *iter,
              real *fret, real (*func)(real *))
{
  if (p == NULL) return -1;
  if (xi == NULL) return -1;
  if (n <= 0) return -1;
  if (iter == NULL) return -1;
  if (fret == NULL) return -1;
  if (func == NULL) return -1;

  real *p_prev = NULL;
  real *p_extrap = NULL;
  real *dir = NULL;
  if (sr_powell_alloc_work(n, &p_prev, &p_extrap, &dir) != 0) return -1;

  sr_powell_run_ctx ctx;
  ctx.p = p;
  ctx.xi = xi;
  ctx.n = n;
  ctx.ftol = ftol;
  ctx.fret = fret;
  ctx.func = func;
  ctx.p_prev = p_prev;
  ctx.p_extrap = p_extrap;
  ctx.dir = dir;

  *fret = (*func)(p);
  sr_powell_copy_prev(&ctx);

  int rc = sr_powell_run(&ctx, iter);
  sr_powell_free_work(p_prev, p_extrap, dir);
  return rc;
}
