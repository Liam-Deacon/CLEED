/************************************************************************
 * <FILENAME>
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *27.09.00 
  file contains function:

  int leed_out_head_2( const char * prg_version, const char * prg_name,
                  FILE * outfile)

 Write header information to output file.

 Changes:
 
 GH/11.08.95 - Creation
 GH/27.09.00 - add program name and version to parameter list

*********************************************************************/

#include <stdio.h>
#include <time.h>

#include "leed.h"

/*!
 * Write header information to output file
 *
 * \param[in] prg_version program version string.
 * \param[in] prg_name program name string.
 * \param[inout] outfile pointer to the output file stream.
 *
 */
int leed_out_head_2 (const char *prg_version, const char *prg_name,
                FILE *outfile)
{
  struct tm *l_time;
  time_t t_time;

  fprintf(outfile, "# ####################################### #\n");
  fprintf(outfile, "#            output from %s\n", prg_name);
  fprintf(outfile, "# ####################################### #\n");

  /************************************************************************
  Write program name and version number and start time to output file
   ************************************************************************/
  fprintf(outfile, "#pn %s\n", prg_name);
  fprintf(outfile, "#vn %s\n", prg_version);

  t_time = time(NULL);
  l_time = localtime(&t_time);

  fprintf(outfile, "#ts %s", asctime(l_time));
  fprintf(outfile, "#\n");

  CONTROL_MSG(CONTROL, "Start date: %s", asctime(l_time));

  return(0);
}  /* end of function leed_out_head */
