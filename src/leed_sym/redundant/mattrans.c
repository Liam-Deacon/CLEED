/*********************************************************************
  GH/15.08.94
  
  mattrans
     Transpose a matrix

  Changes:
  
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "mat.h"

/*
#define ERROR
*/
#define ERROR
#define EXIT_ON_ERROR

#define DIA_COMPLEX -999 + NUM_COMPLEX
#define DIA_REAL    -999 + NUM_REAL

/*======================================================================*/
/*======================================================================*/

mat mattrans(mat Mt, mat M)
/*********************************************************************

  Transpose a matrix
  
  INPUT
    mat Mt
    mat M

  RETURN VALUE: 
    pointer to the matrix (mat) (if successful)
    NULL else.

*********************************************************************/
{
int i_r;
register real *ptr_o, *ptr_t;
real *ptr_o_end;
mat Maux;

/********************************************************************
  Check the input matrix
********************************************************************/
/*
 fprintf(STDCTR," (mattrans):\n");
 matshow(M);
 matshow(Mt);
*/

 if (matcheck(M) < 1)
 {
#ifdef ERROR
  fprintf(STDOUT," *** error (mattrans): matrix does not exist \n");
#endif
#ifdef EXIT_ON_ERROR
  exit(1);
#else
  return(NULL);
#endif
 }

/********************************************************************
 Diagonal Matrix: A diagonal matrix is its own transposed
********************************************************************/
 if (M->mat_type == MAT_DIAG) 
 {
   matcop(M,Mt); 
   return(Mt);
 }
 else
 {
   Maux = matcop(NULL,M);
   Mt = matalloc(Mt, Maux->cols, Maux->rows, Maux->num_type);

   switch(M->num_type)
   {
     case(NUM_REAL):
     {
       ptr_o_end = Maux->rel;
       for(i_r = 1; i_r <= Maux->rows; i_r ++)
       {
         ptr_o = ptr_o_end + 1;
         ptr_o_end += Maux->cols;
         ptr_t = Mt->rel + i_r;
         for(; ptr_o <= ptr_o_end; ptr_o ++, ptr_t += Mt->cols)
         {
           *ptr_t = *ptr_o;
         }
       } /* for i_r */
       break;
     } /* NUM_REAL */

     case(NUM_COMPLEX):
     {
   /* 
     rel
   */
       ptr_o_end = Maux->rel;
       for(i_r = 1; i_r <= Maux->rows; i_r ++)
       {
         ptr_o = ptr_o_end + 1;
         ptr_o_end += Maux->cols;
         ptr_t = Mt->rel + i_r;
         for(; ptr_o <= ptr_o_end; ptr_o ++, ptr_t += Mt->cols)
         {
           *ptr_t = *ptr_o;
         }
       } /* for i_r */

   /* 
     iel
   */
       ptr_o_end = Maux->iel;
       for(i_r = 1; i_r <= Maux->rows; i_r ++)
       {
         ptr_o = ptr_o_end + 1;
         ptr_o_end += Maux->cols;
         ptr_t = Mt->iel + i_r;
         for(; ptr_o <= ptr_o_end; ptr_o ++, ptr_t += Mt->cols)
         {
           *ptr_t = *ptr_o;
         }
       } /* for i_r */
       break;
     } /* NUM_COMPLEX */

   }  /* switch */
   matfree(Maux);
   return(Mt);
 } /* else */
} /* end of function mattrans */
