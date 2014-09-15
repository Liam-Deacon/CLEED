/********************************************************************
GH/29.08.95
file contains function:

   real cr_rp( real *eng, real *e_int, real *t_int, real vi)

Calculate Pendry's R-factor

Changes:
GH/06.10.92 - Creation
GH/29.08.95 - adjust to crfac format.
  
********************************************************************/
/*
#define CONTROL
*/

#include <stdio.h>
#include <math.h>          /* needed for sqrt */

#include "crfac.h"          /* specific definitions etc. */


real cr_rp( real *eng, real *e_int, real *t_int, real vi)

/********************************************************************
 compute Pendry's R-factor:

INPUT:

  real *eng, *e_int, *t_int - (input) lists containing energies, expt.
             and theoretical intensities, respectively, in the same order.
             At least eng must be terminated by F_END_OF_LIST.

  real vi - (input) imaginary part of the optical potential.

DESIGN:

  Rp = S(Ye - Yt)^2 / S(Ye^2 + Yt^2)

RETURN VALUE: 
  Rp, if successful.

********************************************************************/
{
int i_eng;
real rf_sum, exp_y_sum, the_y_sum;
real Y_exp, Y_the, L, e_step;

/* 
 compute integrals
*/

 exp_y_sum = 0.;
 the_y_sum = 0.;
 rf_sum = 0.;
 for(i_eng = 1; 
     ! IS_EQUAL_REAL(eng[i_eng], F_END_OF_LIST); i_eng ++)
 {
  e_step = eng[i_eng] - eng[i_eng-1];

/* theoretical Y function */
  L = ( t_int[i_eng] - t_int[i_eng-1] ) / 
      ( e_step * 0.5 * (t_int[i_eng] + t_int[i_eng-1]) );

  Y_the = L/ ( 1. + L*L*vi*vi);
  the_y_sum += SQUARE(Y_the)*e_step;

/* experimental Y function */
  L = (e_int[i_eng] - e_int[i_eng-1]) / 
      ( e_step * 0.5 * (e_int[i_eng] + e_int[i_eng-1]) );

  Y_exp = L/ ( 1. + L*L*vi*vi);
  exp_y_sum += SQUARE(Y_exp)*e_step;

/* compute R-factor sum */
  rf_sum += SQUARE(Y_the - Y_exp)*e_step;
 }

#ifdef CONTROL
  fprintf(STDCTR, "(cr_rp): exp_sum: %f the_sum: %f rf_sum: %f\n", 
	 exp_y_sum, the_y_sum, rf_sum);
#endif

/* 
   Divide rf_sum by the normalizing factor (exp_y_sum + the_y_sum)
   and return result
*/
 rf_sum /= (exp_y_sum + the_y_sum);
 return (rf_sum);
}
