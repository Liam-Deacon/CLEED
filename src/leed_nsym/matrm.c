/*********************************************************************
  GH/05.06.94
*********************************************************************/

#include <math.h>
#include "mat.h"

real * r_sqmul( real * res, real * a1, real * a2, int n)

/*
  Perform matrix multiplication for square matrices.
  
  parameters
  a1, a2 - input factors
  res    - result (enough space must be allocated)

  return value:
  res
*/

{
long int nn;
long int i_r1, i_c2;

real *ptrr, *ptr_end;
register real *ptr1, *ptr2;

 nn = n*n;

#ifdef CONTROL
    fprintf(STDCTR," (matmul) entering real loop\n");
#endif
 for(ptrr = res + 1, i_r1 = 1; i_r1 < nn; i_r1 += n)
 for(i_c2 = 1; i_c2 <= n; ptrr ++, i_c2 ++ )
 {

   *ptrr = 0.;
   for(ptr1 = a1 + i_r1, ptr2 = a2 + i_c2, ptr_end = a1 + i_r1 + n;
       ptr1 < ptr_end ; ptr1 ++, ptr2 += n)
     *ptrr += *ptr1 * *ptr2;
 } 
 return(res);
}
