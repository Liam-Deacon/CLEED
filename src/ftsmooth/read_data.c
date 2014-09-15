/*********************************************************************
LD/24.04.14
                        FTSMOOTH_READ_DATA

  file contains functions:

  read_data (24.04.14)
     read input x,y data
  
Changes:

*********************************************************************/

#include "ftsmooth.h"


/****************************************************************
*							READ DATA							*
*****************************************************************/
/* subroutine to read input data stream */
int read_data(FILE *in_stream, 
	  FILE *out_stream, double *x, double *fx)
{
  int i_x;
  int N;     /* array max size */

  N = STRSZ; /* 128 elements is (semi-)sensible amount for LEED-IV datasets */
  
  for (i_x = 0; fgets(line_buffer, STRSZ, in_stream) != NULL; )
  {
  
    /* Lines beginning with '#' are interpreted as comments */
    if (line_buffer[0] == '#') fprintf(out_stream,"%s", line_buffer);
    else 
    {
      sscanf(line_buffer,"%le %le", x+i_x, fx+i_x);

      i_x ++;
	  if(i_x >= N) 
	  {
        /* efficiently realloc N*2 amount of memory */
	    N*=2;
        x =  (double *) realloc(x, (N)*sizeof(double) );
        fx = (double *) realloc(fx, (N)*sizeof(double) );
	  }
    }
  }  /* for */
  
  return i_x;
}