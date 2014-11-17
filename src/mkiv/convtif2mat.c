/*********************************************************************
 *                        CONVTIF2MAT.C
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
 *   VJ 28/01/03 - Creation
 *   GH/27.07.03 - 2-byte tiff
 *   LD/15.06.14 - Update to mkiv_image structure
 *********************************************************************/

/*! \file
 *
 * Converts the TIFF parameters, reads off an image, into an #mkiv_image
 * object so that \c mkiv is able to process them, and then
 * converts 1-byte to 2-byte data (\c mkiv understands only 2-BYTE data,
 * hence the conversion).
 */

#include "mkiv.h"
#include <tiffio.h>
#include <stdlib.h>

/*!
 * Converts TIFF #tif_values to an #mkiv_image object.
 *
 * \param tifval Pointer to structure for TIFF data.
 * \param mat_image Pointer to matrix for image data.
 *  * \return
 */
int conv_tif2mat(mkiv_tif_values *tifval, mkiv_image *mat_image)
{
  unsigned long i;
  size_t n_size;

  unsigned short *data_ptr;
  unsigned short pix_shor;
  unsigned char *pix_char_ptr;

  /* Convert tif_values into mkiv_image type*/
  if(tifval == NULL)
  {
    fprintf(stderr, "***error (conv_tif2mat): cannot read TIFF data");
    exit(1);
  }

  mat_image->rows = tifval->imagewidth;
  mat_image->cols = tifval->imagelength;

  /*
   * tifval->ENERGY/CURRENT are not defined.
   *
   *  image->ispare1 = tifval->ENERGY;
   *  image->ispare2 = tifval->CURRENT;
   */

  /* Allocate memory for image data (unsigned short)
   * - Pointer data_ptr will be copied to image->imagedata at the
   * end of the function.
   */
  n_size = mat_image->rows * mat_image->cols;
  data_ptr = (unsigned short *)malloc(n_size * sizeof(unsigned short));

  /* Conversion of TIFF pixel (1 byte or 2 byte) values to 2-byte
   * pixel values (unsigned short)
   */
  pix_char_ptr = (unsigned char *) &pix_shor;

  /* 8 bit: convert 1-byte into 2-byte (unsigned short)*/
  if ( (int)tifval->bitspersample == 8)
  {
    fprintf(stdout,"(conv_tif2mat): 8 bit TIFF\n");

    for (i=0; i < n_size; i++)
    {
      pix_char_ptr[0] = tifval->buf[i];
      *(data_ptr + i) = (unsigned short) pix_char_ptr[0];
    }
  } /* 8 bit */
  /* 16 bit: don't swap high byte and low byte */
  else if ( (int)tifval->bitspersample == 16)
  {
    fprintf(stdout,"(conv_tif2mat): 16 bit TIFF\n" );

    for (i=0; i < n_size; i++)
    {
      pix_char_ptr[1] = tifval->buf[2*i + 1];
      pix_char_ptr[0] = tifval->buf[2*i];
      *(data_ptr + i) = (unsigned short) pix_shor;
    }
  } /* 16 bit */
  else
  {
    fprintf(stderr, "*** error (conv_tif2mat): "
            "unknown TIFF format: bitspersample = %i\n", tifval->bitspersample);
    exit(1);
  }

  if (mat_image->imagedata != NULL)
  {
    fprintf(stderr,"*** error (conv_tif2mat): free mkiv_image buffer\n");
    free(mat_image->imagedata);
  }
  mat_image->imagedata = (uint32 *)data_ptr;

  return(0);
}
