/***********************************************************************
GH/20.09.95
file contains function:

  void sr_amebsa(real **p, real *y, int ndim, 
                 real *pb, real *yb, 
                 real ftol, real (*funk)(), int *iter, real temptr)

 Find minimum by simplex algorithm. (From Num. Rec.)

 Changes:

GH/20.09.95 - Copy vertex to *.ver after each iteration.
            - Criterion of termination is absolute deviation in vertex
              rather than relative deviation.
LD/30.04.14 - Removed dependence on 'cp' and 'date' system calls

***********************************************************************/
#include <math.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>
#include<gsl/gsl_matrix.h>
#include<gsl/gsl_vector.h>
#include<gsl/gsl_multimin.h>

#include "search.h"
#include "copy_file.h"

#ifndef MAX_ITER_AMOEBA        /* should be defined in "search_def.h" */
#define MAX_ITER_AMOEBA 500
#endif

extern char *sr_project;   /* project name */
extern long sa_idum;       /* seed for ran1, defined and initialized in main */

static real tt;            /* communicates with amotsa */

real amotsa(real **, real *, real *, int , real *, real *,
            real (*)(), int , real *, real );

real fluct(long*);


void sr_amebsa_gsl(
    gsl_matrix *p, 
    gsl_vector *y, 
    gsl_vector *pb, 
    gsl_vector *yb,
    real ftol, 
    real (*funk)(), 
    size_t *iter, 
    real temptr
    )

/**************************************************************************


**************************************************************************/
{

FILE *ver_stream;
char ver_file[STRSZ];

int i,ilo,ihi;
int j, m, n; 
int ndim = y->size;
int mpts = y->size + 1;
real rtol, sum, swap;
real yhi, ylo, ynhi, ysave, yt, ytry;

char *old_file;
char *new_file;
time_t result;

 gsl_vector *psum = gsl_vector_alloc(ndim);
 tt = -temptr;
 GET_PSUM


/**************************************************************************
  LOOP 
**************************************************************************/

 for (;;) 
 {
/**************************************************************************
  Determine which point is the highest (worst), next-highest, and
  lowest (best).

  Whenever we look at a vertex, it gets a random thermal fluctuation.
**************************************************************************/
   ilo = 0;
   ihi = 1;
   ynhi = ylo = gsl_vector_get(y, 0) + tt*fluct(&sa_idum);
   yhi = gsl_vector_get(y, 1) + tt*fluct(&sa_idum);
   if( ylo > yhi )
   {
     ihi = 0;
     ilo = 1;
     ynhi = yhi;
     yhi  = ylo;
     ylo  = ynhi;
   }

/* 
  Loop over the points in the simplex. add thermal fluctuations to each
  point.
*/
   for (i=2; i < mpts; i++) 
   {
     yt = gsl_vector_get(y, i) + tt*fluct(&sa_idum);
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

/**************************************************************************
  Compute range (absolute deviation) from highest to lowest and return 
  if satisfactory.
  (fractional range in original code)
**************************************************************************/

   rtol = 2.0*fabs(yhi - ylo);

   if( (rtol < ftol) || (*iter < 0) )
   {
/* If returning, put best value in slot 1 */
     swap = gsl_vector_get(y, 0);
     gsl_vector_set(y, 0, gsl_vector_get(y, ilo);
     gsl_vector_set(y, ilo, swap);
     
     for( n = 0; n < ndim; n++)
     {
       swap = gsl_matrix_get(p, 0, n);
       gsl_matrix_set(p, 0, n, gsl_matrix_get(p, ilo, n));
       gsl_matrix_set(p, ilo, n, swap);
     }
     break;
   }

/**************************************************************************
  Begin a new iteration. First extrapolate by a factor -1 through the face
  of the simplex across from the high point, i.e., reflect the simplex from
  the high point.
**************************************************************************/

   *iter -= 2;

   ytry = amotsa(p, y, psum, ndim, pb, yb, funk, ihi, &yhi, -1.0);

   if( ytry <= ylo )
   {
/* 
   Result better than the best point, so try an additional 
   extrapolation by a factor of 2.
*/
     ytry = amotsa(p, y, psum, ndim, pb, yb, funk, ihi, &yhi, 2.0);
   }
   else if( ytry >= ynhi )
   {
/* 
   The reflected point is worse than the second highest point, so 
   look for an intermediate lower point, i.e. do a one-dimensional
   contraction. 
*/
     ysave = yhi;
     ytry = amotsa(p, y, psum, ndim, pb, yb, funk, ihi, &yhi, 0.5);

     if( ytry >= ysave)
     {
/* 
   Can't seem to get rid of that high point. Better contract around
   the lowest (best) point.
*/
       for( i=1; i <= mpts; i++ )
       {
         if( i != ilo)
         {
           for (j=0; j < ndim; j++)
           { 
              gsl_vector_set(psum, j, 0.5*(gsl_matrix_get(p, i, j) 
                                        + gsl_matrix_get(p, ilo, j)));
              gsl_matrix_set(p, i, j, gsl_vector_get(psum, j)); 
            }

           gsl_vector_set(y, i) = (*funk)(psum);
         }
       } /* for i */

       *iter -= ndim;
       for (n=0; n < ndim; n++)
       {
         for (sum=0.0, m=0; m < mpts; m++) 
            sum += gsl_matrix_get(p, m, n);
         gsl_vector_get(psum, n, sum);                                
       }
     }  /* ytry >= ysave */
   }  /* ytry >= ynhi */
   else
   {
/*
   Correct the evaluation count
*/
     (*iter) ++;
   }
   
/***************************************************************************
  Write y/p to backup file
***************************************************************************/
   old_file = (char *) malloc(sizeof(char) * (strlen(sr_project)+5));
   new_file = (char *) malloc(sizeof(char) * (strlen(sr_project)+5));
   
   /* remove 'cp' system call dependence */
   strcpy(old_file, sr_project);
   strcpy(new_file, sr_project);
   strcat(old_file, ".ver");
   strcat(new_file, ".vbk");
   if (copy_file(old_file, new_file)) 
   {
     fprintf(STDERR, "*** error (sramebsa_gsl): "
        "failed to copy file \"%s\" -> \"%s\"", old_file, new_file);
     exit(1);
   }

   strcpy(ver_file, new_file);
   ver_stream = fopen(ver_file, "w");
   fprintf(ver_stream,"%d %d %s\n", ndim, mpts, sr_project);
   for (i=0; i < mpts; i++) 
   {
     fprintf(ver_stream,"%e ", gsl_vector_get(y, i));
     for(j=0; j < ndim; j++) 
       fprintf(ver_stream,"%e ", gsl_matrix_get(p, i, j));
     fprintf(ver_stream,"\n");
   }
   
   /* add date */
   result = time(NULL);
   fprintf(ver_stream, "%s\n", asctime(localtime(&result)));
   
   fclose(ver_stream);

 }  /* end of BIG LOOP */

 gsl_vector_free(psum);

}  /* end of function sr_amebsa */

/**************************************************************************/

real amotsa(
    gsl_matrix *p, 
    gsl_vector *y, 
    gsl_vector *psum, 
    gsl_vector *pb, 
    real *yb,
    real (*funk)(), 
    int ihi, 
    real *yhi, 
    real fac
    )

/***************************************************************************
  Write y/p to backup file
***************************************************************************/
{

int j;
int ndim = y->size;
real fac1, fac2, yflu, ytry;
real faux, faux2;

 gsl_vector *ptry = gsl_vector_alloc(ndim);
 fac1 = (1.0-fac)/ndim;
 fac2 = fac1-fac;

 for (j=0; j < ndim; j++) 
   gsl_vector_set(ptry, j, gsl_vector_get(psum, j)*fac1 
                                - gsl_matrix_get(p, ihi, j)*fac2);

 ytry = (*funk)(ptry);

 if (ytry < *yb) 
 {
/*
  Save the best ever
*/
   for(j=0; j < ndim; j++) 
     gsl_vector_set(pb, j, gsl_vector_get(ptry, j);
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
   gsl_vector_set(y, ihi, ytry);
   *yhi = yflu;
   for(j = 0; j < ndim; j++)
   {
     faux = gsl_vector_get(psum, j);
     faux2 = gsl_vector_get(ptry, j);
     gsl_vector_get(psum, j, faux + faux2 - gsl_matrix_get(p, ihi, j);
     gsl_matrix_set(p, ihi, j, faux2);
   }
 }

 gsl_vector_free(ptry);
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
