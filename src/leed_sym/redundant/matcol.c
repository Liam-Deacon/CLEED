/*********************************************************************
  GH/23.01.95
  
  mat matcol(mat col, mat M, int col_num)
  
  Extract a single column from a matrix M

  Changes:
  GH/23.01.95 - diagonal matrices are implemented.

*********************************************************************/
#include <math.h>  
#include <stdio.h>
#include <stdlib.h>

#include "mat.h"


mat matcol(mat col, mat M, int col_num)

/* 
  Extract a single column (col_num) from a matrix M.

  parameters:
  col - (output) pointer to the output column, will be created, if NULL.
  M - input matrix.
  col_num - (input) number of column.

  RETURN VALUE:  
  
    col
    NULL         if failed (and EXIT_ON_ERROR is not defined)
*/

{
real *ptr_r, *ptr_M, *ptr_end;
  
/********************************************************************* 
  Check input matrix and column number
*********************************************************************/

/* check validity of the input matrices M1 and M2 */
 if (matcheck(M) < 1)
 {
#ifdef ERROR
   fprintf(STDERR," *** error (matcol): invalid input matrix\n");
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(NULL);
#endif
 }

 if ( (col_num > M->cols) || (col_num < 1) )
 {
#ifdef ERROR
   fprintf(STDERR," *** error (matcol): invalid column number\n");
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(NULL);
#endif
 }

/********************************************************************* 
  Extract column
*********************************************************************/

 col = matalloc(col, M->rows, 1, M->num_type);

 if ( (M->mat_type == MAT_NORMAL) || (M->mat_type == MAT_SQUARE) )
 {
   switch(M->num_type)
   {
     case(NUM_REAL):
     {
       for (ptr_M = M->rel + col_num, 
            ptr_r = col->rel + 1, ptr_end = M->rel + M->rows*M->cols; 
            ptr_M <= ptr_end; ptr_r ++, ptr_M += M->cols)
       { *ptr_r = *ptr_M; } 

       break;
     } /* case REAL */

     case(NUM_COMPLEX):
     {
     /* first real part */
       for (ptr_M = M->rel + col_num,
            ptr_r = col->rel + 1, ptr_end = M->rel + M->rows*M->cols;
            ptr_M <= ptr_end; ptr_r ++, ptr_M += M->cols)
       { *ptr_r = *ptr_M; }

    /* now imaginary part */
       for (ptr_M = M->iel + col_num,
            ptr_r = col->iel + 1, ptr_end = M->iel + M->rows*M->cols;
            ptr_M <= ptr_end; ptr_r ++, ptr_M += M->cols)
       { *ptr_r = *ptr_M; }

       break;
     } /* case COMPLEX */
   }   /* switch */
 }     /* matrix type is not diagonal */
 
 else if (M->mat_type == MAT_DIAG)
 {
   switch(M->num_type)
   {
     case(NUM_REAL):
     {
       for ( ptr_r = col->rel + 1, ptr_end = ptr_r + M->rows;
             ptr_r <= ptr_end; ptr_r ++)
       { *ptr_r = 0.; }

       *(col->rel + col_num) = *(M->rel + col_num);
       break;
     } /* case REAL */

     case(NUM_COMPLEX):
     {
     /* first real part */
       for ( ptr_r = col->rel + 1, ptr_end = ptr_r + M->rows;
             ptr_r <= ptr_end; ptr_r ++)
       { *ptr_r = 0.; }

       *(col->rel + col_num) = *(M->rel + col_num);

    /* now imaginary part */
       for ( ptr_r = col->iel + 1, ptr_end = ptr_r + M->rows;
             ptr_r <= ptr_end; ptr_r ++)
       { *ptr_r = 0.; }

       *(col->iel + col_num) = *(M->iel + col_num);

       break;
     } /* case COMPLEX */
   }   /* switch */
 }     /* matrix type is diagonal */

 else /* neither square nor normal, nor diagonal matrix */
 {
/********************************************************************* 
  matrix type is not implemented!
*********************************************************************/

#ifdef ERROR
   fprintf(STDERR," *** error (matcol): matrix type 0x%x not implemented\n",
           M->mat_type);
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   matfree(col);
   return(NULL);
#endif
 }
 return(col);
}  /* end of function matcol */
