/*********************************************************************
 *                    FTSMOOTH_PROG_HELP.C
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
 *   LD/24.04.14 - creation.
 *********************************************************************/

#include "ftsmooth.h"

/*!
 * prints \c ftsmooth usage to \p output
 */
void ftsmooth_usage(FILE *output) 
{
  fprintf(output, "\nusage:\n\n%s ", FTSMOOTH_PROG);
  fprintf(output, "-i <input> -o <output> "
                  "(-m <mode> -c <cutoff> -t <tailoff> ...)\n\n");
  fprintf(output, "mandatory arguments:\n");
  fprintf(output, "\n\t-i <input>\tspecify input xy data file to process.\n");
  fprintf(output, "\t--input <input>\n");
  fprintf(output, "\n\t-o <output>\tspecify output xy data file.\n");
  fprintf(output, "\t--output <output>\n");
  fprintf(output, "\n");
  fprintf(output, "optional arguments:\n");
  fprintf(output, "\n\t-c <cut>\tenter a cut off value for fourier transform\n");
  fprintf(output, "\t--cutoff <cut>\tthis should be between 0 and 1 "
                  "(default=0.5).\n");
  fprintf(output, "\n\t-d\t\tdelete entries with negative y-values.\n"
                  "\t--delete\n");
  fprintf(output, "\n\t-h\t\tdisplay syntax help.\n\t--help\n");
  fprintf(output, "\n\t-m <mode>\tspecify data smoothing operation mode "
                  "(default='n').\n");
  fprintf(output, "\t--mode <mode>\toptions are: "
                        "'s' smoothing, 'n' no-smoothing.\n");
  fprintf(output, "\n\t--range <bounds>\n");
  fprintf(output, "\t-r <bounds>\tthe accepted x-range for the data, "
                  "specified by\n");
  fprintf(output, "\t\t\t <bounds>. Upper & lower x-axis bounds must be decimals\n");
  fprintf(output, "\t\t\tand where <bounds> may take the following format:\n");
  fprintf(output, "\t\t\t\"lb(1):ub(1),lb(2):ub(2),...,lb(N):ub(N)\"\n");
  fprintf(output, "\t\t\t':' acts as a seperator between lb & ub\n");
  fprintf(output, "\t\t\t',' acts as a seperator between indicies\n");
  fprintf(output, "\t\t\t'i' or '*' will ignore bounds, i.e.\n");
  fprintf(output, "\t\t\t'0.2:10,0.5:*' will output data between 0.2 & max\n");
  fprintf(output, "\t\t\te.g. \"-r i 300\" will only set the upper bound to 300.\n");
  fprintf(output, "\n\t-t <tail>\tenter a tail-off value for fourier transform\n");
  fprintf(output, "\t\t\t(default=10).\n");
  fprintf(output, "\t--tailoff <tail>\n");
  fprintf(output, "\n\t--version\tprint version info.\n");
  fprintf(output, "\n\t-y (<offset>)\toffset the y-axis data by the specified value\n");
  fprintf(output, "\t--yoffset\tnote: if no value is specified, the offset will\n");
  fprintf(output, "\t\t\tbe minima = 0.\n");
  fprintf(output, "\n");
}

/*!
 * print program versioning information.
 */
void ftsmooth_info()
{
  printf("%s - version %s (%s)\n",
        FTSMOOTH_PROG, FTSMOOTH_PROG_VERSION, FTSMOOTH_PROG_PLAT);
  printf("\n%s\n", FTSMOOTH_PROG_SHORTDESC);
  printf("\n%s\n%s\n", FTSMOOTH_PROG_COPYRIGHT, FTSMOOTH_PROG_LICENSE);
  printf("\nContact either Georg Held (g.held@reading.ac.uk) "
         "or %s for reporting bugs or submitting fixes.\n",
         FTSMOOTH_PROG_MAINTAINER);
}
