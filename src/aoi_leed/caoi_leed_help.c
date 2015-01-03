/************************************************************************
 *                        CAOI_LEED_HELP.C
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Description:  file contains functions
 *   usage()
 *     Provides syntax help and other information before exiting
 *   info()
 *     Provides version information then exits
 *
 * Changes:
 *   LD/2014.04.14 - creation
 *********************************************************************/

#include "caoi_leed.h"

void caoi_leed_usage(FILE *output) {
    fprintf(output,"\tusage: \t%s -i <par_file> -o <res_file>", PROG);
    fprintf(output," [-b <bul_file> -e]\n"); 
    fprintf(output, "Options:\n");
    fprintf(output, "  -i <par_file>        : filepath to parameter input file\n");
    fprintf(output, "  -o <res_file>        : filepath to output file\n");
    fprintf(output, "  -b <bul_file>        : filepath to bulk parameter file\n");
    fprintf(output, "  -e                   : early return option\n");
    fprintf(output, "  -h --help            : print help and exit\n");
    fprintf(output, "  -V --version         "
        ": print version and information about this program\n");
    fprintf(output, "\n");
    fprintf(output, "Input files:\n");
    fprintf(output, "  *.inp: input file for LEED simulation\n");
    fprintf(output, "  *.bul: bulk variable file for LEED simulation\n");
    fprintf(output, "\n");
    fprintf(output, "Output files:\n");
    fprintf(output, "  <res_file>: the results output file (usually *.res)" 
            " used for the R factor program\n");
}

void caoi_leed_info()
{
    printf("%s - version %s (%s)\n", PROG, PROG_VERSION, PROG_PLAT);
    printf("\n%s\n", PROG_SHORTDESC);
    printf("\n%s\n%s\n", PROG_COPYRIGHT, PROG_LICENSE);
    printf("\nContact either Georg Held (g.held@reading.ac.uk) " 
        "or %s for reporting bugs or submitting fixes.\n", PROG_MAINTAINER);
}
