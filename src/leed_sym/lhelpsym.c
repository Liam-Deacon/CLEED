/*********************************************************************
LD/24.04.14
  file contains functions:

  usage (24.04.14)
     Provides syntax help and other information before exiting

  info (24.04.14)
     Provides version information then exits
  
Changes:

*********************************************************************/

#include "leed_ver_sym.h"
#include "proghelp.h"

void usage_sym(FILE *output) {
    fprintf(output,"\tusage: \t%s -i <par_file> -o <res_file>", PROG);
    fprintf(output," [-b <bul_file> -r <pro_name> -w <pro_name>]\n"); 
    fprintf(output, "Options:\n");
    fprintf(output, "  -i <par_file>        : filepath to parameter input file\n");
    fprintf(output, "  -o <res_file>        : filepath to output file\n");
    fprintf(output, "  -b <bul_file>        : filepath to bulk parameter file\n");
    fprintf(output, "  -r <pro_name>        : read project file for matrices\n");
    fprintf(output, "  -w <pro_name>        : write project file for matrices\n");
    fprintf(output, "  -h --help            : print help and exit\n");
    fprintf(output, "  -V --version         : print version and information about this program\n");
    fprintf(output, "\n");
    fprintf(output, "Input files:\n");
    fprintf(output, "  *.inp: input file for LEED simulation\n");
    fprintf(output, "  *.bul: bulk variable file for LEED simulation\n");
    fprintf(output, "\n");
    fprintf(output, "Output files:\n");
    fprintf(output, "  <res_file>: the results output file (usually *.res)" 
            " used for the R factor program\n");
}

void info_sym()
{
    printf("%s - version %s (%s)\n", PROG, PROG_VERSION, PROG_PLAT);
    printf("\n%s\n", PROG_SHORTDESC);
    printf("\n%s\n%s\n", PROG_COPYRIGHT, PROG_LICENSE);
    printf("\nContact either Georg Held (g.held@reading.ac.uk) " 
        "or %s for reporting bugs or submitting fixes.\n", PROG_MAINTAINER);
}
