/*********************************************************************
 *                    FTSMOOTH_MAIN.C
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
 *   GH/30.05.1995 - Creation
 *   GH/01.06.1995 - Start transformation from f(x=0) = 0 and interpolate values
 *                   from -x_0 to x_0.
 *   GH/07.06.1995 - x_0 = n_x/4 * x_step
 *   LD/18.06.2013 - allow trimming of datasets with '--range <arg1> <arg2>'
 *********************************************************************/

#include <stdio.h>
#include "ftsmooth.h"

int main(int argc, char *argv[])
{
  char mode;

  int range_flag, offset_flag, del_flag;
  int stdin_flag, stdout_flag;

  size_t i_arg;
  size_t i_x, n_x;
  size_t i_r;

  double *x = NULL;
  double *fx = NULL;

  double cutoff, tailoff;
  double offset = 0.;

  double *ubound = NULL;
  double *lbound = NULL;

  FILE *in_stream = stdin;
  FILE *out_stream = stdout;

  /* Preset arguments */
  mode = 'n';
  cutoff = 0.5;
  tailoff = 10.;
  offset = 0.;
 
  stdin_flag = stdout_flag = 1;
  range_flag = offset_flag = del_flag = 0;

  /* initialise arrays to STRSZ dimensional doubles */
  CLEED_ALLOC_CHECK(ubound = (double *) malloc (STRSZ * sizeof(double) ));
  CLEED_ALLOC_CHECK(lbound = (double *) malloc (STRSZ * sizeof(double) ));
 
  /*
    Check command line and decode arguments
  */

  parse_args(argc, argv, in_stream, out_stream,
	  &stdin_flag, &stdout_flag,&cutoff, &tailoff, &mode,
	  &offset_flag, &offset, &range_flag, &i_r,
	  lbound, ubound, &del_flag);
 
  #ifdef DEBUG
    char *dbg_str = (char*)malloc(sizeof(char)*STRSZ);
    char *dbg_tag = (char*)malloc(sizeof(char)*STRSZ);
 
    strncpy(dbg_str, "all", STRSZ);
    strncpy(dbg_tag, "post-args", STRSZ);
 
    ftsmooth_debug(dbg_str, dbg_tag, argc, argv, in_stream, out_stream,
    	  &stdin_flag, &stdout_flag, &cutoff, &tailoff, &mode,
    	  &offset_flag, &offset, &range_flag, &i_r,
    	  lbound, ubound, &del_flag);
  #endif
	  
  fprintf(out_stream, "# Sin Fourier Smooth: version %3.1f\n",
          (double)atof(FTSMOOTH_PROG_VERSION));
  if (!stdout_flag) /* print if out_stream not equal to stdout */
  {
    printf("#> Sin Fourier Smooth: version %3.1f\n",
           (double)atof(FTSMOOTH_PROG_VERSION));
  }

  /*
 	 initialize x and fx
  */

  CLEED_ALLOC_CHECK(x =  (double *) calloc (1, sizeof(double) ));
  CLEED_ALLOC_CHECK(fx = (double *) calloc (1, sizeof(double) ));

  /* READ INPUT DATA */
  n_x = fts_read_data(in_stream, out_stream, x, fx);
  fclose(in_stream);

  if(!stdout_flag)
  {
    printf("#> End of input (%u data pairs read)\n", n_x);
    printf("#> first x/f(x): %.3e / %.3e\n#> last  x/f(x): %.3e / %.3e \n",
           x[0], fx[0], x[n_x-1], fx[n_x-1]);
  }
 
  /* PROCESS DATA */
  if(offset_flag) /* apply y-offset */
  {
    offset = fts_offset_data(x, fx, n_x, offset, offset_flag);
    if(!stdout_flag)
    {
      printf("#> Offset f(x) by %f\n",offset);
    }
  }
 
  if(mode == 's') /* 5 point smooth */
  {
    ftsmooth(out_stream, x, fx, n_x, cutoff, tailoff, stdout_flag);
  }
  else if(!stdout_flag)
  {
    printf("#> no smooth\n");
  }

  if(del_flag) /* remove negative y-values */
  {
    i_x = n_x;
    n_x = fts_rm_neg_data(x, fx, n_x);
    if(!stdout_flag)
    {
      printf("#> Removed %u negative entries from data\n",i_x-n_x);
    } /* !stdout_flag */
  }
   
  if(range_flag) /* trim data range for x-values */
  {
    i_x = n_x;
    n_x = fts_trim_data(x, fx, n_x, lbound, ubound, i_r);
    if(!stdout_flag)
    {
      printf("#> Trimmed %u entries from data\n",i_x-n_x);
  	  printf("#> %u trimming ranges:\n",i_r);
      for(i_arg=0; i_arg<i_r; i_arg++)
      {
        printf("#> \t%u: %e to %e\n", i_arg, lbound[i_arg], ubound[i_arg]);
      }
    }
  }
 
  /* OUTPUT DATA */
 
  /* finalise output */
  fts_print_data(out_stream, x, fx, n_x);
  fclose(out_stream);
 
  if(!stdout_flag)
  {
    printf("#> End of output (%u data pairs written)\n", n_x);
  }
 
  /* clean up */
  if (x) free(x);
  if (fx) free(fx);
  if (ubound) free(ubound);
  if (lbound) free(lbound);
 
  return(0);
}
