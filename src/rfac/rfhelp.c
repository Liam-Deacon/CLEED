/*********************************************************************
 *                           RFHELP.C
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1994.02.15 - Creation
 *   LD/2014.03.07 - Split from rfac.c
 *                 - Added hard-coded help if no RF_HELP_FILE in env
 *********************************************************************/

/*! \file rfhelp.c
 *
 * Implementation file for \ref file_content, \ref rf_help and \ref rf_info
 * functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "rfac.h"
#include "rfac_ver.h"

/*!
 * \fn file_content
 *
 * \brief Get the content of the file (like 'cat' command).
 *
 * \param *filename string constant containing the name of the file to read.
 *
 * \return pointer to string containing contents of file.
 */
char *file_content(const char* filename)
{
    FILE* file;
    long int size;
    char* content;
    
    file = fopen(filename, "r");
    
    if(file == NULL) return NULL;

    fseek(file, 0, SEEK_END);
    
    size = ftell(file);
    rewind(file);

    content = calloc(size + 1, 1);

    fread(content, 1, size, file);
    fclose(file);

    return content;
}

/* print crfac help */
/*!
 * \fn rf_help
 *
 * \brief Print a short program description explaining all arguments.
 *
 * \param *output file pointer to direct output e.g. stderr
 *
 */
void rf_help(FILE *output)
{
  char *contents = file_content(getenv("RF_HELP_FILE"));

  if ( contents != NULL) /* print file contents */
  {
    fprintf(output, contents);
  }
  else /* file not found */
  {
    /* use hard-coded help */
    fprintf(output, "SYNTAX:");
    fprintf(output, "\n  crfac -c <control_file> -t <theory_file> [OPTIONS...]    ");
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
}


/*!
 * \fn rf_info
 *
 * \brief Print information about the rfac program.
 *
 */
void rf_info()
{
    printf("%s - version %s (%s)\n", PROG, PROG_VERSION, PROG_PLAT);
    printf("%s\n", PROG_SHORTDESC);
    printf("%s\n", PROG_COPYRIGHT);
    printf("%s\n", PROG_LICENSE);
    printf("Contact: %s\n", PROG_MAINTAINER);
}

/********************************************************************/
