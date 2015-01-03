/*********************************************************************
 *                        SRPOWELL.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/19.09.95 - Copy from Num. Rec.
 *   GH/19.09.95 - Criterion of termination is absolute deviation in vertex
 *                 rather than relative deviation.
 ***********************************************************************/

#include <math.h>

#if USE_GSL
# include <gsl/gsl_vector.h>
#endif

#include "csearch.h"

#ifndef MAX_ITER_POWELL      /* should be defined in "search_def.h" */
#define MAX_ITER_POWELL 200
#endif

static real sqrarg;
#define SQR(a) (sqrarg=(a),sqrarg*sqrarg)

/*!
 *  Find minimum by using Powell's method (Num. Rec. chap. 10.5)
 *
 * @param[in,out] p Pointer to vector containing the initial starting point
 * p[1..n]. After completion, \p p returns the coordinates of the minimum.
 * @param[in,out] xi
 * @param n
 * @param ftol
 * @param[out] iter Number of iterations.
 * @param[out] fret Minimum function value found in the search.
 * @param[in] func Pointer to function to be minimised.
 * \see linmin()
 */
void sr_powell(real *p, real **xi, int n, 
               real ftol, int *iter, real *fret, 
               real (*func)() )

/***********************************************************************
  real **xi - (input, output) matrix [1..n][1..n] whose columns contain 
            the initial set of directions (usually the n unit vectors).
            After completion, xi returns the then-current direction set.
           
  int n - (input) dimension of the search

  real ftol - (input) convergence criterion the absolute tolerance in the
            function value such that failure to decrease by more than 
            this amount on one iteration signals completion.

  int *iter - (output)

  real *fret - (output)
  
  real (*func)() - (input)


DESIGN:

  For a description see Num. Rec. p. 412ff.

  - relative convergence criterion has been replaced by absolute value.

FUNCTION CALLS:

  linmin

RETURN VALUES:

 none

***********************************************************************/
{
	int i,ibig,j;
	real t,fptt,fp,del;
	real *pt,*ptt,*xit,*vector();

	pt=vector(1,n);
	ptt=vector(1,n);
	xit=vector(1,n);
	*fret=(*func)(p);

	/* Save the initial point */
	for (j=1;j<=n;j++) pt[j]=p[j];

	/* Start loop over iterations */
	for (*iter=1 ; ; (*iter)++)
	{
		fp = (*fret);
		ibig = 0;
		del = 0.0;  /* will be the biggest function decrease */

    /* In each direction, loop over all directions in the set.
     * - Copy the direction
     * - minimize along it (linmin)
     * - record if it was the largest decrease so far.
     */

		for (i=1;i<=n;i++) {
			for (j=1;j<=n;j++) xit[j]=xi[j][i];
			fptt=(*fret);
			linmin(p,xit,n,fret,func);
			if (fabs(fptt-(*fret)) > del) {
				del=fabs(fptt-(*fret));
				ibig=i;
			}
		}

		/* Check termination criterion*/
		if (2.0*fabs(fp-(*fret)) <= ftol) 
    {
			free_vector(xit, 1);
			free_vector(ptt, 1);
			free_vector(pt, 1);
      fprintf(STDCTR, "(sr_powell): ITERATION NO: %d, FRET = %f\n",
              *iter, *fret);
			return;
		}
		if (*iter == MAX_ITER_POWELL) 
		  nrerror("Too many iterations in routine POWELL");

    /* Construct the extrapolated point and the average direction moved.
     * Save the old starting point.
     */
		for (j=1;j<=n;j++)
		{
			ptt[j]=2.0*p[j]-pt[j];
			xit[j]=p[j]-pt[j];
			pt[j]=p[j];
		}

		/* function value at extrapolated point */
		fptt = (*func)(ptt);
		if (fptt < fp)
		{
			t = 2.0*(fp-2.0*(*fret)+fptt)*SQR(fp-(*fret)-del)-del*SQR(fp-fptt);
			if (t < 0.0)
			{
			  /* Move to the minimum of the new direction, and save the new direction */
				linmin(p,xit,n,fret,func);
				for (j=1; j<=n; j++) xi[j][ibig] = xit[j];
			}
		}
    fprintf(STDCTR,"(sr_powell): ITERATION NO: %d, FRET = %f\n", *iter, *fret);
	}
} /* end of function sr_powell */

#undef SQR
