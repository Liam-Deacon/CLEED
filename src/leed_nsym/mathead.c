/*********************************************************************
  GH/10.06.94
  
  mathead (int cols, int rows, int num_type)
     allocate memory for a cols x rows matrix of type num_type.
     return value: pointer to the matrix (mat) (is successful)
                   NULL else.

*********************************************************************/

#include <stdio.h>
#include "mat.h"

/*!
 * Allocates memory for a \p cols by \p rows matrix of type \p num_type .
 *
 * \param cols Number of columns to allocate for matrix.
 * \param rows Number of rows to allocate for matrix.
 * \param num_type Type of matrix e.g. #MAT_SQUARE or #MAT_NORMAL
 * \see matalloc()
 * \return Pointer to allocated matrix.
 * \retval \c NULL if unable to allocate memory.
 */
mat mathead(size_t cols, size_t rows, int num_type)
{
  mat M;

  if (M = (mat)malloc(sizeof(struct mat_str))) == NULL) return(NULL);
 
  M->cols = cols;
  M->rows = rows;
  if (M->cols == M->rows) M->mat_type = MAT_SQUARE;
  else                    M->mat_type = MAT_NORMAL;

  M->num_type = num_type;

  M->iel = NULL;
  M->rel = NULL;
 
  return(M);
} /* end of function mathead */
