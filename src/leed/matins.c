/*********************************************************************
 *                        MATINS.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/02.02.95 - Creation
 *********************************************************************/

/*! \file
 *
 * Implements matins() function to insert a matrix into a larger one.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mat.h"

#define DIA_COMPLEX -999 + NUM_COMPLEX
#define DIA_REAL    -999 + NUM_REAL

/*!
 * Inserts a matrix into a larger matrix (super matrix).
 * \param[in,out] Mbg
 * \param[in] Msm Pointer to input matrix.
 * \param off_row Position of the first row of matrix \p Msm in \p Mbg
 * \param off_col Position of the first column of matrix \p Msm in \p Mbg
 * \return Pointer to the super matrix.
 * \retval \c NULL if function was unsuccessful and #EXIT_ON_ERROR is
 * not defined.
 */
mat matins(mat Mbg, const mat Msm, size_t off_row, size_t off_col)
{
  size_t size;
  register real *ptr_bg, *ptr_sm;
  real *ptr_end;

  /* Check the input matrix */
  if (matcheck(Msm) < 1)
  {
    ERROR_MSG("input matrix Msm does not exist \n");
    ERROR_RETURN(NULL);
  }
 
  if (matcheck(Mbg) < 0)
  {
    ERROR_MSG("improper matrix Mbg\n");
    ERROR_RETURN(NULL);
  }

  if (matcheck(Mbg) == 0)
  {
    WARNING_MSG("matrix Mbg does not exist\n"
            "   => allocate new matrix matching the dimensions of Msm\n");
    Mbg = matalloc(Mbg, off_row + Msm->rows,
                        off_col + Msm->cols, Msm->num_type);
  }
  else if((off_row + Msm->rows - 1 > Mbg->rows) ||
          (off_col + Msm->cols - 1 > Mbg->cols) ||
          (Msm->num_type !=  Mbg->num_type) )
  {
    ERROR_MSG("matrix dimensions/types do not match\n");
    ERROR_RETURN(NULL);
  }

  /* Diagonal Matrix: */
  if (Msm->mat_type == MAT_DIAG)
  {
    ERROR_MSG("diagonal input matrix not implemented.\n");
    ERROR_RETURN(NULL);
  } /* if diagonal */
  else /* Other matrix types: */
  {
    size = Msm->cols*sizeof(real);
    /* Copy real parts first (for real and complex matrices) */
    ptr_end = Msm->rel + Msm->cols*Msm->rows;
    for(ptr_bg = Mbg->rel + (off_row-1)*Mbg->cols + off_col,
        ptr_sm = Msm->rel+1;
        ptr_sm <= ptr_end;
        ptr_bg += Mbg->cols, ptr_sm += Msm->cols)
    {
      memcpy(ptr_bg, ptr_sm, size );
    }

    if(Msm->num_type == NUM_COMPLEX)
    {
      /* For complex matrix copy also imaginary parts. */
      ptr_end = Msm->iel + Msm->cols*Msm->rows;
      for(ptr_bg = Mbg->iel + (off_row-1)*Mbg->cols + off_col,
          ptr_sm = Msm->iel+1;
          ptr_sm <= ptr_end;
          ptr_bg += Mbg->cols, ptr_sm += Msm->cols)
      {
        memcpy(ptr_bg, ptr_sm, size );
      }
    } /* NUM_COMPLEX */

  } /* else */

  return(Mbg);
} /* end of function matins */
