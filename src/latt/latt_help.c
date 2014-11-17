/*********************************************************************
 *                           LATT_HELP.C
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/24.04.14 - Creation; header info taken from for latt.c
 *               - Added LATT* defines for program information and
 *                 ARG* macros for CLI argument evaluation.
 *********************************************************************/

#include "latt.h"

void latt_usage(FILE *output) {
	fprintf(output,"usage: %s -h <h> -k <k> -l <l> [OPTIONS...]\n", LATT);
  fprintf(output, "\n");
  fprintf(output, "Options:\n");
  fprintf(output, "  -a <lattice_constant> : specify lattice constant a\n");
  fprintf(output, "  -c <lattice_constant> : specify lattice constant c\n");
  fprintf(output, "  -d <max displacement> : max displacement in unit cells\n");
  fprintf(output, "                          [equiv. to d1=d2=a, d3=c*d]\n");
  fprintf(output, "  -h --help             : print help and exit\n");
  fprintf(output, "                          Note argument must not follow -h\n");
	fprintf(output, "  -i --input <input>    : input of basis vectors\n");
  fprintf(output, "  -n --atom <string>    : name of atom if simple input\n");
	fprintf(output, "  --nlayers <depth>     : depth of atomic layers\n");
	fprintf(output, "  --ncells <int>        : number of lateral unit cells\n");
	fprintf(output, "  --natoms <int>        : max number of atoms\n");
	fprintf(output, "  -o --output <output>  : output (default is stdout)\n");
	fprintf(output, "  -s --script <script>  : script on line #2 of output\n");
  fprintf(output, "  -t --lattice <type>   : e.g. 'fcc', 'bcc', 'hcp', 'dia'\n");
  fprintf(output, "  -V --version          : "
                  "print version and information about this program\n");
  fprintf(output, "\n");
  fprintf(output, "Input files:\n");
  fprintf(output, "  *.latt: input file for LEED simulation\n");
  fprintf(output, "\n");
  fprintf(output, "Output files:\n");
  fprintf(output, "  <output>: XYZ format atom output [with script]\n");
}

void latt_info()
{
  printf("%s - version %s (%s)\n", LATT, LATT_VERSION, LATT_PLAT);
  printf("\n%s\n", LATT_SHORTDESC);
  printf("\n%s\n%s\n", LATT_COPYRIGHT, LATT_LICENSE);
  printf("\nContact either Georg Held (g.held@reading.ac.uk) "
         "or %s for reporting bugs or submitting fixes.\n", LATT_MAINTAINER);
}
