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

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_cblas.h>

static gsl_matrix_t *mat_to_gsl_matrix(mat M, real *elements_ptr)
{
  size_t i_row, i_col;
  gsl_matrix *G = gsl_matrix_alloc(M->rows, M->cols);
  
  for (i_row = 0; i_row < M->rows; i_row++)
  {
    for (i_col = 0; i_col < M->cols; i_col++)
    {
      gsl_matrix_set(G, i_row, i_col, 
                     elements_ptr + (i_row - 1) * M->cols + i_col);
    }
  }
  
  return G;
}

static real *gsl_matrix_to_mat_elem_ptr(gsl_matrix *G)
{
  mat m;
  
  m->rows = G->size1;
  m->cols = G->size2;
  
  
  return m;
}

static gsl_matrix *gsl_matrix_get_inverse(gsl_matrix *M)
{
  int s;
  gsl_matrix *inverse = gsl_matrix_alloc(M->size1, M->size2);
  size_t n = ((M->size1 >= M->size2) ? M->size1 : M->size2);
  gsl_permutation *perm = gsl_permutation_alloc(n);
  
  /* perform LU decomposition of matrix M */
  gsl_linalg_LU_decomp(M, perm, &s);
  
  /* Invert the matrix */
  gsl_linalg_LU_invert(M, perm, inverse);
  
  /* free memory */
  gsl_permutation_free(perm);
  
  /* return inverse of matrix */
  return inverse;
}

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
