/*********************************************************************
 *                      MATCOP.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *  GH/16.08.94 - Check if M_dst = M_src;
 *********************************************************************/

/*! \file
 *
 * Contains matcopy() function which copies a matrix.
 */

#include <math.h>   
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "mat.h"

/*!
 * Copies matrix \p M_src into \p M_dst
 *
 * \param[out] M_dst Pointer to the destination matrix. If this is \c NULL , the
 * memory will be allocated. In any case, the memory where the old matrix
 * elements of M_dst are stored will be freed and reallocated.
 * \param[in] M_src Pointer to the source matrix.
 * \return Pointer to the destination matrix.
 * \retval \c NULL if failed or is \p M_src is a \c NULL -pointer (and
 * #EXIT_ON_ERROR is not defined).
 */
mat matcopy(mat M_dst, const mat M_src )
{

  long int size;

  /* Check input matrix */
  if (M_dst == M_src) return(M_src); /* nothing to be done */

  /* check validity of the input matrices M_dst and M_src */
  if ( (matcheck(M_src) < 1) || (matcheck(M_dst) < 0) )
  {
    ERROR_MSG("invalid input matrix\n");
    ERROR_RETURN(NULL);
  }

  /* Check if M_dst == M_src:
   * No copying necessary; return M_dst
   */
  if (M_dst == M_src) return(M_src);

  /* Check if M_dst exists.
   * If not, allocate memory for the matrix structure
   * Free the matrix elements.
   */
  if (matcheck(M_dst) == 0)
  {
    CONTROL_MSG(CONTROL, "M_dst = NULL \n");

    M_dst = ( mat )malloc( sizeof( struct mat_str ));
    M_dst->rel = NULL;
    M_dst->iel = NULL;
  }

  if (M_dst->rel != NULL)
  {
    free(M_dst->rel);
    M_dst->rel = NULL;
  }
  if (M_dst->iel != NULL)
  {
    free(M_dst->iel);
    M_dst->iel = NULL;
  }

  /* Copy matrix parameters */
  M_dst->num_type =  M_src->num_type;
  M_dst->mat_type =  M_src->mat_type;

  M_dst->cols = M_src->cols;
  M_dst->rows = M_src->rows;

  /* Find size of matrix */
  switch(M_src->mat_type)
  {
    case (MAT_DIAG):
    {
      size = (M_src->cols + 1)*sizeof(real);
      break;
    }
    default:
    {
      size = ((M_src->rows * M_src->cols) + 1)*sizeof(real);
      break;
    }
  }

  /* Allocate memory and copy matrix elements */
  if(M_src->num_type == NUM_REAL)
  {
    /* real matrix */
    M_dst->rel = (real *)malloc( size );

    memcpy(M_dst->rel, M_src->rel, size );
  }
  else if (M_src->num_type == NUM_COMPLEX)
  {
    /* complex matrix */
    M_dst->rel = (real *)malloc( size );
    M_dst->iel = (real *)malloc( size );

    memcpy(M_dst->rel, M_src->rel, size );
    memcpy(M_dst->iel, M_src->iel, size );
  }
  else
  {
    matfree(M_dst);

    ERROR_MSG("improper num_type input\n");
    ERROR_RETURN(NULL);
  }

  /* Set magic number and return M_dst */
  M_dst->mag_no = MATRIX;

  return(M_dst);
}  /* end of function matcopy */
