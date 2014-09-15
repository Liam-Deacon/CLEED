/****************************************************************************
                        FTSMOOTH_MAIN (Georg Held 29.05.95)

Fourier Transformation Smooth

Format of input file must be:
     <x>  <y>
Lines beginning with '#' are interpreted as comments.

Changes:

GH/30.05.95 - Creation
GH/01.06.95 - Start transformation from f(x=0) = 0 and interpolate values
              from -x_0 to x_0.
GH/07.06.95 - x_0 = n_x/4 * x_step

LD/18.06.13 - allow trimming of datasets with '--range <arg1> <arg2>'

****************************************************************************/

#include "ftsmooth.h"

/*====================================================================*/
int main(int argc, char *argv[])
{
char mode;

int range_flag, offset_flag, del_flag;
int stdin_flag, stdout_flag;

int i_arg;
int i_x, n_x;
int i_r;

double *x, *fx; 

double cutoff, tailoff;
double offset;

double *ubound, *lbound;

FILE *in_stream;
FILE *out_stream;

/* Preset arguments */

 out_stream = stdout;
 in_stream = stdin;
 mode = 'n';
 cutoff = 0.5;
 tailoff = 10.;
 offset = 0.;
 
 stdin_flag = stdout_flag = 1;
 range_flag = offset_flag = del_flag = 0;

 /* initialise arrays to STRSZ dimensional doubles */
 ubound = (double *) malloc (STRSZ * sizeof(double) );
 lbound = (double *) malloc (STRSZ * sizeof(double) );
 
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
 
 debug(dbg_str, dbg_tag, argc, argv, in_stream, out_stream, 
	  &stdin_flag, &stdout_flag,&cutoff, &tailoff, &mode,
	  &offset_flag, &offset, &range_flag, &i_r,
	  lbound, ubound, &del_flag);
 #endif
	  
 fprintf(out_stream,"# Sin Fourier Smooth: version %3.1f\n",VERSION);
 if (!stdout_flag) /* print if out_stream not equal to stdout */
   printf("#> Sin Fourier Smooth: version %3.1f\n",VERSION);

/*
 initialize x and fx
*/

 x =  (double *) malloc (1 * sizeof(double) );
 fx = (double *) malloc (1 * sizeof(double) );

/* READ INPUT DATA */
 n_x = read_data(in_stream, out_stream, x, fx);
 fclose(in_stream); 

 if(!stdout_flag)
 {
   printf("#> End of input (%d data pairs read)\n", n_x);
   printf("#> first x/f(x): %.3e / %.3e\n#> last  x/f(x): %.3e / %.3e \n", 
            x[0], fx[0], x[n_x-1], fx[n_x-1]);
 }
 
/* PROCESS DATA */
 if(offset_flag) /* apply y-offset */
 {
   offset = offset_data(x, fx, n_x, offset, offset_flag);
   if(!stdout_flag)
      printf("#> Offset f(x) by %f\n",offset);
 }
 
 if(mode == 's') /* 5 point smooth */
   ftsmooth(out_stream, x, fx, n_x, cutoff, tailoff, stdout_flag);
 else
   if(!stdout_flag) printf("#> no smooth\n");

 if(del_flag) /* remove negative y-values */   
 { 
   i_x = n_x;
   n_x = rm_neg_data(x, fx, n_x);
   if(!stdout_flag)
     printf("#> Removed %i negative entries from data\n",i_x-n_x); 
 }
   
 if(range_flag) /* trim data range for x-values */
 {
   i_x = n_x;
   n_x = trim_data(x, fx, n_x, lbound, ubound, i_r);
   if(!stdout_flag)
   {
     printf("#> Trimmed %i entries from data\n",i_x-n_x);
	 printf("#> %i trimming ranges:\n",i_r); 
     for(i_arg=0;i_arg<i_r;i_arg++)
	 {
	   printf("#> \t%i: %e to %e\n", i_arg, lbound[i_arg], ubound[i_arg]);
	 }
   }
 }
 
/* OUTPUT DATA */
 
 /* finalise output */
 print_data(out_stream, x, fx, n_x);
 fclose(out_stream); 
 
 if(!stdout_flag) 
   printf("#> End of output (%d data pairs written)\n",n_x);
 
 /* clean up */
 free(x);
 free(fx);
 if(range_flag)
 {
   free(ubound);
   free(lbound);
 }
 
 return 0;
}
