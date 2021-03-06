/***********************************************************************
GH/21.09.02
 File contains:

  sr_er(int ndim, real dpos, char *bak_file, char *log_file)
 Determine error bars for all parameters.

 Modified:
GH/21.09.02 - copy from srsx

***********************************************************************/

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_vector.h>
#include "search.h"

extern char *sr_project;

/**********************************************************************/

void sr_er_gsl(int ndim, real dpos, char *bak_file, char *log_file)

/***********************************************************************
  ERROR BARS
***********************************************************************/
{

  char *line_buffer;

  int iaux = 0;
  int i_par, j_par;
  int mpar = ndim+1;

  real y_0;

  gsl_vector *x = gsl_vector_alloc(ndim);
  gsl_vector *x_0 = gsl_vector_calloc(ndim);
  gsl_vector *y = gsl_vector_alloc(ndim);
  gsl_vector *err = gsl_vector_alloc(ndim);
  gsl_vector *del = gsl_vector_alloc(ndim);

  real pref, rtol, rr, dr, rfac, rdel=0.;

  FILE *io_stream, *log_stream;

/***********************************************************************
  write title to log file
***********************************************************************/

  if( (log_stream = fopen(log_file, "a")) == NULL) { OPEN_ERROR(log_file); }
  fprintf(log_stream,"=> DETERMINE ERROR BARS:\n\n");

/***********************************************************************
  allocate memory and preset variables
***********************************************************************/

  line_buffer = (char *) malloc(STRSZ * sizeof(char));
  
  mpar = mpar * 1; /* set but not used */
 
/***********************************************************************
  Calculate R factor for minimum.
***********************************************************************/

  fprintf(log_stream, "=> displace parameters from minimum:\n");
  fclose(log_stream);

  y_0 = sr_evalrf_gsl(x_0);

/***********************************************************************
  Read R factor value and RR factor for minimum
***********************************************************************/

  sprintf(line_buffer, "%s.dum", sr_project);
  io_stream = fopen(line_buffer, "r");

  while( fgets(line_buffer, STRSZ, io_stream) != NULL)
  {
    if(
       #ifdef REAL_IS_DOUBLE
         (iaux = sscanf(line_buffer, "%lf %lf", &rfac, &rr) )
       #endif
       #ifdef REAL_IS_FLOAT
         (iaux = sscanf(line_buffer, "%f %f",    &rfac, &rr) )
       #endif
       == 2) break;
   }

/* Stop with error message if reading error */
  if( iaux != 2)
  {
    log_stream = fopen(log_file, "a");
    fprintf(log_stream, "*** error while reading output from %s\n",
            getenv("CSEARCH_RFAC"));
    fclose(log_stream);
    exit(1);
  }

  fclose (io_stream);
 
  pref = y_0 * rr;
  rtol = R_TOLERANCE / rr;

  #ifdef CONTROL
    fprintf(STDCTR, "(sr_er_gsl):"
            " rfac = %.4f, rr = %.4f, rdel = %.4f\n", rfac, rr, rdel);
  #endif

/***********************************************************************
  Calculate R factors for displacements
***********************************************************************/

  for (i_par = 1; i_par <= ndim; i_par ++) 
  {

    del[i_par] = dpos;
    rdel = 1.;

    do
    {
 
      if ( rdel < rtol )
      {
        if( rdel < 0. )
        {
          #ifdef ERROR
            fprintf(STDERR, "*** error (sr_er_gsl):"
                    " minimum not found (%d)\n", i_par);
          #endif
          exit(1);
        }
        else
        {
          #ifdef WARNING
            fprintf(STDWAR, "* warning (sr_er_gsl):"
                    " minimum R factor independent of par. %d\n", i_par);
          #endif
          rdel = 1.;
        }
      }
      else
      {
        del[i_par] = del[i_par] / R_sqrt(rdel);

        for (j_par = 1; j_par <= ndim; j_par ++) 
        {
          if(i_par == j_par)
          {
            x[j_par] = x_0[j_par] + del[j_par];
          }
          else
            x[j_par] = x_0[j_par];
        }

        #ifdef CONTROL
          fprintf(STDCTR, "(sr_er_gsl):"
                  " Calculate function for parameter (%d)\n", i_par);
        #endif

        y[i_par] = sr_evalrf(x);

        rdel =  R_fabs(y[i_par] - y_0) / pref;
      }

    }
    while( ( rdel < 0.75 ) || ( rdel > 1.25) );

  } /* for i_par */


/***********************************************************************
  Calculate error bars and write to log file
***********************************************************************/

  pref = y[1] * rr;
  for (i_par = 1; i_par <= ndim; i_par ++) 
  {
    del[i_par] = R_fabs(del[i_par]);
  }

  if( (log_stream = fopen(log_file, "a")) == NULL) { OPEN_ERROR(log_file); }

  fprintf(log_stream,"\n=> ERROR BARS:\n\n");
  for (i_par = 1; i_par <= ndim; i_par ++) 
  {
    dr = y[i_par] - y_0;
    if (dr < 0.)
    {
      #ifdef WARNING
        fprintf(STDWAR, "* warning (sr_er_gsl):"
                " not at minimum: dr = %.4f\n", dr);
      #endif
      dr = R_fabs(dr);
    }

    if( ! IS_EQUAL_REAL(dr, 0.))
    {
      err[i_par] = R_sqrt(pref/dr) * del[i_par];
      fprintf(log_stream,"%2d: del R = %.4f; del par = %.4f\n", 
              i_par, dr, err[i_par]);
    }
    else
    {
      err[i_par] = -1.;
      fprintf(log_stream,"%2d: del R = %.4f; del par ** undefined **\n",
              i_par, dr );
    }

  } /* for i_par */

  fclose(log_stream);

  /* free memory */
  gsl_vector_free(y);
  gsl_vector_free(x);
  gsl_vector_free(x_0);
  gsl_vector_free(del);
  gsl_vector_free(err);

} /* end of function sr_er */

/***********************************************************************/

