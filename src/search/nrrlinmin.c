/*********************************************************************
 *                    NRRLINMIN.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *16.09.95
file contains functions:

  void linmin(p,xi,n,fret,func)
  real f1dim(real x)

 Find minimum by simplex algorithm. (From Num. Rec.)

 Changes:

NR/16.09.95 - Copy from Num. Rec.
GH/19.09.95 - BRENT_TOLERANCE defined in "search_def.h" (to 2.e-2
              from 2.0e-4)

***********************************************************************/
#include "csearch.h"
#include <errno.h>

/*
   static variables communicating with
   function f1dim
*/
static size_t ncom = 0;
static cleed_vector *pcom = NULL;
static cleed_vector *xicom = NULL;
static cleed_real (*nrfunc)();

void linmin(real *p, real *xi, size_t n, real *fret, real (*func)() )
{
  size_t j;
  real xx, xmin, fx, fb, fa, bx, ax, faux;

  ncom = n;
  pcom = cleed_vector_alloc(n);
  xicom = cleed_vector_alloc(n);
  nrfunc = func;

  if (pcom == NULL) {
    ERROR_MSG("could not allocate memory for pcom\n");
    exit(ENOMEM);
  }

  if (xicom == NULL) {
    ERROR_MSG("could not allocate memory for xicom\n");
    exit(ENOMEM);
  }

  for (j=0; j<n; j++)
  {
    cleed_vector_set(pcom, j, cleed_vector_get(p, j));
    cleed_vector_set(xicom, j, cleed_vector_get(xi, j));
  }

  ax = 0.0;
  xx = 1.0;
  bx = 2.0;
  mnbrak(&ax, &xx, &bx, &fa, &fx, &fb, f1dim);
  *fret = brent(ax, xx, bx, f1dim, BRENT_TOLERANCE, &xmin);

  for (j=0; j<n; j++)
  {
    faux = cleed_vector_get(xi, j);
    cleed_vector_set(xi, j, faux*xmin);
    cleed_vector_set(p, j, cleed_vector_get(p, j) + faux);
  }

  cleed_vector_free(xicom);
  cleed_vector_free(pcom);
}

real f1dim(real x)
{
  size_t j;
  real f;
  cleed_vector *xt = cleed_vector_alloc(ncom);

  if (xt == NULL) {
    ERROR_MSG("could not allocate memory for xt\n");
    exit(ENOMEM);
  }

  if (pcom == NULL || xicom == NULL)
  {
    ERROR_MSG("pcom and xicom must be allocated before use in this function\n");
    exit(EFAULT);
  }

  for (j=0; j<ncom; j++)
    cleed_vector_set(xt, j,
        cleed_vector_get(pcom, j) + x*cleed_vector_get(xicom, j));

  f = (*nrfunc)(xt);

  cleed_vector_free(xt);
  return f;
}
