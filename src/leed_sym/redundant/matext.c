/*********************************************************************
 GH/07.09.95
  
  matext
     Extract a submatrix from a larger one

  Changes:
GH/04.08.95 - Creation (copy from matins)
GH/07.09.95 - bug fix: free Maux.
  
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mat.h"


/*======================================================================*/
/*======================================================================*/

mat matext(mat Msm, mat Mbg, 
           int off_row, int end_row, 
           int off_col, int end_col)
/*********************************************************************

  Transpose a matrix
  
  INPUT
    mat Mbg  - (output) pointer to the result
    mat Msm  - input matrix
    int off_row, off_col position of the (1,1) element of matrix Msm in
               Mbg.
    int end_row, end_col position of the (n,m) (lower right) element
               of matrix Msm in Mbg.

  RETURN VALUE: 
    Msm: pointer to the matrix (mat) (if successful)
    NULL if failed (and EXIT_ON_ERROR is not defined)

*********************************************************************/
{
int i_r;
int size;
register real *ptr_bg, *ptr_sm;
real *ptr_end;
mat Maux = NULL;

/********************************************************************
  Check the input matrix
********************************************************************/

 if (matcheck(Mbg) < 1)
 {
#ifdef ERROR
  fprintf(STDOUT," *** error (matext): input matrix Mbg does not exist \n");
#endif
#ifdef EXIT_ON_ERROR
  exit(1);
#else
  return(NULL);
#endif
 }
 
 if (matcheck(Msm) < 0)
 {
#ifdef ERROR
  fprintf(STDOUT," *** error (matext): improper matrix Msm\n");
#endif
#ifdef EXIT_ON_ERROR
  exit(1);
#else
  return(NULL);
#endif
 }

 if( (off_row > end_row) ||
     (off_col > end_col) ||
     (off_row < 1) || 
     (off_col < 1) ||
     (end_row > Mbg->rows) || 
     (end_col > Mbg->cols)
   )
 {
#ifdef ERROR
  fprintf(STDOUT," *** error (matext): matrix indices do not match:\n");
  fprintf(STDOUT,"\trows: %d -> %d (%d)\tcols:%d -> %d (%d)\n",
          off_row, end_row, Mbg->rows, off_col, end_col, Mbg->cols);
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
 if (Mbg->mat_type == MAT_DIAG) 
 {
#ifdef ERROR
  fprintf(STDOUT,
   " *** error (matext): diagonal input matrix not implemented.\n");
#endif
#ifdef EXIT_ON_ERROR
  exit(1);
#else
  return(NULL);
#endif
 }     /* if diagonal */

/********************************************************************
 Other matrix types:
********************************************************************/
 else
 {
   Maux = matalloc(Maux, 
                   end_row - off_row + 1, 
                   end_col - off_col + 1, 
                   Mbg->num_type);

   size = Maux->cols*sizeof(real);
/*
  Copy real parts first (for real and complex matrices)
*/
   ptr_end = Maux->rel + Maux->cols*Maux->rows;
   for(ptr_bg = Mbg->rel + (off_row-1)*Mbg->cols + off_col, 
       ptr_sm = Maux->rel+1;
       ptr_sm <= ptr_end;
       ptr_bg += Mbg->cols, ptr_sm += Maux->cols)
   {
     memcpy(ptr_sm, ptr_bg, size );
   }

   if(Mbg->num_type == NUM_COMPLEX)
   {
/*
  For complex matrix copy also imag. parts.
*/
     ptr_end = Maux->iel + Maux->cols*Maux->rows;
     for(ptr_bg = Mbg->iel + (off_row-1)*Mbg->cols + off_col, 
         ptr_sm = Maux->iel+1;
         ptr_sm <= ptr_end;
         ptr_bg += Mbg->cols, ptr_sm += Maux->cols)
     {
       memcpy(ptr_sm, ptr_bg, size );
     }
   } /* NUM_COMPLEX */
 }     /* else */

 Msm = matcop(Msm, Maux);
 matfree(Maux);
 return(Msm);
} /* end of function matext */
