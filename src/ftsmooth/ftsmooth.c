/*********************************************************************
LD/24.04.14
                        FTSMOOTH

  file contains functions:

  ftsmooth (24.04.14)
    perform Fourier Transform smoothing on x,y data
  
Changes:

*********************************************************************/

#include "ftsmooth.h"

/****************************************************************
*					Fourier Transformation						*
*****************************************************************/
/* This subroutine performs the Fourier smoothing of the data */
int ftsmooth(FILE *out_stream, double *x, double *fx, size_t n_x,
	  double cutoff, double tailoff, int stdout_flag)
{

  size_t i_x;
  size_t i_k, n_k;
  size_t N;

  double *k, *fk_s, *fk_c; 

  double norm1;
  double x_0;
  double a1, a3;
  double b1, b2;

  double k_step, k_max;
  double x_step, x_max, x_now;
  double faux;
  
/* parameters for x */

  faux   = x[n_x-1] - x[0];
  x_0    = 0.25 * faux;
  x_max  = 2 * (faux + 2. * x_0);
  x_step = faux/(n_x - 1);
  i_k = 0;

  if (x_step <= 0.) 
  {
    fprintf(stderr," *** error:  x step < or = 0.: %.3e\n", x_step);
    exit(1);
  }

/* 
   parameters for cubic/linear interpolation:
    a1,a3: between -x_0 and x_0 (cubic);
    b1,b2: between  x_f and x_f + 2*x_0 (linear);
*/

  faux = x[1] - x[0] + x_0;
  a3 = (fx[1]/faux  - fx[0]/x_0) / (faux*faux - x_0*x_0);
  a1 = fx[0]/x_0 - a3*x_0*x_0;
 
  b1 = fx[n_x-1];
  b2 = - fx[n_x-1]/x_0;

  if (!stdout_flag) printf("#> a1: %.3e, b1: %.3e, b2: %.3e\n", a1, b1, b2);

/* parameters for k */

  faux = cutoff * (1. + 3. / sqrt(tailoff) );    /* (empirical) */
  n_k  = (n_x*(size_t)rint(1.5 * faux)) + 1;

  fprintf(out_stream, "# cutoff: %.3f, tailoff: %.3f => k range: %.3f\n",
			   cutoff, tailoff, faux);
  if (!stdout_flag)
  {
    printf("#> cutoff: %.3f, tailoff: %.3f => k range: %.3f\n",
            cutoff, tailoff, faux);
  }
  k_step = PI/x_max;
  k_max  = 1.5 * n_x * PI / x_max;

  tailoff /= k_max;
  cutoff  *= k_max;
  norm1 = 0.5 / atan(tailoff*cutoff);

/* initialize k and fk_s/c */

  N = (n_k-(n_k % STRSZ))*2; /* use blocks of 128^i */
  if(N <= 0) N = STRSZ;

  k = (double *) malloc (N * sizeof(double) );
  fk_s = (double *) malloc (N * sizeof(double) );
  fk_c = (double *) malloc (N * sizeof(double) );

/* Fourier Transformation */
  #ifdef USE_OPENMP
  #pragma omp parallel for
  #endif
  for( i_k = 0; i_k < n_k; i_k ++)
  {

    k[i_k]  = i_k * k_step;
    fk_s[i_k] = 0.;
    fk_c[i_k] = 0.;
  

/* 
 (a) over linear part: 0 < x < x_0 and x[n_x-1] < x < x[n_x-1] + x_0
*/

    for (x_now = x_step, i_x = 0; x_now < x_0; x_now += x_step, i_x ++)
    {
      faux  = k[i_k] * x_now;
      fk_s[i_k] += (a3*x_now*x_now + a1)*x_now * sin(faux);
	  
	  #ifdef DEBUG
      if (i_k == 0) fprintf(stdout,"%e %e\n", 
                      x_now, (a3*x_now*x_now + a1)*x_now );
	  #endif

      faux  = k[i_k] * (x[n_x-1] - x[0] + x_0 + x_now);
      fk_s[i_k] += (b1 + b2 * x_now) * sin(faux);

	  #ifdef DEBUG
      if (i_k == 1) fprintf(stdout,"%e %e\n", 
              (x[n_x-1] - x[0] + x_0 + x_now), (b1 + b2 * x_now) );
	  #endif
    } /* for x_now */

    if ((i_k == 0) && (!stdout_flag)) 
      printf("#> %d (2 x %d) interpolated function values are used \n", 
        2*i_x, i_x);

/* 
 (b) over actual function: x[0] < x < x[n_x-1]
*/
    for (i_x = 0; i_x < n_x; i_x ++)
    {
      x_now = x[i_x] - x[0] + x_0;
      faux  = k[i_k] * x_now;
      fk_s[i_k] += fx[i_x] * sin(faux);

	 #ifdef DEBUG
     if (i_k == 0) fprintf(stdout,"%e %e\n", x_now, fx[i_x] );
	 #endif
	 
    } /* for i_x */

    faux = tailoff * (cutoff - k[i_k]);
    faux =  0.5 + norm1 * atan(faux);
    fk_s[i_k] *= k_step * SQRT_PI * faux;

 }  /* for i_k */

  if(!stdout_flag)
    printf("#> last point in FT (%d): k = %.3f weight = %.3f\n", 
         i_k-1, k[i_k-1],faux);

/* 
  back transformation (for data points 2 to n_x - 3):
  Use the input values of x.
*/

  x_step = SQRT_PI * x_max / (n_x * 3.);
 
  for( i_x = 0; i_x < n_x; i_x ++)
  {
    x_now = x[i_x] - x[0] + x_0;
    for (i_k = 0, fx[i_x] = 0.; i_k < n_k; i_k ++)
    {
      faux  = x_now * k[i_k];
      fx[i_x] += fk_s[i_k]*sin(faux);
    } /* for i_x */

    fx[i_x] *= x_step;

  }  /* for i_k */

/*
  5 point smooth (if required):
*/

  fprintf(out_stream,"# 5 point smooth\n");
  if(!stdout_flag) printf("#> 5 point smooth\n");

  /* 1st data point no smooth */
  fprintf(out_stream,"%e %e\n", x[0], fx[0] );

  /* 2nd data point 3 point smooth */
  faux = 0.25 * (fx[0] + fx[2]) + 0.5 * fx[1];
  fx[1] = faux;

  /* data points 2 to n_x - 3 */
  for( i_x = 2; i_x < n_x - 2; i_x ++)
  {
    faux = 0.0625*(fx[i_x-2] + fx[i_x+2]) + 
           0.25  *(fx[i_x-1] + fx[i_x+1]) + 
           0.375 * fx[i_x];
    fx[i_x] = faux;
  }  /* for i_x */

  /* 2nd last data point 3 point smooth */
  faux = 0.25 * (fx[n_x-1] + fx[n_x-3]) + 0.5 * fx[n_x-2];
  fx[n_x-2] = faux;

  /* last data point =  no smooth */

  /* clean up */
  free(k); 
  free(fk_s);
  free(fk_c);
  
  return 0;
}
