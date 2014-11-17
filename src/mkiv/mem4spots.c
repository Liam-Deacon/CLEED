/*********************************************************************
 *                        MEM4SPOTS.C
 *
 *  Copyright 1993-2014 Christian Stellwag <leed@iron.E20.physik.tu-muenchen.de>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   CS/01.09.93 - creation
 **************************************************************************/

/*! \file
 *
 * Contains mem4spot() function to allocate memory for spots.
 */

#include "mkiv.h"
#include <stdlib.h>

/*!
 * Allocates memory for #mkiv_reflex array. The amount of memory needed is determined
 * by the formula:
 * \code
 * memsize = sizeof(mkiv_reflex) * sum(sizeof(screen)/sizeof(bases) )
 * \endcode
 *
 * \param[out] spot_max Pointer for number of spots in array
 * \param[in] a Pointer to substrate basis vectors.
 * \param ndom Number of superstructure domains.
 * \param[in] superlat Pointer to array of superstructure basis vectors.
 * \param[in] image Pointer LEED image data.
 *  * \return Pointer to #mkiv_reflex array
 * \retval \c NULL if unable to allocate memory (and #EXIT_ON_ERROR not defined).
 */
mkiv_reflex *mem4spots(size_t *spot_max, const mkiv_vector a[], size_t ndom,
                  const mkiv_domain *superlat, const mkiv_image *image)
{
  mkiv_reflex *spot = NULL;
  size_t i;
  float base_area;
  long screen_area = (int)( PI/4. * image->rows * image->cols );

  /* calculate number of substrate spots within screen */
  base_area = fabs( a[1].xx * a[2].yy - a[2].xx * a[1].yy );
  *spot_max = RNDUP( (float)screen_area / base_area );

  /* calculate number of superstructure spots within screen */
  for (i=0; i < ndom; i++)
  {
    base_area = fabs( a[1].len * superlat[i].lind11 *
                      a[2].len * superlat[i].lind22 -
                      a[1].len * superlat[i].lind12 *
                      a[2].len * superlat[i].lind21 );
    *spot_max += RNDUP( (float)screen_area / base_area );
  }

  /* allocate memory */
  /*!FIXME: *spot_max = 1000;  */  /* changed ..... */
  if ( (spot = (mkiv_reflex *) malloc( *spot_max * sizeof(mkiv_reflex) ) ) == NULL )
  {
    #ifdef EXIT_ON_ERROR
    ERR_EXIT("mem4spots : Not enough memory for spot structure array");
    #else
    return(NULL);
    #endif
  }

  return(spot);
}
