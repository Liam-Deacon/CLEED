/*********************************************************************
 *                        CONVMAT2TIF.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 1993-2014 Christian Stellwag <leed@iron.E20.physik.tu-muenchen.de>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   VJ/07.02.03 - Creation
 *   GH/26.05.05 - introduce log
 *   LD/16/06.14 - update to mkiv_image
 *********************************************************************/

/*! \file
 *
 * Converts the image matrix parameters (mkiv_image)
 * into TIFF parameters (tif_values) and applies log scaling for displaying.
 * It then allocates memory for \c buf and copies all \c imagedata to \c buf.
 * It is used in function mkiv_tif_write() (mkiv_tif_write.c), a routine in functions
 * such as mkiv_draw_bounds() .
 */

#include "mkiv.h"
#include <stdlib.h>
#include <math.h>

/*!
 * Converts #mkiv_image to #tif_values
 *
 * \param[in] mat_image Matrix of image data.
 * \param[out] tif_image Pointer to TIFF image. \c tif_image->buf should be
 * either \c NULL or point to a valid array of characters.
 *  * \return
 */
int mkiv_mat2tif(const mkiv_image *mat_image, mkiv_tif_values *tif_image)
{
  unsigned long i;
  size_t n_size;

  uint16_t *data_shor;
  uint8_t *data_char;

  uint16_t pix_shor, max_val, min_val;
  uint8_t pix_char;

  uint8_t *lut;

  double norm;
     
  /*  Convert mkiv_image into tif_values */
  tif_image->imagewidth  = mat_image->rows;
  tif_image->imagelength = mat_image->cols;

  tif_image->bitspersample = 8;
  tif_image->compression = 1;
  tif_image->photometricinterpretation = 1;

  /* These numbers do not seem to matter */
  tif_image->stripoffsets = 134765640;
  tif_image->rowsperstrip = 11;
  tif_image->stripbytecounts = 134765424;

  /*
  tif_image->stripoffsets = 134775496;
  tif_image->rowsperstrip = 20;
  tif_image->stripbytecounts = 134775408;
  */
  tif_image->xresolution = 0;
  tif_image->yresolution = 0;
  tif_image->planarconfiguration = 1;
  tif_image->resolutionunit = 2;

  /*
   * tif_image->ENERGY/CURRENT are not defined.
   *
   * tif_image->ENERGY = image->ispare1;
   * tif_image->CURRENT = image->ispare2;
   */

  /* Allocate memory for tif_image pointer buf */
  n_size = tif_image->imagelength * tif_image->imagewidth;
  data_char = (uint8_t *) malloc(sizeof(uint8_t)*n_size);

  if (data_char == NULL)
  { 
    ERROR_MSG("Failed to allocate memory for buf\n");
    ERROR_RETURN(-1);
  }

  /* First: find minimum / maxiumum value in mat_image->imagedata */

  /* Allocate memory for LUT */
  lut = (uint8_t *)calloc(256, sizeof(uint8_t));
  for (i=0; i < 256; i++)
  {
    lut[i] = (uint8_t) (46.018 * log( (double) i ) );
/*
    lut[i] = (unsigned char) ( 318.75 *  i / (i + 63.75) );
    lut[i] = (unsigned char) (46.018 * log( (double) i ) );
    fprintf(stdout, "i = %d, lut = %d\n", i, lut[i]); 
*/
  }

  data_shor = (uint16_t *)mat_image->imagedata;

  max_val = 1;
  min_val = data_shor[0];

  for(i=0; i < n_size; i++)
  {
    max_val = MAX(*(data_shor + i), max_val);
    min_val = MIN(*(data_shor + i), min_val);
  }

  if (max_val > min_val) norm = 255. / (max_val - min_val);
  else norm = 255.;

  fprintf(stdout, "(mkiv_mat2tif_log): min/max_val = %d/%d, norm = %f\n",
                   min_val, max_val, norm);

  /* normalize all imagedata and copy to buf */
  for (i=0; i < n_size; i++)
  {
    pix_shor = *(data_shor + i);
    pix_char = (uint8_t)( (pix_shor - min_val) * norm);
    *(data_char + i) = lut[pix_char];
  }                

  if (tif_image->buf != NULL)
  {
    fprintf(stderr, "(mkiv_mat2tif_log): free buffer\n");
    free(tif_image->buf);
  }
  tif_image->buf = (char*) data_char;

  return(0);
}
