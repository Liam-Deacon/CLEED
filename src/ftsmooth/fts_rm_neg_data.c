/*********************************************************************
 *                      FTS_RM_NEG_DATA.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/24.04.14 - creation (split from ftsmooth.c)
 *********************************************************************/

/*!
 * \file
 * \brief Implementation of fts_rm_neg_data() function.
 */

#include "ftsmooth.h"

/*!
 * Removes negative \p fx entries.
 *
 * This is useful as intensity data cannot have negative values.
 *
 * @note \c mkiv often produces negative data entries.
 *
 * \param x array of x-axis values.
 * \param fx array of y-axis values.
 * \param n_x number of elements in \p x and \p fx arrays.
 * \return number of elements after removal of negative entries.
 */
size_t fts_rm_neg_data(double *x, double *fx, size_t n_x)
{
  size_t ix, it, n_t, N;
  double *trim_x, *trim_fx;
  
  n_t = n_x;

  /* first pass to get number of values after trimming */
  for (ix=0; ix<n_x; ix++)
	{
    if (fx[ix] < 0.) n_t--;
	}
  
  /* stop if no trimming required */
  if(n_x == n_t) return n_x;
  
  /* allocate new matrices */
  trim_x  = (double *) malloc (n_t * sizeof(double) );
  trim_fx = (double *) malloc (n_t * sizeof(double) );
  
  /* second pass to add values to trimmed matrices */
  it = 0;
  for (ix=0; ix<n_x; ix++)
  {
    if (fx[ix] < 0.) /* then add new entry into trimmed data */
    {
      trim_x[it] = x[ix];
      trim_fx[it] = fx[ix];
      it++;
    }
  }
  
  /* reallocate space for trimmed data
	 using factors of STRSZ to reduce memory fragmentation */
  if(n_t>STRSZ)
  {
    N=(n_t-(n_t%STRSZ))*2; /* N=2^i blocks, where i is an integer */
  }
  else  
  {
    N=STRSZ; 
  }
	
  if(n_t>=N-1) 
  {
    fprintf(stderr, "error: trimmed data (%i) > %i!\n", n_t, N);
    exit(-1);
  }
  
  x  = (double *) realloc(trim_x, (N * sizeof(double)));
  fx = (double *) realloc(trim_fx,(N * sizeof(double)));  
  
  /* clean up */
  free(trim_x);
  free(trim_fx);
  
  return n_t;
}
