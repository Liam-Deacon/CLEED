/*********************************************************************
 * <FILENAME>
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

  void linmin_gsl(p, xi, fret, func)
  real f1dim(real x)

 Find minimum by simplex algorithm. 

 Changes:

GH/19.09.95 - BRENT_TOLERANCE defined in "search_def.h" (to 2.e-2 
              from 2.0e-4)

***********************************************************************/
#include "csearch.h"
#include <gsl/gsl_vector.h>

#ifndef BRENT_TOLERANCE         /* tolerance passed to function brent;
                                   should be defined in "search_def.h" */
#define BRENT_TOLERANCE 2.0e-2
#endif

/* 
   static variables communicating with 
   function f1dim 
*/
static int ncom=0;
static gsl_vector *pcom;
static gsl_vector *xicom;
static real (*nrfunc)();

void linmin_gsl(
    gsl_vector *p, 
    gsl_vector *xi, 
    real *fret, 
    real (*func)() 
    )
{
	int j;
	real xx, xmin, fx, fb, fa, bx, ax;
    size_t n;
    
	n = ncom = p->size;
    
    /* allocate memory for vectors */
    pcom = gsl_vector_alloc(n);
    xicom = gsl_vector_alloc(n);
    
    nrfunc = func;
	
    for (j=0; j < n; j++) 
    {
		gsl_vector_set(pcom, j, gsl_vector_get(p, j));
		gsl_vector_set(xicom, j, gsl_vector_get(xi, j);
	}
    
    /* bracket minimiser */
	ax = 0.0;
	xx = 1.0;
	bx = 2.0;
	mnbrak(&ax, &xx, &bx, &fa, &fx, &fb, f1dim);
	
    /* get root */
    *fret = brent(ax, xx, bx, f1dim, BRENT_TOLERANCE, &xmin);
    
    gsl_vector_scale(xi, xmin);
    gsl_vector_add(p, xi);
    
    /* free memory */
	gsl_vector_free(xicom);
	gsl_vector_free(pcom);
}

/***********************************************************************/

real f1dim(real x)
{
        int j;
        real fx;
        gsl_vector *xt;
        gsl_vector *xxicom;
        
        /* initialise vectors */
        gsl_vector_memcpy(xt, pcom);
        gsl_vector_memcpy(xxicom,, xicom);
        
        /* perform operations */
        gsl_vector_scale( xxicom, (double)x );
        gsl_vector_add( xt, xxicom );
            
        fx = (*nrfunc)(xt);
        
        /* free memory */
        gsl_vector_free(xt);
        gsl_vector_free(xxicom);
        
        return fx;
}

/***********************************************************************/
