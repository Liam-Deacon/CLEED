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

#include "mat.h"


/*!
 * Display the member valuess of #mat for matrix \p M and print to #STDOUT
 * \param[in] M Matrix whose header is to be displayed.
 */
void matshowpar(const mat M)
{
  
  if (matcheck(M) < 1)
  {
    ERROR_MSG("matrix does not exist \n");
    return;
  }

  fprintf(STDOUT, "(matshowpar):\n");
  fprintf(STDOUT, "\t(int) mag_no   = 0x%4x\n", M->mag_no);
  fprintf(STDOUT, "\t(int) blk_type = 0x%4x\n", M->blk_type);
  fprintf(STDOUT, "\t(int) mat_type = 0x%4x\n", M->mat_type);
  fprintf(STDOUT, "\t(int) num_type = 0x%4x\n", M->num_type);
  fprintf(STDOUT, "\t(int) rows     =   %4d\n", M->rows);
  fprintf(STDOUT, "\t(int) cols     =   %4d\n", M->cols);
  fprintf(STDOUT, "\t(real *) rel   = 0x%x\n", (size_t)M->rel);
  fprintf(STDOUT, "\t(real *) iel   = 0x%x\n", (size_t)M->iel);

}  /* end of function matshowpar */
