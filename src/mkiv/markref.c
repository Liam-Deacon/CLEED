/*********************************************************************
 *                        WRITETIF.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   SU/02.07.91
 *   GH/07.08.92
 *   CS/30.07.93
 *   VJ/29.01.03
 *   GH/25.05.05 - adjust norm.
 *   LD/16.06.14 - mkiv_image update
 **************************************************************************/

/*! \file
 *
 * Contains mkiv_mark_reflex() function for drawing circles around spots.
 */

#include "mkiv.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*!
 * Draws a circle around each of the measurable mkiv_reflex positions and plots the
 * corresponding indices above each mkiv_reflex if desired.
 *
 * \param nspot The number of spots in \p spot array.
 * \param spot Pointer to an array of reflexes.
 * \param image Pointer to image data.
 * \param thick Thickness of circle lines.
 * \param radius Radius of circle lines.
 * \param color Graylevel of circle lines.
 * \param ind Flag for drawing indices. The options are as follows:
 * + \p ind < 0 : don't draw indices, no writing to ima.byte
 * + \p ind = 0 : no indices, write to ima.byte
 * + \p ind > 0 : draw indices and write to ima.byte
 * \param fname Filename to write output to (default is "ima.byte").
 *  * \return
 * \retval -1 if function is unsuccessful and #EXIT_ON_ERROR is not defined.
 */
int mkiv_mark_reflex(size_t nspot, mkiv_reflex spot[], mkiv_image *image, double thick,
                double radius, int color, int ind, char *fname)
{
  register int h, v, pos;
  register size_t k, i;
  register size_t nncoo;               /* number of circle coordinates  */
  size_t ncoo = 10 + 10*(size_t)(radius*thick);  /* max number of circle coordinates */
  mkiv_position *coo;                  /* coordinates of circle line    */
  double norm, rad;                     /* radius & color of circle line */
  size_t cols = image->cols;           /* number of columns in image */
  size_t rows = image->rows;           /* number of rows in image */
  size_t n_size = image->rows*image->cols;

  uint16_t *im = (uint16_t *)image->imagedata;
  uint16_t val, max_val = 1;
  
  /* find maximum value in image->imagedata */
  for( i = 0; i < n_size; i ++) max_val = MAX(max_val, im[i]);
  norm = (double) max_val / 256.;

  /* allocate space for the coordinate structure */
  coo = (mkiv_position*) calloc( ncoo, sizeof(mkiv_position) );
  if ( coo == NULL )
  {
    ERROR_MSG("No space for array coord -> calloc failed!\n");
    ERROR_RETURN(-1);
  }
	
  /* calculate coordinates */
  nncoo = 0;
  for (h = -(int)radius; h <= (int)radius; h++)
  {
    for (v = -(int)radius; v <= (int)radius; v++)
    {
      rad = PYTH(h,v);
      if ( rad > radius || rad < radius-thick ) continue;
      coo[nncoo].xx = h;
      coo[nncoo].yy = v;
      if ( ++nncoo >= ncoo )
      {
        ERROR_MSG("Too many circle line coordinates (ncoo=%u)...\n", ncoo);
        ERROR_RETURN(-1);
      }
    }
  }

  /* draw circles around spots */
  for (k = 0; k < nspot; k++)
  {
    h = (int) spot[k].x0;
    v = (int) spot[k].y0;
    val = (uint16_t)(100. * norm);
        
    if ( spot[k].control & SPOT_DESI)
    {
      val = (uint16_t)(180. * norm); /* desired */
    }
    if ( spot[k].control & SPOT_REF )
    {
      val = (uint16_t)(255. * norm); /* reference */
    }
        
    if (!(spot[k].control & SPOT_GOOD_S2N))
    {
      val = (uint16_t)(100. * norm); /* bad s/n */
    }

    if ( spot[k].control & SPOT_OUT )
    {
      val = (uint16_t)(100. * norm); /* touched bounds.*/
    }

    if ( spot[k].control & SPOT_EXCL)
    {
      val = (uint16_t)(60. * norm);  /* excluded */
    }

    for (i=0; i < nncoo; i++)
    {
      pos = cols*(v+coo[i].yy) + h + coo[i].xx;
      if ( (pos > 0) && ( pos < (int)(cols*rows) ) )
        im[pos] = val;
    }
  }

  /* plot indices */
  if (ind > 0) mkiv_plot_indices(image, nspot, spot);

  /* Convert internal matrix into TIFF parameters so that XV is able to display
   * them and write the new data into output TIFF file */
  if (strlen(fname) == 0 || fname == NULL)
  {
    realloc(fname, sizeof(char)*FILENAME_MAX);
    strcpy(fname, "ima.byte");
  }
    
  if(ind >= 0)
  {
    if (mkiv_output_tif(image, fname))
    {
      ERROR_MSG("mkiv_output_tif failed!\n");
      ERROR_RETURN(-1);
    }
  }

  free(coo);
    
  return(0);
}
