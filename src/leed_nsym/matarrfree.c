/*********************************************************************
 *                        MATARRFREE.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
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
 * Implements matarrfree() function to free a complete array of matrices.
 */

#include <stdio.h>
#include <stdlib.h>
#include "mat.h"

/*!
 * Frees a complete array of matrices previously allocated by matarralloc()
 *
 * \param M Pointer to matrix array (terminated by blk_type = #BLK_END ).
 * \return Normal C convention return code.
 * \retval 0 if successful.
 * \retval -1 if function failed.
 */
int matarrfree(mat M)
{
  size_t i_mat;

  /* Check the validity of the pointer M */
  if ( matcheck(M) < 1)
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (matarrfree): improper input for 'M'\n");
    #endif

    return(-1);
  }

  /* If pointer M is valid, go through the existing array and free memory
   * allocated for the single matrices.
   * Finally free the array itself.
   */
  for(i_mat = 0;
      ( (M+i_mat)->mag_no == MATRIX) && ( (M+i_mat)->blk_type == BLK_ARRAY);
      i_mat ++)
  {
    matfree(M+i_mat);
  }
 free(M);
 
 return(0);
} /* end of function matarrfree */
