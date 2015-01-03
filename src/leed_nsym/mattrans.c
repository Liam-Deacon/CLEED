/*********************************************************************
 *                        MATTRANS.C
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/15.08.94 - creation
 *********************************************************************/

/*! \file
 *
 * Implements mattrans() function for computing the transpose of a matrix.
 */

#include <stdio.h>
#include <stdlib.h>
#include "mat.h"

#define DIA_COMPLEX -999 + NUM_COMPLEX
#define DIA_REAL    -999 + NUM_REAL

/*!
 * Returns the transpose \p Mt of a matrix \p M .
 *
 * \param[out] Mt The output transpose.
 * \param[in] M Input matrix to transpose.
 * \return Pointer to the matrix transpose.
 * \retval \c NULL if function is unsuccessful.
 */
mat mattrans(mat Mt, const mat M)
{
  size_t i_r;
  register real *ptr_o, *ptr_t;
  real *ptr_o_end;
  mat Maux = NULL;

  /* Check the input matrix */
  if (matcheck(M) < 1)
  {
    ERROR_MSG("matrix does not exist \n");
    ERROR_RETURN(NULL);
  }

  /* Diagonal Matrix: A diagonal matrix is its own transpose */
  if (M->mat_type == MAT_DIAG)
  {
    matcopy(M, Mt);
  }
  else
  {
    Maux = matcopy(NULL, M);
    Mt = matalloc(Mt, Maux->cols, Maux->rows, Maux->num_type);

    switch(M->num_type)
    {
      case(NUM_REAL):
      {
        ptr_o_end = Maux->rel;
        for(i_r = 1; i_r <= Maux->rows; i_r ++)
        {
          ptr_o = ptr_o_end + 1;
          ptr_o_end += Maux->cols;
          ptr_t = Mt->rel + i_r;
          for(; ptr_o <= ptr_o_end; ptr_o ++, ptr_t += Mt->cols)
          {
            *ptr_t = *ptr_o;
          }
        } /* for i_r */
        break;
      } /* NUM_REAL */

      case(NUM_COMPLEX):
      {
        /* real part */
        ptr_o_end = Maux->rel;
        for(i_r = 1; i_r <= Maux->rows; i_r ++)
        {
          ptr_o = ptr_o_end + 1;
          ptr_o_end += Maux->cols;
          ptr_t = Mt->rel + i_r;
          for(; ptr_o <= ptr_o_end; ptr_o ++, ptr_t += Mt->cols)
          {
            *ptr_t = *ptr_o;
          }
        } /* for i_r */

        /* imaginary part */
        ptr_o_end = Maux->iel;
        for(i_r = 1; i_r <= Maux->rows; i_r ++)
        {
          ptr_o = ptr_o_end + 1;
          ptr_o_end += Maux->cols;
          ptr_t = Mt->iel + i_r;
          for(; ptr_o <= ptr_o_end; ptr_o ++, ptr_t += Mt->cols)
          {
            *ptr_t = *ptr_o;
          }
        } /* for i_r */
        break;
      } /* NUM_COMPLEX */

    }  /* switch */
    matfree(Maux);
  } /* else */

  return(Mt);
} /* end of function mattrans */
