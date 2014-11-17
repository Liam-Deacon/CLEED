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


int leed_out_head_2 (const char *prg_version, const char *prg_name,
                FILE *outfile)

/************************************************************************

 write header information to output file.
 
 INPUT:

  leed_crystal *bulk_par - (input) for future use

  const char * prg_version   - (input) program version

  const char * prg_name      - (input) name of program.

  FILE * outfile - (input) pointer to the output file were the output 
            is written to.

 DESIGN:

  "#pn" name of program
  "#vn" No of version.
  "#ts" start time and date.

 RETURN VALUES:

   1   if o.k.
  -1   if failed

*************************************************************************/
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
 
 fprintf(outfile, "#ts %s", asctime(l_time) );
 fprintf(outfile, "#\n");

#ifdef CONTROL
 fprintf(STDCTR,"(leed_out_head): Start date: %s", asctime(l_time) );
#endif
 

 return(1);
}  /* end of function leed_out_head */
/************************************************************************/
