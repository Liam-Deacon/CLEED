/***********************************************************************
GH/19.09.95
 File contains:

  sr_po_gsl(int ndim, char *bak_file, char *log_file)
 Perform a search according to POWELL'S METHOD

 Driver for routine AMOEBA (using GNU Scientific Library)
 Modified:
 GH/23.08.1995
 LD/01.07.2014 - Modified to use gsl_vector and gsl_matrix types
***********************************************************************/

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "search.h"
#include "gh_stddef.h"

/**********************************************************************/

void sr_po_gsl(size_t ndim, char *bak_file, char *log_file)
/***********************************************************************
  Perform a search according to POWELL'S METHOD
***********************************************************************/

{

size_t i_par = 0;
size_t j_par;
size_t nfunc;

real rmin;
gsl_matrix *xi = gsl_matrix_alloc(ndim, ndim);
gsl_vector *p = gsl_vector_calloc(ndim);

FILE *log_stream;

/***********************************************************************
  POWELL'S METHOD
***********************************************************************/

 if( (log_stream = fopen(log_file, "a")) == NULL) { OPEN_ERROR(log_file); }
 fprintf(log_stream,"=> POWELL'S METHOD\n\n");

/***********************************************************************
  Set up initial direction set
***********************************************************************/

 if(strncmp(bak_file, "---", 3) == 0)
 {
   fprintf(log_stream,"=> Set up initial direction set:\n");
   fclose(log_stream);

   gsl_matrix_set_identity(xi);
 }
 else
 {
/* Read direction set from file */
 }

/***********************************************************************
  Enter search function sr_powell_gsl
***********************************************************************/

#ifdef CONTROL
 fprintf(STDCTR, "(sr_po): Enter sr_powell_gsl #%i\n", i_par);
#endif

 fprintf(log_stream, 
         "=> Start search (abs. tolerance = %.3e)\n", R_TOLERANCE);

/* close log file before entering the search */
 fclose(log_stream);  

 sr_powell_gsl(p, xi, R_TOLERANCE, &nfunc, &rmin);

/***********************************************************************
  Write final results to log file
***********************************************************************/

#ifdef CONTROL
 fprintf(STDCTR,"(sr_po): %d iterations in sr_powell_gsl\n", nfunc);
#endif

 if( (log_stream = fopen(log_file, "a")) == NULL) { OPEN_ERROR(log_file); }

 fprintf(log_stream,"\n=> No. of iterations in sr_powell_gsl: %3d\n", nfunc);
 fprintf(log_stream,"=> Optimum parameter set:\n");
 for (j_par = 0; j_par < ndim; j_par++ )
   fprintf(log_stream, "%.6f ", gsl_vector_get(p, j_par));
 fprintf(log_stream,"\n");

 fprintf(log_stream,"=> Optimum function value:\n");
 fprintf(log_stream,"rmin = %.6f\n", rmin);

 fclose(log_stream);

 /* free memory */
 gsl_matrix_free(xi);
 gsl_vector_free(p);

}  /* end of function sr_po */

/***********************************************************************/
