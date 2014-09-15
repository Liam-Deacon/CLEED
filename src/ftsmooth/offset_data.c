/*********************************************************************
LD/24.04.14
                        OFFSET_DATA

  file contains functions:

  offset_data (24.04.14)
    offset data by specified amount
  
Changes:

*********************************************************************/

#include "ftsmooth.h"


/********************************************************************
*							OFFSET DATA								*
*********************************************************************/
/* subroutine to offset f(x) values */
int offset_data(double *x, double *fx, int n_x, double offset,
		int offset_flag)
{
  int i;
  double xmin = DBL_MAX;
  double ymin = DBL_MAX;

 /* select offset type */
  switch (offset_flag)
  {
    case OFFSET_X_TO_VALUE:
      /* offset f(x) by offset */
	  for (i=0;i<n_x;i++) 
	    x[i] = x[i] + offset;
      break;
	 
    case OFFSET_X_TO_ZERO:
      /* check for lowest value of y */
      #ifdef _USE_OPENMP
      #pragma omp parallel for
      #endif
	  for (i=0;i<n_x;i++) 
	    if (x[i] < xmin) xmin = x[i];
	 
      /* offset f(x) by ymin */
      #ifdef _USE_OPENMP
	  #pragma omp parallel for
      #endif
      for (i=0;i<n_x;i++) 
	    x[i] = x[i] - xmin;
	  break;
    
    case OFFSET_Y_TO_VALUE:
      /* offset f(x) by offset */
	  for (i=0;i<n_x;i++) 
	    fx[i] = fx[i] + offset;
      break;
	 
    case OFFSET_Y_TO_ZERO:
      /* check for lowest value of y */
      #ifdef _USE_OPENMP
      #pragma omp parallel for
      #endif
	  for (i=0;i<n_x;i++) 
	    if (fx[i] < ymin) ymin = fx[i];
	 
      /* offset f(x) by ymin */
      #ifdef _USE_OPENMP
	  #pragma omp parallel for
      #endif
      for (i=0;i<n_x;i++) 
	    fx[i] = fx[i] - ymin;
	  break;
	  
    default:
      return -1; /* offset_flag not recognised, do not process offset */
  }
 
  return 0;
}