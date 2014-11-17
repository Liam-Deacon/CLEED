/*********************************************************************
 *                           PATT.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@diamond.ac.uk>
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *  Version 1.0: 22.03.93 (GH)
 *   GH/11.06.93 - Print also spot indices: optional can be switched off with
 *                 option "-ni".
 *   GH/08.02.94 - spot indices for commensurate structures as rational numbers
 *                 (instead of floats).
 *
 *  Version 1.3: 31.05.94
 *   GH/31.05.94 - different symbols are used for spots of different domains. The
 *                 option "-ns" forces the program to use only one symbol.
 *   GH/01.06.94 - Make fractions nicer.
 *
 *  Version 1.4: 11.06.13 (LD)
 *   LD/11.06.13 - Print only GS or SS spot indicies with "-gs" & "-ss" options.
 *   LD/11.06.13 - Coloured output of domains, control with '--rgb', '--black',
 *                 or '--greyscale'.
 *   LD/11.06.13 - Print special symbol characters with '${VARNAME}' in input file
 *                 for drawing header.
 *   LD/12.06.13 - Added printing simulated screen and gun with '--screen' & '--gun'
 *   LD/13.06.13 - Added printing of reciprocal lattice vectors  with '--vectors'
 *   LD/14.06.13 - Added clipping of spots (and vectors) to MAX_RADIUS with '--clip'
 *   LD/14.06.13 - Added energy scaling of Ewald construction with '--energy' or '-e'
 *   LD/14.06.13 - Added title using '--title' option
 *   LD/16.06.13 - Partial fix vectors text placement bug
 *   LD/17.06.13 - Added stdin default (with prompts) if no input file specified
 *********************************************************************/

/*! \file
 *  \brief patt -- LEED pattern visualisation program
 *
 * \c patt simulates Low Energy Electron Diffraction (LEED) patterns
 * to help in analysis of complex surface structures. Diffraction peaks
 * (spots), indices, reciprocal lattice vectors & different domains can
 * all be visualised using vector graphics (e.g. PostScript).
 *
 * \note A Cairo graphics backend has been developed which enables a variety
 * of additional output formats, including SVG, PDF and PNG. It can be enabled
 * by adding \c _USE_CAIRO to the compiler definitions and linking to the
 * libcairo library during compilation.
 */

#include <ctype.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "patt.h"
#include "patt_def.h"

/*!
 * main() entry into \c patt program.
 *
 * \param argc number of command line arguments.
 * \param argv command line argument strings.
 * \return #patt_error - code to indicate program success.
 * \retval #PATT_SUCCESS on successful completion.
 */
int main(int argc, char *argv[])
{
  patt_drawing *drawing = &drawing_default;

  patt_args(argc, argv, drawing);

  /* interactive terminal */
  if (isatty(fileno(stdin)) || drawing->interactive == true)
  {
    return(patt_session(drawing));
  }
  else /* stdin a pipe or file */
  {
    return(patt_draw((const patt_drawing*) drawing));
  }


  return(PATT_SUCCESS);
} /* main */

int patt_session(const patt_drawing *drawing)
{
  char buf[BUFSIZ];
  char u;

  printf("%s [%s] - %s\n", PATT_PROG, PATT_PROG_VERSION, PATT_PROG_SHORTDESC);
  printf("%s\n", PATT_PROG_COPYRIGHT);
  printf("This program is released under the terms of the %s\n",
         PATT_PROG_LICENSE);
  printf("\nType 'help' to get started...\n\n");

  while(gets(buf))
  {
    if (strncmp("quit", buf, 1) == 0) exit(PATT_SUCCESS);

    if (strncmp("help", buf, 1) == 0) patt_usage(stdout);

  }

  return(PATT_SUCCESS);
}
