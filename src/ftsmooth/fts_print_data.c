/*********************************************************************
 *                           FTS_PRINT_DATA.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/24.04.14 - creation.
 *********************************************************************/

/*! \file
 *
 * Implementation file for fts_print_data() function.
 */

#include "ftsmooth.h"

/*!
 * Subroutine to output x & f(x) values, deleting negative f(x)
 * values if required. It will return the number of data lines written
 *
 * \param out_stream output stream to print to e.g. \c stdout
 * \param x array of x values.
 * \param fx array of y values.
 * \param n_x number of elements in \p x and \p fx .
 * \return integer representing function success.
 * \retval 0 indicates success.
 */
int fts_print_data(FILE *out_stream, double *x, double *fx, size_t n_x)
{
  size_t i_x;
  
  for(i_x = 0; i_x < n_x; i_x ++)
	{
    fprintf(out_stream, "%e %e\n", x[i_x], fx[i_x] );
	}
 
  return(0);
}

