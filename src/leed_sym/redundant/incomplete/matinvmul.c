/*********************************************************************
  GH/28.06.94 
  file contains function:

  matinvmul
!!!!!!!!!!!!!!! not finished jet !!!!!!!!!!!!!!!

*********************************************************************/

#include <math.h>
#include <stdio.h>
#include "mat.h"

/********************************************************************/

mat matinvmul( mat X, mat A, mat B)

/*
  Invert a matrix by LU decomposition (real or complex).

  parameters:
  A_1- input: pointer to the inverse matrix.
  A  - input: pointer to the matrix to be inverted.

  return value:
     pointer to the inverse matrix.
 
*/

{
int i, d, n;
int *indx;

mat Alu;

/* 
  check type of input matrix 
*/
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

/* 
  check if A_1 exists. If not, allocate memory for the matrix structure 
*/
 if (A_1 == NULL)
 {
#ifdef CONTROL
  fprintf(STDCTR," (matinv) A_1 = NULL \n");
#endif
  A_1 = ( mat )malloc( sizeof( struct mat_str ));
  A_1->rel = NULL;
  A_1->iel = NULL;
 }

 A_1->rows = A_1->cols = n;
 
/*
 If the matrix is diagonal, the inversion is trivial 
 (currently not implemented)
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

 /* else (not diagonal) */
 A_1->mat_type = MAT_SQUARE;

 if (A_1->rel != NULL) {free(A_1->rel); A_1->rel = NULL;}
 if (A_1->iel != NULL) {free(A_1->iel); A_1->iel = NULL;}

 indx = (int *)malloc( (n+1)*sizeof(int));

/* Copy A into temporary storage Alu */

 Alu = matcop(NULL, A);
 

 switch(A->num_type)
 {
/*
 real matrix
*/
   case (NUM_REAL):
   { 
     A_1->num_type = NUM_REAL;

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

     if( (A_1->rel = r_luinv (Alu->rel, indx, n)) == NULL)
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

/*
 complex matrix
*/

   case (NUM_COMPLEX):
   {
     A_1->num_type = NUM_COMPLEX;

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


     if( (A_1 = c_luinv (Alu->rel, Alu->iel, indx, n)) == NULL)
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
