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
 *29.12.95
 File contains:

  sr_sx(int ndim, real dpos, char *bak_file, char *log_file)
 Perform a search according to the SIMPLEX METHOD
 Driver for routine AMOEBA (using the GNU Scientific Library)

 Modified:
GH/23.08.95
GH/29.12.95 - insert dpos in parameter list: initial displacement
              can be specified through a command line option.
LD/01.07.2014 - modified for compatibility with GSL.

***********************************************************************/

#include <stdio.h>
#include <strings.h>
#include <math.h>
#include <stdlib.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include "csearch.h"

/**********************************************************************/

void sr_sx_gsl(size_t ndim, real dpos, char *bak_file, char *log_file)

/***********************************************************************
  SIMPLEX METHOD
***********************************************************************/
{

size_t i_par, j_par;
size_t mpar, nfunc=0;
real faux;

gsl_vector *x = gsl_vector_alloc(ndim);
gsl_vector *y = gsl_vector_alloc(ndim);
gsl_matrix *p = gsl_matrix_alloc(ndim, ndim);

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

 if(strncmp(bak_file, "---", 3) == 0)
 {
   fprintf(log_stream,"=> Set up vertex:\n");
   fclose(log_stream);

   /* set initial matrix parameters to zero */
   for(i_par = 1; i_par <= ndim; i_par ++)
   {
     gsl_matrix_set(p, 0, i_par-1, 0.);
   }

   for (i_par = 1; i_par <= mpar; i_par ++) 
   {
     for (j_par = 1; j_par <= ndim; j_par ++) 
     {
       faux = gsl_matrix_get(p, 0, j_par-1);
       if(i_par == (j_par+1))
         faux += dpos;
       gsl_vector_set(x, j_par-1, faux);
       gsl_matrix_set(p, i_par-1, j_par-1, faux);
     }

     #ifdef CONTROL
       fprintf(STDCTR,"(sr_sx): Calculate function for vertex(%d)\n", i_par);
     #endif

     gsl_vector_set(y, i_par-1, sr_evalrfac_gsl(x));
   }
 }
 else
 {
   fprintf(log_stream, "=> Read vertex from \"%s\":\n", bak_file);
   sr_rdver_gsl(bak_file, y, p);
   fclose(log_stream);
 }

/***********************************************************************
  Enter amoeba
***********************************************************************/

 #ifdef CONTROL
   fprintf(STDCTR, "(sr_sx_gsl): Enter amoeba\n");
 #endif

 if( (log_stream = fopen(log_file, "a")) == NULL) { OPEN_ERROR(log_file); }
 fprintf(log_stream, "=> Start search (abs. tolerance = %.3e)\n", R_TOLERANCE);
 fclose(log_stream);

 sr_amoeba_gsl(p, y, R_TOLERANCE, &nfunc);

/***********************************************************************
  Write final results to log file
***********************************************************************/

 #ifdef CONTROL
   fprintf(STDCTR, "(sr_sx_gsl): %d function evaluations in sr_amoeba_gsl\n", 
           nfunc);
 #endif

 if( (log_stream = fopen(log_file, "a")) == NULL) { OPEN_ERROR(log_file); }

 fprintf(log_stream, "\n=> No. of function evaluations in sr_amoeba_gsl: %3d\n",
         nfunc);
 fprintf(log_stream, "=> Vertices and function values of final 3-d simplex:\n");
 
/* print 1st line */

 fprintf(log_stream, "%3d:",1);
 for (j_par = 0; j_par < ndim; j_par++ ) 
   fprintf(log_stream, "%7.4f ", gsl_matrix_get(p, 0, j_par));
 fprintf(log_stream,"%7.4f\n", gsl_vector_get(y, 0));

/* print other lines */
 for (i_par = 0; i_par < mpar; i_par++ ) 
 {
   fprintf(log_stream, "%3d ", i_par+1);
   for (j_par = 0; j_par < ndim; j_par++ ) 
   {
     faux = gsl_matrix_get(p, i_par, j_par);
     fprintf(log_stream, "%7.4f ", faux);
     gsl_matrix_set(p, 0, j_par, gsl_matrix_get(p, 0, j_par)+faux);
   }
   fprintf(log_stream, "%7.4f\n", gsl_vector_get(y, i_par));
   faux = gsl_vector_get(y, 0);
   gsl_vector_set(y, 0, faux+gsl_vector_get(y, i_par) );
 }
 
 fprintf(log_stream, "\navg:");
 for (j_par=0; j_par < ndim; j_par++) 
   fprintf(log_stream, "%7.4f ", gsl_matrix_get(p, 0, j_par)/mpar);
 fprintf(log_stream, "%7.4f\n", gsl_vector_get(y, 0)/mpar);

 fclose(log_stream);

 /* free memory */
 gsl_matrix_free(p);
 gsl_vector_free(x);
 gsl_vector_free(y);
 
} /* end of function sr_sx */

/***********************************************************************/

