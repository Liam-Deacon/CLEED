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
static int ncom = 0;
static real *pcom = 0, *xicom = 0, (*nrfunc)();

void linmin(real *p, real *xi, int n, real *fret, real (*func)() )
{
	int j;
	real xx, xmin, fx, fb, fa, bx, ax;
	real *vector();

	ncom = n;
	pcom = vector(1, n);
	xicom = vector(1, n);
	nrfunc = func;

	for (j=1; j<=n; j++)
	{
		pcom[j]=p[j];
		xicom[j]=xi[j];
	}

	ax = 0.0;
	xx = 1.0;
	bx = 2.0;
	mnbrak(&ax, &xx, &bx, &fa, &fx, &fb, f1dim);
	*fret = brent(ax, xx, bx, f1dim, BRENT_TOLERANCE, &xmin);

	for (j=1; j<=n; j++)
	{
		xi[j] *= xmin;
		p[j] += xi[j];
	}

	free_vector(xicom,1);
	free_vector(pcom,1);
}

real f1dim(real x)
{
  int j;
  real f, *xt, *vector();

  xt = vector(1, ncom);
  for (j=1; j<=ncom; j++) xt[j] = pcom[j] + x*xicom[j];

  f = (*nrfunc)(xt);

  free_vector(xt,1);
  return f;
}
