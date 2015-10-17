/*********************************************************************
 *													simplex.c
 *
 *  Copyright (C) 2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/18 Mar 2015 - creation
 *********************************************************************/

/*!
 * \file simplex.c
 * \author Liam Deacon
 * \date 18 Mar 2015
 */

#include <stdbool.h>
#include "cleed_vector.h"
#include "cleed_matrix.h"
#include "search.h"

static const double ZEPS = 1e-10;

int check_tol(double fmax, double fmin, double ftol)
{
  double delta = fabs(fmax - fmin);
  double accuracy = (fabs(fmax) + fabs(fmin)) * ftol;

  return (delta < (accuracy + ZEPS));
}

cleed_basic_matrix *make_simplex(const cleed_vector *point, size_t dim)
{
  size_t i, j;
  cleed_basic_matrix *simplex = cleed_basic_matrix_alloc(dim + 1, dim);

  for (i = 0; i < dim + 1; i++)
  {
    for (j = 0; j < dim; j++)
    {
      cleed_basic_matrix_set(simplex, i, j, dim, cleed_vector_get(point, j));
    }
  }

  for (i = 0; i < dim; i++)
  {
    cleed_basic_matrix_set(simplex, i, i, dim,
        cleed_basic_matrix_get(simplex, i, i, dim) + 1.0);
  }

  return simplex;
}

void evaluate_simplex(const cleed_basic_matrix *simplex, size_t dim,
                      cleed_vector *fx,
                      cleed_real (*func)(const cleed_vector*))
{
  const cleed_vector *xi;
  for (size_t i = 0; i < dim + 1; i++) {
    xi = &simplex[i*dim];
    cleed_vector_set(fx, i, (*func)(xi));
  }
}

void simplex_extremes(const cleed_vector *fx, size_t dim,
                      size_t *ihi, size_t *ilo, size_t *inhi)
{
  size_t i;
  cleed_real fx_i;

  if (fx[0] > fx[1])
  {
    *ihi = 0;
    *ilo = *inhi = 1;
  }
  else
  {
    *ihi = 1;
    *ilo = *inhi = 0;
  }

  for (i = 2; i < dim + 1; i++)
  {
    fx_i = cleed_vector_get(fx, i);
    if ( fx_i <= cleed_vector_get(fx, *ilo))
    {
      *ilo = i;
    }
    else if (fx_i > cleed_vector_get(fx, *ihi))
    {
      *inhi = *ihi;
      *ihi = i;
    }
    else if (fx_i > cleed_vector_get(fx, *inhi))
    {
      *inhi = i;
    }
  }
}

void simplex_bearings(cleed_basic_matrix *simplex, size_t dim,
                      cleed_vector *midpoint, cleed_vector *line, size_t ihi)
{
  size_t i, j;

  for (j = 0; j < dim; j++) {
    cleed_vector_set(midpoint, j, 0.0);
  }

  for (i = 0; i < dim + 1; i++) {
    if (i != ihi) {
      for (j = 0; j < dim; j++) {
        cleed_vector_set(midpoint, j,
            cleed_vector_get(midpoint, j) +
            cleed_basic_matrix_get(simplex, i, j, dim));
      }
    }
  }

  for (j = 0; j < dim; j++) {
    cleed_vector_set(midpoint, j, cleed_vector_get(midpoint, j) / dim);
    cleed_vector_set(line, j, cleed_basic_matrix_get(simplex, ihi, j, dim) -
                                 cleed_vector_get(midpoint, j));
  }
}

int update_simplex(cleed_vector *point, size_t dim, cleed_real *fmax,
                   cleed_vector *midpoint, cleed_vector *line,
                   cleed_real scale, cleed_real (*func)(const cleed_vector *))
{
  int update = 0;
  size_t i;
  cleed_vector *next = cleed_vector_alloc(dim);
  cleed_real fx;

  for (i = 0; i < dim; i++) {
    cleed_vector_set(next, i,
        cleed_vector_get(midpoint, i) + scale*cleed_vector_get(line, i));
  }

  fx = (*func)(next);

  if (fx < *fmax) {
    for (i = 0; i < dim; i++) {
      cleed_vector_set(point, i, cleed_vector_get(next, i));
    }
    *fmax = fx;
    update = 1;
  }

  cleed_vector_free(next);
  return update;
}

void contract_simplex(cleed_basic_matrix *simplex, size_t dim,
                      cleed_vector *fx, size_t ilo,
                      cleed_real (*func)(const cleed_vector *))
{
  size_t i, j;
  for (i = 0; i < dim + 1; i++)
    if (i != ilo) {
      for (j = 0; j < dim; j++) {
        cleed_basic_matrix_set(simplex, i, j, dim,
            (cleed_basic_matrix_get(simplex, ilo, j, dim) +
             cleed_basic_matrix_get(simplex, i, j, dim))*0.5);
      }
      cleed_vector_set(fx, i, (*func)(&simplex[i*dim]));
    }
}

cleed_real amoeba(cleed_basic_matrix *simplex, cleed_vector *point,
                  size_t dim, cleed_real (*func)(const cleed_vector *),
                  size_t *nfunc, cleed_real ftol, size_t iter_max,
                  int (*callback)(const cleed_vector *,
                      const cleed_basic_matrix *, size_t))
{
  size_t ihi, ilo, inhi, j;
  cleed_real fmin;
  cleed_vector *fx = cleed_vector_alloc(dim + 1);
  cleed_vector *midpoint = cleed_vector_alloc(dim);
  cleed_vector *line = cleed_vector_alloc(dim);
  cleed_vector *xi = NULL;
  cleed_real *yi;
  /* cleed_basic_matrix *simplex = make_simplex(point, dim); */


  *nfunc = 0;

  evaluate_simplex(simplex, dim, fx, func);

  while (*nfunc < iter_max)
  {
    simplex_extremes(fx, dim, &ihi, &ilo, &inhi);
    simplex_bearings(simplex, dim, midpoint, line, ihi);

    xi = &simplex[ihi*dim];
    yi = &fx[ihi];

    if (check_tol(*yi, cleed_vector_get(fx, ilo), ftol)) {
      break;
    }

    update_simplex(xi, dim, yi, midpoint, line, -1.0, func);

    if (*yi < cleed_vector_get(fx, ilo)) {
      update_simplex(xi, dim, yi, midpoint, line, -2.0, func);
    }
    else if (*yi >= cleed_vector_get(fx, inhi)) {
      if (!update_simplex(xi, dim, yi, midpoint, line, 0.5, func)) {
        contract_simplex(simplex, dim, fx, ilo, func);
      }
    } /* else if */

    callback(xi, simplex, dim);

    *nfunc += 1;
  } /* while (true) */

  if (*nfunc == iter_max)
  {
    ERROR_MSG("Too many iterations in simplex search\n");
    exit(1);
  }

  /* update and finalize */
  for (j = 0; j < dim; j++) {
    cleed_vector_set(point, j, cleed_basic_matrix_get(simplex, ilo, j, dim));
  }
  fmin = cleed_vector_get(fx, ilo);

  cleed_vector_free(fx);
  cleed_vector_free(midpoint);
  cleed_vector_free(line);
  /* cleed_basic_matrix_free(simplex); */

  return fmin;
}

/* wrapper */
void sr_simplex(cleed_basic_matrix *p, cleed_vector *y, size_t n_dim,
                cleed_real ftol, cleed_real (*funk)(), size_t *nfunk)
{
  amoeba(p, y, n_dim, funk, nfunk, ftol, MAX_ITER_AMOEBA, sr_mkver);
}

