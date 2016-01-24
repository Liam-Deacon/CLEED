/*********************************************************************
 *                        mkiv_mk_mask.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 1993-2014 Christian Stellwag <leed@iron.E20.physik.tu-muenchen.de>
 *  Copyright 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
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
 * Contains mkiv_mk_mask() function for drawing boundaries of the LEED screen onto an
 * image, producing a mask.
 */

#include "mkiv.h"
#include <stdlib.h>
#include <string.h>

/*!
 * Draws a mask for the boundaries of the LEED screen.
 * \param[in,out] image Pointer to #mkiv_image image data structure.
 * \param[in] center positioninate of the center of the LEED screen.
 * \param r_outer Radius of the outer boundary of the LEED screen.
 * \param r_inner Radius of the inner boundary of the LEED screen (i.e the
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
 * \note If \p center , \p r_outer or \p r_inner contain negative values then
 * the modulus of that value will be used. An additional warning message will
 * be printed if #WARNING_LOG is defined.
 */
int mkiv_mk_mask(mkiv_image *image, const mkiv_position *center,
                 double r_outer, double r_inner, int write, const char *mask_path)
{
  double phi, rad;
  size_t i;
  size_t cols = image->rows;
  size_t n_size = image->rows * image->cols;
  int x0 = center->xx;
  int y0 = center->yy;
  char fname[FILENAME_MAX];
  uint16_t *mask = (uint16_t *) image->imagedata;

  /* check for negative values */
  if (r_outer < 0)
  {
    WARNING_MSG("radius r_outer %6.4lf < 0; will use |r_outer|\n", r_outer);
    r_outer = fabs(r_outer);
  }
  if (r_inner < 0)
  {
    WARNING_MSG("radius r_inner %6.4lf < 0; will use |r_inner|\n", r_inner);
    r_inner = fabs(r_inner);
  }
  if (x0 < 0)
  {
    WARNING_MSG("x-center x0 < 0; will use |x0|\n");
    x0 = abs(x0);
  }
  if (y0 < 0)
  {
    WARNING_MSG("y-center y0 < 0; will use |y0|\n");
    y0 = abs(y0);
  }

  /* no mask -> draw r_outer, r_inner */
  for (i=0; i < n_size; i++) mask[i] = 0;

  for (phi=0; phi < 2*PI; phi+=.001)
  {
    /* draw r_outer and r_inner */
    for( rad = r_inner; rad <= r_outer; rad += 0.5)
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
    if (mkiv_output_tif(image, fname))
    {
      ERROR_MSG("write to '%s' failed!\n", fname);
    }
  }
  else
  {
    fprintf(stderr, "write mask to '%s'\n", fname);
    if (mkiv_output_tif(image, fname) != 0)
    {
      ERROR_MSG("write to '%s' failed!\n", fname);
      ERROR_RETURN(-1);
    }
  }
  return(0);
}
