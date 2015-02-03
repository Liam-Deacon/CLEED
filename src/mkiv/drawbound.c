/*********************************************************************
 *                       mkiv_draw_bounds.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   VJ/06.02.03 - Creation
 *   GH/27.07.03 - adjust all values to maximum value in image
 *   LD/16.06.14 - update for mkiv_image
 *********************************************************************/

/*! \file
 *
 * Implements mkiv_draw_bounds() function.
 */

#include "mkiv.h"

#include <stdlib.h>
#include <stdio.h>

#ifndef WHITE
#define WHITE 0xFFFF
#endif
#ifndef BLACK
#define BLACK 0x0000
 #endif

/*!
 * Draws the boundaries of the LEED-screen. This is either the mask or
 * r_outer/r_inner if no mask was defined.
 *
 * \param image LEED image.
 * \param imask Mask image.
 * \param center positioninates of center of LEED pattern on image.
 * \param r_outer Outer radius of LEED screen from \p center .
 * \param r_inner Inner radius of LEED screen from \p center .
 * \param write If > 0 then write to "ima.byte", else write to \p fname
 * \param fname Output filename string.
 *  * \return
 */
int mkiv_draw_bounds(mkiv_image *image, mkiv_image *imask, mkiv_position *center, 
              double r_outer, double r_inner, int write, char *fname)
{
  size_t i, j;
  int k;
  size_t num;
  size_t h = (size_t)center->xx;
  size_t v = (size_t)center->yy;
  size_t cols = image->rows;
  size_t rows = image->cols;

  uint16_t max_val;

  double phi;

  uint16_t *mask, *im;
  char file_path[FILENAME_MAX];

  im = (uint16_t *) image->imagedata ;

  if (!imask)
  {
    /* no mask -> draw r_outer, r_inner */
    for (k=-20; k<=20; k++)
    {
      /* draw center */
      im[(int)(v*cols) + ((int)h + k)] = WHITE;    /* brute force casting */
      im[((int)v + k)*(int)cols + (int)h] = WHITE; /* brute force casting */
    }
    for (phi=0; phi < 2*PI; phi+=.005)
    {
      /* draw r_outer and r_inner */
      im[cols*(int)(v + r_inner*sin(phi)) + h + (int)(r_inner*cos(phi))] = WHITE;
      im[cols*(int)(v + r_outer*sin(phi)) + h + (int)(r_outer*cos(phi))] = WHITE;
    }
  }
  else
  {
    /* use mask */
    mask = (uint16_t *) imask->imagedata;

    /* First: find max. value: */
    max_val = 1;
    for (i=0; i < rows; i++)
    {
      for (j=0; j < cols; j++)
      {
        num = i*cols + j;
        max_val = MAX(im[num], max_val);
      }
    }

    /* invert all values outside the active area: */
    for (i=0; i < rows; i++)
    {
      for (j=0; j < cols; j++)
      {
        num = i*cols + j;
        if ( mask[num] == BLACK ) im[num] = BLACK;
      }
    }
     
    for (i=0; i < rows; i++)
    {
      for (j=0; j < cols; j+=50)
      {
        num = i*cols + j;
        im[num] = max_val - im[num];
      }
    }
        
    for (i=0; i < rows; i+=50)
    {
      for (j=0; j < cols; j++)
      {
        num = i*cols + j;
        im[num] = max_val - im[num];
      }
    }

  }

  if (write < 0) strcpy(file_path, fname);
  else if(write > 0) strcpy(file_path, "ima.byte");
    
  if (write)      /* write to "ima.byte" if desired */
  {
    if (mkiv_output_tif(image, file_path))
    {
      ERROR_MSG("failed to write to '%s'!", file_path);
      ERROR_RETURN(-1);
    }
  }
    
  return(0);
}
