/***********************************************************************
GH/29.12.95
 File contains:

  sr_sx(int ndim, real dpos, char *bak_file, char *log_file)
 Perform a search according to the SIMPLEX METHOD
 Driver for routine AMOEBA (From numerical recipes)

 Modified:
GH/23.08.95
GH/29.12.95 - insert dpos in parameter list: initial displacement
              can be specified through a command line option.

***********************************************************************/

#include <stdio.h>
#include <strings.h>
#include <math.h>
#include <stdlib.h>
#include "cleed_cstring.h"
#include "search.h"

/**********************************************************************/

void sr_sx(int ndim, real dpos, char *bak_file, char *log_file)

/***********************************************************************
  SIMPLEX METHOD
***********************************************************************/
{

int i_par, j_par;
int mpar, nfunc;

real *x,*y,**p;

FILE *log_stream;

/***********************************************************************
  SIMPLEX METHOD
***********************************************************************/

 if( (log_stream = fopen(log_file, "a")) == NULL) { OPEN_ERROR(log_file); }
 fprintf(log_stream,"=> SIMPLEX SEARCH:\n\n");

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
     for (j_par =1; j_par <= ndim; j_par ++) 
     {
       if(i_par == (j_par+1))
       {
         x[j_par] = p[i_par][j_par] = p[1][j_par] + dpos;
       }
       else
         x[j_par] = p[i_par][j_par] = p[1][j_par];
     }

#ifdef CONTROL
     fprintf(STDCTR,"(sr_sx): Calculate function for vertex(%d)\n", i_par);
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
  Enter amoeba
***********************************************************************/

#ifdef CONTROL
   fprintf(STDCTR,"(sr_sx): Enter amoeba\n");
#endif

 if( (log_stream = fopen(log_file, "a")) == NULL) { OPEN_ERROR(log_file); }
 fprintf(log_stream,"=> Start search (abs. tolerance = %.3e)\n", R_TOLERANCE);
 fclose(log_stream);

 sr_amoeba(p,y,ndim,R_TOLERANCE,sr_evalrf,&nfunc);

/***********************************************************************
  Write final results to log file
***********************************************************************/

#ifdef CONTROL
   fprintf(STDCTR,"(sr_sx): %d function evaluations in sr_amoeba\n", 
           nfunc);
#endif

 if( (log_stream = fopen(log_file, "a")) == NULL) { OPEN_ERROR(log_file); }

 fprintf(log_stream,"\n=> No. of function evaluations in sr_amoeba: %3d\n",
         nfunc);
 fprintf(log_stream,"=> Vertices and function values of final 3-d simplex:\n");
 
/* print 1st line */

 fprintf(log_stream,"%3d:",1);
 for (j_par = 1; j_par <= ndim; j_par++ ) 
   fprintf(log_stream, "%7.4f ", p[1][j_par]);
 fprintf(log_stream,"%7.4f\n", y[1]);

/* print other lines */
 for (i_par = 2; i_par <= mpar; i_par++ ) 
 {
   fprintf(log_stream,"%3d ",i_par);
   for (j_par = 1; j_par <= ndim; j_par++ ) 
   {
     fprintf(log_stream, "%7.4f ",p[i_par][j_par]);
     p[1][j_par] += p[i_par][j_par];
   }
   fprintf(log_stream,"%7.4f\n",y[i_par]);
   y[1] += y[i_par];
 }
 
 fprintf(log_stream,"\navg:");
 for (j_par=1;j_par<= ndim;j_par++) 
   fprintf(log_stream, "%7.4f ",p[1][j_par]/mpar);
 fprintf(log_stream,"%7.4f\n",y[1]/mpar);

 fclose(log_stream);

 free_matrix(p,1, mpar,1);
 free_vector(y,1);
 free_vector(x,1);
} /* end of function sr_sx */

/***********************************************************************/
