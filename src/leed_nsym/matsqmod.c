/*********************************************************************
 *                        MATSQMOD.C
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
 * Calculate the square of the modulus of each matrix element.
 */

#include <stdio.h>
#include "mat.h"
#include <stdlib.h>

#define DIA_COMPLEX -999 + NUM_COMPLEX
#define DIA_REAL    -999 + NUM_REAL

/*!
 * Calculates the square of the modulus of each element in a matrix.
 *
 * \param[out] Msq Pointer to the resultant matrix.
 * \param[in] M Pointer to the input matrix.
 * \return Pointer to the result matrix.
 * \retval \c NULL if the function is unsuccessful.
 */
mat matsqmod(mat Msq, const mat M)
{
  register real *ptr_r, *ptr_i, *ptr_sq;
  real *ptr_end;
  mat Maux;

  /* Check the input matrix */
  if (matcheck(M) < 1)
  {
    ERROR_MSG("input matrix does not exist \n");
    ERROR_RETURN(NULL);
  }

  /* Diagonal Matrix: */
  if (M->mat_type == MAT_DIAG)
  {
    Maux = matalloc(NULL, M->rows, M->cols, NUM_REAL | MAT_DIAG);

    switch(M->num_type)
    {
      case(NUM_REAL):
      {
        for(ptr_r = M->rel+1, ptr_sq = Maux->rel+1,
            ptr_end = M->rel + M->cols;
            ptr_r <= ptr_end;
            ptr_r ++, ptr_sq ++)
        {
          *ptr_sq = SQUARE(*ptr_r);
        }
        break;
      } /* NUM_REAL */

      case(NUM_COMPLEX):
      {
        for(ptr_r = M->rel+1, ptr_i = M->iel+1, ptr_sq = Maux->rel+1,
            ptr_end = M->rel + M->cols;
            ptr_r <= ptr_end;
            ptr_r ++, ptr_i ++, ptr_sq ++)
        {
          *ptr_sq = SQUARE(*ptr_r) + SQUARE(*ptr_i);
        }
        break;
      } /* NUM_COMPLEX */
    } /* switch */
  } /* if diagonal */

  /* Other matrix types: */
  else
  {
    Maux = matalloc(NULL, M->rows, M->cols, NUM_REAL);

    switch(M->num_type)
    {
      case(NUM_REAL):
      {
        for(ptr_r = M->rel+1, ptr_sq = Maux->rel+1,
            ptr_end = M->rel + M->cols*M->rows;
            ptr_r <= ptr_end;
            ptr_r ++, ptr_sq ++)
        {
          *ptr_sq = SQUARE(*ptr_r);
        }
        break;
      } /* NUM_REAL */

      case(NUM_COMPLEX):
      {
        for(ptr_r = M->rel+1, ptr_i = M->iel+1, ptr_sq = Maux->rel+1,
            ptr_end = M->rel + M->cols*M->rows;
            ptr_r <= ptr_end;
            ptr_r ++, ptr_i ++, ptr_sq ++)
        {
          *ptr_sq = SQUARE(*ptr_r) + SQUARE(*ptr_i);
        }
        break;
      } /* NUM_COMPLEX */
    } /* switch */
  } /* else */

  /* Copy Maux to Msq and return */
  Msq = matcopy(Msq, Maux);
  matfree(Maux);

  return(Msq);
} /* end of function matsqmod */
