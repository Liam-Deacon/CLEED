/*********************************************************************
 *                        LHELP.C
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
 *********************************************************************/

/*! \file
 *
 * Provides functions for displaying command line syntax and version information
 * for the LEED program.
 */

#include "leed_ver.h"
#include "leed_help.h"

/*!
 * Prints LEED program help to \p output stream.
 *
 * Provides syntax help and other information before exiting
 *
 * \param output File pointer to direct output to e.g. \c stdout
 */
void leed_usage(FILE *output) {
    fprintf(output,"\tusage: \t%s -i <par_file> -o <res_file>", LEED_PROG);
    fprintf(output," [-b <bul_file> -e]\n"); 
    fprintf(output, "Options:\n");
    fprintf(output, "  -i <par_file>        : "
                    "filepath to parameter input file\n");
    fprintf(output, "  -o <res_file>        : filepath to output file\n");
    fprintf(output, "  -b <bul_file>        : "
                    "filepath to bulk parameter file\n");
    fprintf(output, "  -e                   : early return option\n");
    fprintf(output, "  -h --help            : print help and exit\n");
    fprintf(output, "  -V --version         : "
        "print version and information about this program\n");
    fprintf(output, "\n");
    fprintf(output, "Input files:\n");
    fprintf(output, "  *.inp: input file for LEED simulation\n");
    fprintf(output, "  *.bul: bulk variable file for LEED simulation\n");
    fprintf(output, "\n");
    fprintf(output, "Output files:\n");
    fprintf(output, "  <res_file>: the results output file (usually *.res)" 
            " used for the R factor program\n");
}

/*!
 * Displays LEED program version information.
 */
void leed_info()
{
    printf("%s - version %s (%s)\n",
           LEED_PROG, LEED_PROG_VERSION, LEED_PROG_PLAT);
    printf("\n%s\n", LEED_PROG_SHORTDESC);
    printf("\n%s\n%s\n", LEED_PROG_COPYRIGHT, LEED_PROG_LICENSE);
    printf("\nContact either Georg Held (g.held@reading.ac.uk) " 
           "or %s for reporting bugs or submitting fixes.\n",
           LEED_PROG_MAINTAINER);
}
