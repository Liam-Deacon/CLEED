/*********************************************************************
 *                        WRITETIF.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/28.07.03 - creation
 *   LD/02.03.14 - adapted
 *********************************************************************/

/*! \file
 *
 * Writes tiff output images.
 */

#include <stdio.h>
#include <string.h>
#include "mkiv.h"

#define TIFF_SET_FIELD(img, tag, val) TIFFSetField(img->tif_out, tag, val)

/*!
 * Writes TIFF image data to file.
 *
 * \param[out] tifimage
 * \param[in] filename File path to write to.
 *  * \return
 */
int writetif(mkiv_tif_values *tifimage, const char *filename)
{
  unsigned long row;

  if (tifimage->tif_out != NULL) free(tifimage->tif_out);
  if ( (tifimage->tif_out = (TIFF *) TIFFOpen(filename, "w")) == NULL)
  {
    fprintf(stderr, "***error (writetif): cannot write to '%s'\n", filename);
  }

  TIFF_SET_FIELD(tifimage, TIFFTAG_IMAGEWIDTH, tifimage->imagewidth);
  TIFF_SET_FIELD(tifimage, TIFFTAG_IMAGELENGTH, tifimage->imagelength);
  TIFF_SET_FIELD(tifimage, TIFFTAG_BITSPERSAMPLE, tifimage->bitspersample);
  TIFF_SET_FIELD(tifimage, TIFFTAG_COMPRESSION, tifimage->compression);
  TIFF_SET_FIELD(tifimage, TIFFTAG_PHOTOMETRIC, tifimage->photometricinterpretation);
  TIFF_SET_FIELD(tifimage, TIFFTAG_ROWSPERSTRIP, tifimage->rowsperstrip);
  TIFF_SET_FIELD(tifimage, TIFFTAG_XRESOLUTION, tifimage->xresolution);
  TIFF_SET_FIELD(tifimage, TIFFTAG_YRESOLUTION, tifimage->yresolution);
  TIFF_SET_FIELD(tifimage, TIFFTAG_PLANARCONFIG, tifimage->planarconfiguration);
  TIFF_SET_FIELD(tifimage, TIFFTAG_RESOLUTIONUNIT, tifimage->resolutionunit);

  for (row = 0; row < tifimage->imagelength; row++)
  {
    TIFFWriteScanline( tifimage->tif_out,
                       (tifimage->buf + row*tifimage->imagewidth),
                       row,
                       2*tifimage->imagewidth);
  }

  TIFFClose(tifimage->tif_out);

  return(0); /* success */
}
