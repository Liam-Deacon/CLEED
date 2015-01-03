/*********************************************************************
 *                        MATEXT.C
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/04.08.95 - Creation (copy from matins)
 *   GH/07.09.95 - bug fix: free Maux.
 *********************************************************************/

/*! \file
 *
 * Implements matext() function to extract a sub-matrix.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mat.h"

/*!
 * Extracts a sub-matrix from a larger one.
 *
 * \param[out] Msm Pointer to the extracted sub-matrix.
 * \param[in] Mbg Pointer to the input matrix.
 * \param off_row Starting row number of \p Mbg to extract.
 * \param end_row Ending row number of \p Mbg to extract.
 * \param off_col Starting column number of \p Mbg to extract.
 * \param end_col Ending row number of \p Mbg to extract.
 * \return Pointer to the extracted sub-matrix
 * \retval \c NULL if unsuccessful and #EXIT_ON_ERROR is not defined.
 */
mat matextract(mat Msm, const mat Mbg,
               size_t off_row, size_t end_row, size_t off_col, size_t end_col)
{
  size_t size;
  register real *ptr_bg, *ptr_sm;
  real *ptr_end;
  mat Maux = NULL;

  /* Check the input matrix */
  if (matcheck(Mbg) < 1)
  {
    ERROR_MSG("input matrix Mbg does not exist\n");
    ERROR_RETURN(NULL);
  }
 
  if (matcheck(Msm) < 0)
  {
    ERROR_MSG("improper matrix Msm\n");
    ERROR_RETURN(NULL);
  }

  if( (off_row > end_row)   ||
      (off_col > end_col)   ||
      (off_row < 1)         ||
      (off_col < 1)         ||
      (end_row > Mbg->rows) ||
      (end_col > Mbg->cols)
   )
  {
    ERROR_MSG("matrix indices do not match:\n");
              "\trows: %d -> %d (%d)\tcols:%d -> %d (%d)\n",
              off_row, end_row, Mbg->rows, off_col, end_col, Mbg->cols);
    ERROR_RETURN(NULL);
  }

  /* Diagonal Matrix: */
  if (Mbg->mat_type == MAT_DIAG)
  {
    ERROR_MSG("diagonal input matrix not implemented.\n");
    ERROR_RETURN(NULL);
  } /* if diagonal */

  /* Other matrix types: */
  else
  {
    Maux = matalloc(Maux, end_row-off_row+1, end_col-off_col+1, Mbg->num_type);
    size = Maux->cols*sizeof(real);

    /* Copy real parts first (for real and complex matrices) */
    ptr_end = Maux->rel + Maux->cols*Maux->rows;
    for(ptr_bg = Mbg->rel + (off_row-1)*Mbg->cols + off_col,
        ptr_sm = Maux->rel+1;
        ptr_sm <= ptr_end;
        ptr_bg += Mbg->cols, ptr_sm += Maux->cols)
    {
      memcpy(ptr_sm, ptr_bg, size );
    }

    if(Mbg->num_type == NUM_COMPLEX)
    {
      /* For complex matrix copy also imaginary parts. */
      ptr_end = Maux->iel + Maux->cols*Maux->rows;
      for(ptr_bg = Mbg->iel + (off_row-1)*Mbg->cols + off_col,
          ptr_sm = Maux->iel+1;
          ptr_sm <= ptr_end;
          ptr_bg += Mbg->cols, ptr_sm += Maux->cols)
      {
        memcpy(ptr_sm, ptr_bg, size );
      }
    } /* NUM_COMPLEX */

  } /* else */

  Msm = matcopy(Msm, Maux);
  matfree(Maux);

  return(Msm);
} /* end of function matext */
