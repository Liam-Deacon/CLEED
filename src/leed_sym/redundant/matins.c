/*********************************************************************
  GH/02.02.95
  
  matins
     Insert a matrix into a larger one

  Changes:
  
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mat.h"


#define DIA_COMPLEX -999 + NUM_COMPLEX
#define DIA_REAL    -999 + NUM_REAL

/*======================================================================*/
/*======================================================================*/

mat matins(mat Mbg, mat Msm, int off_row, int off_col)
/*********************************************************************

  Transpose a matrix
  
  INPUT
    mat Mbg  - (output) pointer to the result
    mat Msm  - input matrix
    int off_row, off_col position of the (1,1) element of matrix Msm in
               Mbg.

  RETURN VALUE: 
    Mbg: pointer to the matrix (mat) (if successful)
    NULL if failed (and EXIT_ON_ERROR is not defined)

*********************************************************************/
{
int i_r;
int size;
register real *ptr_bg, *ptr_sm;
real *ptr_end;
mat Maux;

/********************************************************************
  Check the input matrix
********************************************************************/

 if (matcheck(Msm) < 1)
 {
#ifdef ERROR
  fprintf(STDOUT," *** error (matins): input matrix Msm does not exist \n");
#endif
#ifdef EXIT_ON_ERROR
  exit(1);
#else
  return(NULL);
#endif
 }
 
 if (matcheck(Mbg) < 0)
 {
#ifdef ERROR
  fprintf(STDOUT," *** error (matins): improper matrix Mbg\n");
#endif
#ifdef EXIT_ON_ERROR
  exit(1);
#else
  return(NULL);
#endif
 }

 if (matcheck(Mbg) == 0)
 {
#ifdef WARNING
  fprintf(STDWAR," * warning (matins): matrix Mbg does not exist\n");
  fprintf(STDWAR,"   => allocate new matrix matching the dimensions of Msm\n");
#endif
  Mbg = matalloc(Mbg, off_row + Msm->rows, off_col + Msm->cols, Msm->num_type);
 }
 else if((off_row + Msm->rows - 1 > Mbg->rows) || 
         (off_col + Msm->cols - 1 > Mbg->cols) ||
         (Msm->num_type !=  Mbg->num_type) )
 {
#ifdef ERROR
  fprintf(STDOUT," *** error (matins): matrix dimensions/types do not match\n");
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
 if (Msm->mat_type == MAT_DIAG) 
 {
#ifdef ERROR
  fprintf(STDOUT,
   " *** error (matins): diagonal input matrix not implemented.\n");
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
   size = Msm->cols*sizeof(real);
/*
  Copy real parts first (for real and complex matrices)
*/
   ptr_end = Msm->rel + Msm->cols*Msm->rows;
   for(ptr_bg = Mbg->rel + (off_row-1)*Mbg->cols + off_col, 
       ptr_sm = Msm->rel+1;
       ptr_sm <= ptr_end;
       ptr_bg += Mbg->cols, ptr_sm += Msm->cols)
   {
     memcpy(ptr_bg, ptr_sm, size );
   }

   if(Msm->num_type == NUM_COMPLEX)
   {
/*
  For complex matrix copy also imag. parts.
*/
     ptr_end = Msm->iel + Msm->cols*Msm->rows;
     for(ptr_bg = Mbg->iel + (off_row-1)*Mbg->cols + off_col, 
         ptr_sm = Msm->iel+1;
         ptr_sm <= ptr_end;
         ptr_bg += Mbg->cols, ptr_sm += Msm->cols)
     {
       memcpy(ptr_bg, ptr_sm, size );
     }
   } /* NUM_COMPLEX */
 }     /* else */

   return(Mbg);
} /* end of function matins */
