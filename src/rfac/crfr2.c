/********************************************************************
GH/07.09.95
file contains function:

   real cr_r2( real *eng, real *e_int, real *t_int)

Calculate R2-factor

Changes:
  GH/02.10.92
  GH/26.10.92 -  change normalisation
  GH/04.01.93
  GH/01.03.93 -  change normalisation factor of T/E IV curves
  GH/07.09.95 -  Adaption for CRFAC
********************************************************************/

#include <stdio.h>
#include <math.h>          /* needed for fabs */

#include "crfac.h"          /* specific definitions etc. */

real cr_r2( real *eng, real *e_int, real *t_int)

/********************************************************************
 Calculate R2-factor:

INPUT:

  real *eng, *e_int, *t_int - (input) lists containing energies, expt.
             and theoretical intensities, respectively, in the same order.
             At least eng must be terminated by F_END_OF_LIST.

DESIGN:

  R2 = sqrt { S(It - norm_te*Ie)^2 / S (It)^2 }

  norm_te = sqrt( S|It|^2 / S|Ie|^2);

  note: Normalisation is changed with respect to common use. Instead
  S (It)^2,
  now: S (It - <It>)^2 where <It> = (S It)/ dE.



RETURN VALUE: 
               R2-factor, if successful.
	       F_FAIL, if failed.
********************************************************************/
{
int i_eng, n_eng;
real exp_sq_sum, the_sq_sum;  /* sum of expt. / theor. intensities */
real rf_sum, norm_sum;
real norm_te, the_avg, aux;

/* 
 compute integrals
*/

 exp_sq_sum = 0.;
 the_sq_sum = 0.;
 the_avg = 0.;

 for(i_eng = 0; 
     ! IS_EQUAL_REAL(eng[i_eng], F_END_OF_LIST); i_eng ++)
 {
   exp_sq_sum += SQUARE( e_int[i_eng] );
   the_sq_sum += SQUARE( t_int[i_eng] );
   the_avg    += t_int[i_eng];
 }
 n_eng = i_eng;

 the_avg /= n_eng;
 norm_te = R_sqrt(the_sq_sum/exp_sq_sum);

#ifdef CONTROL
  fprintf(STDCTR,"(cr_r2): <It>: %e\n", (float)the_avg);
  fprintf(STDCTR,"(cr_r2): norm: %f \n", (float)norm_te);
#endif

/*
 compute R-factor (rf_sum) and normalisation factor (norm_sum)
*/

 rf_sum = 0.;
 norm_sum = 0.;
 for(i_eng = 0; i_eng < n_eng; i_eng ++)
 {
  aux = t_int[i_eng] - norm_te * e_int[i_eng];
  rf_sum +=  SQUARE(aux);

  aux = t_int[i_eng] - the_avg;
  norm_sum += SQUARE(aux);
 }

#ifdef ERROR /* print error output if required */
  if (norm_sum < 0.)
   printf("*** error (cr_r2): norm_sum is negative: %e\n",(float)norm_sum);
#endif


 aux = R_sqrt(rf_sum/norm_sum);
 return (aux);
}  /* end of function cr_r2 */
