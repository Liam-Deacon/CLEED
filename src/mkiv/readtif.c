/*********************************************************************
 *                        mkiv_read_tif.C
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
 *   VJ/00.02.03 - creation
 *   GH/19.03.03 - allocate tif_values inside subroutine.
 *   GH/27.07.03 - allocate buffer according to bitspersample (8/16)
 *   LD/02.03.14 - check image files exist before trying to open &
 *                 prevent IO error
 ****************************************************************************/

/*! \file
 *
 * Contains mkiv_read_tif() function for reading a TIFF file from disk and storing as
 * a #tif_values structure.
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "mkiv.h"

#define TIFF_GET_FIELD(tif, tag, val) TIFFGetField(tif->tif_in, tag, &tif->val)

/*!
 * Reads a TIFF \p input file from disk into \p tifimage #tif_values structure.
 *
 * \param[out] tifimage TIFF structure to load values into.
 * \param[in] input Filename or path to TIFF input file.
 *  * \return Integer representing function success (using normal C convention).
 * \retval -1 if function encounters an error and #EXIT_ON_ERROR is not defined.
 */
int mkiv_read_tif(mkiv_tif_values *tifimage, const char *input)
{
  unsigned long row;
  long imagelength, scanlinesize;

  /* Check tifimage and allocate memory */
  if(tifimage == NULL)
  {
    if ( (tifimage = (mkiv_tif_values*)
                        malloc(sizeof(mkiv_tif_values))) == NULL )
    {
      ERROR_MSG("tifimage is NULL\n");
      ERROR_EXIT_RETURN(1, -1);
    }
  }
  else /* free buffer */
  {
    free(tifimage->buf);
    tifimage->buf = NULL;
  }

  /* Open input file and read parameters */
  if ( !file_exists(input) )
  {
    ERROR_MSG("cannot read tiff file '%s'\n", input);
    ERROR_EXIT_RETURN(1, -1);
  }
  tifimage->tif_in = (TIFF *)TIFFOpen(input, "r");

  TIFF_GET_FIELD(tifimage, TIFFTAG_IMAGEWIDTH, imagewidth);
  TIFF_GET_FIELD(tifimage, TIFFTAG_IMAGELENGTH, imagelength);
  TIFF_GET_FIELD(tifimage, TIFFTAG_BITSPERSAMPLE, bitspersample);
  TIFF_GET_FIELD(tifimage, TIFFTAG_COMPRESSION, compression);
  TIFF_GET_FIELD(tifimage, TIFFTAG_PHOTOMETRIC, photometricinterpretation);
  TIFF_GET_FIELD(tifimage, TIFFTAG_STRIPOFFSETS, stripoffsets);
  TIFF_GET_FIELD(tifimage, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
  TIFF_GET_FIELD(tifimage, TIFFTAG_STRIPBYTECOUNTS, stripbytecounts);
  TIFF_GET_FIELD(tifimage, TIFFTAG_XRESOLUTION, xresolution);
  TIFF_GET_FIELD(tifimage, TIFFTAG_YRESOLUTION, yresolution);
  TIFF_GET_FIELD(tifimage, TIFFTAG_PLANARCONFIG, planarconfiguration);
  TIFF_GET_FIELD(tifimage, TIFFTAG_RESOLUTIONUNIT, resolutionunit);

  /*
   * Allocate buffer for image data:
   * 8 bit:
   *  imagelength =  tifimage->imagelength;
   *  scanlinesize = TIFFScanlineSize(tifimage->tif_in);
   *
   * 16 bit:
   *  imagelength =  2 * tifimage->imagelength;
   *  scanlinesize = TIFFScanlineSize(tifimage->tif_in);
   */
  fprintf(stdout, "(v_mkiv_read_tif): read file %s\n", input);

  if(tifimage->bitspersample == 8)
  {
      fprintf(stdout, "(v_mkiv_read_tif): 8 bit TIFF\n");
      imagelength =  tifimage->imagelength;
      scanlinesize = TIFFScanlineSize(tifimage->tif_in);
  }
  else if(tifimage->bitspersample == 16)
  {
      fprintf(stdout, "(v_mkiv_read_tif): 16 bit TIFF\n" );
      /* before: imagelength =  2* tifimage->imagelength; */
      imagelength =  tifimage->imagelength;
      scanlinesize = TIFFScanlineSize(tifimage->tif_in);
  }
  else
  {
      ERROR_MSG("unknown TIFF format: bitspersample = %d\n",
              tifimage->bitspersample);
      ERROR_EXIT_RETURN(1, -1);
  }

  fprintf(stdout, "(mkiv_read_tif): length = %ld, width = %ld, "
          "scanlinesize = %ld, buffersize = %" PRIu64 "\n",
          imagelength, tifimage->imagewidth, scanlinesize,
          imagelength * scanlinesize);

  tifimage->buf = (char *)malloc((size_t)(imagelength * scanlinesize) * sizeof(char));
  if(tifimage->buf == NULL) 
  {
    ERROR_MSG("memory allocation failed (buffer)\n");
    ERROR_EXIT_RETURN(1, -1);
  }

  for (row = 0; row < tifimage->imagelength; row++)
  {
    TIFFReadScanline ( tifimage->tif_in,
                       tifimage->buf+row * scanlinesize,
                       row, scanlinesize );
  }

  #ifdef CONTROL
  fprintf(stdout, "(mkiv_read_tif):\n");
  fprintf(stdout, "imagewidth = %" PRIu64 "\n", tifimage->imagewidth);
  fprintf(stdout, "imagelength = %" PRIu64 "\n", tifimage->imagelength);
  fprintf(stdout, "bitspersample = %" PRIu16 "\n", tifimage->bitspersample);
  fprintf(stdout, "compression = %" PRIu16 "\n", tifimage->compression);
  fprintf(stdout, "photometricinterpretation = %" PRIu16 "\n",
          tifimage->photometricinterpretation);
  fprintf(stdout, "stripoffsets = %" PRIu64 "\n", tifimage->stripoffsets);
  fprintf(stdout, "rowsperstrip = %" PRIu64 "\n", tifimage->rowsperstrip);
  fprintf(stdout, "stripbytecounts = %" PRIu64 "\n", tifimage->stripbytecounts);
  fprintf(stdout, "xresolution = %f\n", tifimage->xresolution);
  fprintf(stdout, "yresolution = %f\n", tifimage->yresolution);
  fprintf(stdout, "planarconfiguration = %d\n",
          tifimage->planarconfiguration);
  fprintf(stdout, "resolutionunit = %d\n",tifimage->resolutionunit);
  #endif

  TIFFClose(tifimage->tif_in);
  
  return(0); /* success */
}
