/*********************************************************************
LD/24.04.14
                        PRINT_DATA

  file contains functions:

  print_data (24.04.14)
    print (x,y) data
  
Changes:

*********************************************************************/

#include "ftsmooth.h"

/********************************************************************
*					PRINT OUTPUT DATA								*
*********************************************************************/
/* subroutine to output x & f(x) values, deleting negative f(x) 
values if required. It will return the number of datalines written */
int print_data(FILE *out_stream, double *x, double *fx, size_t n_x)
{
  size_t i_x;
  
  for(i_x = 0; i_x < n_x; i_x ++)
	fprintf(out_stream,"%e %e\n", x[i_x], fx[i_x] );
 
  return 0;
}

