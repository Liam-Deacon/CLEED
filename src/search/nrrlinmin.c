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
	pcom = CLEED_VECTOR_ALLOC(n);
	xicom = CLEED_VECTOR_ALLOC(n);
	nrfunc = func;

	for (j=0; j<n; j++)
	{
		CLEED_VECTOR_SET(pcom, j, CLEED_VECTOR_GET(p, j));
		CLEED_VECTOR_SET(xicom, j, CLEED_VECTOR_GET(xi, j));
	}

	ax = 0.0;
	xx = 1.0;
	bx = 2.0;
	mnbrak(&ax, &xx, &bx, &fa, &fx, &fb, f1dim);
	*fret = brent(ax, xx, bx, f1dim, BRENT_TOLERANCE, &xmin);

	for (j=0; j<n; j++)
	{
	  faux = CLEED_VECTOR_GET(xi, j);
		CLEED_VECTOR_SET(xi, j, faux*xmin);
		CLEED_VECTOR_SET(p, j, CLEED_VECTOR_GET(p, j) + faux);
	}

	CLEED_VECTOR_FREE(xicom);
	CLEED_VECTOR_FREE(pcom);
}

real f1dim(real x)
{
  size_t j;
  real f;
  cleed_vector *xt = CLEED_VECTOR_ALLOC(ncom);

  for (j=0; j<ncom; j++)
    CLEED_VECTOR_SET(xt, j,
        CLEED_VECTOR_GET(pcom, j) + x*CLEED_VECTOR_GET(xicom, j));

  f = (*nrfunc)(xt);

  CLEED_VECTOR_FREE(xt);
  return f;
}
