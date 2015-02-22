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

/*!
 * \note The relative convergence criterion has been replaced by absolute value.
 */

#include <math.h>

#include "csearch.h"

/*!
 *  Find minimum by using Powell's method (Num. Rec. chap. 10.5).
 *  For a description see Num. Rec. p. 412ff.
 *
 * \param[in,out] p Pointer to vector containing the initial starting point
 * p[1..n]. After completion, \p p returns the coordinates of the minimum.
 * \param[in,out] xi \p n \times \p n matrix whose columns contain the initial set
 * of directions (usually \p n unit vectors). After completion, \p xi returns
 * the then-current direction set.
 * \param n Dimension of the search.
 * \param[in] ftol Convergence criterion the absolute tolerance in the
 * function value such that failure to decrease by more than  this amount on
 * one iteration signals completion.
 * \param[out] iter Number of iterations.
 * \param[out] fret Minimum function value found in the search.
 * \param[in] func Pointer to function to be minimised.
 * \see linmin()
 */
void sr_powell(cleed_vector *p, cleed_basic_matrix *xi, size_t n,
               cleed_real ftol, size_t *iter, cleed_real *fret,
               cleed_real (*func)() )
{
	size_t i, ibig, j;
	real t, fptt, fp, del;

	cleed_vector *pt = cleed_vector_alloc(n);
	cleed_vector *ptt = cleed_vector_alloc(n);
	cleed_vector *xit = cleed_vector_alloc(n);

	*fret = (*func)(p);

	/* Save the initial point */
	for (j=0; j < n; j++) cleed_vector_set(pt, j, cleed_vector_get(p,j));

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

		for (i=0; i<n; i++)
		{
			for (j=0; j<n; j++)
      {
			  cleed_vector_set(xit, j, cleed_basic_matrix_get(xi, j, i, n));
      }
			fptt = (*fret);
			linmin(p, xit, n, fret, func);
			if (fabs( fptt - (*fret) ) > del)
			{
				del = fabs( fptt - (*fret) );
				ibig = i;
			}
		}

		/* Check termination criterion*/
		if (2.0*fabs( fp - (*fret) ) <= ftol)
    {
			CONTROL_MSG(CONTROL, "ITERATION NO: %u, FRET = %f\n", *iter, *fret);
      cleed_vector_free(pt);
      cleed_vector_free(ptt);
      cleed_vector_free(xit);
			return;
		}
		if (*iter == MAX_ITER_POWELL) 
		{
		  ERROR_MSG("Too many iterations in routine POWELL\n");
		  cleed_vector_free(pt);
      cleed_vector_free(ptt);
      cleed_vector_free(xit);
		  exit(1);
		}

    /* Construct the extrapolated point and the average direction moved.
     * Save the old starting point.
     */
		for (j=0; j<n; j++)
		{
			cleed_vector_set(ptt, j, 2.0*cleed_vector_get(p, j) - cleed_vector_get(pt, j));
			cleed_vector_set(xit, j, cleed_vector_get(p, j) - cleed_vector_get(pt, j));
			cleed_vector_set(pt, j, cleed_vector_get(p, j));
		}

		/* function value at extrapolated point */
		fptt = (*func)(ptt);
		if (fptt < fp)
		{
			t = 2.0*(fp - 2.0*(*fret) + fptt)*pow((fp - (*fret) - del), 2) -
			    del*pow((fp - fptt), 2);
			if (t < 0.0)
			{
			  /* Move to the minimum of the new direction, and save the new direction */
				linmin(p, xit, n, fret, func);
				for (j=0; j<n; j++)
        {
				  cleed_basic_matrix_set( xi, j, ibig, n, cleed_vector_get(xit, j) );
        }
			}
		}
    CONTROL_MSG(CONTROL, "ITERATION NO: %u, FRET = %f\n", *iter, *fret);
	}
} /* end of function sr_powell */
