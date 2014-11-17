/*********************************************************************
 *                      FTSMOOTH_DEBUG.C
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/24.04.14 - creation.
 *********************************************************************/

#include "ftsmooth.h"

#ifdef DEBUG
/****************************************************************
*						DEBUG VALUES							*
*****************************************************************/
/* subroutine to print variables to aid in debugging  */
void ftsmooth_debug(char *debug_str, char *tag, int argc, char *argv[],
	  FILE *in_stream, FILE *out_stream, 
	  int *stdin_flag, int *stdout_flag,
	  double *cutoff, double *tailoff, char *mode,
	  int *offset_flag, double *offset, int *range_flag, int *i_r,
	  double *lbound, double *ubound, int *del_flag)
{
  int i;
  if((strcmp(debug_str, "ftsmooth")) && (strcmp(debug_str, "offset")) &&
     (strcmp(debug_str, "range")) && (strcmp(debug_str, "delete")) && 
	 (strcmp(debug_str, "flags")) && (strcmp(debug_str, "files")) &&
	 (strcmp(debug_str, "all")))
	 {
	   debug_str = (char*) malloc((strlen(debug_str)+1)*sizeof(char));
	   strcpy(debug_str, "all"); /* default to everything if no match */
	 }

  fprintf(stderr,"$>\n$> DEBUG INFO: (%s)\n", tag);
  if((strcmp(debug_str, "all")) || (strcmp(debug_str, "ftsmooth")))
    fprintf(stderr,"$> cutoff = %f, tailoff = %f\n", *cutoff, *tailoff);
  if((strcmp(debug_str, "all")) || (strcmp(debug_str, "offset")))	
    fprintf(stderr,"$> offset_flag = %i, offset = %f\n", *offset_flag, *offset);
  if((strcmp(debug_str, "all")) || (strcmp(debug_str, "range")))
  {
    fprintf(stderr,"$> range_flag = %i, n = %i\n", *range_flag, *i_r);
	for(i=0;i<*i_r;i++)
	{
	  if(fabs(lbound[i]- DBL_MIN) < DBL_EPSILON)
	    if(fabs(ubound[i] - DBL_MAX) < DBL_EPSILON)
		  fprintf(stderr,"$>\tx[%i]: MIN to MAX\n", i);
		else
		  fprintf(stderr,"$>\tx[%i]: MIN to %.6f\n", i, ubound[i]);
	   else
	     if(fabs(ubound[i] - DBL_MAX) > DBL_EPSILON)
		   fprintf(stderr,"$>\tx[%i]: %.6f to MAX\n", i, lbound[i]);
		 else
		   fprintf(stderr,"$>\tx[%i]: %.6f to %.6f\n", i, lbound[i], ubound[i]);
	}
  }
  if((strcmp(debug_str, "all")) || (strcmp(debug_str, "files")))
  {
    fprintf(stderr,"$> input = %i, output = %i\n", 
	   fileno(in_stream), fileno(out_stream)); 
	fprintf(stderr,"$> stdin_flag = %i, stdout_flag = %i\n",
	  *stdin_flag, *stdout_flag);
  }
  if((strcmp(debug_str, "all")) || (strcmp(debug_str, "delete")))
    fprintf(stderr,"$> delete negative values = %i\n", *del_flag);
  
  fprintf(stderr,"$> mode = %s\n", mode);
  
  fprintf(stderr,"$> command line arguments:\n\n");
  
  fprintf(stderr,"$>================================\n");
  fprintf(stderr,"$>i\t|\targv[i]\n");
  fprintf(stderr,"$>================================\n");
  for (i=0; i<argc; i++) 
    fprintf(stderr, "$>%i\t|\t%s\n", i, argv[i]);
  fprintf(stderr,"$>\n");
  
  fprintf(stderr,"$>");
  free(debug_str);
  exit(-1);
}
#endif
