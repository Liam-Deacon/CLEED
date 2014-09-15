/*********************************************************************
  GH/08.06.94

    mat matcop( mat M1, mat M2 )

  Changes:
  
  GH/16.08.94 - Check if M1 = M2;

*********************************************************************/

#include <math.h>   
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "mat.h"


mat matcop( mat M1, mat M2 )

/*********************************************************************
  Copy matrix M2 to matrix M1.

  parameters:
  M1 - pointer to the destination matrix. If this is NULL, the memory will
       be allocated. In any case, the memory where the old matrix elements
       of M1 are stroed will be freed and reallocated.

  M2 - pointers to the source matrix.

  return value: M1

*********************************************************************/

{

long size, i;

/********************************************************************* 
  Check input matrix
*********************************************************************/

/* check validity of the input matrices M1 and M2 */
 if ( (matcheck(M2) < 1) || (matcheck(M1) < 0) )
 {
#ifdef ERROR
  fprintf(STDERR," *** error (matcop): invalid input matrix\n");
#endif
#ifdef EXIT_ON_ERROR
  exit(1);
#else
  return(NULL);
#endif
 }

/* 
  Check if M1 == M2:
  No copying necessary; return M1
*/
 if (M1 == M2) return(M2);

/********************************************************************* 
  Check if M1 exists. 
  If not, allocate memory for the matrix structure 
  Free the matrix elements.
*********************************************************************/
 if (matcheck(M1) == 0)
 {
#ifdef CONTROL
  fprintf(STDCTR," (matcop) M1 = NULL \n");
#endif
  M1 = ( mat )malloc( sizeof( struct mat_str ));
  M1->rel = NULL;
  M1->iel = NULL;
 }

 if (M1->rel != NULL) {free(M1->rel); M1->rel = NULL;}
 if (M1->iel != NULL) {free(M1->iel); M1->iel = NULL;}

/********************************************************************* 
  Copy matrix parameters
*********************************************************************/
 M1->num_type =  M2->num_type;
 M1->mat_type =  M2->mat_type;

 M1->cols = M2->cols;
 M1->rows = M2->rows;

/********************************************************************* 
  Find size of matrix
*********************************************************************/
 switch(M2->mat_type)
 {
   case (MAT_DIAG): { size = (M2->cols + 1)*sizeof(real); break;}
   default:         { size = ((M2->rows * M2->cols) + 1)*sizeof(real); break;}
 }

/********************************************************************* 
  Allocate memory and copy matrix elements
*********************************************************************/
 if(M2->num_type == NUM_REAL)
 {
   /*
    real matrix
   */
   M1->rel = (real *)malloc( size );

   memcpy(M1->rel, M2->rel, size );
 }
 else if (M2->num_type == NUM_COMPLEX)
 {
   /*
    complex matrix
   */
   M1->rel = (real *)malloc( size );
   M1->iel = (real *)malloc( size );
   
   memcpy(M1->rel, M2->rel, size );
   memcpy(M1->iel, M2->iel, size );
 }
 else
 {
#ifdef ERROR
   fprintf(STDERR," *** error (matcop): improper num_type input\n");
#endif
   matfree(M1);
   return(NULL);
 }

/********************************************************************* 
  Set magic number and return M1
*********************************************************************/
  M1->mag_no = MATRIX;
  return(M1);

}  /* end of function matcop */
