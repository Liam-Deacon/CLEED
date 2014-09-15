/*********************************************************************
  GH/18.01.95
  
  real matdiff(mat M1, mat M2)
  
  Calculate the difference between two matrices

  Changes:
  GH/18.01.95 - diagonal matrices are not implemented!

*********************************************************************/
#include <math.h>  
#include <stdio.h>
#include <stdlib.h>
#include "mat.h"

real matdiff(mat M1, mat M2)

/* 
  Calculate the difference between two matrices i.e. the sum of the
  moduli of the difference between the matrix elements.

  parameters:
  M1,M2 - input matrices (must have the same dimensions)

  RETURN VALUE:  
  
    difference
    -1.         if failed (and EXIT_ON_ERROR is not defined)
*/

{
real diff;
real *ptr_1, *ptr_2, *ptr_end;
long int nn;
  
/********************************************************************* 
  Check input matrix
*********************************************************************/

/* check validity of the input matrices M1 and M2 */
 if ( (matcheck(M1) < 1) || (matcheck(M2) < 1) )
 {
#ifdef ERROR
   fprintf(STDERR," *** error (matdiff): invalid input matrix\n");
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(-1.);
#endif
 }

 if ((M1->cols != M2->cols) || (M1->rows != M2->rows) )
 {
#ifdef ERROR
   fprintf(STDERR," *** error (matdiff): matrix dimensions do not match\n");
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(-1.);
#endif
 }

/********************************************************************* 
  Calculate difference for valid matrix types
*********************************************************************/

 diff = 0.;

 if ( ( (M1->mat_type == MAT_NORMAL) || (M1->mat_type == MAT_SQUARE) ) &&
      ( (M1->mat_type == MAT_NORMAL) || (M1->mat_type == MAT_SQUARE) ) )
 {
   nn = M1->cols * M1->cols;
   switch(M1->num_type)
   {
     case(NUM_REAL):
     {
       for (ptr_1 = M1->rel + 1, ptr_2 = M2->rel + 1, ptr_end = M1->rel + nn;
            ptr_1 <= ptr_end; ptr_1 ++, ptr_2 ++)
       {
         diff += R_fabs(*ptr_1 - *ptr_2);
       } 
       break;
     } /* case REAL */

     case(NUM_COMPLEX):
     {
     /* first calculate diffrerence of real part */
       for (ptr_1 = M1->rel + 1, ptr_2 = M2->rel + 1, ptr_end = M1->rel + nn;
            ptr_1 <= ptr_end; ptr_1 ++, ptr_2 ++)
       {
         diff += R_fabs(*ptr_1 - *ptr_2);
       }

    /* add diffrerence of imaginary part */
       for (ptr_1 = M1->iel + 1, ptr_2 = M2->iel + 1, ptr_end = M1->iel + nn;
            ptr_1 <= ptr_end; ptr_1 ++, ptr_2 ++)
       {
         diff += R_fabs(*ptr_1 - *ptr_2);
       }
       break;
     } /* case COMPLEX */
   }  /* switch */
 }     /* matrix type is not diagonal */
 
 else  /* one matrix is diagonal */
 {

/********************************************************************* 
  diagonal matrices are not implemented!
*********************************************************************/

#ifdef ERROR
   fprintf(STDERR," *** error (matdiff): diagonal matrices not implemented\n");
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(-1.);
#endif
 }
 return(diff);
}  /* end of function matdiff */
