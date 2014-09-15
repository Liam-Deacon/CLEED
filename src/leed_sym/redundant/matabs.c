/*********************************************************************
  GH/18.01.95
  
  real matabs(mat M)
  
  Calculate the modulus of a matrix (sum of moduli of elements)

  Changes:
  GH/18.01.95 - diagonal matrices are not implemented!

*********************************************************************/
#include <math.h>  
#include <stdio.h>
#include <stdlib.h>

#include "mat.h"


real matabs(mat M)

/* 
  Calculate the modulus of matrix M, i.e. the sum of the moduli of the 
  matrix elements.

  parameters:
  M - input matrix

  RETURN VALUE:  
  
    modulus (mabs)
    -1.         if failed (and EXIT_ON_ERROR is not defined)
*/

{
real mabs;
real *ptr_r, *ptr_i, *ptr_end;
long nn;
  
/********************************************************************* 
  Check input matrix
*********************************************************************/

/* check validity of the input matrices M */
 if (matcheck(M) < 0)
 {
#ifdef ERROR
   fprintf(STDERR," *** error (matabs): invalid input matrix\n");
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(-1.);
#endif
 }

/********************************************************************* 
  Calculate modulus for valid matrix types
*********************************************************************/

 mabs = 0.;

 if ( (M->mat_type == MAT_NORMAL) || (M->mat_type == MAT_SQUARE) )
 {
   nn = M->cols * M->cols;
   switch(M->num_type)
   {
     case(NUM_REAL):
     {
       for (ptr_r = M->rel + 1, ptr_end = M->rel + nn;
            ptr_r <= ptr_end; ptr_r ++)
       {
         mabs += R_fabs(*ptr_r);
       } 
       break;
     } /* case REAL */

     case(NUM_COMPLEX):
     {
       for (ptr_r = M->rel + 1, ptr_i = M->iel + 1, ptr_end = M->rel + nn;
            ptr_r <= ptr_end; ptr_r ++, ptr_i ++)
       {
         mabs += R_cabs(*ptr_r, *ptr_i);
       }
       break;
     } /* case COMPLEX */
   }  /* switch */
 }     /* matrix type is not diagonal */
 
 else  if (M->mat_type == MAT_DIAG)
 {
   nn = M->cols;
   switch(M->num_type)
   {
     case(NUM_REAL):
     {
       for (ptr_r = M->rel + 1, ptr_end = M->rel + nn;
            ptr_r <= ptr_end; ptr_r ++)
       {
         mabs += R_fabs(*ptr_r);
       }
       break;
     } /* case REAL */

     case(NUM_COMPLEX):
     {
       for (ptr_r = M->rel + 1, ptr_i = M->iel + 1, ptr_end = M->rel + nn;
            ptr_r <= ptr_end; ptr_r ++, ptr_i ++)
       {
         mabs += R_cabs(*ptr_r, *ptr_i);
       }
       break;
     } /* case COMPLEX */
   }  /* switch */
 }  /* diag. matrix */

 else /* not a valid matrix type */
 {
#ifdef ERROR
   fprintf(STDERR," *** error (matabs): %d not a valid matrix type\n", 
           M->mat_type);
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(-1.);
#endif
 }
 return(mabs);
}  /* end of function matabs */
