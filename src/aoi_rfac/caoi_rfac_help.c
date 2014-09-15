/*********************************************************************
LD/24.04.14
  file contains functions:

  usage (24.04.14)
     Provides syntax help and other information before exiting

  info (24.04.14)
     Provides version information then exits
  
Changes:

*********************************************************************/

#include "caoi_rfac.h"
#include <stdio.h>

void usage(FILE *output) {
    fprintf(output, "\n %s -c <control_file> -t <theory_file> [OPTIONS...]    ", PROG);
    fprintf(output, "\n");
    fprintf(output, "\n  ");
    fprintf(output, "\nOPTIONS:");
    fprintf(output, "\n  ");
    fprintf(output, "\n  --print");
    fprintf(output, "\n  -a <flag> ");
    fprintf(output, "\n      specify which group ID's appear in output.");
    fprintf(output, "\n	  arguments: \"all\", \"average\".");
    fprintf(output, "\n	  default: \"average\".");
    fprintf(output, "\n	  note only first two letters are significant.");
    fprintf(output, "\n");
    fprintf(output, "\n  --control");
    fprintf(output, "\n  -c <filename> ");
    fprintf(output, "\n      specify control file for averaging and assigning data input.");
    fprintf(output, "\n	  e.g. *.ctr");
    fprintf(output, "\n");
    fprintf(output, "\n  --help");
    fprintf(output, "\n  -h ");
    fprintf(output, "\n        Print help file.");
    fprintf(output, "\n");
    fprintf(output, "\n  --output");
    fprintf(output, "\n  -o <output file> ");
    fprintf(output, "\n      specify a file name for the r factor output to be written to.");
    fprintf(output, "\n	  valid arguments: \"stdout\" / \"-\", \"single\", <file name>.");
    fprintf(output, "\n	  default: \"stdout\".");
    fprintf(output, "\n");
    fprintf(output, "\n  --rfactor");
    fprintf(output, "\n  -r <r_factor>: specify a particular R-factor to be used for comparison.");
    fprintf(output, "\n	  valid arguments: \"r1\", \"r2\", \"rb\", \"rp\".");
    fprintf(output, "\n	  default: \"rp\".");
    fprintf(output, "\n");
    fprintf(output, "\n  --shift");
    fprintf(output, "\n  -s <shift>");
    fprintf(output, "\n      specify an energy range for shifting experimental and");
    fprintf(output, "\n	  theoretical data with respect to each other (Ee = Et + shift).");
    fprintf(output, "\n	  1-3 arguments: <first shift>{,<last shift>,<step>}");
    fprintf(output, "\n      default: -10, 10, 0.5;");
    fprintf(output, "\n");
    fprintf(output, "\n  --theory");
    fprintf(output, "\n  -t <filename>");
    fprintf(output, "\n      specify theoretical input file (outside control file) e.g. *.res");
    fprintf(output, "\n");
    fprintf(output, "\n  --potential");
    fprintf(output, "\n  -v <potential>");
    fprintf(output, "\n      imaginary part of optical potential (used for smoothing and for ");
    fprintf(output, "\n      computation of Pendry's R-factor).");
    fprintf(output, "\n	  arguments: floating point number [eV].");
    fprintf(output, "\n	  default: 4.0");
    fprintf(output, "\n");
    fprintf(output, "\n  --write");
    fprintf(output, "\n  -w <filename> ");
    fprintf(output, "\n      specify file name for iv curves output.");
}

void info()
{
    printf("%s - version %s (%s)\n", PROG, PROG_VERSION, PROG_PLAT);
    printf("\n%s\n", PROG_SHORTDESC);
    printf("\n%s\n%s\n", PROG_COPYRIGHT, PROG_LICENSE);
    printf("\nContact either Georg Held (g.held@reading.ac.uk) " 
        "or %s for reporting bugs or submitting fixes.\n", PROG_MAINTAINER);
}