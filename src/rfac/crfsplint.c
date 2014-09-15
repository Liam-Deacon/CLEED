/********************************************************************
GH/29.08.95
File contains function:

  real cr_splint(real eng, struct crelist *list, int leng)

Cubic spline interpolation

Changes:

GH/29.08.95 - Creation (copy from splint.c in Numerical Receipes)

********************************************************************/
#include <stdlib.h>
#include "crfac.h"

real cr_splint(real eng, struct crelist *list, int leng)

/********************************************************************
 Cubic spline interpolation

INPUT:

 real eng - (input) energy value for which interpolation is 
          to be performed.
 struct crelist *list - (input, output) list of energy/intensity/deriv2
          values the latter of which is generated in function cr_spline.
 int leng - (input) number of elements in list.

DESIGN:

 For a description see Num. Rec. Capter 3.3.

RETURN VALUE:

 y: interpolated intensity.
********************************************************************/
{
real y;
int klo,khi,k;
real h,b,a;

/* 
  Find the right place in list by means of bisection. 
*/
 klo=0;
 khi=leng-1;
 while (khi-klo > 1) 
 {
   k=(khi+klo) >> 1;                     /* integer division by 2 */
   if ((list+k)->energy > eng) khi=k;
   else klo=k;
 }

 h = (list+khi)->energy - (list+klo)->energy;
 if (IS_EQUAL_REAL(h, 0.0)) 
 {
#ifdef ERROR
   fprintf(STDERR,"*** error (cr_splint): Bad list input\n");
#endif
   exit(1);
 }

 a=((list+khi)->energy - eng)/h;
 b=(eng - (list+klo)->energy)/h;

/* 
  Evaluate cubic spline polynomial 
*/
 y = a*(list+klo)->intens + b*(list+khi)->intens + 
     ( (a*a*a-a)*(list+klo)->deriv2 + (b*b*b-b)*(list+khi)->deriv2 )*(h*h)/6.0;

 return(y);
} /* end of function cr_splint */
/********************************************************************/
