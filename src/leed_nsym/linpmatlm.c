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

  char fmt_buffer[STRSZ];
  char linebuffer[STRSZ];
  FILE *inp_stream;

  /* Open input file */
  if((inp_stream = fopen(filename, "r")) == NULL)
  {
    ERROR_MSG("could not open file \"%s\"\n", filename);
    ERROR_RETURN(NULL);
  }

  /* Read the first line of the input file which contains the
   * energy and the maximum l quantum number (lmax). */
  while( *fgets(linebuffer, STRSZ, inp_stream) == '#'); /* skip comment lines */

  sprintf(fmt_buffer, "%%u %%%sf", CLEED_REAL_FMT);
  if ( linebuffer == NULL)     /* EOF found */
  {
    ERROR_MSG("unexpected EOF found while reading file \"%s\"\n", filename);
    exit(1);
  }
  else if( sscanf(linebuffer, fmt_buffer, &l_max_in, &eng) < 2)
  {
    ERROR_MSG("improper input line in file \"%s\":\n%s", filename, linebuffer);
    ERROR_EXIT_RETURN(LEED_FILE_IO_ERROR, NULL);
  }

#if WARNING_LOG
  if (l_max_in > l_max)
  {
    WARNING_MSG("dataset of input file is greater than specified matrix "
                "(l_max: input %u, matrix %u)\n", l_max_in, l_max);
  }
#endif

  eng = cleed_real_sqrt(2*eng);

  CONTROL_MSG(CONTROL, "l_max_in: %u, eng %f)\n", l_max_in, eng);

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
  sprintf(fmt_buffer, "%%u %%d %%u %%d, %%d %%d %%%sf %%%sf",
          CLEED_REAL_FMT, CLEED_REAL_FMT);
  while(fgets(linebuffer, STRSZ, inp_stream) != NULL)
  {
    i_str = 0;
    while(*(linebuffer+i_str) == ' ') i_str++;

    iaux = sscanf(linebuffer+i_str, fmt_buffer,
                  &l1, &m1, &l2, &m2, &iaux1, &iaux2, &r_part, &i_part );

    if (iaux < 8 || iaux == EOF)
    {
      ERROR_MSG("unable to read values from file \"%s\":\n%s",
                filename, linebuffer);
      ERROR_EXIT_RETURN(LEED_FILE_IO_ERROR, NULL);
    }

    if ((l1 <= l_max) && (l2 <= l_max))
    {
      if (m1 < 0) pos_1 = (l1 + 1)*l1 - (size_t)abs(m1) + 1;
      else        pos_1 = (l1 + 1)*l1 + (size_t)m1 + 1;

      if (m2 < 0) pos_2 = (l2 + 1)*l2 - (size_t)abs(m2) + 1;
      else        pos_2 = (l2 + 1)*l2 + (size_t)m2 + 1;

      *rmatel(pos_1, pos_2, Mat) = eng * r_part ;
      *imatel(pos_1, pos_2, Mat) = eng * i_part ;
    }

  } /* End while */

  /* clean up */
  fclose(inp_stream);
   
  return(Mat);
}/* end of function leed_inp_mat_lm */
