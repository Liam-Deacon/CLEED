/*********************************************************************
 *                        LINPMATLM.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/04.05.2000 - creation
 *********************************************************************/

/*! \file
 *
 * Read matrix elements which are specified through their angular momentum and
 * magnetic (l,m) quantum numbers and store in natural order.
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>

#include "mat.h"
#include "leed_def.h"

/*!
 * Reads in an array of non diagonal scattering matrices for a single atom
 * or a complex of atoms for each energy.
 *
 * These non diagonal T-matrices describes anisotropic vibrational
 * amplitudes of the atoms.
 *
 *
 * The format of the written output is:
 * \code
 * # (optional leading comment lines)
 * #
 * <l_max> <energy>
 * l1 m1 l2 m2 i1 i2  <real part> <imag. part>
 * l1 m1 l2 m2 i1 i2  <real part> <imag. part>
 * l1 m1 l2 m2 i1 i2  <real part> <imag. part>
 * l1 m1 l2 m2 i1 i2  <real part> <imag. part>
 * l1 m1 l2 m2 i1 i2  <real part> <imag. part>
 * .
 * .
 * .
 * .
 * \endcode
 *
 * \param[out] Mat Pointer to the non-diagonal T-matrix.
 * \param l_max Maximum orbital angular momentum quantum number \f$ l_{max} \f$
 * \param[in] filename File name string for the stored matrix.
 * \return Pointer to non-diagonal T-matrix.
 * \retval NULL if failed and #EXIT_ON_ERROR is not defined.
 */
mat leed_inp_mat_lm(mat Mat, size_t l_max, const char *filename)
{

  int iaux, iaux1, iaux2;
  size_t i_str;
  size_t l_max_in, l1, l2;
  int m1, m2;
  size_t pos_1, pos_2;
  real eng;
  real r_part, i_part;

  char linebuffer[STRSZ];
  FILE *inp_stream;

  /* Open input file */
  if((inp_stream = fopen(filename, "r")) == NULL)
  {
    #ifdef ERROR
    fprintf(STDERR, "\n*** error (leed_inp_mat_lm): "
        "could not open file \"%s\"\n", filename);
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return NULL;
    #endif
  }

  /* Read the first line of the input file which contains the
   * energy and the maximum l quantum number (lmax). */
  while( *fgets(linebuffer, STRSZ, inp_stream) == '#'); /* skip comment lines */

  if ( linebuffer == NULL)     /* EOF found */
  {
    #ifdef ERROR
    fprintf(STDERR, "\n*** error (leed_inp_mat_lm): "
        "unexpected EOF found while reading file \"%s\"\n", filename);
    #endif
    exit(1);
  }
   
  #ifdef REAL_IS_DOUBLE
  else if( sscanf(linebuffer, "%u %lf", &l_max_in, &eng) < 2)
  #else
  else if( sscanf(linebuffer, "%u %f",  &l_max_in, &eng) < 2)
  #endif
  {
    #ifdef ERROR
    fprintf(STDERR, "\n*** error (leed_inp_mat_lm): "
        "improper input line in file \"%s\":\n%s", filename, linebuffer);
    #endif

    #ifdef EXIT_ON_ERROR
    exit(LEED_FILE_IO_ERROR);
    #else
    return(NULL);
    #endif
  }

  #ifdef WARNING
  if (l_max_in > l_max)
  {
    fprintf(STDWAR, "* warning (leed_inp_mat_lm): "
      "dataset of input file is greater than specified matrix "
      "(l_max: input %u, matrix %u)\n", l_max_in, l_max);
  }
  #endif

  eng = R_sqrt(2*eng);

  #ifdef CONTROL
  fprintf(STDCTR, " (leed_inp_mat_lm): l_max_in: %u, eng %f)\n", l_max_in, eng);
  #endif

  /* allocate memory */
  pos_1 = pos_2 = (l_max + 1)*(l_max + 1);
  Mat = matalloc(Mat, pos_1, pos_2, NUM_COMPLEX);

  /* Read the next lines and store each matrix element:
   * Design:
   *
   * (l1, m1)   (l2,m2)   (real part)   (imaginary part)
   *
   * matrixposition 1: (l1 + 1)l1 + m1 + 1
   * matrixposition 2: (l2 + 2)l2 + m2 + 1
   */
  while(fgets(linebuffer, STRSZ, inp_stream) != NULL)
  {
    i_str = 0;
    while(*(linebuffer+i_str) == ' ') i_str++;

    #ifdef REAL_IS_DOUBLE
    iaux = sscanf(linebuffer+i_str, "%u %d %u %d %d %d %lf %lf",
    #else
    iaux = sscanf(linebuffer+i_str, "%u %d %u %d %d %d %f %f",
    #endif
                  &l1, &m1, &l2, &m2, &iaux1, &iaux2, &r_part, &i_part );

    if (iaux < 8 || iaux == EOF)
    {
      #ifdef ERROR
      fprintf(STDERR, "\n*** error (leed_inp_mat_lm): "
          "unable to read values from file \"%s\":\n%s", filename, linebuffer);
      #endif

      #ifdef EXIT_ON_ERROR
      exit(LEED_FILE_IO_ERROR);
      #else
      return(NULL);
      #endif
    }

    if ((l1 <= l_max) && (l2 <= l_max))
    {
      pos_1 = (l1 + 1)*l1 + m1 + 1;
      pos_2 = (l2 + 1)*l2 + m2 + 1;

      RMATEL(pos_1, pos_2, Mat) = eng * r_part ;
      IMATEL(pos_1, pos_2, Mat) = eng * i_part ;
    }


  } /* End while */

  /* clean up */
  fclose(inp_stream);
   
  return(Mat);
}/* end of function leed_inp_mat_lm */
