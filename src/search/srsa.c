/*********************************************************************
 *                      SRSA.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/20.09.95 - Creation (copy from srsx.c)
 *   GH/29.12.95 - insert dpos in parameter list: initial displacement
 *                 can be specified through a command line option.
 ***********************************************************************/

#include <stdio.h>
#include <strings.h>
#include <math.h>
#include <stdlib.h>
#include "csearch.h"

static const double START_TEMP = 3.5L;
static const double EPSILON = 0.25L;
enum { MAX_ITER_SA = 200 };

long sa_idum = -1;                /*!< seed for random number generator */

/*! \file
 *
 * Perform a search according to the SIMULATED ANNEALING (SIMPLEX) METHOD
 * Driver for routine sr_amebsa (From Numerical Recipes)
 */
void sr_sa(size_t n_dim, real dpos, const char *bak_file, const char *log_file)
{

  size_t i_par, j_par;
  size_t m_par = n_dim + 1;
  size_t nfunc;

  real temp, rmin;

  cleed_vector *x = CLEED_VECTOR_ALLOC(n_dim);
  cleed_vector *y = CLEED_VECTOR_ALLOC(n_dim);
  cleed_basic_matrix *p = CLEED_BASIC_MATRIX_ALLOC(m_par, n_dim);

  FILE *log_stream;

/***********************************************************************
 * SIMULATED ANNEALING (SIMPLEX METHOD)
 ***********************************************************************/

  if( (log_stream = fopen(log_file, "a")) == NULL)
  {
    ERROR_MSG("Could not open log file '%s'\n", log_file);
  }
  fprintf(log_stream, "=> SIMULATED ANNEALING:\n\n");

/***********************************************************************
 * Set up vertex if no vertex file was specified, read vertex otherwise.
 ***********************************************************************/

  if(strncmp(bak_file, "---", 3) == 0)
  {
    fprintf(log_stream,"=> Set up vertex:\n");
    fclose(log_stream);

    for(i_par = 0; i_par < n_dim; i_par ++)
    {
      CLEED_BASIC_MATRIX_SET(p, 0, i_par, m_par, n_dim, 0.);
    }

    for (i_par = 0; i_par < m_par; i_par ++)
    {
      for (j_par = 0; j_par < n_dim; j_par ++)
      {
        if(i_par == (j_par+1))
        {
          CLEED_VECTOR_SET(x, j_par,
                     CLEED_BASIC_MATRIX_GET(p, 0, m_par, n_dim, j_par) + dpos);
          CLEED_BASIC_MATRIX_SET(p, i_par, j_par, m_par, n_dim,
                     CLEED_BASIC_MATRIX_GET(p, 0, m_par, n_dim, j_par) + dpos);
        }
        else
        {
          CLEED_VECTOR_SET(x, j_par, CLEED_BASIC_MATRIX_GET(p, 0, m_par, n_dim, j_par));
          CLEED_BASIC_MATRIX_SET(p, i_par, j_par, m_par, n_dim,
                                  CLEED_BASIC_MATRIX_GET(p, 0, m_par, n_dim, j_par));
        }
      }

      CONTROL_MSG(CONTROL, "Calculate function for vertex(%d)\n", i_par);

      CLEED_VECTOR_SET(y, i_par, SR_RF(x));
    }
  }
  else
  {
    fprintf(log_stream, "=> Read vertex from \"%s\":\n", bak_file);
    sr_rdver(bak_file, y, p, (int)n_dim);
    fclose(log_stream);
  }

/***********************************************************************
 * Enter temperature loop
 ***********************************************************************/

  CONTROL_MSG(CONTROL, "Enter temperature loop\n");

  if( (log_stream = fopen(log_file, "a")) == NULL)
  {
    ERROR_MSG("Could not append to log file '%s'\n", log_file);
  }
  fprintf(log_stream, "=> Start search (abs. tolerance = %.3e)\n", R_TOLERANCE);
  fclose(log_stream);

  rmin = 100.;
  for(temp = START_TEMP; temp > R_TOLERANCE; temp *= (1. - EPSILON) )
  {
    CONTROL_MSG(CONTROL, "temperature = %.4f\n", temp);
    nfunc = MAX_ITER_SA;
    sr_amebsa(p, y, n_dim, x, &rmin, temp, sr_evalrf, &nfunc, temp);
  }

/***********************************************************************
 * Write final results to log file
 ***********************************************************************/

  CONTROL_MSG(CONTROL, "%d function evaluations in sr_amebsa\n", nfunc);

  if( (log_stream = fopen(log_file, "a")) == NULL)
  {
    ERROR_MSG("Could not append to log file '%s'\n", log_file);
  }

  fprintf(log_stream, "\n=> No. of iterations in sr_powell: %3d\n", nfunc);
  fprintf(log_stream, "=> Optimum parameter set and function value:\n");

  for (j_par = 0; j_par <= n_dim; j_par++ )
  {
    fprintf(log_stream, "%.6f ", CLEED_VECTOR_GET(x, j_par) );
  }
  fprintf(log_stream, "\nrmin = %.6f\n", rmin);

  fclose(log_stream);

  CLEED_VECTOR_FREE(x);
  CLEED_VECTOR_FREE(y);
  CLEED_BASIC_MATRIX_FREE(p);

} /* end of function sr_sa */

