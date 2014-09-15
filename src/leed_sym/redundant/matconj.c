/*********************************************************************
  GH/15.08.94
  
  mattrans
     Transpose and conjugate a matrix

  Changes:
  
*********************************************************************/

#include <stdio.h>
#include "mat.h"

#define DIA_COMPLEX -999 + NUM_COMPLEX
#define DIA_REAL    -999 + NUM_REAL

/*======================================================================*/
/*======================================================================*/

mat matconj(mat Mt, mat M)
/*********************************************************************

  Transpose a matrix
  
  INPUT
    mat Mt
    mat M

  DESIGN
    mat-types: MAT_DIAG, MAT_NORMAL, MAT_SQUARE.
     
  RETURN VALUE: 
    pointer to the matrix (mat) (if successful)
    NULL else.

*********************************************************************/
{
int i_r;
register real *ptr_o, *ptr_t;
real *ptr_o_end;

mat Maux;

/*
  Check the input matrix
*/
 if (matcheck(M) < 1)
 {
#ifdef ERROR
  fprintf(STDOUT," *** error (matconj): matrix does not exist \n");
#endif
  return(NULL);
 }

/*
 First diagonal matrices
 A diagonal matrix needs just to be conjugated
*/
 if (M->mat_type == MAT_DIAG) 
 {
   switch(M->num_type)
   {
     case(NUM_REAL):
     {
   /*
     No changes necessary for real diagonal matrices.
   */
       Mt = matcop(Mt,M);
       return(Mt);
       break;
     } /* NUM_REAL */

     case(NUM_COMPLEX):
     {
   /*
     Only conj. complex for complex diagonal matrices.
   */
       Mt = matcop(Mt,M);
       ptr_o_end = Mt->iel+Mt->rows;
       for(ptr_o = Mt->iel+1; ptr_o <= ptr_o_end; ptr_o ++)
       {
         *ptr_o = - (*ptr_o);
       }
       return(Mt);
       break;       
     }  /* NUM_COMPLEX */
   }  /* switch M->num_type */
 }  /* if diagonal */

/*
 Now nondiagonal matrices
*/

 else  /* not diagonal */
 {

   switch(M->num_type)
   {
     case(NUM_REAL):
     {
   /*
     Only transposition of a real matrix
   */
       Mt = mattrans(Mt, M);
       return(Mt);
       break;
     } /* NUM_REAL */

     case(NUM_COMPLEX):
     {
   /*
     Transposition and conjugation of a complex matrix
   */
       Maux = matcop(NULL,M);
       Mt = matalloc(Mt, Maux->cols, Maux->rows, Maux->num_type);
   /* 
     rel: only transposition
   */
       ptr_o_end = Maux->rel;
       for(i_r = 1; i_r <= Maux->rows; i_r ++)
       {
         ptr_o = ptr_o_end + 1;
         ptr_o_end += Maux->cols;
         ptr_t = Mt->rel + i_r;
         for(; ptr_o <= ptr_o_end; ptr_o ++, ptr_t += Maux->rows)
         {
           *ptr_t = *ptr_o;
         }
       } /* for i_r */

   /* 
     iel: transposition and conjugation
   */
       ptr_o_end = Maux->iel;
       for(i_r = 1; i_r <= Maux->rows; i_r ++)
       {
         ptr_o = ptr_o_end + 1;
         ptr_o_end += Maux->cols;
         ptr_t = Mt->iel + i_r;
         for(; ptr_o <= ptr_o_end; ptr_o ++, ptr_t += Maux->rows)
         {
           *ptr_t = -(*ptr_o);
         }
       } /* for i_r */
       matfree(Maux);
       return(Mt);
       break;
     } /* NUM_COMPLEX */
   }  /* switch */
 } /* else */
} /* end of function matconj */
