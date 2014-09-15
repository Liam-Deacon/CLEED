/*********************************************************************
  GH/08.06.94

 int matfree( mat M )
     Free the memory allocated for a matrix

 Changes:
  
 GH/26.08.94 - Remove MAT_ERROR
*********************************************************************/
#include <malloc.h>
#include "mat.h"


int matfree( mat M )

/*********************************************************************
  Free the memory allocated for a matrix

  parameters:
  M - pointer to the matrix.

  return value: 1 if successful, 0 if failed.

*********************************************************************/
{
/* 
  check input matrix
*/
 if ( matcheck(M) < 1)
 {
#ifdef ERROR
   fprintf(STDERR," *** error (matfree): improper input \n");
#endif
   return(0);
 }

 if (M->rel != NULL) free(M->rel);
 if (M->iel != NULL) free(M->iel);

 free(M);
 return(1);

}  /* end of function matfree */
