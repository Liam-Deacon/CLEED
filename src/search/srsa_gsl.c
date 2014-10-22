/************************************************************************
 *                            SRSA_GSL.C
 *
 *  Copyright 1995-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1995.09.20 - Creation of srsa.c (copy from srsx.c)
 *   GH/1995.12.29 - insert dpos in parameter list: initial displacement
 *                   can be specified through a command line option.
 *   LD/2014.05.25 - Creation (copy from srsa.c)
 *                 - Adapted to use GSL
 ***********************************************************************/

/*!
 * \file srsa_gsl.c
 *
 * \brief Search according to simulated annealing (simplex) method.
 *
 * Perform a search according to the simulated annealing (simplex) method.
 * Driver for routine sr_amebsa_gsl() (using GNU Scientific Library).
 *
 * \warning This routine needs to be tested thoroughly and as such is under
 * development and should be considered as highly experimental (unstable).
 *
 */

#include <stdio.h>
#include <strings.h>
#include <math.h>
#include <stdlib.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_siman.h>

#include "csearch.h"

#define START_TEMP     3.5
#define EPSILON        0.25
#define ALPHA          4
#define MAX_ITER_SA  200
#define ITER_SA      100

/* set up parameters for this simulated annealing run */
#define N_TRIES 200         /*!< # points do we try before stepping */
#define ITERS_FIXED_T 1000  /*!< # iterations for each T */
#define STEP_SIZE 1.0       /*!< max step size in random walk */
#define K 1.0               /*!< Boltzmann constant */
#define T_INITIAL 3.5       /*!< initial temperature */
#define MU_T 1.003          /*!< damping factor for temperature */
#define T_MIN 2.0e-6        /*!< minimum temperature */

long sa_idum = -1;           /*!< seed for random number generator */

gsl_siman_params_t params = {
     N_TRIES, ITERS_FIXED_T, STEP_SIZE,
     1.0, START_TEMP, MU_T, T_MIN
};

/*!
 *
 * Performs simulated annealing using the GNU Scientific Library.
 *
 * \param n_dim ??
 * \param dpos ??
 * \param bak_file filename of the backup file for the search vertex.
 * \param log_file filename of the log file.
 */
void sr_sa_gsl(size_t n_dim, real dpos,
               const char *bak_file, const char *log_file)
{
  size_t i_par, j_par;
  size_t m_par, n_func;

  real temp, rmin;
  double faux;

  gsl_vector *x = gsl_vector_alloc(n_dim);
  gsl_vector *y = gsl_vector_alloc(n_dim);
  gsl_matrix *p = gsl_matrix_alloc(n_dim, n_dim);

  FILE *log_stream;

  const gsl_rng_type *T;
  gsl_rng *r;

/***********************************************************************
 * SIMULATED ANNEALING (SIMPLEX METHOD)
 ***********************************************************************/

  /* setup random number generator */
  gsl_rng_env_setup();
  T = gsl_rng_default;
  r = gsl_rng_alloc(T);

 
  if( (log_stream = fopen(log_file, "a")) == NULL)
  {
    OPEN_ERROR(log_file);
  }
  fprintf(log_stream, "=> SIMULATED ANNEALING:\n\n");
 

/***********************************************************************
 * Set up vertex if no vertex file was specified, read vertex otherwise.
 ***********************************************************************/

  m_par = n_dim + 1;

  if(strncmp(bak_file, "---", 3) == 0)
  {
    fprintf(log_stream, "=> Set up vertex:\n");
    fclose(log_stream);

    for(i_par = 0; i_par < n_dim; i_par ++)
    {
      gsl_matrix_set(p, 0, i_par, 0.);
    }

    for (i_par = 0; i_par < m_par; i_par ++)
    {
      for (j_par = 0; j_par < n_dim; j_par ++)
      {
        faux = gsl_matrix_get(p, 0, j_par);
        if(i_par == (j_par+1))
        {
          faux += dpos;
        }
        gsl_vector_set(x, j_par, faux);
        gsl_matrix_set(p, i_par, j_par, faux);
      } /* for j_par */

      #ifdef CONTROL
      fprintf(STDCTR, "(sr_sa): Calculate function for vertex(%d)\n", i_par);
      #endif

      gsl_vector_set(y, i_par, sr_evalrf_gsl(x));

    } /* for i_par */

  } /* if strncmp */
  else
  {
    fprintf(log_stream, "=> Read vertex from \"%s\":\n", bak_file);
    sr_rdver_gsl(bak_file, y, p);
    fclose(log_stream);
  }

/***********************************************************************
 * Enter temperature loop
 ***********************************************************************/

  #ifdef CONTROL
  fprintf(STDCTR, "(sr_sa_gsl): Enter temperature loop\n");
  #endif

  if( (log_stream = fopen(log_file, "a")) == NULL)
  {
    OPEN_ERROR(log_file);
  }
  fprintf(log_stream, "=> Start search (abs. tolerance = %.3e)\n", R_TOLERANCE);
  fclose(log_stream);

  rmin = 100.;
  n_func = -1;
  for(temp = START_TEMP; temp > R_TOLERANCE; temp *= (1. - EPSILON) )
  {
    #ifdef CONTROL
    fprintf(STDCTR, "(sr_sa_gsl): temperature = %.4f\n", temp);
    #endif

    n_func = MAX_ITER_SA;
    sr_amebsa_gsl(p, y, x, &rmin, temp, sr_evalrf_gsl, &n_func, temp);
  }

/***********************************************************************
 * Write final results to log file
 ***********************************************************************/

  #ifdef CONTROL
  fprintf(STDCTR, "(sr_sa_gsl): %d function evaluations in sr_amebsa\n",
          n_func);
  #endif

  if( (log_stream = fopen(log_file, "a")) == NULL)
  {
    OPEN_ERROR(log_file);
  }

  fprintf(log_stream, "\n=> No. of iterations in sr_powell: %3d\n", n_func);
  fprintf(log_stream, "=> Optimum parameter set and function value:\n");

  for (j_par = 1; j_par <= n_dim; j_par++ )
  {
    fprintf(log_stream, "%.6f ", gsl_vector_get(x, j_par-1));
  }
  fprintf(log_stream, "\nrmin = %.6f\n", rmin);

  fclose(log_stream);

  /* free memory */
  gsl_matrix_free(p);
  gsl_vector_free(x);
  gsl_vector_free(y);
  gsl_rng_free(r);

} /* end of function sr_sa_gsl */
