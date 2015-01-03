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


 Modified:
 GH/23.08.95
***********************************************************************/

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <math.h>
#include "csearch.h"
#include "gh_stddef.h"

/*!
 * Perform a search according to POWELL'S METHOD.
 *
 * \param n_dim number of dimensions for search?
 * \param bak_file filename of vertex backup file *.vbk
 * \param log_file filename of log file.
 */
void sr_po(size_t n_dim, const char *bak_file, const char *log_file)
{
  size_t i_par, j_par;
  size_t nfunc;

  real rmin;
  real *p,**xi;

  FILE *log_stream;

/***********************************************************************
 * POWELL'S METHOD
 ***********************************************************************/

  if( (log_stream = fopen(log_file, "a")) == NULL) { OPEN_ERROR(log_file); }
  fprintf(log_stream, "=> POWELL'S METHOD\n\n");

/***********************************************************************
 * Set up initial direction set
 ***********************************************************************/
  i_par = 1;
  p = vector(1, n_dim);
  xi = matrix(1, n_dim,1, n_dim);

  if(strncmp(bak_file, "---", 3) == 0)
  {
    fprintf(log_stream, "=> Set up initial direction set:\n");
    fclose(log_stream);

    for(i_par = 1; i_par <= n_dim; i_par ++)
    {
      p[i_par] = 0.;
    }

    for (i_par = 1; i_par <= n_dim; i_par ++)
    {
      for (j_par =1; j_par <= n_dim; j_par ++)
      {
        if(i_par == j_par) xi[i_par][j_par] = 1.;
        else xi[i_par][j_par] = 0.;
      }
    }

  } /* if */
  else
  {
    /* Read direction set from file */
  }

/***********************************************************************
 * Enter search function sr_powell
 ***********************************************************************/

  CONTROL_MSG(CONTROL, "Enter sr_powell #%i\n", i_par);

  fprintf(log_stream, "=> Start search (abs. tolerance = %.3e)\n", R_TOLERANCE);

  /* close log file before entering the search */
  fclose(log_stream);

  sr_powell(p, xi, n_dim, R_TOLERANCE, &nfunc, &rmin, sr_evalrf);

/***********************************************************************
 * Write final results to log file
 ***********************************************************************/

  CONTROL_MSG(CONTROL, "%d iterations in sr_powell\n", nfunc);

  if( (log_stream = fopen(log_file, "a")) == NULL) { OPEN_ERROR(log_file); }

  fprintf(log_stream, "\n=> No. of iterations in sr_powell: %3d\n", nfunc);
  fprintf(log_stream, "=> Optimum parameter set:\n");
  for (j_par = 1; j_par <= n_dim; j_par++ )
  {
   fprintf(log_stream, "%.6f ", p[j_par]);
  }
  fprintf(log_stream, "\n");

  fprintf(log_stream, "=> Optimum function value:\n");
  fprintf(log_stream, "rmin = %.6f\n", rmin);

  fclose(log_stream);

  /* free memory */
  free_matrix(xi, 1, n_dim, 1);
  free_vector(p, 1);

}  /* end of function sr_po */
