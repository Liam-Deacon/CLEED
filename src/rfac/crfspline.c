/********************************************************************
  GH/11.08.95
  file contains function:

  void cr_spline(struct crelist *list, int leng, real yp1,real ypn)

 Prepare cubic spline

 Changes:

 GH/11.08.95 - Creation (copy from spline.c in Numerical Receipes)

********************************************************************/
#include <malloc.h>

#include "crfac.h"


void cr_spline(struct crelist *list, int leng)

/********************************************************************
 First stage of cubic spline

INPUT:
 
 struct crelist *list - (input, output) list of energy/intensity 
          values to be interpolated by cubic spline.
          The function will generate the structure elements 
          deriv2 which will be used for interpolation in function
          cr_splint.
 int leng - (input) number of elements in list.

DESIGN:

 For a description see Num. Rec. Capter 3.3. The boundary conditions
 are set for natural spline (zero second derivative on both boundaries).

RETURN VALUE:
 
 No return values.
********************************************************************/
{
int i,k;
real p,qn,sig,un;
real *buf;

 buf = (real *)malloc( leng * sizeof(real) );

 (list+0)->deriv2 = buf[0] = 0.0;

 for (i=1; i <= leng-2; i++) 
 {
   sig = ( (list+i)->energy - (list+i-1)->energy )/
         ( (list+i+1)->energy - (list+i-1)->energy);
   p = sig * (list+i-1)->deriv2 + 2.0;
   (list+i)->deriv2 = (sig-1.0)/p;
   buf[i] = ( (list+i+1)->intens - (list+i)->intens )/
            ( (list+i+1)->energy - (list+i)->energy ) - 
            ( (list+i)->intens - (list+i-1)->intens )/
            ( (list+i)->energy - (list+i-1)->energy );
   buf[i] = ( 6.0*buf[i] /
            ( (list+i+1)->energy - (list+i-1)->energy ) - sig*buf[i-1])/p;
 }

 qn=un=0.0;

 (list+leng-1)->deriv2 = ( un - qn*buf[leng-2] ) / 
                         ( qn*(list+leng-2)->deriv2 + 1.0);
 for (k=leng-2; k>=0; k--)
   (list+k)->deriv2 = (list+k)->deriv2 * (list+k+1)->deriv2 + buf[k];

 free(buf);
}  /* end of function cr_spline */
/********************************************************************/
