/*********************************************************************
 *                      MATCHECK.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/26.08.94 - creation
 *********************************************************************/

/*!
 *
 * Contains matcheck() function for checking if a pointer is a matrix type.
 */

#include <stdio.h>
#include "mat.h"

/*!
 * Checks if a pointer points to a matrix type.
 *
 * \param[in] M Pointer to test.
 * \return Integer indicating result.
 * \retval 1 \p M points to a #mat matrix structure.
 * \retval 0 \p M is a \c NULL pointer.
 * \retval -1 \p M does not point to a matrix structure.
 */
int matcheck(const mat M)
{
  if (M == NULL) return(0);
  else
  {
    if (M->mag_no == MATRIX) return(1);
  }

  return(-1);
}  /* end of function matcheck */
