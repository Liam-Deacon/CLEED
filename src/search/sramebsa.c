/*********************************************************************
 *                        SRAMEBSA.C
 *
 * Changes:
 *   NR/20.09.95 - Copy from Num. Rec. (type in from book !!!)
 *   GH/20.09.95 - Copy vertex to *.ver after each iteration.
 *               - Criterion of termination is absolute deviation in
 *                 vertex rather than relative deviation.
 *   LD/30.04.14 - Removed dependence on 'cp' and 'date' system calls
 *********************************************************************/

/*!
 * Finds the minimum by simulated annealing with simplex algorithm. (From Num. Rec.)
 */

#include <math.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>

#include "csearch.h"
#include "copy_file.h"

extern long sa_idum;       /* seed for ran1, defined and initialized in main */

static real tt;            /* communicates with amotsa */

static inline real fluct( long *i_ptr );

real amotsa(cleed_basic_matrix *p, cleed_vector *y, cleed_vector *psum, size_t n_dim,
            cleed_vector *pb, cleed_real *yb, cleed_real (*funk)(),
            size_t ihi, cleed_real *yhi, cleed_real fac);

void sr_amebsa(cleed_basic_matrix *p, cleed_vector *y, size_t n_dim,
    cleed_vector *pb, cleed_vector *yb, cleed_real ftol,
    cleed_real (*funk)(), int *iter, cleed_real temptr)
{
  size_t i, ilo, ihi;
  size_t j, m, n;
  size_t mpts = n_dim+1;
  real rtol, sum, swap;
  real yhi, ylo, ynhi, ysave, yt, ytry;
  cleed_vector *psum = cleed_vector_alloc(n_dim);

  tt = -temptr;

  /* get sum of parameters */
  for (n=0; n < n_dim; n++)
  {
    for (sum=0.0, m=0; m < mpts; m++)
      sum += cleed_basic_matrix_get(p, m, n, n_dim);
    cleed_vector_set(psum, n, sum);
  }

  /* LOOP */
  for (;;)
  {
    /* Determine which point is highest (worst), next-highest, & lowest (best).
     *
     * Whenever we look at a vertex, it gets a random thermal fluctuation. */
    ilo = 0;
    ihi = 1;
    ynhi = ylo = cleed_vector_get(y, 0) + tt*fluct(&sa_idum);
    yhi = cleed_vector_get(y, 1) + tt*fluct(&sa_idum);

    if( ylo > yhi )
    {
      ihi = 0;
      ilo = 1;
      ynhi = yhi;
      yhi  = ylo;
      ylo  = ynhi;
    }

    /* Loop over the points in the simplex.
     * Add thermal fluctuations to each point */
    for (i=2; i < mpts; i++)
    {
      yt = cleed_vector_get(y, i) + tt*fluct(&sa_idum);
      if( yt <= ylo )
      {
        ilo = i;
        ylo = yt;
      }
      if( yt > yhi )
      {
        ynhi = yhi;
        ihi = i;
        yhi = yt;
      }
      else if( yt > ynhi )
      {
        ynhi = yt;
      }
    }

    /* Compute range (absolute deviation) from highest to lowest and return
     *  if satisfactory. (fractional range in original code) */

    rtol = 2.0*fabs(yhi - ylo);

    if( (rtol < ftol) || (*iter < 0) )
    {
      /* If returning, put best value in slot 1 */
      swap = cleed_vector_get(y, 0);
      cleed_vector_set(y, 0, cleed_vector_get(y, ilo));
      cleed_vector_set(y, ilo, swap);

      for(n=0; n < n_dim; n++)
      {
        swap = cleed_basic_matrix_get(p, 0, n, n_dim);
        cleed_basic_matrix_set(p, 0, n, n_dim,
            cleed_basic_matrix_get(p, ilo, n, n_dim));
        cleed_basic_matrix_set(p, ilo, n, n_dim, swap);
      }
      break;
    }

    /* Begin a new iteration. First extrapolate by a factor -1 through the face
     * of the simplex across from the high point, i.e., reflect the simplex from
     * the high point. */
    *iter -= 2;

    ytry = amotsa(p, y, psum, n_dim, pb, yb, funk, ihi, &yhi, -1.0);

    if( ytry <= ylo )
    {
      /* Result better than the best point, so try an additional
       * extrapolation by a factor of 2. */
      ytry = amotsa(p, y, psum, n_dim, pb, yb, funk, ihi, &yhi, 2.0);
    }
    else if( ytry >= ynhi )
    {
      /* The reflected point is worse than the second highest point, so
       * look for an intermediate lower point, i.e. do a one-dimensional
       * contraction. */
      ysave = yhi;
      ytry = amotsa(p, y, psum, n_dim, pb, yb, funk, ihi, &yhi, 0.5);

      if( ytry >= ysave)
      {
        /* Can't seem to get rid of that high point. Better contract around
         * the lowest (best) point. */
        for(i=0; i < mpts; i++ )
        {
          if( i != ilo)
          {
            for (j=0; j < n_dim; j++)
            {
              cleed_vector_set(psum, j, 0.5*(
                cleed_basic_matrix_get(p, i, j, n_dim) +
                cleed_basic_matrix_get(p, ilo, j, n_dim)) );
              cleed_basic_matrix_set(p, i, j, n_dim, cleed_vector_get(psum, j));
            }
            cleed_vector_set(y, i, (*funk)(psum));
          }
        } /* for i */

        *iter -= (int)n_dim;

        /* Recompute psum */
        for (n=0; n < n_dim; n++)
        {
          for (sum=0.0, m=0; m < mpts; m++)
            sum += cleed_basic_matrix_get(p, m, n, n_dim);
          cleed_vector_set(psum, n, sum);
        }
      } /* ytry >= ysave */
    } /* ytry >= ynhi */
    else
    {
      /* Correct the evaluation count */
      (*iter) ++;
    }

    /* Write y/p to backup file */
    sr_mkver(y, p, n_dim);

  } /* end of BIG LOOP */

  cleed_vector_free(psum);

} /* end of function sr_amebsa */


real amotsa(cleed_basic_matrix *p, cleed_vector *y, cleed_vector *psum, size_t n_dim,
            cleed_vector *pb, cleed_real *yb, cleed_real (*funk)(),
            size_t ihi, cleed_real *yhi, cleed_real fac)
{

  size_t j;
  real fac1, fac2, yflu, ytry, faux, *ptry;

  ptry = cleed_vector_alloc(n_dim);
  fac1 = (1.0 - fac)/n_dim;
  fac2 = fac1 - fac;

  for (j=0; j < n_dim; j++)
    cleed_vector_set(ptry, j, cleed_vector_get(psum, j)*fac1 -
                     cleed_basic_matrix_get(p, ihi, j, n_dim)*fac2);

  ytry = (*funk)(ptry);

  if (ytry < *yb)
  {
    /*
  Save the best ever
     */
    for(j=0; j < n_dim; j++)
      cleed_vector_set(pb, j, cleed_vector_get(ptry, j));
    *yb = ytry;
  }

  /***************************************************************************
   * We added a thermal fluctuation to all the current vertices, but we
   * subtract it here, so as to give the simplex a thermal "Brownian motion":
   * it likes to accept any suggested change.
   ***************************************************************************/

  yflu = ytry - tt*fluct(&sa_idum);

  if(yflu < *yhi)
  {
    cleed_vector_set(y, ihi, ytry);
    *yhi = yflu;
    for(j=0; j < n_dim; j++)
    {
      faux = cleed_vector_get(psum, j);
      cleed_vector_set(psum, j, faux + cleed_vector_get(ptry, j) -
                cleed_basic_matrix_get(p, ihi, j, n_dim));
      cleed_basic_matrix_set(p, ihi, j, n_dim, cleed_vector_get(ptry, j));
    }
  }

  cleed_vector_free(ptry);
  return (ytry);

}  /* end of function amotsa */

/**************************************************************************/

static inline real fluct( long *i_ptr )
{
  return ( (real)cleed_real_log( ran1(i_ptr) ) );
}  /* end of function amotsa */

/**************************************************************************/
