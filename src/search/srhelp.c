/************************************************************************
 *                            SRHELP.C
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/2014.04.24 - creation
 ***********************************************************************/

/*!
 * \file
 * \author Liam Deacon <liam.deacon@diamond.ac.uk>
 * \brief provides syntax help and version information for \c csearch .
 *
 */

#include "search_ver.h"
#include <stdio.h>

/*!
 * Output program syntax for command line arguments.
 *
 * \param output where to direct the output e.g. to \c stdout or \c stderr .
 */
void search_usage(FILE *output) {
	fprintf(output, "usage: \t%s -i <inp_file> \n", SEARCH);
  fprintf(output,
          "      \t   [-d <delta> -v <vertex_file> -s <search_type> ...]\n");
  fprintf(output, "\n");
  fprintf(output, "Options:\n");
  fprintf(output, "  -b <bul_file>         : bulk parameter input file\n"
          "                         (assumed same prefix as <inp_file>)\n");
	fprintf(output, "  -c <ctr_file>         : control file for IV curves\n"
          "                         (assumed same prefix as <inp_file>)\n");
  fprintf(output, "  -d <delta>            : initial displacement\n");
  fprintf(output, "  -h --help             : print help and exit\n");
	fprintf(output, "  -i <inp_file>         : surface parameter input file\n");
	fprintf(output, "  -s <search_type>      : can be \n"
                  "                          'ga' = genetic algorithm\n"
                  "                          'sa' = simulated annealing\n"
                  "                          'si' = simplex method (default)\n"
                  "                          'sx' = simplex - duplicate\n"
                  "                          'po' = simulated annealing\n");
  fprintf(output, "  -v <vertex_file>      : "
          "file to read vertex information if resuming search\n");
  fprintf(output, "  -V --version          : "
          "print version and information about this program\n");
  fprintf(output, "\n");
}

/*!
 * Provides version information for \c csearch .
 */
void search_info()
{
    printf("%s - version %s (%s)\n", SEARCH, SEARCH_VERSION, SEARCH_PLAT);
    printf("\n%s\n", SEARCH_SHORTDESC);
    printf("\n%s\n%s\n", SEARCH_COPYRIGHT, SEARCH_LICENSE);
    printf("\nContact either Georg Held (g.held@reading.ac.uk) " 
        "or %s for reporting bugs or submitting fixes.\n", SEARCH_MAINTAINER);
}
