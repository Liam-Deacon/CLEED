/*********************************************************************
  GH/20.01.95
  
  matarralloc 
     Allocate an array of matrices memory for a cols x rows matrix.

  Changes:
  
  GH/20.01.95 - Create (copy from matalloc)

*********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "mat.h"

/*======================================================================*/
/*======================================================================*/

mat matarralloc(mat M, int length)
/*********************************************************************

  Allocate memory for an array of matrices. Only the magic number of the
  matrices and the block type will be set to the right value. All other 
  matrix parameters have to be set afterwards (e.g. using matalloc).
  
  INPUT
    mat M
    int length number of matrices in the array.

  RETURN VALUE: 
    pointer to the matrix array (mat) (if successful)
    NULL else.

*********************************************************************/
{
int mat_ch, i_mat;

 if( (mat_ch = matcheck(M)) < 0) 
 {

/*********************************************************************
  Check the validity of the pointer M and abort if not valid
*********************************************************************/
 
#ifdef ERROR
   fprintf(STDERR," *** error (matarralloc): Invalid pointer \n");
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(NULL);
#endif
 }
 else if(mat_ch > 0)
 {

/*********************************************************************
  If pointer M is valid, check if the existing array matches with
  the input parameter length.
*********************************************************************/
 
   for(i_mat = 0; 
       ( (M+i_mat)->mag_no == MATRIX) && 
       ( (M+i_mat)->blk_type == BLK_ARRAY);
       i_mat ++)
     ;
   if ( (i_mat == length)  &&  ( (M+i_mat)->mag_no == MATRIX) && 
        ((M+i_mat)->blk_type  == BLK_END) 
      )
/* return M, if matching */
     return(M);
   else
   {
/* free attached memory (including terminator), if M is not matching */
     for(i_mat = 0; 
         ( (M+i_mat)->mag_no == MATRIX) && 
         ( (M+i_mat)->blk_type == BLK_ARRAY);
         i_mat ++)
       matfree(M+i_mat);
     matfree(M+i_mat);
   }
 }   /* else if mat_ch > 0 */

/*********************************************************************
  If pointer M is NULL (mat_ch = 0) or valid but the array length did
  not match, allocate new array.
*********************************************************************/

#ifdef CONTROL
   fprintf(STDCTR,"(matarralloc): create new matrix array of length %d\n",
           length);
#endif

 M = ( mat )malloc( (length+1) * sizeof(struct mat_str));

/* Set magic numbers and blk_type for the array elements (and reset r/iel) */

 for(i_mat = 0; i_mat < length; i_mat ++)
 {
   (M+i_mat)->mag_no = MATRIX;
   (M+i_mat)->blk_type = BLK_ARRAY;
   (M+i_mat)->rel = (M+i_mat)->iel = NULL;
 }
 
/* Set magic numbers and blk_type for the terminator */
 (M+length)->mag_no = MATRIX; 
 (M+length)->blk_type = BLK_END; 
 (M+i_mat)->rel = (M+i_mat)->iel = NULL;

 return(M);

}  /* end of function matarralloc */
