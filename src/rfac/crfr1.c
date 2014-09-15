/********************************************************************
GH/07.09.95
file contains function:

   real cr_r1( real *eng, real *e_int, real *t_int)

Calculate R2-factor

Changes:
  GH/01.10.92
  GH/26.10.92 - Change normalisation.
********************************************************************/

#include <stdio.h>
#include <math.h>          /* needed for fabs */

#include "crfac.h"          /* specific definitions etc. */

real cr_r1( real *eng, real *e_int, real *t_int)

/********************************************************************
 compute R1-factor:

INPUT:

  real *eng, *e_int, *t_int - (input) lists containing energies, expt.
             and theoretical intensities, respectively, in the same order.
             At least eng must be terminated by F_END_OF_LIST.

DESIGN:

  R1 = S|It - norm_te*Ie| / S |It|

  norm_te = S|It| / S|Ie|;

  note: Normalisation factor (denominator) is changed with respect to
  common use. Instead S |It|, now S |It- SIt/dE| is used.

RETURN VALUE: 
     R1-factor, if successful.
     F_FAIL, if failed.
********************************************************************/
{
int i_eng, n_eng;
real exp_sum, the_sum;
real rf_sum, norm_sum;
real norm_te;

/* 
 calculate integrals
*/

 exp_sum = 0.;
 the_sum = 0.;

 for(i_eng = 0; 
     ! IS_EQUAL_REAL(eng[i_eng], F_END_OF_LIST); i_eng ++)
 {
   exp_sum += R_fabs( e_int[i_eng] );
   the_sum += t_int[i_eng];
 }
 n_eng = i_eng;

 norm_te = the_sum / exp_sum;             /* ratio theor. / expt. curve */
 the_sum = the_sum / n_eng;               /* average theor. intensity */

/*
 calculate R-factor
*/

 rf_sum = 0.;
 norm_sum = 0.;
 for(i_eng = 0; i_eng < n_eng; i_eng ++)
 {
   rf_sum   += R_fabs( t_int[i_eng] - norm_te * e_int[i_eng] );
   norm_sum += R_fabs( t_int[i_eng] - the_sum );
 }

 return (rf_sum/norm_sum);
}
