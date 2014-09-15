/*********************************************************************
  GH/03.08.94 
  file contains:

  cri_*    - basic mathematical operations for complex arguments. The
             input of arguments is done for real (*_r) and imaginary part 
             (*_i) separately; the output of the result is done by 
             pointers to the respective variables (again real and imaginary 
             part separately). The typical argument list is:
             
             void cri_<func> ( res_r, res_i, arg1_r, arg1_i [, arg2_r, argi_i] )
  
  cri_mul  - complex multiplication (04.08.94)
  cri_div  - complex division (03.08.94)
  cri_sqrt - complex square root (03.08.94)
  cri_exp  - complex exponent exp(z) (03.08.94)
  cri_expi - complex exponent exp(i*z) (03.08.94)
  cri_powi - integer exponent of i (23.08.94)
  
Changes:

GH/18.08.94 - type real instead of double
GH/30.08.97 - all functions are void delete all return statements).

*********************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mat.h"

/*=====================================================================*/
/*=====================================================================*/

void cri_mul(real *res_r, real *res_i,
            real fac1_r, real fac1_i, real fac2_r, real fac2_i)

/************************************************************************

 complex multiplication

 res = fac1 * fac2

 res_r, res_i  - result ( input of pointers ).
 fac1_r, fac1_i, fac2_r, fac2_i
               - factors (arguments);

************************************************************************/
{
 *res_r = fac1_r*fac2_r - fac1_i*fac2_i;
 *res_i = fac1_r*fac2_i + fac1_i*fac2_r;
}

/*=====================================================================*/
/*=====================================================================*/

void cri_div(real *res_r, real *res_i, 
            real num_r, real num_i, real den_r, real den_i)

/************************************************************************

 complex division

 res = num/den

 res_r, res_i  - result ( input of pointers ).
 num_r, num_i  - numerator (argument)
 den_r, den_i  - denominator (argument);

************************************************************************/
{
real r, faux;
 if (R_fabs(den_r) >= R_fabs(den_i))
 {
   r = den_i/den_r;
   faux = den_r + r*den_i;
   *res_r = (num_r + r * num_i)/faux;
   *res_i = (num_i - r * num_r)/faux;
 }
 else
 {
   r = den_r/den_i;
   faux = den_i + r*den_r;
   *res_r = ( num_i + r * num_r)/faux;
   *res_i = (-num_r + r * num_i)/faux;
 }
} /* end of function cri_div */

/*=====================================================================*/
/*=====================================================================*/

void cri_sqrt(real *res_r, real *res_i, real arg_r, real arg_i)

/************************************************************************
 
 complex square root 
 
 res = sqrt(arg)

 res_r, res_i  - result ( input of pointers ).
 arg_r, arg_i  - argument of the root function (argument)

************************************************************************/
{
 real x, y, w, r;
 if (IS_EQUAL_REAL(arg_r, 0.0) && IS_EQUAL_REAL(arg_i, 0.0))
 {
   *res_i = *res_r = 0.;
 }
 else
 {
   x = R_fabs(arg_r); 
   y = R_fabs(arg_i);

   if (x >= y) 
   {
     r=y/x;
     w=R_sqrt(x)*R_sqrt(0.5*(1.0+R_sqrt(1.0+r*r)));
   }
   else
   {
     r=x/y;
     w=R_sqrt(y)*R_sqrt(0.5*(r+R_sqrt(1.0+r*r)));
   } 
   if (arg_r > 0.0)
   {
     *res_r = w;
     *res_i = arg_i/(2. * w);
   }
   else
   {
     *res_i = (arg_i >= 0.) ? w:-w;
     *res_r = arg_i/(2. * *res_i);
   }
 }
} /* end of function cri_sqrt */

/*=====================================================================*/
/*=====================================================================*/

void cri_exp(real *res_r, real *res_i, real arg_r, real arg_i)

/************************************************************************

 complex exponent

 res = R_exp(arg)

 res_r, res_i  - result ( input of pointers ).
 arg_r, arg_i  - argument of the exponetial function (argument)

************************************************************************/
{
real faux;
 if (IS_EQUAL_REAL(arg_i, 0.))
 
 {
   *res_r = R_exp(arg_r);
   *res_i = 0.;
 } 
 else 
 {
   if (IS_EQUAL_REAL(arg_r, 0.))
   {
     *res_r = R_cos(arg_i);
     *res_i = R_sin(arg_i);
   }
   else
   {
     faux = R_exp(arg_r);
     *res_r = faux * R_cos(arg_i);
     *res_i = faux * R_sin(arg_i);
   }
 }
} /* end of function cri_exp */

/*=====================================================================*/
/*=====================================================================*/

void cri_expi(real *res_r, real *res_i, real arg_r, real arg_i)

/************************************************************************

 complex exponent

 res = exp(i*arg)

 res_r, res_i  - result ( input of pointers ).
 arg_r, arg_i  - argument of the exponetial function (argument)

************************************************************************/
{
real faux;

/*
  Exchange values of arg_r and arg_i
*/

 faux = arg_i;
 arg_i = arg_r;
 arg_r = -faux;

/*
  From now on expi is identical to exp.
*/

 if (IS_EQUAL_REAL(arg_i, 0.))
 {
   *res_r = R_exp(arg_r);
   *res_i = 0.;
 }
 else
 {
   if (IS_EQUAL_REAL(arg_r, 0.))
   {
     *res_r = R_cos(arg_i);
     *res_i = R_sin(arg_i);
   }
   else
   {
     faux = R_exp(arg_r);
     *res_r = faux * R_cos(arg_i);
     *res_i = faux * R_sin(arg_i);
   }
 }
} /* end of function cri_expi */

/*=====================================================================*/
/*=====================================================================*/

void cri_powi(real *res_r, real *res_i, int arg_int)

/************************************************************************

 integer power of i

 res = i^arg_int

 res_r, res_i  - result ( input of pointers ).
 arg_int       - integer argument: exponent

************************************************************************/
{
 arg_int = arg_int%4;
 switch(arg_int)
 {
   case(0):
   { *res_r = 1.;
     *res_i = 0.;
     break; }
   case(1): case(-3):
   { *res_r = 0.;
     *res_i = 1.;
     break; }
   case(2): case(-2):
   { *res_r =-1.;
     *res_i = 0.;
     break; }
   case(3): case(-1):
   { *res_r = 0.;
     *res_i =-1.;
     break; }
 }

} /* end of function cri_expi */

/*=====================================================================*/
/*=====================================================================*/
