/*********************************************************************
 *                        MATWRITE.C
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/07.08.95 - Creation (copy from matext)
 *********************************************************************/

/*! \file
 *
 * Implements matwrite() function for writing a matrix to file.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mat.h"

/*!
 * Writes matrix to file.
 *
 * \param[in] M Pointer to the matrix to print.
 * \param file File pointer for output stream.
 * \return total number of bytes written to \p file
 * \retval -1 if function encountered an error and #EXIT_ON_ERROR is not
 * defined.
 * \note Error and control messages can be enabled by defining #ERROR and
 * #CONTROL respectively when compiling.
 */
int matwrite(const mat M, FILE *file)
{
  size_t n_el;
  size_t tot_size;

  /* Check the input matrix */
  if (matcheck(M) < 1)
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (matwrite): input matrix does not exist \n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(-1);
    #endif
  }
 
  /* Diagonal Matrix: */
  if (M->mat_type == MAT_DIAG)
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (matwrite): "
            "diagonal input matrix not implemented.\n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(-1);
    #endif
  } /* if diagonal */

  /* Other matrix types: */
  else
  {
    /* First write matrix header */
    if( fwrite(M, sizeof(struct mat_str), 1, file) != 1 )
    {
      #ifdef ERROR
      fprintf(STDERR, "*** error (matwrite): "
              "output error while writing header\n");
      #endif

      #ifdef EXIT_ON_ERROR
      exit(1);
      #else
      return(-1);
      #endif
    }

    tot_size = sizeof(struct mat_str);
   
    /* Write real parts first (for real and complex matrices) */
    n_el = M->cols * M->rows;
    if( fwrite(M->rel+1, sizeof(real), n_el, file) != n_el )
    {
      #ifdef ERROR
      fprintf(STDERR, "*** error (matwrite): "
          "output error while writing reals\n");
      #endif

      #ifdef EXIT_ON_ERROR
      exit(1);
      #else
      return(-1);
      #endif
    }
    tot_size += n_el * sizeof(real);

    /* For complex matrix copy also imaginary parts. */
    if(M->num_type == NUM_COMPLEX)
    {
      if( fwrite(M->iel+1, sizeof(real), n_el, file) != n_el )
      {
        #ifdef ERROR
        fprintf(STDERR, "*** error (matwrite): "
                "output error while writing imags\n");
        #endif

        #ifdef EXIT_ON_ERROR
        exit(1);
        #else
        return(-1);
        #endif
      }
      tot_size += n_el * sizeof(real);
    } /* NUM_COMPLEX */
  } /* else */

  #ifdef CONTROL
  fprintf(STDCTR, "(matwrite): %d bytes written\n", tot_size);
  #endif

  return((int)tot_size);
} /* end of function matwrite */
