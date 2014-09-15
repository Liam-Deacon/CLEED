/*********************************************************************
  GH/23.01.95
  
  mat matrow(mat row, mat M, int row_num)
  
  Extract a single row from a matrix M

  Changes:
  GH/23.01.95 - diagonal matrices are implemented.

*********************************************************************/
#include <math.h>  
#include <stdio.h>
#include <stdlib.h>
#include "mat.h"

mat matrow(mat row, mat M, int row_num)

/* 
  Extract a single row (row_num) from a matrix M.

  parameters:
  row - output) pointer to the output matrix, will be created, if NULL.
  M - input matrix.
  row_num - (input) number of row.

  RETURN VALUE:  
  
    row
    NULL         if failed (and EXIT_ON_ERROR is not defined)
*/

{
real *ptr_r, *ptr_M, *ptr_end;
  
/********************************************************************* 
  Check input matrix and row number
*********************************************************************/

/* check validity of the input matrices M1 and M2 */
 if (matcheck(M) < 1)
 {
#ifdef ERROR
   fprintf(STDERR," *** error (matrow): invalid input matrix\n");
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(NULL);
#endif
 }

 if ( (row_num > M->rows) || (row_num < 1) )
 {
#ifdef ERROR
   fprintf(STDERR," *** error (matcol): invalid row number\n");
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(NULL);
#endif
 }

/********************************************************************* 
  Extract row
*********************************************************************/

 row = matalloc(row, 1, M->cols, M->num_type);

 if ( (M->mat_type == MAT_NORMAL) || (M->mat_type == MAT_SQUARE) )
 {
   switch(M->num_type)
   {
     case(NUM_REAL):
     {
       for (ptr_M = M->rel + M->cols*(row_num - 1) + 1, 
            ptr_r = row->rel + 1, ptr_end = ptr_M + M->cols; 
            ptr_M <= ptr_end; ptr_r ++, ptr_M ++)
       { *ptr_r = *ptr_M; } 

       break;
     } /* case REAL */

     case(NUM_COMPLEX):
     {
     /* first real part */
       for (ptr_M = M->rel + M->cols*(row_num - 1) + 1, 
            ptr_r = row->rel + 1, ptr_end = ptr_M + M->cols;
            ptr_M <= ptr_end; ptr_r ++, ptr_M ++)
       { *ptr_r = *ptr_M; }

    /* now imaginary part */
       for (ptr_M = M->iel + M->cols*(row_num - 1) + 1, 
            ptr_r = row->iel + 1, ptr_end = ptr_M + M->cols;
            ptr_M <= ptr_end; ptr_r ++, ptr_M ++)
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
       for ( ptr_r = row->rel + 1, ptr_end = ptr_r + M->cols;
             ptr_r <= ptr_end; ptr_r ++)
       { *ptr_r = 0.; }

       *(row->rel + row_num) = *(M->rel + row_num);
       break;
     } /* case REAL */

     case(NUM_COMPLEX):
     {
     /* first real part */
       for ( ptr_r = row->rel + 1, ptr_end = ptr_r + M->cols;
             ptr_r <= ptr_end; ptr_r ++)
       { *ptr_r = 0.; }

       *(row->rel + row_num) = *(M->rel + row_num);

    /* now imaginary part */
       for ( ptr_r = row->iel + 1, ptr_end = ptr_r + M->cols;
             ptr_r <= ptr_end; ptr_r ++)
       { *ptr_r = 0.; }

       *(row->iel + row_num) = *(M->iel + row_num);

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
   fprintf(STDERR," *** error (matrow): matrix type 0x%x not implemented\n",
           M->mat_type);
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   matfree(row);
   return(NULL);
#endif
 }
 return(row);
}  /* end of function matrow */
