/*********************************************************************
  GH/20.01.95
  
  matarrfree 
     free a complete array of matrices.

  Changes:
  
  GH/20.01.95 - Create

*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "mat.h"


/*======================================================================*/
/*======================================================================*/

int matarrfree(mat M)
/*********************************************************************

  Free a complete array of matrices previously allocated by matarralloc.
  
  INPUT
    mat M - pointer to a matrix array (terminated by blk_type = BLK_END).

  RETURN VALUE: 
    return value: 1 if successful, 0 if failed.

*********************************************************************/
{
int i_mat;

/*********************************************************************
  Check the validity of the pointer M
*********************************************************************/
 
 if ( matcheck(M) < 1)
 {
#ifdef ERROR
   fprintf(STDERR," *** error (matarrfree): improper input \n");
#endif
   return(0);
 }

/*********************************************************************
  If pointer M is valid, go through the existing array and free memory
  allocated for the single matrices.
  Finally free the array itself.
*********************************************************************/
 
 for(i_mat = 0; 
     ( (M+i_mat)->mag_no == MATRIX) && ( (M+i_mat)->blk_type == BLK_ARRAY);
     i_mat ++)
   matfree(M+i_mat);

 free(M);
 return(1);
}  /* end of function matarrfree */
