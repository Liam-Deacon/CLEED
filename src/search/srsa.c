/***********************************************************************
GH/29.12.95
 File contains:

  sr_sa(int ndim, char *bak_file, char *log_file)
 Perform a search according to the SIMULATED ANNEALING (SIMPLEX) METHOD
 Driver for routine sr_amebsa (From Numerical Recipes)

Changes
GH/20.09.95 - Creation (copy from srsx.c)
GH/29.12.95 - insert dpos in parameter list: initial displacement
              can be specified through a command line option.

***********************************************************************/

#include <stdio.h>
#include <strings.h>
#include <math.h>
#include <stdlib.h>
#include "search.h"

#define START_TEMP     3.5
#define EPSILON        0.25
#define ALPHA          4
#define MAX_ITER_SA  200
#define ITER_SA      100

/**********************************************************************/

long sa_idum = -1;                /* seed for random number generator */

void sr_sa(int ndim, real dpos, char *bak_file, char *log_file)

/***********************************************************************
  SIMULATED ANNEALING

***********************************************************************/
{

int i_par, j_par;
int mpar, nfunc;

real temp, rmin;
real *x,*y,**p;

FILE *log_stream;

/***********************************************************************
  SIMULATED ANNEALING (SIMPLEX METHOD)
***********************************************************************/

 if( (log_stream = fopen(log_file, "a")) == NULL) { OPEN_ERROR(log_file); }
 fprintf(log_stream,"=> SIMULATED ANNEALING:\n\n");

/***********************************************************************
  Set up vertex if no vertex file was specified, read vertex otherwise.
***********************************************************************/

 mpar = ndim + 1;

 x = vector(1, ndim);
 y = vector(1, mpar);
 p = matrix(1, mpar,1, ndim);

 if(strncmp(bak_file, "---", 3) == 0)
 {
   fprintf(log_stream,"=> Set up vertex:\n");
   fclose(log_stream);

   for(i_par = 1; i_par <= ndim; i_par ++)
   {
     p[1][i_par] = 0.;
   }

   for (i_par = 1; i_par <= mpar; i_par ++) 
   {
     for (j_par =1;j_par <= ndim;j_par ++) 
     {
       if(i_par == (j_par+1))
       {
         x[j_par]=p[i_par][j_par] = p[1][j_par] + dpos;
       }
       else
         x[j_par]=p[i_par][j_par] = p[1][j_par];
     }

#ifdef CONTROL
     fprintf(STDCTR,"(sr_sa): Calculate function for vertex(%d)\n", i_par);
#endif

     y[i_par] = sr_evalrf(x);
   }
 }
 else
 {
   fprintf(log_stream,"=> Read vertex from \"%s\":\n", bak_file);
   sr_rdver(bak_file, y, p, ndim);
   fclose(log_stream);
 }

/***********************************************************************
  Enter temperature loop
***********************************************************************/

#ifdef CONTROL
   fprintf(STDCTR,"(sr_sa): Enter temperature loop\n");
#endif

 if( (log_stream = fopen(log_file, "a")) == NULL) { OPEN_ERROR(log_file); }
 fprintf(log_stream,"=> Start search (abs. tolerance = %.3e)\n", R_TOLERANCE);
 fclose(log_stream);

 rmin = 100.;
 nfunc = -1;
 for(temp = START_TEMP; temp > R_TOLERANCE; temp *= (1. - EPSILON) )
 {
#ifdef CONTROL
   fprintf(STDCTR,"(sr_sa): temperature = %.4f\n", temp);
#endif
   nfunc = MAX_ITER_SA;
   sr_amebsa(p, y, ndim, x, &rmin, temp, sr_evalrf, &nfunc, temp);
 }

/***********************************************************************
  Write final results to log file
***********************************************************************/

#ifdef CONTROL
   fprintf(STDCTR,"(sr_sa): %d function evaluations in sr_amebsa\n", 
           nfunc);
#endif

 if( (log_stream = fopen(log_file, "a")) == NULL) { OPEN_ERROR(log_file); }

 fprintf(log_stream,"\n=> No. of iterations in sr_powell: %3d\n", nfunc);
 fprintf(log_stream,"=> Optimum parameter set and function value:\n");

 for (j_par = 1; j_par <= ndim; j_par++ )
   fprintf(log_stream, "%.6f ", x[j_par]);
 fprintf(log_stream,"\nrmin = %.6f\n", rmin);

 fclose(log_stream);

 free_matrix(p,1, mpar,1);
 free_vector(y,1);
 free_vector(x,1);

} /* end of function sr_sa */

/***********************************************************************/

