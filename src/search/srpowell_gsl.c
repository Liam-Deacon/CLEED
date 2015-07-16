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

  void sr_powell(real *p,real **xi, int n,
               real ftol, int *iter, real *fret,
               real (*func)() )

 Find minimum by Powell's method. (From Num. Rec.)

 Changes:

GH/19.09.95 - Copy from Num. Rec.
GH/19.09.95 - Criterion of termination is absolute deviation in vertex
              rather than relative deviation.

***********************************************************************/
#include <math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multiroots.h>

#include "search.h"

int powell (const gsl_vector *x, void *params, gsl_vector *f)
{
   /*!TODO: this is where the code needs to evaluate the current set 
            of parameters... PLEASE HELP! */
   return GSL_SUCCESS;
}

int sr_powell_gsl(gsl_vector *x, gsl_matrix *M, real epsilon, 
                  size_t *iter, real *fret)
/***********************************************************************
 Find minimum by using Powell's method (GSL methodology)

INPUT:

  gsl_vector *p - (input, output) initial starting point p[0..n-1]. After 
            completion, p returns the coordinates of the minimum.

  gsl_matrix *xi - (input, output) matrix [0..n-1][0..n-1] whose columns contain 
            the initial set of directions (usually the n unit vectors).
            After completion, xi returns the then-current direction set.

  real ftol - (input) convergence criterion the absolute tolerance in the
            function value such that failure to decrease by more than 
            this amount on one iteration signals completion.

  int *iter - (output) number of iterations.

  real *fret - (output) minimum function value found in the search.

DESIGN:

  - relative convergence criterion has been replaced by absolute value.

FUNCTION CALLS:

  ????

RETURN VALUES:

 status - GSL code for solver.

***********************************************************************/
{
    double x_root;

    int status = GSL_CONTINUE;
    size_t n = x->size;

    *fret = (*f)(x);

    const gsl_multiroot_fsolver_type *T = gsl_multiroot_fsolver_hybrid;
    gsl_multiroot_fsolver *solver = gsl_multiroot_fsolver_alloc(T, n);
    gsl_multiroot_function F;
    
    /* setup solver */
    F.f = &powell;
    F.params = (void *) NULL;
    F.n = n;
    
    gsl_multiroot_fsolver_set(solver, &F, x);    
    
    /* iterate solver to find solution */
    for (*iter = 0;
         *iter < MAX_ITER_POWELL && status == GSL_CONTINUE;
         *iter += 1)
    {
        /* Iterate one step of the solver */
        status = gsl_multiroot_fsolver_iterate(solver);

        /* Get the new approximate solution */
        x_root = gsl_multiroot_fsolver_root(solver);

        *fret = f (fgsl_multiroot_fsolver_f(solver));
        CONTROL_MSG(CONTROL, "ITERATION NO: %d, FRET = %f\n", *iter, *fret);
        
        /* Check to see if the solution is within epsilon */
        status = gsl_multiroot_test_residual(solver->f, (double) epsilon);
        
    }
    
    /* Free the solver */
    gsl_multiroot_fsolver_free(solver);

    if (status == GSL_CONTINUE) {
        ERROR_MSG("too many iterations\n");
    } else if (status != GSL_SUCCESS) {
        ERROR_MSG("%s\n", gsl_strerror(status));
    }
    
    return (status);
} /* end of function sr_powell_gsl */

/**********************************************************************/
