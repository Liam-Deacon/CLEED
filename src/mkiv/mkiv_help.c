/*********************************************************************
 *                        MKIV_HELP.C
 *
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/24.04.14 - creation
 *********************************************************************/

/*! \file
 *
 * Contains mkiv_usage() and mkiv_info()
 */

#include <stdio.h>
#include "mkiv_ver.h"

/* Print usage */
void mkiv_usage(FILE *output) 
{ 
    fprintf(output,"Syntax:\n%s\n\n", MKIV);
    fprintf(output, "Options:\n");
    fprintf(output, "  -i --input  <prefix> : "
            "followed by the fname prefix to input files [default='mkiv']\n");
    fprintf(output, "  -o --output <prefix> : "
            "followed by the fname prefix to output files [default='mkiv']\n");
    fprintf(output, "  -b --current <float> : "
            "followed by the beam current value\n");
    fprintf(output, "  -B --beam <prefix>   : "
            "followed by the prefix for .raw and .smo files [default='beam']\n");
    fprintf(output, "  -h --help            : print help\n");
    fprintf(output, "  -q --quiet --quick   : "
            "faster, no graphical output, only few printf's\n");
    fprintf(output, "  -v --verbose         : "
            "give more detailed information during computations\n");
    fprintf(output, "  -V --version         : "
            "give version and additional information about this program\n");
    fprintf(output, "\n");
    fprintf(output, "Input files:\n");
    fprintf(output, "  *.inp: input file for mkiv run\n");
    fprintf(output, "  *.var: variable file for mkiv run\n");
    fprintf(output, "  *.pos: position file for mkiv run\n");
    fprintf(output, "  LEED-images: image sequence in the form "
            "'*.[0-9]+?[0-9]+?[0-9]' where the extension is the energy.\n");
    fprintf(output, "  *.mask: a 2-bit mask of the LEED screen with black "
            "regions ignored during processing.\n");
    fprintf(output, "\n");
    fprintf(output, "Output files:\n");
    fprintf(output, "  *.ivdat: where ivdat is the I(V) data and filename "
            "prefix is the output prefix above\n");
    fprintf(output, "  *.param: parameter file for mkiv run "
            "(note filename may include path)\n");
    fprintf(output, "  *.raw : file for raw beam dump\n");
    fprintf(output, "  *.smo : file for smoothed beam dump\n");
}

/* print useful version information */
void mkiv_info()
{
    printf("%s - version %s (%s)\n", MKIV, MKIV_VERSION, PROG_PLAT);
    printf("%s\n", MKIV_SHORTDESC);
    printf("%s\n", MKIV_COPYRIGHT);
    printf("%s\n", MKIV_LICENSE);
    printf("Contact: %s\n", MKIV_MAINTAINER);
}
