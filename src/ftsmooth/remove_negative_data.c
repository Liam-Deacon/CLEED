/*********************************************************************
LD/24.04.14
                        FTSMOOTH_REM_NEG_DATA

  file contains functions:

  rm_neg_data (24.04.14)
     removes negative f(x) data entries
  
Changes:

*********************************************************************/

#include "ftsmooth.h"


/********************************************************************
*						REMOVE NEGATIVE DATA						*
*********************************************************************/
/* subroutine to remove negative f(x) entries, this is essential for
Intensity data as there can be no negative intensities
NOTE: mkiv often produces negative data
 */
size_t rm_neg_data(double *x, double *fx, size_t n_x)
{
  size_t ix, it, n_t, N;
  double *trim_x, *trim_fx;
  
  n_t = n_x;

  /* first pass to get number of values after trimming */
  for (ix=0; ix<n_x; ix++)
	if (fx[ix] < 0.) n_t--; 
  
  /* stop if no trimming required */
  if(n_x == n_t) return n_x;
  
  /* allocate new matrices */
  trim_x  = (double *) malloc (n_t * sizeof(double) );
  trim_fx = (double *) malloc (n_t * sizeof(double) );
  
  /* second pass to add values to trimmed matrices */
  it = 0;
  for (ix=0; ix<n_x; ix++)
	if (fx[ix] < 0.) /* then add new entry into trimmed data */
	{
	  trim_x[it] = x[ix];
	  trim_fx[it] = fx[ix];
	  it++;  
	}
  
  /* reallocate space for trimmed data
	 using factors of STRSZ to reduce memory fragmentation */
  if(n_t>STRSZ)
    N=(n_t-(n_t%STRSZ))*2; /* N=2^i blocks, where i is an integer */
  else  
    N=STRSZ; 
	
  if(n_t>=N-1) 
  {
    fprintf(stderr, "error: trimmed data (%i) > %i!\n", n_t, N);
	exit(5);
  }
  
  x  = (double *) realloc(trim_x, (N * sizeof(double)));
  fx = (double *) realloc(trim_fx,(N * sizeof(double)));  
  
  /* clean up */
  free(trim_x);
  free(trim_fx);
  
  return n_t;
}
