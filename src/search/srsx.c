/*********************************************************************
 *                        SRSX.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/29.12.95 - insert dpos in parameter list: initial displacement
 *                 can be specified through a command line option.
 *********************************************************************/

/*!
 * \file
 * \brief Driver for routine AMOEBA (From "Numerical Recipes in C")
 */

#include <stdio.h>
#include <strings.h>
#include <math.h>
#include <stdlib.h>
#include "csearch.h"
#include "gh_stddef.h"

/*!
 * Performs an amoeba (downhill simplex) search.
 *
 * \param n_dim
 * \param dpos
 * \param bak_file filename of the vertex backup file *.vbk
 * \param log_file filename of \c cleed control log.
 */
void sr_sx(size_t n_dim, real dpos, const char *bak_file, const char *log_file)
{

  size_t i_par, j_par;  /* matrix index (loop) variables */
  size_t nfunc;
  size_t mpar = n_dim + 1;
  cleed_real faux;

  FILE *log_stream;

  cleed_vector *x = cleed_vector_alloc(n_dim);
  cleed_vector *y = cleed_vector_alloc(mpar);
  cleed_basic_matrix *p = cleed_basic_matrix_alloc(mpar, n_dim);

  /* SIMPLEX METHOD */
  if( (log_stream = fopen(log_file, "a")) == NULL)
  {
    ERROR_MSG("Could not open log file '%s' for reading\n", log_file);
  }

  fprintf(log_stream, "=> SIMPLEX SEARCH:\n\n");

  /* Set up vertex if no vertex file was specified, read vertex otherwise. */
  if(strncmp(bak_file, "---", 3) == 0)
  {
    fprintf(log_stream, "=> Set up vertex:\n");
    fclose(log_stream);

    /* redundant if calloc memory:
     * for(i_par = 0; i_par < n_dim; i_par ++)
     * {
     *   cleed_basic_matrix_set(p, 0, i_par, mpar, n_dim, 0.);
     * }
    */

    for (i_par = 0; i_par < mpar; i_par ++)
    {
      for (j_par = 0; j_par < n_dim; j_par ++)
      {
        if(i_par == (j_par+1))
        {
          faux = cleed_basic_matrix_get(p, 0, j_par, n_dim) + dpos;
          cleed_basic_matrix_set(p, i_par, j_par, n_dim, faux);
          cleed_vector_set(x, j_par, faux);
        }
        else
        {
          faux = cleed_basic_matrix_get(p, 0, j_par, n_dim);
          cleed_basic_matrix_set(p, i_par, j_par, n_dim, faux);
          cleed_vector_set(x, j_par, faux);
        }
      } /* for j_par */

      CONTROL_MSG(CONTROL, "Calculate function for vertex(%d)\n", i_par);

      cleed_vector_set(y, i_par, SR_RF(x));

   } /* for i_par */

  } /* if bak_file == '---' */
  else
  {
    fprintf(log_stream, "=> Read vertex from \"%s\":\n", bak_file);
    sr_rdver(bak_file, y, p, (int)n_dim);
    fclose(log_stream);
  }

  /* Enter amoeba */

  CONTROL_MSG(CONTROL, "Enter amoeba\n");

  if( (log_stream = fopen(log_file, "a")) == NULL)
  {
    ERROR_MSG("Could not append to log file '%s'\n", log_file);
  }
  fprintf(log_stream, "=> Start search (abs. tolerance = %.3e)\n", R_TOLERANCE);
  fclose(log_stream);

  sr_amoeba(p, y, n_dim, R_TOLERANCE, SR_RF, &nfunc);

  /* Write final results to log file */

  CONTROL_MSG(CONTROL, "%d function evaluations in sr_amoeba\n", nfunc);

  if( (log_stream = fopen(log_file, "a")) == NULL)
  {
    ERROR_MSG("Could not append to log file '%s'\n", log_file);
  }

  fprintf(log_stream, "\n=> No. of function evaluations in sr_amoeba: %3d\n",
          nfunc);
  fprintf(log_stream, "=> Vertices and function values of final 3-d simplex:\n");
 
  /* print 1st line */
  fprintf(log_stream, "%3d:", 0);
  for (j_par = 0; j_par < n_dim; j_par++ )
  {
    fprintf(log_stream, "%7.4f ", cleed_basic_matrix_get(p, 0, j_par, n_dim));
  }
  fprintf(log_stream, "%7.4f\n", cleed_vector_get(y, 0));

  /* print other lines */
  for (i_par = 1; i_par < mpar; i_par++ )
  {
    fprintf(log_stream, "%3d ", i_par);
    for (j_par = 0; j_par < n_dim; j_par++ )
    {
      faux = cleed_basic_matrix_get(p, 0, j_par, n_dim);
      fprintf(log_stream, "%7.4f ", faux);
      faux += cleed_basic_matrix_get(p, i_par, j_par, n_dim);
      cleed_basic_matrix_set(p, 0, j_par, n_dim, faux);
    }
    faux = cleed_vector_get(y, i_par);
    fprintf(log_stream, "%7.4f\n", faux);
    faux += cleed_vector_get(y, 0);
    cleed_vector_set(y, 0, faux);
  }
 
  fprintf(log_stream, "\navg:");
  for (j_par=0; j_par < n_dim; j_par++ )
  {
    fprintf(log_stream, "%7.4f ",
        cleed_basic_matrix_get(p, 0, j_par, n_dim) / mpar);
  }
  fprintf(log_stream, "%7.4f\n", cleed_vector_get(y, 0) / mpar);

  fclose(log_stream);

  /* free memory */
  cleed_vector_free(x);
  cleed_vector_free(y);
  cleed_basic_matrix_free(p);

} /* end of function sr_sx */
