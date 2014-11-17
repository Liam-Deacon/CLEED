/*********************************************************************
 *                       PATT_DRAW.C
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *********************************************************************/

/*! \file
 *
 * Contains the patt_draw() function.
 *
 * \note Depending on the output @patt_drawing::format , and whether the program
 * is compiled with \c _USE_CAIRO defined and linked to libcairo, then Cairo
 * is used as a vector graphics backend. Otherwise, the old postscript routines
 * are used for generating the LEED pattern.
 */

#include <stdio.h>
#include <strings.h>
#include <cairo.h>
#include <math.h>
#include "spots.h"
#include "patt.h"

/*!
 * Selects backend to use for drawing the LEED pattern.
 *
 * \param drawing #patt_drawing instance containing all the parameters needed
 * for drawing the LEED pattern.
 * \return #patt_error code indicating function success.
 * \retval #PATT_FORMAT_ERROR The graphics backend does not support the
 * format specified in #patt_drawing::format of \p drawing .
 */
int patt_draw(const patt_drawing *drawing)
{

  #ifdef _USE_CAIRO /* use Cairo graphics backend */
    return(patt_draw_cairo(drawing));
  #else /* old postscript backend */
    if ((drawing->format == PATT_PS_OLD) ||
        (drawing->format == PATT_UNKNOWN_FORMAT))
    {
      return(patt_draw_ps(out_stream, drawing));
    }
  #endif

  return(PATT_FORMAT_ERROR);  /* format not supported by backend */
}

