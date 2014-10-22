/*********************************************************************
 *                      FTS_READ_DATA.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/24.04.14 - creation (split from ftsmooth.c)
 *********************************************************************/

/*!
 * \file
 * @author Liam Deacon <liam.deacon@diamond.ac.uk>
 * \brief Implementation of fts_read_data() function.
 */

#include "ftsmooth.h"

/*!
 * Reads input data stream and assigns
 *
 * \param in_stream file pointer to read data in from e.g. \c stdin.
 * \param out_stream file pointer to echo comments to e.g. \c stdout.
 * \param[out] x array of x values.
 * \param[out] fx array of y values.
 * \return number of elements in arrays.
 */
size_t fts_read_data(FILE *in_stream, FILE *out_stream, double *x, double *fx)
{
  size_t i_x;
  size_t N = 1024;     /* array max size */
  
  for (i_x = 0; fgets(line_buffer, STRSZ, in_stream) != NULL; )
  {
  
    /* Lines beginning with '#' are interpreted as comments */
    if (line_buffer[0] == '#') fprintf(out_stream, "%s", line_buffer);
    else 
    {
      sscanf(line_buffer, "%le %le", x+i_x, fx+i_x);

      i_x ++;
      if(i_x >= N)
      {
        /* efficiently realloc N*2 amount of memory */
        N*=2;
        x =  (double *) realloc(x, N*sizeof(double) );
        fx = (double *) realloc(fx, N*sizeof(double) );
      } /* if (i_x >= N) */

    }

  }  /* for */
  
  return i_x;
}
