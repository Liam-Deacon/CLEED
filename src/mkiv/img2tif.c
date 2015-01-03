/*********************************************************************
 *                      img2tif.c
 *
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*! \file
 *
 * Provides img2tif() function for converting an arbitrary image format to
 * 8/16bit TIFF.
 *
 * \note img2tif() uses ImageMagick libraries as the conversion backend, but
 * must be enabled by defining \c USE_MAGICK when compiling.
 * 
 */

#include <math.h>
#include <stdio.h>
#include <strings.h>

#ifdef USE_MAGICK
# include <wand/magick_wand.h>
#endif

#if defined(_WIN32) || defined(__WIN32__)
# define PATH_SEPARATOR ';' /*!< Separator for PATH names on Windows */
#else
# define PATH_SEPARATOR ':' /*!< Unix separator for PATH names */
#endif

/*!
 * Converts an arbitrary image into a a TIFF file.
 *
 * \param[in] img_filename
 * \param[out] tif_filename
 * \return
 */
int img2tif(const char *img_filename, char *tif_filename)
{
  size_t n = strlen(img_filename);

  /* Check that the file is not a TIFF file */
  if (strncasecmp(*(img_filename+n-4), ".tif", 4) == 0 ||
      strncasecmp(*(img_filename+n-5), ".tiff", 5) == 0)
  {
    return(0); /* file is already a TIFF file */
  }

#ifdef USE_MAGICK /* convert image using ImageMagick */
  MagickWand *mw = NULL;

  /* Initialise Wand */
  MagickWandGenesis();

  /* Create a wand */
  mw = NewMagickWand();

  /* Read the input image */
  MagickReadImage(mw, img_filename);

  if (strcasecmp(MagickGetFormat(mw), "TIFF") != 0)
    return(-1); /* input is a TIFF */

  /* allocate memory for TIFF output filename */
  if (tif_filename == NULL)
    tif_filename = (char *) malloc(FILENAME_MAX*sizeof(char));

  /* create new filename */
  strcpy(tif_filename, img_filename);
  remove_ext(tif_filename, '.', PATH_SEPARATOR);
  strcat(tif_filename, ".tif");

  /* Check bit depth */
  if (MagickGetImageDepth() > 16)
    MagickSetImageDepth(16); /* only 8 or 16bit TIFF is supported by mkiv */

  /* Write the output TIFF image */
  MagickSetFormat(mw, "TIFF");
  MagickWriteImage(mw, tif_filename);

  /* Tidy up */
  if(mw) mw = DestroyMagickWand(mw);

  MagickWandTerminus();
#else
  fprintf(stderr, "*warning (img2tif): cannot convert image '%s' to TIFF\n"
          "This function must be compiled and linked to the ImageMagick "
          "libraries\n", img_filename);
#endif
  return(0);
}
