/***********************************************************************
GH/29.12.95
 File contains:

  sr_sa(int ndim, char *bak_file, char *log_file)
 Perform a search according to the SIMULATED ANNEALING (SIMPLEX) METHOD
 Driver for routine sr_amebsa

Changes
GH/20.09.95 - Creation (copy from srsx.c)
GH/29.12.95 - insert dpos in parameter list: initial displacement
              can be specified through a command line option.

***********************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "search.h"
#include "sr_alloc.h"

#define START_TEMP     3.5
#define EPSILON        0.25
#define ALPHA          4
#define MAX_ITER_SA  200
#define ITER_SA      100

/**********************************************************************/

long sa_idum = -1;                /* seed for random number generator */

void sr_sa(int ndim, real dpos, const char *bak_file, const char *log_file)

/***********************************************************************
  SIMULATED ANNEALING

***********************************************************************/
{

int i_par, j_par;
int mpar;
int nfunc = 0;

real temp, rmin;
real *x = NULL, *y = NULL, **p = NULL;

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

 x = sr_alloc_vector((size_t)ndim);
 y = sr_alloc_vector((size_t)mpar);
 p = sr_alloc_matrix((size_t)mpar, (size_t)ndim);
 if (x == NULL || y == NULL || p == NULL)
 {
   sr_free_vector(x);
   sr_free_vector(y);
   sr_free_matrix(p);
   fprintf(STDERR, "*** error (sr_sa): allocation failure\n");
   exit(1);
 }

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
 nfunc = 0;
 for(temp = START_TEMP; temp > R_TOLERANCE; temp *= (1. - EPSILON) )
 {
#ifdef CONTROL
   fprintf(STDCTR,"(sr_sa): temperature = %.4f\n", temp);
#endif
   int budget = MAX_ITER_SA;
   sr_amebsa_cfg cfg;
   cfg.ftol = temp;
   cfg.funk = sr_evalrf;
   cfg.temptr = temp;
   (void)sr_amebsa(p, y, ndim, x, &rmin, &cfg, &budget);
   nfunc += budget;
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

 sr_free_matrix(p);
 sr_free_vector(y);
 sr_free_vector(x);

} /* end of function sr_sa */

/***********************************************************************/
