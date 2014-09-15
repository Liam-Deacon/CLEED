/*********************************************************************
  GH/08.06.94 
  file contains function:

  matinv

Changes
GH/08.06.94 - Creation
GH/20.07.95 - Change call of function c_luinv

*********************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mat.h"

/********************************************************************/

mat matinv( mat A_1, mat A)

/*********************************************************************
  Invert a matrix by LU decomposition (real or complex).

  parameters:
  A_1- input: pointer to the inverse matrix.
  A  - input: pointer to the matrix to be inverted.

  return value:
     pointer to the inverse matrix.
 
*********************************************************************/

{
int i, d, n;
int *indx;

mat Alu;

 Alu = NULL;

/********************************************************************* 
  check input matrix 
*********************************************************************/
/* check type of input matrix */
 if ( (A->mat_type != MAT_SQUARE) && (A->mat_type != MAT_DIAG) )
 {
#ifdef ERROR
  fprintf(STDERR," *** error (matinv): improper input type of matrix\n");
#endif
  return(NULL);
 }

/* check dimensions of input matrix A */
 else if (A->cols != A->rows) 
 {
#ifdef ERROR
  fprintf(STDERR,
          " *** error (matinv): dimensions of input matrix do not match\n");
#endif
  return(NULL);
 }
 n = A->cols;
 
/*********************************************************************
  Backup of input matrix A
*********************************************************************/

/* Copy A into temporary storage Alu */

 Alu = matcop(Alu, A);

/* Allocate A_1 (if it does not exist.) */ 

 A_1 = matalloc( A_1, n, n, Alu->num_type);

/*********************************************************************
  DIAGONAL MATRIX
*********************************************************************/

/*
 If the matrix is diagonal, the inversion is trivial 
 (but currently not implemented)
*/

 if (A->mat_type == MAT_DIAG)
 {
#ifdef ERROR
   fprintf(STDERR,
   " *** error (matinv): inversion of diagonal matrices is not implemented \n");
#endif
   matfree(A_1);
   return(NULL);
 }

/*********************************************************************
  NON-DIAGONAL MATRIX
*********************************************************************/

 indx = (int *)calloc( (n+1), sizeof(int));

 switch(Alu->num_type)
 {
                    /*******************************
                         Real matrix (NUM_REAL)
                    *******************************/
   case (NUM_REAL):
   { 
     if( (d = r_ludcmp(Alu->rel, indx, n)) == 0)
     {
#ifdef ERROR
       fprintf(STDERR,
          " *** error (matinv): LU decomposition (r_ludcmp)\n");
#endif
       matfree(A_1);
       matfree(Alu);
       return(NULL);
     }
#ifdef CONTROL
     fprintf(STDCTR, " (matinv) indx: ");
     for (i=1; i<=n; i++) fprintf(STDCTR," %d", indx[i]);
     fprintf(STDCTR,"\n");
#endif

     if( r_luinv (A_1->rel, Alu->rel, indx, n) == 0)
     {
#ifdef ERROR
       fprintf(STDERR,
          " *** error (matinv): LU backsubstitution r1 (r_luinv)\n");
#endif
       matfree(Alu);
       return(NULL);
     }
     matfree(Alu);
     break;
   }  /* REAL */

                    /*******************************
                       Complex matrix (NUM_COMPLEX)
                    *******************************/

   case (NUM_COMPLEX):
   {
     if( (d = c_ludcmp(Alu->rel, Alu->iel, indx, n)) == 0)
     {
#ifdef ERROR
       fprintf(STDERR,
          " *** error (matinv): LU decomposition (r_ludcmp)\n");
#endif
       matfree(A_1);
       matfree(Alu);
       return(NULL);
     }
#ifdef CONTROL
     fprintf(STDCTR, " (matinv) indx: ");
     for (i=1; i<=n; i++) fprintf(STDCTR," %d", indx[i]);
     fprintf(STDCTR,"\n");
#endif


     if( c_luinv (A_1->rel, A_1->iel, Alu->rel, Alu->iel, indx, n) == 0 )
     {
#ifdef ERROR
       fprintf(STDERR,
          " *** error (matinv): LU backsubstitution r1 (r_luinv)\n");
#endif
       matfree(Alu);
       return(NULL);
     }

     matfree(Alu);
     break;
   }  /* COMPLEX */

 }   /* switch num_type */ 
 free(indx);
 return(A_1);
}
/********************************************************************/
