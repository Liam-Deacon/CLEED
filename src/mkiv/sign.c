/*********************************************************************
 *                        SIGN.C
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
 *   SU/04.07.91 - creation.
 *   CS/30.07.93 -
 *   GH/27.07.03 - include value in parameter list (white).
 ***************************************************************************/

/*! \file
 *
 * Implements sign() function.
 */

#include "mkiv.h"
#include "signs.h"
#include <stdlib.h>

/*!
 * Plots a character (i.e. numbers, fractions, minus and semicolon).
 *
 * \param image
 * \param chr character code
 * \param xx x-coordinates of the upper left corner of the image.
 * \param yy y-coordinate of the upper left corner of the image.
 * \param value unsigned short value for white color.
 *  * \return C style return code indicating function success.
 * \retval 0 if successful.
 * \retval -1 if \p chr is invalid.
 */
int sign(mkiv_image *image, unsigned char chr,
         size_t xx, size_t yy, unsigned short value)
{
  register size_t j, k, pos;
  size_t i, cols = image->rows, rows = image->cols;
  unsigned short *im = (unsigned short *)image->imagedata;

  chr -= 45;
  if (chr > 34 ) /* unsigned so always positive due to wrapping */
  {
    #ifdef EXIT_ON_ERROR
    ERR_EXIT("(sign): unknown character!");
    #else
    return(-1);
    #endif
  }

  pos = cols*yy + xx;
  if ( xx < cols && yy < rows-7 )
  {
    for (k=0, i=0; i<11; i++, pos+=cols-7 )
    {
      for ( j=0; j<7; j++, k++, pos++)
      {
        im[pos] = a[chr][k] * value;
      }
    }
  }

  return(0);
}
