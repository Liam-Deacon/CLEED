/*********************************************************************
LD/24.04.14
  file contains functions:

  usage (24.04.14)
     Provides syntax help and other information before exiting

  info (24.04.14)
     Provides version information then exits
  
Changes:

*********************************************************************/

#include "patt_ver.h"
#include "patt_def.h"
#include <stdio.h>

/* Print Patt usage */
void patt_usage(FILE *output)
{
   fprintf(output,"syntax:\npatt ");
   fprintf(output,"-i <input> -o <output> (-ni -rs <spot size> -rg <spot size> ...)\n\n");
   fprintf(output,"mandatory arguments:\n");
   fprintf(output,"-i <input>\tspecify input file path.\n");
   fprintf(output,"-o <output>\tspecify input file path.\n");
   fprintf(output,"\noptional arguments:\n");
   fprintf(output,"-fn <string>\tcustom footnote string.\n"); 
   fprintf(output,"--index (<GS flags> <SS flags>)\n");
   fprintf(output,"\tspecify indexes to show\n");
   fprintf(output,"\t<*flags> are a list of file indices to show\n");
   fprintf(output,"\te.g. '--index 0,2 1*' will print\n");
   fprintf(output,"\tGS spots from files 0 & 2\nSS spots from 1 to n\n");
   fprintf(output,"--index-gs\n");
   fprintf(output,"-gi\t\tonly print GS spot indices.\n");
   fprintf(output,"--index-ss\n");
   fprintf(output,"-si\t\tonly print SS spot indices.\n");
   fprintf(output,"--no-index\n");
   fprintf(output,"-ni\t\tdo not print spot indices.\n");
   fprintf(output,"--no-symbols\n");
   fprintf(output,"-ns\t\tdo not use different symbols\n");
   fprintf(output,"\t\tfor superstructure spots.\n");
   fprintf(output,"--radius\n");
   fprintf(output,"--radius-ss <spot size>\n");
   fprintf(output,"-rs <spot size>\tspot size of SS spots.\n");
   fprintf(output,"\t\tdefault: %2.1f\n", RADIUS_SS);
   fprintf(output,"--radius-gs\n");
   fprintf(output,"-rg <spot size>\tspot size of GS spots.\n");
   fprintf(output,"\t\tdefault: %2.1f\n", RADIUS_GS);
   fprintf(output,"--black\t\tdo not colour domains (default).\n");
   fprintf(output,"--greyscale\tuse greyscale coloured domains.\n");
   fprintf(output,"--rgb\t\tuse coloured domains.\n");
   fprintf(output,"--overlap\tprint SS spot strokes over GS spots.\n");
   fprintf(output,"--no-fill\tdo not fill any spots or screen.\n");
   fprintf(output,"\t\tnote: to only fill GS or SS spots\n");
   fprintf(output,"\t\tuse '--no-fill' then '--gs-fill'\n");
   fprintf(output,"\t\tor '--ss-fill', respectively.\n");
   fprintf(output,"--gs-fill\tfill GS (integer order) spots.\n");
   fprintf(output,"--ss-fill\tfill SS (superstructure) spots.\n");
   fprintf(output,
    "--gun <size> <angle>\n\t\tprint a simulated electron gun\n");
   fprintf(output,"\t\tnote: arguments are optional.\n");
   fprintf(output,"--screen\tprint a simulated LEED screen.\n");
   fprintf(output,"--screen-fill\tfill in screen if '--no-fill' used.\n");
   fprintf(output,"--legend\tadd legend for domains.\n");
   fprintf(output,"--help -h\tprint this help.\n");
   fprintf(output,"--energy -e\tenergy in eV.\n");
   fprintf(output,"--title -t\tTitle for figure.\n");
   fprintf(output,"--clip -c\tClip to edge of screen.\n");
   fprintf(output,"--version\tprint version info.\n");
   fprintf(output,"\nThe input is as follows:\n\n<Input file>\n");
   fprintf(output,"\nc 2 Domains of (3 2 / -2 1) = (r7 x r7)\n");
   fprintf(output,"1.0  1732 \ta1 <real lattice vector>\n");
   fprintf(output,"1.0 -1732 \ta2 <real lattice vector>\n");
   fprintf(output,
    "1.5	  	radius <= longest radius rez. Lattice vector * radius>\n");
   fprintf(output,
    "2 	  	Number of domains\n# M1\n   3 2 M1 	<first domain>\n");
   fprintf(output,
    "-2 1 M1\n# M2\nSy 		<Domain: mirroring the first on the 2nd y-axis>\n");
   fprintf(output,"<End Input>\n\n");
   fprintf(output,"#	- Comment (input file only)\n");
   fprintf(output,
    "c 	- Headline Comment (appears in the plot as a headline)\n\n");
   fprintf(output,
    "Possible representations of the SS-matrix:\n\n");
   fprintf(output,"m11 m12 (real matrix)\nm21 m22\n\n");
   fprintf(output,
    "R<phi> 	 - rotation of the SS prior to <phi> degrees\n");
   fprintf(output,
    "Sx	 - mirroring the previous SS on the x-axis\n");
   fprintf(output,
   "Sy 	 - mirroring the previous SS on the y-axis\n\n");
   fprintf(output,"Special input file variables (using '${VARNAME}'):\n");
   
}

void patt_info()
{
    printf("%s - version %s (%s)\n", PROG, PROG_VERSION, PROG_PLAT);
    printf("\n%s\n", PROG_SHORTDESC);
    printf("\n%s\n%s\n", PROG_COPYRIGHT, PROG_LICENSE);
    printf("\nContact either Georg Held (g.held@reading.ac.uk) " 
        "or %s for reporting bugs or submitting fixes.\n", PROG_MAINTAINER);
}
