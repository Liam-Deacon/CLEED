/*********************************************************************
  GH/15.08.94
  
  matsqmod
     calculate the square of the modulus of each matrix element.

  Changes:
  
*********************************************************************/

#include <stdio.h>
#include "mat.h"
#include <stdlib.h>

#define DIA_COMPLEX -999 + NUM_COMPLEX
#define DIA_REAL    -999 + NUM_REAL

/*======================================================================*/
/*======================================================================*/

mat matsqmod(mat Msq, mat M)
/*********************************************************************

  Transpose a matrix
  
  INPUT
    mat Msq  - (output) pointer to the result
    mat M    - input matrix

  RETURN VALUE: 
    pointer to the matrix (mat) (if successful)
    NULL else.

*********************************************************************/
{
register real *ptr_r, *ptr_i, *ptr_sq;
real *ptr_end;
mat Maux;

/********************************************************************
  Check the input matrix
********************************************************************/

 if (matcheck(M) < 1)
 {
#ifdef ERROR
  fprintf(STDOUT," *** error (matsqmod): input matrix does not exist \n");
#endif
#ifdef EXIT_ON_ERROR
  exit(1);
#else
  return(NULL);
#endif
 }

/********************************************************************
 Diagonal Matrix:
********************************************************************/
 if (M->mat_type == MAT_DIAG) 
 {
   Maux = matalloc(NULL, M->rows, M->cols, NUM_REAL | MAT_DIAG);

   switch(M->num_type)
   {
     case(NUM_REAL):
     {
       for(ptr_r = M->rel+1, ptr_sq = Maux->rel+1,
           ptr_end = M->rel + M->cols;
           ptr_r <= ptr_end; 
           ptr_r ++, ptr_sq ++)
       {
         *ptr_sq = SQUARE(*ptr_r);
       } 
       break;
     } /* NUM_REAL */

     case(NUM_COMPLEX):
     {
       for(ptr_r = M->rel+1, ptr_i = M->iel+1, ptr_sq = Maux->rel+1,
           ptr_end = M->rel + M->cols;
           ptr_r <= ptr_end; 
           ptr_r ++, ptr_i ++, ptr_sq ++)
       {
         *ptr_sq = SQUARE(*ptr_r) + SQUARE(*ptr_i);
       }
       break;
     } /* NUM_COMPLEX */
   }   /* switch */
 }     /* if diagonal */
/********************************************************************
 Other matrix types:
********************************************************************/
 else
 {
   Maux = matalloc(NULL, M->rows, M->cols, NUM_REAL);

   switch(M->num_type)
   {
     case(NUM_REAL):
     {
       for(ptr_r = M->rel+1, ptr_sq = Maux->rel+1,
           ptr_end = M->rel + M->cols*M->rows;
           ptr_r <= ptr_end; 
           ptr_r ++, ptr_sq ++)
       {
         *ptr_sq = SQUARE(*ptr_r);
       } 
       break;
     } /* NUM_REAL */

     case(NUM_COMPLEX):
     {
       for(ptr_r = M->rel+1, ptr_i = M->iel+1, ptr_sq = Maux->rel+1,
           ptr_end = M->rel + M->cols*M->rows;
           ptr_r <= ptr_end; 
           ptr_r ++, ptr_i ++, ptr_sq ++)
       {
         *ptr_sq = SQUARE(*ptr_r) + SQUARE(*ptr_i);
       }
       break;
     } /* NUM_COMPLEX */
   }   /* switch */
 }     /* else */

/*
   Copy Maux to Msq and return
*/
   Msq = matcop(Msq, Maux);
   matfree(Maux);
   return(Msq);
} /* end of function matsqmod */
