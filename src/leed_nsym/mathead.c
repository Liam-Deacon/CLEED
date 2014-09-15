/*********************************************************************
  GH/10.06.94
  
  mathead (int cols, int rows, int num_type)
     allocate memory for a cols x rows matrix of type num_type.
     return value: pointer to the matrix (mat) (is successful)
                   NULL else.

*********************************************************************/

#include <stdio.h>
#include "mat.h"


mat mathead(int cols, int rows, int num_type)
/* 
  allocate memory for a matrix
*/
{
mat M;

 M = (mat)malloc(sizeof(struct mat_str));
 
 M->cols = cols;
 M->rows = rows;
 if (M->cols == M->rows) M->mat_type = MAT_SQUARE;
 else                    M->mat_type = MAT_NORMAL;

 M->num_type = num_type;

 M->iel = NULL;
 M->rel = NULL;
 
 
 return(M);
}  /* end of function matalloc */
