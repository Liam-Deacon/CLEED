/********************************************************************
GH/07.09.95
file contains function:

   real cr_rb( real *eng, real *e_int, real *t_int)

Calculate R2-factor

Changes:
  GH/19.02.93
  GH/08.02.94  Change Normalistion (Cuncorr = 0.67)
  GH/07.09.95 -  Adaption for CRFAC
********************************************************************/
#include <stdio.h>
#include <math.h>          /* needed for sqrt */
#include "crfac.h"         /* specific definitions etc. */

#define CNORM 0.666667     /* 0.67 normalisation to uncorrelated curves */

real cr_rb( real *eng, real *e_int, real *t_int)

/********************************************************************
 Calculate Rb1-factor:

INPUT:

  real *eng, *e_int, *t_int - (input) lists containing energies, expt.
             and theoretical intensities, respectively, in the same order.
             At least eng must be terminated by F_END_OF_LIST.

DESIGN:

  Rb1 = (1 - C) / (1 - CNORM).

  C =  S(It*Ie) / sqrt{ (S It^2) * (S Ie^2) }
  CNORM = 2/3 (cf. G. Held, Dissertation)

RETURN VALUE:
               R2-factor, if successful.
               F_FAIL, if failed.

********************************************************************/
{
int i_eng;
real exp_sq_sum, the_sq_sum;
real rf_sum;

/* 
 Calculate integrals and R-factor
*/

 exp_sq_sum = 0.;
 the_sq_sum = 0.;
 rf_sum = 0.;

 for(i_eng = 0; 
     ! IS_EQUAL_REAL(eng[i_eng], F_END_OF_LIST); i_eng ++)
 {
   rf_sum += e_int[i_eng] * t_int[i_eng];

   exp_sq_sum += SQUARE(e_int[i_eng]);
   the_sq_sum += SQUARE(t_int[i_eng]);
 }

 rf_sum /= R_sqrt(the_sq_sum * exp_sq_sum);

#ifdef CONTROL
  fprintf(STDCTR,"(cr_rb): rf_sum: %f,", rf_sum);
#endif

 rf_sum = ( (1. - rf_sum) / (1. - CNORM) );

#ifdef CONTROL
  fprintf(STDCTR," rfac: %f \n", rf_sum);
#endif
 return (rf_sum);
}
