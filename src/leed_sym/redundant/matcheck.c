/*********************************************************************
  GH/26.08.94
  
  matcheck
     Check if a pointer points to a matrix type

*********************************************************************/

#include <stdio.h>
#include "mat.h"


/*======================================================================*/
/*======================================================================*/

int matcheck(mat M)

/*********************************************************************

  Check if a pointer points to a matrix type.
  
  INPUT
    mat M

  RETURN VALUE: 
    1  M points to a matrix structure.
    0  M is a NULL-pointer.
   -1  M does not point to a matrix structure.

*********************************************************************/
{

 if (M == NULL)           
   return(0);
 else 
 {
   if (M->mag_no == MATRIX) 
     return(1);
   else 
     return(-1);
 }
}  /* end of function matcheck */
