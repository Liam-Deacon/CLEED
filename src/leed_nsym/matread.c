/*********************************************************************
 *                        MATREAD.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/07.08.95 - Creation (copy from matwrite)
 *   GH/09.08.95 - Allocate mat_str for NULL input
 *********************************************************************/

/*! \file
 *
 * Implements matread() function to read matrix from a file.
 */

#include <stdio.h>
#include <malloc.h>
#include "mat.h"
#include <stdlib.h>

/*!
 * Reads matrix from a file.
 *
 * \param[out] M Pointer to the matrix (can be enter function as NULL).
 * \param file File pointer to read matrix from.
 * \return Pointer to read matrix.
 * \retval \c NULL if function was unsuccessful.
 */
mat matread(mat M, FILE *file)
{
  size_t n_el, size, tot_size;
  int check;

  /* Check the input matrix */
  if( (check = matcheck(M)) < 0)
  {
    ERROR_MSG("input matrix has improper type\n");
    ERROR_RETURN(NULL);
  }
  else
  {
    if(check == 0) M = (mat) malloc( sizeof(struct mat_str) );
    else
    {
      free(M->rel); M->rel = NULL;
      free(M->iel); M->iel = NULL;
    }
  }

  /* First read matrix header */
  if( fread(M, sizeof(struct mat_str), 1, file) != 1 )
  {
    ERROR_MSG("input error while reading header\n");
    ERROR_RETURN(NULL);
  }
  tot_size = sizeof(struct mat_str);
   
  if( M->mag_no != MATRIX )
  {
    ERROR_MSG("input error (magic number)\n");
    ERROR_RETURN(NULL);
  }
 
  /* Diagonal Matrix: Not implemented */
  if (M->mat_type == MAT_DIAG)
  {
    ERROR_MSG("diagonal matrix not implemented.\n");
    ERROR_RETURN(NULL);
  } /* if diagonal */
  else /* Other matrix types: */
  {
    /* Read real parts first (for real and complex matrices) */
    n_el = M->cols * M->rows;
    size = (n_el+1) * sizeof(real);

    if( (M->rel = (real *) malloc(size)) == NULL)
    {
      ERROR_MSG("allocation error (reals)\n");
      ERROR_RETURN(NULL);
    }

    M->iel = NULL;          /* (for now, will be changed for complex M) */

    if( fread(M->rel+1, sizeof(real), n_el, file) != n_el )
    {
      ERROR_MSG("input error while reading reals\n");
      ERROR_RETURN(NULL);
    }
    tot_size += n_el * sizeof(real);

    /* For complex matrix copy also imaginary parts. */
    if(M->num_type == NUM_COMPLEX)
    {

      if( (M->iel = (real *) malloc(size)) == NULL)
      {
        ERROR_MSG("allocation error (imag.)\n");
        ERROR_RETURN(NULL);
      }

      if( fread(M->iel+1, sizeof(real), n_el, file) != n_el )
      {
        ERROR_MSG("input error while reading imag. part\n");
        ERROR_RETURN(NULL);
      }
      tot_size += n_el * sizeof(real);
    } /* NUM_COMPLEX */

  } /* else */

  CONTROL_MSG(CONTROL, "%d byte read\n", tot_size);

  return(M);
} /* end of function matread */
