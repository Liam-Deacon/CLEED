/*********************************************************************
 *                       DRAWBOUND.C
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
 * Implements drawbound() function.
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
 * router/rinner if no mask was defined.
 *
 * \param image LEED image.
 * \param imask Mask image.
 * \param center positioninates of center of LEED pattern on image.
 * \param router Outer radius of LEED screen from \p center .
 * \param rinner Inner radius of LEED screen from \p center .
 * \param write If > 0 then write to "ima.byte", else write to \p fname
 * \param fname Output filename string.
 *  * \return
 */
int drawbound(mkiv_image *image, mkiv_image *imask, mkiv_position *center, 
              float router, float rinner, int write, char *fname)
{
  size_t i, j;
  int k;
  size_t num;
  size_t h = (size_t)center->xx;
  size_t v = (size_t)center->yy;
  size_t cols = image->rows;
  size_t rows = image->cols;

  unsigned short max_val;

  double phi;

  unsigned short *mask, *im;
  char file_path[FILENAME_MAX];

  im = (unsigned short *) image->imagedata ;

  if (!imask)
  {
    /* no mask -> draw router, rinner */
    for (k=-20; k<=20; k++)
    {
      /* draw center */
      im[v*cols + (h + k)] = WHITE;
      im[(v + k)*cols + h] = WHITE;
    }
    for (phi=0; phi < 2*PI; phi+=.005)
    {
      /* draw router and rinner */
      im[cols*(int)(v + rinner*sin(phi)) + h + (int)(rinner*cos(phi))] = WHITE;
      im[cols*(int)(v + router*sin(phi)) + h + (int)(router*cos(phi))] = WHITE;
    }
  }
  else
  {
    /* use mask */
    mask = (unsigned short *) imask->imagedata;

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
    if (out_tif(image, file_path))
    {
      ERR_EXIT_X("(drawbound): failed to write to '%s'!", file_path);
    }
  }
    
  return(0);
}
