/*********************************************************************
 *                        MATSHOWPAR.C
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/20.01.95 - Creation
 *********************************************************************/

/*! \file
 *
 * Shows the member parameters of the a #mat instance.
 */

#include <math.h>  
#include <stdio.h>
#include <string.h>

#include "mat.h"


/*!
 * Display the member values of #mat for matrix \p M and print to \p stream
 *
 * \param stream Pointer to output file stream.
 * \param[in] M Matrix whose header is to be displayed.
 *
 * \return pointer to a string containing the matrix parameters.
 * \note No output will be written if \p stream is \c NULL
 * \warning A zero length string will be returned on error and an
 * message printed to \c stderr
 */
const char *matshowpar(FILE *stream, const mat M)
{
  static char str[10*STRSZ] = "";
  size_t len = 0;
  if (matcheck(M) < 1)
  {
    ERROR_MSG("matrix does not exist\n");
    return "";
  }

  len = sprintf(str, "(%s):\n", __func__);
  len += sprintf(str+len, "\t(int) mag_no   = 0x%4x\n", M->mag_no);
  len += sprintf(str+len, "\t(int) blk_type = 0x%4x\n", M->blk_type);
  len += sprintf(str+len, "\t(int) mat_type = 0x%4x\n", M->mat_type);
  len += sprintf(str+len, "\t(int) num_type = 0x%4x\n", M->num_type);
  len += sprintf(str+len, "\t(int) rows     =   %4d\n", M->rows);
  len += sprintf(str+len, "\t(int) cols     =   %4d\n", M->cols);
  len += sprintf(str+len, "\t(real *) rel   = 0x%x\n", (size_t)M->rel);
  len += sprintf(str+len, "\t(real *) iel   = 0x%x\n", (size_t)M->iel);

  /* print output if stream is valid */
  if (stream) fprintf(stream, str);

  return str;

}  /* end of function matshowpar */
