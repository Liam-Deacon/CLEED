/****************************************************************************
                        PATTERN (Georg Held 22.03.93)
NAME
	pattern -- LEED pattern visualisation program  

DESCRIPTION
	Pattern simulates Low Energy Electron Diffraction (LEED) patterns
to help in analysis of complex surface structures. Diffraction peaks 
(spots), indices, reciprocal lattice vectors & different domains can 
all be visualised using vector graphics (PostScript). 

INPUT FILE
	The input file specifies the coordinate system, substrate (GS) and
superstructure (SS), as well as any SS domains. The file has the following
format:
	* lines beginning with '#' are ignored by the program.
	* lines beginning with 'c' are treated as comments, note that
	  the first 'c' comment block will be printed as a legend.
	* real space lattice vectors a1 & a2 have the form: <x> <y> a[1-2]
	* radius is the maximum radius of Ewald construction in units of: a[1-2]
    * number of domains
    * superstructure matrix M1 & M2 of the form: <x> <y> M[1-2]
    * domain mirroring or rotation: Sx, Sy or R<#>

BUGS
	Please report any bugs (with a description on how to reproduce
the problem) to liam.deacon@diamond.ac.uk

AUTHORS
	Georg Held <g.held@reading.ac.uk>
	Liam Deacon <liam.deacon@diamond.ac.uk>

CHANGE-LOG

Version 1.0: 22.03.93
11.06.93:   Print also spot indices: optional can be switched off with
            option "-ni".
08.02.94:   spot indices for commensurate structures as rational numbers
            (instead of floats).

Version 1.3: 31.05.94
31.05.94:   different symbols are used for spots of different domains. The
            option "-ns" forces the program to use only one symbol.
01.06.94:   Make fractions nicer.

Version 1.4: 11.06.13 (LD)
11.06.13:	Print only GS or SS spot indicies with "-gs" & "-ss" options. 
11.06.13:	Coloured output of domains, control with '--rgb', '--black',
			or '--greyscale'.
11.06.13:	Print special symbol characters with '${VARNAME}' in input file
			for drawing header.
12.06.13:	Added printing simulated screen and gun with '--screen' & '--gun'
13.06.13:	Added printing of reciprocal lattice vectors  with '--vectors'
14.06.13:	Added clipping of spots (and vectors) to MAX_RADIUS with '--clip'
14.06.13:	Added energy scaling of Ewald construction with '--energy' or '-e' 
14.06.13:	Added title using '--title' option
16.06.13:	Partial fix vectors text placement bug
17.06.13:	Added stdin default (with prompts) if no input file specified

NOTES
	Compile with: gcc -lm pattern.c -o pattern
*/
/***************************************************************************/

#include <ctype.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "patt.h"
#include "patt_def.h"


/*========================================================================*/

int main(int argc, char *argv[])
{
  patt_drawing *drawing = &drawing_default;

  patt_args(argc, argv, drawing);

  if (isatty(fileno(stdin)) || drawing->interactive == true) /* interactive terminal */
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

/****************************************************************************/

/*========================================================================*/







