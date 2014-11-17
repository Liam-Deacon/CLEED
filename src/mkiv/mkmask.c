/*********************************************************************
 *                        MKMASK.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 1993-2014 Christian Stellwag <leed@iron.E20.physik.tu-muenchen.de>
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   CS/18.08.93
 *   VJ/06.02.03
 *   GH/11.03.03
 *   LD/16/06/14 - update for mkiv_image
 *********************************************************************/

/*! \file
 *
 * Contains mkmask() function for drawing boundaries of the LEED screen onto an
 * image, producing a mask.
 */

#include "mkiv.h"
#include <stdlib.h>

/*!
 * Draws a mask for the boundaries of the LEED screen.
 * \param[in,out] image Pointer to #mkiv_image image data structure.
 * \param[in] center positioninate of the center of the LEED screen.
 * \param router Radius of the outer boundary of the LEED screen.
 * \param rinner Radius of the inner boundary of the LEED screen (i.e the
 * region which is shadowed by the electron gun).
 * \param write Flag indicating write action:
 * - write > 0 : write to "ima.byte".
 * - write < 0 : write to \p mask_path.
 * - write = 0 : no action.
 * \param[in] mask_path Filepath string to mask file if \p write > 0.
 *  * \return Integer indicating function success (normal C style).
 * \retval 0 if function completed successfully.
 * \retval -1 if the function encountered an error and #EXIT_ON_ERROR is not
 * defined.
 * \note If \p center , \p router or \p rinner contain negative values then
 * the modulus of that value will be used. An additional warning message will
 * be printed if #WARNING is defined.
 */
int mkmask(mkiv_image *image, const mkiv_position *center, float router, float rinner,
          int write, const char *mask_path)
{
  float phi, rad;
  size_t i;
  size_t cols = image->rows;
  size_t n_size = image->rows * image->cols;
  int x0 = center->xx;
  int y0 = center->yy;
  char fname[FILENAME_MAX];
  unsigned short *mask = (unsigned short *) image->imagedata;

  /* check for negative values */
  if (router < 0)
  {
    #ifdef WARNING
    fprintf(stderr, "*warning (mkmask): "
            "radius router < 0; will use |router|\n");
    #endif
    router = fabs(router);
  }
  if (rinner < 0)
  {
    #ifdef WARNING
    fprintf(stderr, "*warning (mkmask): "
            "radius rinner < 0; will use |rinner|\n");
    #endif
    rinner = fabs(rinner);
  }
  if (x0 < 0)
  {
    #ifdef WARNING
    fprintf(stderr, "*warning (mkmask): x-center x0 < 0; will use |x0|\n");
    #endif
    x0 = abs(x0);
  }
  if (y0 < 0)
  {
    #ifdef WARNING
    fprintf(stderr, "*warning (mkmask): y-center y0 < 0; will use |y0|\n");
    #endif
    y0 = abs(y0);
  }

  /* no mask -> draw router, rinner */
  for (i=0; i < n_size; i++) mask[i] = 0;

  for (phi=0; phi < 2*PI; phi+=.001)
  {
    /* draw router and rinner */
    for( rad = rinner; rad <= router; rad += 0.5)
    {
      mask[cols*(int)(y0 + rad*sin(phi)) + x0 + (int)(rad*cos(phi))] = 0xFFFF;
    }
  }

  if (write > 0)
  {
    strcpy(fname, "mask.byte");
  }
  else if(write < 0)
  {
    strcpy(fname, mask_path);
  }
  else
  {
    strcpy(fname, "mask_new.byte");
  }

  if (write)
  {
    if (out_tif(image, fname))
    {
      fprintf(stderr, "***error (out_tif): write to '%s' failed!\n", fname);
    }
  }
  else
  {
    fprintf(stderr, "write mask to '%s'\n", fname);
    if (out_tif(image, fname))
    {
      #ifdef EXIT_ON_ERROR
      ERR_EXIT_X("(out_tif): write to '%s' failed!\n", fname);
      #else
      return(-1);
      #endif
    }
  }
  return(0);
}
