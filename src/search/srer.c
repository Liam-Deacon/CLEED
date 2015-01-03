/*********************************************************************
 *                            SRER.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/21.09.02 - copy from srsx
 *   LD/17.10.14 - adapted for condition GSL usage (enabled with
 *                 'USE_GSL' added to defines when compiling)
 *********************************************************************/

/*!
 * \file
 * \author Georg Held <g.held@reading.ac.uk>
 * \brief Determine error bars for all parameters.
 */

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <math.h>

#ifdef USE_GSL
#include <gsl/gsl_vector.h>
#endif

#ifdef USE_GSL
#define VECTOR_SET(vec, i, val)     gsl_vector_set(vec, i, val)
#define VECTOR_ALLOC(vec, n)        gsl_vector* vec = gsl_vector_alloc(n)
#define VECTOR_FREE(vec)            gsl_vector_free(vec)
#define
#else
#define VECTOR_SET(vec, i, val)   vec[i] = val
#define VECTOR_ALLOC(vec, n)      real* vec = vector(1, n)
#define VECTOR_FREE(vec)          free_vector(vec, 1)
#endif

#include "csearch.h"


extern char *sr_project;

/*!
 * Calculates error bars for all parameters.
 *
 * \param n_dim number of dimensions for parameter vector.
 * \param dpos displacement.
 * \param log_file filename of search log.
 */
void sr_er(size_t n_dim, real dpos, const char *log_file)
{

  char line_buffer[STRSZ];

  int iaux = 0;
  size_t i_par, j_par;
  size_t m_par = n_dim+1;

  real y_0;

  #ifdef USE_GSL
  gsl_vector *x = gsl_vector_alloc(n_dim);
  gsl_vector *x_0 = gsl_vector_calloc(n_dim);
  gsl_vector *y = gsl_vector_alloc(n_dim);
  gsl_vector *err = gsl_vector_alloc(n_dim);
  gsl_vector *del = gsl_vector_alloc(n_dim);
  #else
  real *x = vector(1, (int)n_dim);
  real *x_0 = vector(1, (int)n_dim);
  real *y = vector(1, (int)n_dim);
  real *err = vector(1, (int)n_dim);
  real *del = vector(1, (int)n_dim);
  #endif /* _USE-GSL */

  real pref, rtol, rr, dr, rfac, rdel=0.;

  FILE *io_stream, *log_stream;

  /* write title to log file */
  if( (log_stream = fopen(log_file, "a")) == NULL) { OPEN_ERROR(log_file); }
  fprintf(log_stream, "=> DETERMINE ERROR BARS:\n\n");

  /* allocate memory and preset variables */
  m_par = n_dim + 1;

  /* Calculate R factor for minimum. */
  fprintf(log_stream, "=> displace parameters from minimum:\n");
  fclose(log_stream);

  for(i_par = 1; i_par <= n_dim; i_par ++) VECTOR_SET(x_0, i_par, 0.);

  y_0 = SR_EVALRF(x_0);

  /* Read R factor value and RR factor for minimum */
  sprintf(line_buffer, "%s.dum", sr_project);
  io_stream = fopen(line_buffer, "r");

  while( fgets(line_buffer, STRSZ, io_stream) != NULL)
  { ;
    if( (iaux = sscanf(line_buffer, "%" REAL_FMT "f %" REAL_FMT "f",
                       &rfac, &rr) ) == 2) break;
  }

  /* Stop with error message if reading error */
  if( iaux != 2)
  {
    log_stream = fopen(log_file, "a");
    fprintf(log_stream, "*** error (srer): cannot read output from '%s'\n",
            getenv("CSEARCH_RFAC"));
    fclose(log_stream);
    exit(SR_ENVIRONMENT_VARIABLE_ERROR);
  }

  fclose (io_stream);

  pref = y_0 * rr;
  rtol = R_TOLERANCE / rr;

  #ifdef CONTROL
  fprintf(STDCTR, "(sr_er): rfac = %.4f, rr = %.4f, rdel = %.4f\n", rfac, rr, rdel);
  #endif

  /* Calculate R factors for displacements */
  for (i_par = 1; i_par <= n_dim; i_par ++)
  {
    #ifdef USE_GSL
    gsl_vector_set(del, i_par-1, pos);
    #else
    del[i_par] = dpos;
    #endif

    rdel = 1.;

    do
    {
      if ( rdel < rtol )
      {
        if( rdel < 0. )
        {
          ERROR_MSG("minimum not found (%d)\n", i_par);
          exit(SR_FAILURE);
        }
        else
        {
          WARNING_MSG("minimum R factor independent of par. %d\n", i_par);
          rdel = 1.;
        }
      }
      else
      {
        #ifdef USE_GSL
        gsl_vector_set(del, i_par-1,
                        gsl_vector_get(del, i_par-1) / R_sqrt(rdel));
        #else
        del[i_par] = del[i_par] / R_sqrt(rdel);
        #endif

        for (j_par = 1; j_par <= n_dim; j_par ++)
        {
          if(i_par == j_par)
          {
            #ifdef USE_GSL
            gsl_vector_set(x, j_par-1, gsl_vector_get(x_0, j_par-1)
                                          + gsl_vector_get(del, j_par-1));
            #else
            x[j_par] = x_0[j_par] + del[j_par];
            #endif
          }
          #ifdef USE_GSL
          else gsl_vector_set(x, j_par-1, gsl_vector_get(x_0, j_par-1));
          #else
          else x[j_par] = x_0[j_par];
          #endif
        }

        CONTROL_MSG(CONTROL, "Calculate function for parameter (%d)\n", i_par);

        #ifdef USE_GSL
        gsl_vector_set(y, i_par-1, SR_EVALRF(x));
        rdel = R_fabs(gsl_vector_get(y, i_par-1) - y_0) / pref;
        #else
        y[i_par] = sr_evalrf(x);
        rdel = R_fabs(y[i_par] - y_0) / pref;
        #endif
      }

    }
    while( ( rdel < 0.75 ) || ( rdel > 1.25) );

  } /* for i_par */

  /* Calculate error bars and write to log file */
  #ifdef USE_GSL
  pref = gsl_vector_get(y, 0) * rr;
  for (i_par = 1; i_par <= ndim; i_par ++)
  {
    gsl_vector_set(del, i_par-1, R_fabs(gsl_vector_get(del, i_par-1)) );
  }
  #else
  pref = y[1] * rr;
  for (i_par = 1; i_par <= n_dim; i_par ++) del[i_par] = R_fabs(del[i_par]);
  #endif

  if( (log_stream = fopen(log_file, "a")) == NULL) { OPEN_ERROR(log_file); }

  fprintf(log_stream, "\n=> ERROR BARS:\n\n");
  for (i_par = 1; i_par <= n_dim; i_par ++)
  {
    #ifdef USE_GSL
    dr = gsl_vector_get(y, i_par-1) - y_0;
    #else
    dr = y[i_par] - y_0;
    #endif

    if (dr < 0.)
    {
      WARNING_MSG("not at minimum: dr = %.4f\n", dr);
      dr = R_fabs(dr);
    }

    if( ! IS_EQUAL_REAL(dr, 0.))
    {
      #ifdef USE_GSL
      faux = R_sqrt(pref/dr) * gsl_vector_get(del, i_par-1);
      gsl_vector_set(err, i_par-1, faux);
      fprintf(log_stream, "%2d: del R = %.4f; del par = %.4f\n",
              i_par, dr, faux);
      #else
      err[i_par] = R_sqrt(pref/dr) * del[i_par];
      fprintf(log_stream, "%2d: del R = %.4f; del par = %.4f\n",
              i_par, dr, err[i_par]);
      #endif
    }
    else
    {
      #ifdef USE_GSL
      gsl_vector_set(err, i_par-1, -1.);
      #else
      err[i_par] = -1.;
      #endif
      fprintf(log_stream, "%2d: del R = %.4f; del par ** undefined **\n",
             i_par, dr);
    }

  } /* for i_par */

  fclose(log_stream);

  /* free memory */
  VECTOR_FREE(y);
  VECTOR_FREE(x);
  VECTOR_FREE(x_0);
  VECTOR_FREE(del);
  VECTOR_FREE(err);

} /* end of function sr_er */
