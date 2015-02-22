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
  char fmt_buffer[STRSZ];
  char line_buffer[STRSZ];

  int iaux = 0;
  size_t i_par, j_par;

  real y_0;

  /* allocate memory and preset variables */
  cleed_vector *x = cleed_vector_alloc(n_dim);
  cleed_vector *x_0 = cleed_vector_alloc(n_dim);
  cleed_vector *y = cleed_vector_alloc(n_dim);
  cleed_vector *err = cleed_vector_alloc(n_dim);
  cleed_vector *del = cleed_vector_alloc(n_dim);

  real faux, pref, rtol, rr, dr, rfac, rdel=0.;

  FILE *io_stream, *log_stream;

  /* write title to log file */
  if( (log_stream = fopen(log_file, "a")) == NULL)
  {
    ERROR_MSG("Could not append to log file '%s'\n", log_file);
  }
  fprintf(log_stream, "=> DETERMINE ERROR_LOG BARS:\n\n");

  /* Calculate R factor for minimum. */
  fprintf(log_stream, "=> displace parameters from minimum:\n");
  fclose(log_stream);

  /*! changed first element of i_par loop from 1 to 0 */
  for(i_par = 0; i_par < n_dim; i_par ++) cleed_vector_set(x_0, i_par, 0.);

  y_0 = SR_RF(x_0);

  /* Read R factor value and RR factor for minimum */
  sprintf(line_buffer, "%s.dum", sr_project);
  io_stream = fopen(line_buffer, "r");

  sprintf(fmt_buffer, "%%%sf %%%sf", CLEED_REAL_FMT, CLEED_REAL_FMT);
  while( fgets(line_buffer, STRSZ, io_stream) != NULL)
  {
    if( (iaux = sscanf(line_buffer, fmt_buffer, &rfac, &rr) ) == 2) break;
  }

  /* Stop with error message if reading error */
  if( iaux != 2)
  {
    log_stream = fopen(log_file, "a");
    ERROR_MSG("cannot read output from log file '%s'\n", getenv("CSEARCH_RFAC"));
    fclose(log_stream);
    exit(SR_ENVIRONMENT_VARIABLE_ERROR);
  }

  fclose (io_stream);

  pref = y_0 * rr;
  rtol = R_TOLERANCE / rr;

  CONTROL_MSG(CONTROL, "rfac = %.4f, rr = %.4f, rdel = %.4f\n", rfac, rr, rdel);

  /* Calculate R factors for displacements */
  for (i_par = 0; i_par < n_dim; i_par ++)
  {
    cleed_vector_set(del, i_par, dpos);
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
        cleed_vector_set(del, i_par,
                         cleed_vector_get(del, i_par) / cleed_real_sqrt(rdel));

        for (j_par = 0; j_par < n_dim; j_par ++)
        {
          if(i_par == j_par)
          {
            cleed_vector_set(x, j_par, cleed_vector_get(x_0, j_par) +
                                       cleed_vector_get(del, j_par));
          }
          else
          {
            cleed_vector_set(x, j_par, cleed_vector_get(x_0, j_par));
          }
        }

        CONTROL_MSG(CONTROL, "Calculate function for parameter (%d)\n", i_par);

        /* evaluate R-factor */
        cleed_vector_set(y, i_par, SR_RF(x));
        rdel = cleed_real_fabs(cleed_vector_get(y, i_par) - y_0) / pref;

      }

    }
    while( ( rdel < 0.75 ) || ( rdel > 1.25) );

  } /* for i_par */

  /* Calculate error bars and write to log file */
  pref = cleed_vector_get(y, 0) * rr;
  for (i_par = 0; i_par < n_dim; i_par ++)
  {
    cleed_vector_set(del, i_par, cleed_real_fabs(cleed_vector_get(del, i_par)) );
  }

  if( (log_stream = fopen(log_file, "a")) == NULL)
  {
    ERROR_MSG("Could not append to log file '%s'\n", log_file);
  }

  fprintf(log_stream, "\n=> ERROR_LOG BARS:\n\n");
  for (i_par = 0; i_par < n_dim; i_par ++)
  {
    dr = cleed_vector_get(y, i_par) - y_0;

    if (dr < 0.)
    {
      WARNING_MSG("not at minimum: dr = %.4f\n", dr);
      dr = cleed_real_fabs(dr);
    }

    if( ! IS_EQUAL_REAL(dr, 0.))
    {
      faux = cleed_real_sqrt(pref/dr) * cleed_vector_get(del, i_par);
      cleed_vector_set(err, i_par, faux);
      fprintf(log_stream, "%2d: del R = %.4f; del par = %.4f\n",
              i_par, dr, faux);
    }
    else
    {
      cleed_vector_set(err, i_par, -1.);
      fprintf(log_stream, "%2d: del R = %.4f; del par ** undefined **\n",
              i_par, dr);
    }

  } /* for i_par */

  fclose(log_stream);

  /* free memory */
  cleed_vector_free(y);
  cleed_vector_free(x);
  cleed_vector_free(x_0);
  cleed_vector_free(del);
  cleed_vector_free(err);

} /* end of function sr_er */
