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
    #ifdef ERROR
    fprintf(STDERR, "*** error (matread): input matrix has improper type\n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(NULL);
    #endif
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
    #ifdef ERROR
    fprintf(STDERR, "*** error (matread): input error while reading header\n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(NULL);
    #endif
  }
  tot_size = sizeof(struct mat_str);
   
  if( M->mag_no != MATRIX )
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (matread): input error (magic number)\n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(NULL);
    #endif
  }
 
  /* Diagonal Matrix: Not implemented */
  if (M->mat_type == MAT_DIAG)
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (matread): diagonal matrix not implemented.\n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(NULL);
    #endif
  } /* if diagonal */
  else /* Other matrix types: */
  {
    /* Read real parts first (for real and complex matrices) */
    n_el = M->cols * M->rows;
    size = (n_el+1) * sizeof(real);

    if( (M->rel = (real *) malloc(size)) == NULL)
    {
      #ifdef ERROR
      fprintf(STDERR, "*** error (matread): allocation error (reals)\n");
      #endif

      #ifdef EXIT_ON_ERROR
      exit(1);
      #else
      return(NULL);
      #endif
    }

    M->iel = NULL;          /* (for now, will be changed for complex M) */

    if( fread(M->rel+1, sizeof(real), n_el, file) != n_el )
    {
      #ifdef ERROR
      fprintf(STDERR, "*** error (matread): "
              "input error while reading reals\n");
      #endif

      #ifdef EXIT_ON_ERROR
      exit(1);
      #else
      return(NULL);
      #endif
    }
    tot_size += n_el * sizeof(real);

    /* For complex matrix copy also imaginary parts. */
    if(M->num_type == NUM_COMPLEX)
    {

      if( (M->iel = (real *) malloc(size)) == NULL)
      {
        #ifdef ERROR
        fprintf(STDERR, "*** error (matread): allocation error (imag.)\n");
        #endif

        #ifdef EXIT_ON_ERROR
        exit(1);
        #else
        return(NULL);
        #endif
      }

      if( fread(M->iel+1, sizeof(real), n_el, file) != n_el )
      {
        #ifdef ERROR
        fprintf(STDERR, "*** error (matread): "
                "input error while reading imag. part\n");
        #endif

        #ifdef EXIT_ON_ERROR
        exit(1);
        #else
        return(NULL);
        #endif
      }
      tot_size += n_el * sizeof(real);
    } /* NUM_COMPLEX */

  } /* else */

  #ifdef CONTROL
  fprintf(STDCTR, "(matread): %d byte read\n", tot_size);
  #endif

  return(M);
} /* end of function matread */
