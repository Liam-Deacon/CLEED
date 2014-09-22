/*********************************************************************
LD/24.04.14
                        TRIM_DATA

  file contains functions:

  offset_data (24.04.14)
    offset data by specified amount
  
Changes:

*********************************************************************/

#include "ftsmooth.h"


/********************************************************************
*							TRIM DATA								*
*********************************************************************/
/* subroutine to trim x range */
size_t trim_data(double *x, double *fx, size_t n_x,
	  double *lbound, double *ubound, size_t n_r)
{
  size_t ix, ir, it, n_t;
  int write_x;
  size_t N;
  double *trim_x, *trim_fx;
  
  n_t = n_x;

  /* first pass to get number of values after trimming */
  #ifdef _USE_OPENMP
  #pragma omp parallel for
  #endif
  for (ix=0; ix<n_x; ix++)
  {
    write_x = 0;
    for (ir=0; ir<n_r; ir++)
      if ((x[ix]>=lbound[ir]) && (x[ix]<=ubound[ir])) write_x = 1;
	if (!write_x) n_t -= 1; 
  }  
  
  /* stop if no trimming required */
  if(n_x == n_t) return n_x;
  
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
      if ((x[ix]>=lbound[ir]) && (x[ix]<=ubound[ir])) write_x = 1;
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
