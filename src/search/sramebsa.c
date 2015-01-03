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
 * Find minimum by simplex algorithm. (From Num. Rec.)
 */

#include <math.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>

#include "csearch.h"
#include "copy_file.h"

#ifndef MAX_ITER_AMOEBA        /* should be defined in "search_def.h" */
#define MAX_ITER_AMOEBA 500
#endif

extern char *sr_project;   /* project name */
extern long sa_idum;       /* seed for ran1, defined and initialized in main */

#define GET_PSUM \
for (n=1;n<=ndim;n++)                              \
{                                                  \
  for (sum=0.0, m=1; m<=mpts; m++) sum += p[m][n]; \
  psum[n]=sum;                                     \
}

static real tt;            /* communicates with amotsa */

real amotsa(real **, real *, real *, int , real *, real *,
            real (*)(), int , real *, real );

real fluct(long*);

void sr_amebsa(real **p, real *y, int ndim, real *pb, real *yb,
               real ftol, real (*funk)(), int *iter, real temptr)
{

  FILE *ver_stream;
  char ver_file[STRSZ];

  int i,ilo,ihi;
  int j, m, n;
  int mpts;
  real rtol, sum, swap;
  real yhi, ylo, ynhi, ysave, yt, ytry, *psum;

  char *old_file;
  char *new_file;
  time_t result;

  mpts = ndim+1;
  psum = vector(1,ndim);
  tt = -temptr;
  GET_PSUM


  /* LOOP */
  for (;;)
  {
    /* Determine which point is highest (worst), next-highest, & lowest (best).
     *
     * Whenever we look at a vertex, it gets a random thermal fluctuation. */
    ilo = 1;
    ihi = 2;
    ynhi = ylo = y[1] + tt*fluct(&sa_idum);
    yhi =        y[2] + tt*fluct(&sa_idum);

    if( ylo > yhi )
    {
      ihi = 1;
      ilo = 2;
      ynhi = yhi;
      yhi  = ylo;
      ylo  = ynhi;
    }

    /* Loop over the points in the simplex.
     * Add thermal fluctuations to each point */
    for (i=3; i<=mpts; i++)
    {
      yt = y[i] + tt*fluct(&sa_idum);
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
      swap = y[1];
      y[1] = y[ilo];
      y[ilo] = swap;
     
      for( n=1; n <= ndim; n++)
      {
        swap = p[1][n];
        p[1][n] = p[ilo][n];
        p[ilo][n] = swap;
      }
      break;
    }

    /* Begin a new iteration. First extrapolate by a factor -1 through the face
     * of the simplex across from the high point, i.e., reflect the simplex from
     * the high point. */
    *iter -= 2;

    ytry = amotsa(p, y, psum, ndim, pb, yb, funk, ihi, &yhi, -1.0);

    if( ytry <= ylo )
    {
      /* Result better than the best point, so try an additional
       * extrapolation by a factor of 2. */
      ytry = amotsa(p, y, psum, ndim, pb, yb, funk, ihi, &yhi, 2.0);
    }
    else if( ytry >= ynhi )
    {
      /* The reflected point is worse than the second highest point, so
       * look for an intermediate lower point, i.e. do a one-dimensional
       * contraction. */
      ysave = yhi;
      ytry = amotsa(p, y, psum, ndim, pb, yb, funk, ihi, &yhi, 0.5);

      if( ytry >= ysave)
      {
        /* Can't seem to get rid of that high point. Better contract around
         * the lowest (best) point. */
        for( i=1; i <= mpts; i++ )
        {
          if( i != ilo)
          {
            for (j=1; j<=ndim; j++)
            {
              psum[j]=0.5*(p[i][j] + p[ilo][j]);
              p[i][j]=psum[j];
            }

            y[i]=(*funk)(psum);
          }
        } /* for i */

        *iter -= ndim;
        GET_PSUM;                   /* Recompute psum */
      } /* ytry >= ysave */
    } /* ytry >= ynhi */
    else
    {
      /* Correct the evaluation count */
      (*iter) ++;
    }
   
    /* Write y/p to backup file */
    old_file = (char *) malloc(sizeof(char) * (strlen(sr_project)+5));
    new_file = (char *) malloc(sizeof(char) * (strlen(sr_project)+5));
   
    /* remove 'cp' system call dependence */
    strcpy(old_file, sr_project);
    strcpy(new_file, sr_project);
    strcat(old_file, ".ver");
    strcat(new_file, ".vbk");
    if (copy_file(old_file, new_file) != 0)
    {
      ERROR_MSG("failed to copy file \"%s\" -> \"%s\"", old_file, new_file);
      exit(SR_FILE_IO_ERROR);
    }

    strcpy(ver_file, new_file);
    ver_stream = fopen(ver_file,"w");
    fprintf(ver_stream,"%d %d %s\n",ndim, mpts, sr_project);
    for (i = 1; i<= mpts; i++)
    {
      fprintf(ver_stream,"%e ", y[i]);
      for(j=1; j<= ndim; j++) fprintf(ver_stream,"%e ", p[i][j]);
      fprintf(ver_stream,"\n");
    }
   
    /* add date */
    result = time(NULL);
    fprintf(ver_stream, "%s\n", asctime(localtime(&result)));
   
    fclose(ver_stream);

  } /* end of BIG LOOP */

  free_vector(psum,1);

} /* end of function sr_amebsa */


real amotsa(real **p, real *y, real *psum, int ndim, 
                   real *pb, real *yb,
                   real (*funk)(), int ihi, real *yhi, real fac)

/***************************************************************************
  Write y/p to backup file
***************************************************************************/
{

int j;
real fac1, fac2, yflu, ytry, *ptry;

 ptry=vector(1,ndim);
 fac1=(1.0-fac)/ndim;
 fac2=fac1-fac;

 for (j=1; j<=ndim; j++) 
   ptry[j]=psum[j]*fac1 - p[ihi][j]*fac2;

 ytry=(*funk)(ptry);

 if (ytry < *yb) 
 {
/*
  Save the best ever
*/
   for(j=1; j<=ndim; j++) 
     pb[j] = ptry[j];
   *yb = ytry;
 }

/***************************************************************************
  We added a thermal fluctuation to all the current vertices, but we
  subtract it here, so as to give the simplex a thermal "Brownian motion":
  it likes to accept any suggested change.
***************************************************************************/

 yflu = ytry - tt*fluct(&sa_idum);

 if(yflu < *yhi)
 {
   y[ihi] = ytry;
   *yhi = yflu;
   for(j=1; j<=ndim; j++)
   {
     psum[j] += ptry[j] - p[ihi][j];
     p[ihi][j] = ptry[j];
   }
 }

 free_vector(ptry,1);
 return ytry;

}  /* end of function amotsa */

/**************************************************************************/

real fluct( long *i_ptr )
{
real res;

  res = R_log( ran1(i_ptr) );
  return(res);
}  /* end of function amotsa */

/**************************************************************************/
