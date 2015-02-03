/*********************************************************************
 *                       mkiv_draw_bounds.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *********************************************************************/

/*! \file
 *
 */

#include "mkiv.h"
#include <stdlib.h>

#define BLACK 0x0000

/*!
 * Draws a grid overlay on top of the LEED image
 *
 * \param image LEED image.
 * \param write If 1: write to "ima.byte", else: use \p fname
 * \param fname Filename of output file.
 */
int mkiv_draw_grid(mkiv_image *image, int write, const char *fname)
{
  size_t i, j, num;
  size_t cols = image->cols;
  size_t rows = image->rows;

  unsigned short *im = (unsigned short *) image->imagedata ;;
  char file_path[FILENAME_MAX];

  for (i=0; i<rows; i++)
  {
    for (j=0; j<cols; j+=50)
    {
      num = i*cols+j;
      im[num] = 0xFFFF - im[num];
    }
  }

  for (i=0; i<rows; i+=50)
  {
    for (j=0; j<cols; j++)
    {
      num = i*cols+j;
      im[num] = 0xFFFF - im[num];
    }
  }

  if (write < 0) strcpy(file_path, fname);
  else if (write > 0) strcpy(file_path, "ima.byte");
    
  if (write)      /* write to "ima.byte" if desired */
  {
    if (mkiv_output_tif(image, fname))
    {
      ERROR_MSG("writing image '%s' failed!\n", fname);
      ERROR_RETURN(-1);
    }
  }
    
  return(0);
}
