/*********************************************************************
 *                      LOUTHEAD.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/11.08.95 - Creation
 *********************************************************************/

/*! \file */

#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "leed.h"

/*!
 * Writes header information to output file.
 *
 * \param[in] outfile File pointer to output file.
 */
void leed_output_header(FILE *outfile)
{
  extern bool SYMMETRY_ENABLED;
  const bool cleed_sym = SYMMETRY_ENABLED;

  struct tm *l_time;
  time_t t_time;

  fprintf(outfile, "# ####################################### #\n");
  fprintf(outfile, "#            output from CLEED            #\n%s",
       cleed_sym ? "#            (symmetry enabled)           #\n" : "");
  fprintf(outfile, "# ####################################### #\n");

  /* Write version number and start time to output file */
  fprintf(outfile, "#vn %s\n", LEED_VERSION);

  t_time = time(NULL);
  l_time = localtime(&t_time);
 
  fprintf(outfile, "#ts %s", asctime(l_time) );
  fprintf(outfile, "#\n");

  CONTROL_MSG(CONTROL, "Start date: %s", asctime(l_time) );
 
}  /* end of function leed_out_head */
