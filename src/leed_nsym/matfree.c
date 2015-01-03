/*********************************************************************
 *                      MATFREE.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/26.08.94 - Remove MAT_ERROR
 *********************************************************************/

/*! \file
 *
 * Contains matfree() function for recovering allocated memory from a matrix.
 */

#include <malloc.h>
#include "mat.h"

/*!
 * Frees a matrix from memory.
 *
 * \param[in] M Matrix to be freed.
 * \return
 * \retval 0 if function is successful.
 * \retval -1 if function is unsuccessful.
 */

int matfree( mat M )
{
  /* check input matrix */
  if ( matcheck(M) < 1)
  {
    ERROR_MSG("improper input \n");
    return(-1);
  }

  if (M->rel != NULL) free(M->rel);
  if (M->iel != NULL) free(M->iel);

  free(M);

  return(0);
}  /* end of function matfree */
