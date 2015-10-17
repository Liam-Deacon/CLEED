/*********************************************************************
 *                      FTS_TRIM_DATA.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/24.04.14 - creation.
 *********************************************************************/

/*! \file
 *
 * Implementation of fts_trim_data() function.
 */

#include "ftsmooth.h"

/*!
 * Trims x range of data such that \c lbound <= \c x <= \c ubound .
 *
 * \param x array of x values.
 * \param fx array of y values.
 * \param n_x number of elements in arrays.
 * \param lbound lower value of x that is valid.
 * \param ubound upper value of x that is valid.
 * \param n_r number of elements in arrays after trimming.
 * \return number of elements in trimmed data set.
 */
size_t fts_trim_data(double *x, double *fx, size_t n_x,
	                   double *lbound, double *ubound, size_t n_r)
{
  size_t ix, ir, it, n_t;
  int write_x;
  size_t N;
  double *trim_x, *trim_fx;
  
  n_t = n_x;

  /* first pass to get number of values after trimming */
  #ifdef USE_OPENMP
  #pragma omp parallel for
  #endif
  for (ix=0; ix<n_x; ix++)
  {
    write_x = 0;
    for (ir=0; ir<n_r; ir++)
    {
      if ((x[ix]>=lbound[ir]) && (x[ix]<=ubound[ir])) write_x = 1;
    }
    if (!write_x) n_t -= 1;
  }  
  
  /* stop if no trimming required */
  if(n_x == n_t) return(n_x);
  
  /* allocate new matrices */
  trim_x  = (double *) malloc (n_t * sizeof(double) );
  trim_fx = (double *) malloc (n_t * sizeof(double) );
  
  /* second pass to add values to trimmed matrices */
  it = 0;
  #ifdef _OMP_H_DEF
  #pragma omp parallel for
  #endif
  for (ix=0; ix<n_x; ix++)
  {
    write_x = 0;
    for (ir=0; ir<n_r; ir++)
    {
      if ((x[ix]>=lbound[ir]) && (x[ix]<=ubound[ir])) write_x = 1;
    }
    if (write_x)
    {
      /* add new entry into trimmed data */
      trim_x[it] = x[ix];
      trim_fx[it] = fx[ix];
      it++;
    }
  }
  
  /* reallocate space for trimmed data
	 using factors of STRSZ to reduce memory fragmentation */
  if(n_t > STRSZ) N = (n_t-(n_t % STRSZ))*2; /* N=2^i blocks, where i is an integer */
  else N = STRSZ;
	
  if(n_t >= N-1)
  {
    ERROR_MSG("trimmed data (%i) > %i!\n", n_t, N);
    exit(-1);
  }
  
  x  = (double *) realloc(trim_x, (N * sizeof(double)));
  fx = (double *) realloc(trim_fx,(N * sizeof(double)));
  
  /* clean up */
  free(trim_x);
  free(trim_fx);
  
  return(n_t);
}
