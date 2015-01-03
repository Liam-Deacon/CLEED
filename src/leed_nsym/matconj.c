/*********************************************************************
 *                        MATCONJ.C
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
 * Implements matconj() function for calculating the conjugate of of a matrix.
 */

#include <stdio.h>
#include "mat.h"

#define DIA_COMPLEX -999 + NUM_COMPLEX
#define DIA_REAL    -999 + NUM_REAL

/*!
 * Returns the transpose and conjugate \p Mt of a matrix \p M .
 *
 * \param[out] Mt Pointer to the transpose & conjugate output matrix.
 * \param[in] M Pointer to the input matrix.
 * \return Pointer to the transpose and conjugate matrix.
 * \retval \c NULL if function is unsuccessful.
 */
mat matconj(mat Mt, const mat M)
{
  size_t i_r;
  register real *ptr_o, *ptr_t;
  real *ptr_o_end;

  mat Maux = NULL;

  /* Check the input matrix */
  if (matcheck(M) < 1)
  {
    ERROR_MSG("matrix does not exist \n");
    return(NULL);
  }

  /* First diagonal matrices: A diagonal matrix needs just to be conjugated */
  if (M->mat_type == MAT_DIAG)
  {
    switch(M->num_type)
    {
      case(NUM_REAL):
      {
        /* No changes necessary for real diagonal matrices. */
        Mt = matcopy(Mt,M);
        return(Mt);
        break;
      } /* NUM_REAL */

      case(NUM_COMPLEX):
      {
        /* Only conjugate complex for complex diagonal matrices. */
        Mt = matcopy(Mt,M);
        ptr_o_end = Mt->iel+Mt->rows;
        for(ptr_o = Mt->iel+1; ptr_o <= ptr_o_end; ptr_o ++)
        {
          *ptr_o = - (*ptr_o);
        }
        return(Mt);
        break;
      }  /* NUM_COMPLEX */
    }  /* switch M->num_type */
  }  /* if diagonal */

  /* Now non-diagonal matrices */
  else  /* not diagonal matrix */
  {
    switch(M->num_type)
    {
      case(NUM_REAL):
      {
        /* Only do transposition for real matrix */
        Mt = mattrans(Mt, M);
        return(Mt);
        break;
      } /* NUM_REAL */

      case(NUM_COMPLEX):
      {
        /* Do transposition and conjugation for a complex matrix */
        Maux = matcopy(NULL,M);
        Mt = matalloc(Mt, Maux->cols, Maux->rows, Maux->num_type);

        /* real part: only transposition */
        ptr_o_end = Maux->rel;
        for(i_r = 1; i_r <= Maux->rows; i_r ++)
        {
          ptr_o = ptr_o_end + 1;
          ptr_o_end += Maux->cols;
          ptr_t = Mt->rel + i_r;
          for(; ptr_o <= ptr_o_end; ptr_o ++, ptr_t += Maux->rows)
          {
            *ptr_t = *ptr_o;
          }
        } /* for i_r */

        /* Imaginary part: transposition and conjugation */
        ptr_o_end = Maux->iel;
        for(i_r = 1; i_r <= Maux->rows; i_r ++)
        {
          ptr_o = ptr_o_end + 1;
          ptr_o_end += Maux->cols;
          ptr_t = Mt->iel + i_r;
          for(; ptr_o <= ptr_o_end; ptr_o ++, ptr_t += Maux->rows)
          {
            *ptr_t = -(*ptr_o);
          }
        } /* for i_r */
        matfree(Maux);
        return(Mt);
        break;
      } /* NUM_COMPLEX */

    }  /* switch */

  } /* else */

  return(NULL);
} /* end of function matconj */
