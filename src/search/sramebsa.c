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

extern char *sr_project;   /* project name */
extern long sa_idum;       /* seed for ran1, defined and initialized in main */

static real tt;            /* communicates with amotsa */

static inline real fluct( long *i_ptr );

real amotsa(cleed_basic_matrix *p, cleed_vector *y, cleed_vector *psum, size_t n_dim,
            cleed_vector *pb, cleed_real *yb, cleed_real (*funk)(),
            size_t ihi, cleed_real *yhi, cleed_real fac);

void sr_amebsa(cleed_basic_matrix *p, cleed_vector *y, size_t n_dim,
    cleed_vector *pb, cleed_vector *yb, cleed_real ftol,
    cleed_real (*funk)(), size_t *iter, cleed_real temptr)
{
  FILE *ver_stream;
  char ver_file[STRSZ];

  size_t i, ilo, ihi;
  size_t j, m, n;
  size_t mpts = n_dim+1;
  real rtol, sum, swap;
  real yhi, ylo, ynhi, ysave, yt, ytry;
  cleed_vector *psum = CLEED_VECTOR_ALLOC(n_dim);

  char old_file[FILENAME_MAX];
  char new_file[FILENAME_MAX];
  time_t result;

  tt = -temptr;

  /* get sum of parameters */
  for (n=0; n < n_dim; n++)
  {
    for (sum=0.0, m=0; m < mpts; m++)
      sum += CLEED_BASIC_MATRIX_GET(p, m, n, mpts, n_dim);
    CLEED_VECTOR_SET(psum, n, sum);
  }

  /* LOOP */
  for (;;)
  {
    /* Determine which point is highest (worst), next-highest, & lowest (best).
     *
     * Whenever we look at a vertex, it gets a random thermal fluctuation. */
    ilo = 0;
    ihi = 1;
    ynhi = ylo = CLEED_VECTOR_GET(y, 0) + tt*fluct(&sa_idum);
    yhi = CLEED_VECTOR_GET(y, 1) + tt*fluct(&sa_idum);

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
      yt = CLEED_VECTOR_GET(y, i) + tt*fluct(&sa_idum);
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
      swap = CLEED_VECTOR_GET(y, 0);
      CLEED_VECTOR_SET(y, 0, CLEED_VECTOR_GET(y, ilo));
      CLEED_VECTOR_SET(y, ilo, swap);

      for(n=0; n < n_dim; n++)
      {
        swap = CLEED_BASIC_MATRIX_GET(p, 0, n, mpts, n_dim);
        CLEED_BASIC_MATRIX_SET(p, 0, n, mpts, n_dim,
          CLEED_BASIC_MATRIX_GET(p, ilo, n, mpts, n_dim));
        CLEED_BASIC_MATRIX_SET(p, ilo, n, mpts, n_dim, swap);
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
              CLEED_VECTOR_SET(psum, j, 0.5*(
                CLEED_BASIC_MATRIX_GET(p, i, j, mpts, n_dim) +
                CLEED_BASIC_MATRIX_GET(p, ilo, j, mpts, n_dim)) );
              CLEED_BASIC_MATRIX_SET(p, i, j, mpts, n_dim, CLEED_VECTOR_GET(psum, j));
            }
            CLEED_VECTOR_SET(y, i, (*funk)(psum));
          }
        } /* for i */

        *iter -= n_dim;

        /* Recompute psum */
        for (n=0; n < n_dim; n++)
        {
          for (sum=0.0, m=0; m < mpts; m++)
            sum += CLEED_BASIC_MATRIX_GET(p, m, n, mpts, n_dim);
          CLEED_VECTOR_SET(psum, n, sum);
        }
      } /* ytry >= ysave */
    } /* ytry >= ynhi */
    else
    {
      /* Correct the evaluation count */
      (*iter) ++;
    }

    /* Write y/p to backup file */

    /* remove 'cp' system call dependence */
    sprintf(old_file, "%s%s", sr_project, ".ver");
    sprintf(new_file, "%s%s", sr_project, ".vbk");
    if (copy_file(old_file, new_file) != 0)
    {
      ERROR_MSG("failed to copy file \"%s\" -> \"%s\"", old_file, new_file);
      exit(SR_FILE_IO_ERROR);
    }

    strcpy(ver_file, new_file);
    ver_stream = fopen(ver_file, "w");
    fprintf(ver_stream, "%d %d %s\n", n_dim, mpts, sr_project);
    for (i=0; i < mpts; i++)
    {
      fprintf(ver_stream, "%e ", y[i]);
      for(j=0; j < n_dim; j++)
        fprintf(ver_stream, "%e ", CLEED_BASIC_MATRIX_GET(p, i, j, mpts, n_dim));
      fprintf(ver_stream, "\n");
    }

    /* add date */
    result = time(NULL);
    fprintf(ver_stream, "%s\n", asctime(localtime(&result)));

    fclose(ver_stream);

  } /* end of BIG LOOP */

  CLEED_VECTOR_FREE(psum);

} /* end of function sr_amebsa */


real amotsa(cleed_basic_matrix *p, cleed_vector *y, cleed_vector *psum, size_t n_dim,
            cleed_vector *pb, cleed_real *yb, cleed_real (*funk)(),
            size_t ihi, cleed_real *yhi, cleed_real fac)
{

  size_t j;
  real fac1, fac2, yflu, ytry, faux, *ptry;

  ptry = CLEED_VECTOR_ALLOC(n_dim);
  fac1 = (1.0 - fac)/n_dim;
  fac2 = fac1 - fac;

  for (j=0; j < n_dim; j++)
    CLEED_VECTOR_SET(ptry, j, CLEED_VECTOR_GET(psum, j)*fac1 -
                     CLEED_BASIC_MATRIX_GET(p, ihi, j, n_dim+1, n_dim)*fac2);

  ytry = (*funk)(ptry);

  if (ytry < *yb)
  {
    /*
  Save the best ever
     */
    for(j=0; j < n_dim; j++)
      CLEED_VECTOR_SET(pb, j, CLEED_VECTOR_GET(ptry, j));
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
    CLEED_VECTOR_SET(y, ihi, ytry);
    *yhi = yflu;
    for(j=0; j < n_dim; j++)
    {
      faux = CLEED_VECTOR_GET(psum, j);
      CLEED_VECTOR_SET(psum, j, faux + CLEED_VECTOR_GET(ptry, j) -
                CLEED_BASIC_MATRIX_GET(p, ihi, j, n_dim+1, n_dim));
      CLEED_BASIC_MATRIX_SET(p, ihi, j, n_dim+1, n_dim, CLEED_VECTOR_GET(ptry, j));
    }
  }

  CLEED_VECTOR_FREE(ptry);
  return (ytry);

}  /* end of function amotsa */

/**************************************************************************/

static inline real fluct( long *i_ptr )
{
  return ( (real)R_log( ran1(i_ptr) ) );
}  /* end of function amotsa */

/**************************************************************************/
