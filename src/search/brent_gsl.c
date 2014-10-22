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
 *19.09.95

file contains function:

  real brent(real ax, real bx, real cx, real(*f)(real), real tol, real *xmin)

 Find minimum in one dimension (Brent's method)

 Changes:

GH/19.09.95 - Criterion of termination is absolute deviation in vertex
              rather than relative deviation.

***********************************************************************/

#include <math.h>
#include "csearch.h"

#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>

#ifndef MAX_ITERATIONS
#define MAX_ITERATIONS 100
#endif

/***********************************************************************
wrapper for GSL root finder using Brent's method
***********************************************************************/
double sr_brent_eval(double x, void *params)
{
    /*!TODO: fix this casting of object pointer to function pointer
             as it may cause undefined behaviour */
    real (*f)() = (real*) params;    
    return (double) f(x);
}

real brent(
    real ax,            /* first bracketing abscissa */
    real bx,            /* bracketing abscissa where bx is between ax & cx */
    real cx,            /* third bracketing abscissa */
    real(*f)(),         /* function to evaluate */
    real epsilon,       /* tolerance for convergence criterion */
    real *xmin          /* abscissa of the minimum */
    )
{
    size_t iter=0;
    const gsl_root_fsolver_type *solver_type = gsl_root_fsolver_brent;
    gsl_root_fsolver *solver =  gsl_root_fsolver_alloc(solver_type);
    gsl_function F;
    double x_lo = (double) ax;
    double x = (double) bx;
    double x_hi = (double) cx;
    
    int status = GSL_CONTINUE;
    
    /* setup solver */
    F.function = &sr_brent_eval;
    F.params = (void *)f;
    
    gsl_root_fsolver_set(solver, &F, x_lo, x_hi);    
    
    /* iterate solver to find solution */
    for (iter = 0; iter < MAX_ITERATIONS && status == GSL_CONTINUE; ++iter) 
    {
        /* Iterate one step of the solver */
        status = gsl_root_fsolver_iterate(solver);

        /* Get the new approximate solution */
        x = gsl_root_fsolver_root(solver);
        x_lo = gsl_root_fsolver_x_lower(solver);
        x_hi = gsl_root_fsolver_x_upper(solver);

        /* Check to see if the solution is within epsilon */
        status = gsl_root_test_interval(x_lo, x_hi, 0, (double) epsilon);
        
    }
    
    
    /* Free the solver */
    gsl_root_fsolver_free(solver);

    if (status == GSL_CONTINUE) {
        fprintf(STDERR, "***error (brent): too many iterations\n");
    } else if (status != GSL_SUCCESS) {
        fprintf(STDERR, "***error (brent): %s\n", gsl_strerror(status));
    }
    
    *xmin = x;
    
    return f(x);
}
