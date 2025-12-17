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

// cppcheck-suppress functionLength
// cppcheck-suppress functionComplexity
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

  real a = (ax < cx) ? ax : cx;
  real b = (ax < cx) ? cx : ax;

  real x = bx, w = bx, v = bx;
  real fx = sr_line_eval(x, ctx);
  real fw = fx;
  real fv = fx;
  if (evals) *evals += 1;

  real d = 0.0;
  real e = 0.0;

  for (int iter = 0; iter < itmax; iter++) {
    real m = (a + b) * (real)0.5;
    real tol1 = tol * (real)fabs((double)x) + zeps;
    real tol2 = (real)2.0 * tol1;

    if ((real)fabs((double)(x - m)) <= (tol2 - (b - a) * (real)0.5)) {
      if (xmin) *xmin = x;
      return fx;
    }

    if ((real)fabs((double)e) > tol1) {
      real r = (x - w) * (fx - fv);
      real q = (x - v) * (fx - fw);
      real p = (x - v) * q - (x - w) * r;
      q = (real)2.0 * (q - r);
      if (q > 0.0) p = -p;
      q = (real)fabs((double)q);

      real etmp = e;
      e = d;

      if ((real)fabs((double)p) >= (real)fabs((double)(0.5 * q * etmp)) ||
          p <= q * (a - x) || p >= q * (b - x)) {
        e = (x >= m) ? (a - x) : (b - x);
        d = cgold * e;
      } else {
        d = p / q;
        real u = x + d;
        if (u - a < tol2 || b - u < tol2) {
          d = (x < m) ? tol1 : -tol1;
        }
      }
    } else {
      e = (x >= m) ? (a - x) : (b - x);
      d = cgold * e;
    }

    real u = ((real)fabs((double)d) >= tol1) ? (x + d) : (x + ((d > 0) ? tol1 : -tol1));
    real fu = sr_line_eval(u, ctx);
    if (evals) *evals += 1;

    if (fu <= fx) {
      if (u >= x) a = x; else b = x;
      v = w; fv = fw;
      w = x; fw = fx;
      x = u; fx = fu;
    } else {
      if (u < x) a = u; else b = u;
      if (fu <= fw || w == x) {
        v = w; fv = fw;
        w = u; fw = fu;
      } else if (fu <= fv || v == x || v == w) {
        v = u; fv = fu;
      }
    }
  }

  if (xmin) *xmin = x;
  return fx;
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

// cppcheck-suppress functionLength
// cppcheck-suppress functionComplexity
int sr_powell(real *p, real **xi, int n, real ftol, int *iter,
              real *fret, real (*func)(real *))
{
  if (p == NULL || xi == NULL || n <= 0 || iter == NULL || fret == NULL || func == NULL) {
    return -1;
  }

  real *p_prev = sr_alloc_vector((size_t)n);
  real *p_extrap = sr_alloc_vector((size_t)n);
  real *dir = sr_alloc_vector((size_t)n);
  if (p_prev == NULL || p_extrap == NULL || dir == NULL) {
    sr_free_vector(p_prev);
    sr_free_vector(p_extrap);
    sr_free_vector(dir);
    return -1;
  }

  *fret = (*func)(p);
  for (int j = 1; j <= n; j++) p_prev[j] = p[j];

  for (*iter = 1; *iter <= MAX_ITER_POWELL; (*iter)++) {
    real f_start = *fret;
    real biggest_drop = 0.0;
    int best_dir = 1;

    for (int i = 1; i <= n; i++) {
      for (int j = 1; j <= n; j++) dir[j] = xi[j][i];
      real f_before = *fret;
      if (sr_linmin(p, dir, n, fret, func) != 0) {
        sr_free_vector(dir);
        sr_free_vector(p_extrap);
        sr_free_vector(p_prev);
        return -2;
      }
      real drop = (real)fabs((double)(f_before - *fret));
      if (drop > biggest_drop) {
        biggest_drop = drop;
        best_dir = i;
      }
    }

    /* Absolute convergence based on improvement. */
    if ((real)fabs((double)(f_start - *fret)) <= (real)0.5 * ftol) {
      sr_free_vector(dir);
      sr_free_vector(p_extrap);
      sr_free_vector(p_prev);
      return 0;
    }

    /* Extrapolate point and direction. */
    for (int j = 1; j <= n; j++) {
      p_extrap[j] = (real)2.0 * p[j] - p_prev[j];
      dir[j] = p[j] - p_prev[j];
      p_prev[j] = p[j];
    }

    real f_extrap = (*func)(p_extrap);
    if (f_extrap < f_start) {
      /*
       * Perform an extra line minimisation along the extrapolated direction
       * and replace the most successful direction.
       */
      real f_saved = *fret;
      if (sr_linmin(p, dir, n, fret, func) == 0 && *fret < f_saved) {
        for (int j = 1; j <= n; j++) xi[j][best_dir] = dir[j];
      }
    }
  }

  sr_free_vector(dir);
  sr_free_vector(p_extrap);
  sr_free_vector(p_prev);
  return -3;
}
