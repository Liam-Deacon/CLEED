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

       fprintf(output,"1.0  1732 \ta1 <real lattice vector>\n");
   fprintf(output,"1.0 -1732 \ta2 <real lattice vector>\n");
   fprintf(output,"1.5	  	radius <= longest radius rez. Lattice vector * radius>\n");
   fprintf(output,"2 	  	Number of domains\n# M1\n   3 2 M1 	<first domain>\n");
   fprintf(output,"-2 1 M1\n# M2\nSy 		<Domain: mirroring the first on the 2nd y-axis>\n");
   fprintf(output,"<End Input>\n\n");
   fprintf(output,"#	- Comment (input file only)\n");
   fprintf(output,"c 	- Headline Comment (appears in the plot as a headline)\n\n");
   fprintf(output,"Possible representations of the SS-matrix:\n\n");
   fprintf(output,"m11 m12 (real matrix)\nm21 m22\n\n");
   fprintf(output,"R<phi> 	 - rotation of the SS prior to <phi> degrees\n");
   fprintf(output,"Sx	 - mirroring the previous SS on the x-axis\n");
   fprintf(output,"Sy 	 - mirroring the previous SS on the y-axis\n\n");
   fprintf(output,"Special input file variables (using '${VARNAME}'):\n");

BUGS
    Please report any bugs (with a description on how to reproduce
the problem) to l.deacon@pgr.reading.ac.uk

AUTHORS
    Georg Held <g.held@reading.ac.uk>
    Liam Deacon <l.deacon@pgr.reading.ac.uk>

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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strings.h>
#include <ctype.h>

#ifdef USE_MAGICK
#include <MagickCore>
#include <MagickWand>
#endif

#ifdef USE_CONFIG
#include "config.h"
#endif

#define VERSION "PATTERN - version 1.4 LD/13.06.13"
#define NOTES "Pattern is part of the CLEED package by Georg Held\n\t(contact g.held@reading.ac.uk)"
#ifndef BUG_CONTACT
#define BUG_CONTACT "Liam Deacon (l.m.deacon@gmail.co.uk)"
#endif

#define SPOT_GS 0
#define SPOT_SS 1
#define STRSZ 128

#define OFF_H      400.
#define OFF_W      300.

#define LINE_WIDTH 2.
#define MAX_RADIUS 200.

#define MAX_INPUT_FILES 6
#define PS_TEMP_FILE "tmp_patt.ps"

#define RADIUS_GS 10.
#define GREY_GS    0.

#define RADIUS_SS  5.

/* define postscript colours */
#define PSRED "1 0 0 setrgbcolor"			//red
#define PSDKRED "0.7 0.1 0 setrgbcolor"		//dark red
#define PSGREEN "0 1 0 setrgbcolor"			//green
#define PSBLUE "0 0 1 setrgbcolor"			//blue
#define PSDKBLUE "0 0 0.5 setrgbcolor"		//dark blue
#define PSLTBLUE "0 1 1 setrgbcolor"		//light blue
#define PSMAGENTA "1 0 1 setrgbcolor"		//magenta
#define PSYELLOW "1 1 0 setrgbcolor"		//yellow
#define PSWHITE "1 1 1 setrgbcolor"			//white
#define PSBLACK "0 0 0 setrgbcolor"			//black
#define PSORANGE "1.0 0.7 0.0 setrgbcolor"	//orange
#define PSDKORANGE "1 0.33 0 setrgbcolor"	//dark orange
#define PSPURPLE "0.7 0.3 1.0 setrgbcolor"	//purple
#define PSBROWN "0.7 0.3 0.0 setrgbcolor"	//brown
#define PSDKGREEN "0.0 0.5 0.0 setrgbcolor"	//dark green
#define PSGREY "0.5 setgray"				//grey
#define PSDKGREY "0.25 setgray"				//dark grey
#define PSLTGREY "0.75 setgray"				//light grey
#define PSGOLD "0.83 0.83 0.17 setrgbcolor" //'gold'
#define PSDKCYAN "0 0.67 0.67 setrgbcolor"  //dark cyan
#define PSCYAN "0.08 0.92 0.92 setrgbcolor" //cyan

#define EDGE_COLOUR "0.55 1 0.55 setrgbcolor"
#define SCREEN_COLOUR "0.85 1 0.85 setrgbcolor"
#define GUN_COLOUR "0 0 0 setrgbcolor"

/* alter the following list for order of preference */
#define NUM_COLOURS 30
#define NUM_GREYS 9
#define NUM_SUBS 54

/* declare function prototypes & constants */
void add_PSvectors(FILE *out_stream);

int decode_vectors(float spot_type, int *do_vectors, char *argv, int str_length);

int draw_final(FILE *out_stream, int gun_flag, float gun_thickness,
    float gun_radians, float screen_thickness, int screen_flag,
    int vectors_flag, char *vectors_str, int ifiles, int ev_flag,
    float eV, char *title_str);

int draw_gun(FILE *out_stream, float gun_thickness, float gun_radius,
    float position_radians, float edge_thickness, int screen_flag);

int draw_initial(FILE *out_stream, int ifiles, char *title_str,
    float *pos_title, char *footnote, float *pos_footnote, int screen_flag,
    float screen_thickness, int fill_screen, int ev_flag,
    float eV, float *pos_ev, int clip_flag, int vectors_flag);

int draw_title(FILE *in_stream, FILE *out_stream, int ii, int ifiles,
        char *colour, int *i_line_offset);

int draw_screen(FILE *out_stream, float edge_thickness, int fill);

int draw_spot(FILE *out_stream, float x, float y,
    float spot_size, int shape, char *colour, char *fill);

int draw_vectors(FILE *out_stream, int i_dom, int ii, char *colour, float *a1, float *a2,
        float spot_radius, char dummystr[][STRSZ], float spot, char *vectors_str);

int nice_frac(int *zahler, int *nenner);

int magick_ps2png(char *filename_in, char *filename_out);

char *colour(int colour_flag, int ifile, int nfiles, int spot_type, int i_dom);

char colours[NUM_COLOURS][STRSZ] = {
    PSRED, PSBLUE, PSGREEN, PSMAGENTA, PSLTBLUE, PSDKGREEN,
    PSORANGE, PSCYAN, PSYELLOW, PSBROWN, PSPURPLE, PSGREY,
    PSDKRED, PSDKCYAN, PSDKGREY, PSDKORANGE, PSLTBLUE, PSGOLD,
    //repeat for now as not enough colours
    PSRED, PSBLUE, PSGREEN, PSMAGENTA, PSLTBLUE, PSDKGREEN,
    PSORANGE, PSCYAN, PSYELLOW, PSBROWN, PSPURPLE, PSGREY
};

char file_colours[6][2][STRSZ] = {
    {PSBLACK, PSDKGREY}, {PSDKRED, PSRED},
    {PSBLUE, PSLTBLUE}, {PSDKGREEN, PSGREEN},
    {PSDKORANGE, PSORANGE}, {PSGOLD, PSYELLOW}
};

char greyscale[NUM_GREYS][STRSZ] = {
    "0.1 setgray", "0.2 setgray", "0.3 setgray",
    "0.4 setgray", "0.5 setgray", "0.6 setgray",
    "0.7 setgray", "0.8 setgray", "0.9 setgray"
};

/* map to postscript symbol font */
char substitutes[NUM_SUBS][2][STRSZ] = {
 { "SQRT", "\\326" }, { "√", "\\326" },
 { "DEGREES", "\\260" }, { "°", "\\260" },
 { "MUL", "\\264" }, { "×", "\\264" },
 { "ALPHA", "\\141" }, { "α", "\\141" },
 { "BETA", "\\142" }, { "β", "\\142" },
 { "GAMMA", "\\147" }, { "γ", "\\147" },
 { "ALT-GAMMA", "\\107" }, { "Γ", "\\107" },
 { "DELTA", "\\104" }, { "Δ", "\\104" },
 { "ALT-DELTA", "\\144" }, { "δ", "\\144" },
 { "THETA", "\\161" }, { "θ", "\\161" },
 { "ALT-THETA", "\\112" }, { "Θ", "\\112" },
 { "PHI", "\\146" }, { "Φ", "\\146" },
 { "ALT-PHI", "\\152" }, { "φ", "\\152" },
 { "LAMBDA", "\\154" }, { "λ", "\\154" },
 { "KAPPA", "\\153" }, { "κ", "\\153" },
 { "PLUSMINUS", "\\261" }, { "±", "\\261" },
 { "PROPORTIONAL", "\\265" }, { "∝", "\\265" },
 { "OMEGA", "\\127" }, { "Ω", "\\127" },
 { "ALT-OMEGA", "\\167" }, { "ω", "\\167" },
 { "MU", "\\155" }, { "μ", "\\155" },
 { "NU", "\\156" }, { "ν", "\\156" },
 { "NOTEQ", "\\271" }, { "!=", "\\271" }, { "≠", "\\271" },
 { "APPROXEQ", "\\273" }, { "~=", "\\273" }, { "≈", "\\273" },
 { "PI", "\\160" }, { "π", "\\160" },
 { "PSI", "\\131" }, { "Ψ", "\\131" },
 { "ALT-PSI", "\\171" }, { "ψ", "\\171" }
};

char line_buffer[STRSZ];

/* Search string & convert to postscript where needed */
int print_ps_substitute(FILE *output, char *str)
{
    int i, j, k, last;
    char substr[STRSZ];
    char prestr[STRSZ];
    char poststr[STRSZ];
    strcpy(poststr, str);

    if (str[0] == '\0') return 0;

    //printf("%s\n", str);
    last=0;
    for (i=0; i<=strlen(str)-1;i++)
    {
      if (str[i] == '$' && str[i+1] == '{' )
      {
       i++; j=0;
       strcpy(substr, "\0");
       strcpy(prestr, "\0");
       fprintf(output,"/Times-Roman findfont 20 scalefont setfont (");
       while (str[i] != '}' && i<=STRSZ-2)
       {
        i++;
        substr[j] = str[i];
        j++;
       }
       //printf("%i %i %i\n", i, j, last);

       for (k=last;k<=i-j-2;k++)
       {
         if (str[k] == '(' || str[k] == ')' ||
             str[k] == '{'  || str[k] == '}' )
              fprintf(output, "%c", '\\' );
         if (str[k] == '\0') break;
         fprintf(output,"%c", str[k]);
         prestr[k] = str[k];
       }

       last = i+1;
       fprintf(output, ") show\n");
       for (k=0; k<=NUM_SUBS-1;k++)
       {
        if (strncmp(substr, substitutes[k][0], j-1) == 0)
        {
          fprintf(output, "/Symbol findfont 20 scalefont setfont (%s) show\n", substitutes[k][1]);
        }
       }
      }
    }
    fprintf(output, "/Times-Roman findfont 20 scalefont setfont (");
    for (k=last+j; k<=STRSZ-1;k++)
    {
     if ((str+k == NULL) || (str[k] == '\0')) break;
     fprintf(output, "%c", str[k]);
    }
    fprintf(output,") show\n");
    return last;
}

/*========================================================================*/
char *fget_nocomm(char *line_buffer, FILE *in_stream, FILE *out_stream)
/*
 Lines beginning with '#' are interpreted as comments.
*/
{
  while( (fgets(line_buffer, STRSZ, in_stream) != NULL) &&
     (*line_buffer == '#') )
  {
   printf ("%s", line_buffer);
   fprintf(out_stream,"%s %s","%", line_buffer);
  }
  if (line_buffer == NULL) printf("***input error");
 return(line_buffer);
}
/*========================================================================*/

/* Print Patt usage */
void usage(FILE *output, char *argv)
{
   fprintf(output,"syntax:\n%s ",argv);
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
   fprintf(output,"--gun <size> <angle>\n\t\tprint a simulated electron gun\n");
   fprintf(output,"\t\tnote: arguments are optional.\n");
   fprintf(output,"--screen\tprint a simulated LEED screen.\n");
   fprintf(output,"--screen-fill\tfill in screen if '--no-fill' used.\n");
   fprintf(output,"--legend\tadd legend for domains.\n");
   fprintf(output,"--help -h\tprint this help.\n");
   fprintf(output,"--version\tprint version info.\n");
   fprintf(output,"\nThe input is as follows:\n\n<Input file>\n");
   fprintf(output,"\nc 2 Domains of (3 2 / -2 1) = (r7 x r7)\n");
   fprintf(output,"1.0  1732 \ta1 <real lattice vector>\n");
   fprintf(output,"1.0 -1732 \ta2 <real lattice vector>\n");
   fprintf(output,"1.5	  	radius <= longest radius rez. Lattice vector * radius>\n");
   fprintf(output,"2 	  	Number of domains\n# M1\n   3 2 M1 	<first domain>\n");
   fprintf(output,"-2 1 M1\n# M2\nSy 		<Domain: mirroring the first on the 2nd y-axis>\n");
   fprintf(output,"<End Input>\n\n");
   fprintf(output,"#	- Comment (input file only)\n");
   fprintf(output,"c 	- Headline Comment (appears in the plot as a headline)\n\n");
   fprintf(output,"Possible representations of the SS-matrix:\n\n");
   fprintf(output,"m11 m12 (real matrix)\nm21 m22\n\n");
   fprintf(output,"R<phi> 	 - rotation of the SS prior to <phi> degrees\n");
   fprintf(output,"Sx	 - mirroring the previous SS on the x-axis\n");
   fprintf(output,"Sy 	 - mirroring the previous SS on the y-axis\n\n");
   fprintf(output,"Special input file variables (using '${VARNAME}'):\n");

}

int main(int argc, char *argv[])
{
int gun_flag;
int screen_flag;
int stdin_flag;
int overlap_flag;   /* 1 to overlap GS spots with SS spot strokes */
int colour_flag;	/* 1 for use rgb, 0 for black, -1 for greyscale */
int clip_flag;
int vectors_flag;
int ev_flag;
int rg_flag;
int rs_flag;
int png_flag;

int i_arg;

float pos_ev[3];
float pos_footnote[3];
float pos_title[3];
float translate[2];

char title_str[STRSZ];
char fill_gs[6] = "fill";
char fill_ss[6] = "fill";
int fill_screen;

int index;                      /* write indices to output */
int symbols;                    /* use different symbols for superstructures*/

int i1, i2, s1, s2;
int imax_1, imax_2;
int smax_1, smax_2;

int i_dom, n_dom;

float x_a1[2], x_a2[2];             /* real lattice vectors GS */

float a1[2], a2[2];                 /* reciprocal lattice vectors GS */
float b1[2], b2[2];                 /* reciprocal lattice vectors SS */

float M11, M12, M21, M22;           /* SS matrix */
float m11, m12, m21, m22;           /* used SS matrix */
float N11, N12, N21, N22;           /* Rotation  matrix */
float phi;                          /* Rotation angle */

float aux1, aux2;
float det;                          /* Det of SS-matrix */
int det_int;                        /* Det of SS-matrix */
float spot_ss, spot_gs;
float ix, iy, x, y;
float ind_1, ind_2;                 /* SS index */
int ind_1_int, ind_2_int;           /* SS index */
float radius;
int i_line_offset;
char comm;                          /* commensurate or not */
char text[STRSZ];
char text2[STRSZ];
char dummystr[5][STRSZ];
char vectors_str[STRSZ*8];
char output_filename[STRSZ*2];

char* footnote = NULL;


int ifiles, ii;
int do_gs_vectors[MAX_INPUT_FILES];
int do_ss_vectors[MAX_INPUT_FILES][STRSZ];
int do_gs_indices[MAX_INPUT_FILES];
int do_ss_indices[MAX_INPUT_FILES];

float gun_radians, gun_thickness, screen_thickness;
float eV;

FILE *in_streams[MAX_INPUT_FILES];
FILE *in_stream;
FILE *out_stream;

/* Preset out_stream */
 out_stream = stdout;
 in_stream = stdin;
 in_streams[0] = stdin;

 index = 1;
 symbols = 1;

 ifiles = 1;

 spot_gs = RADIUS_GS;
 spot_ss = RADIUS_SS;

 clip_flag = 0;
 colour_flag = 1; //use rgb as default
 ev_flag = 0;
 png_flag = 0;
 overlap_flag = 0;
 screen_flag = 0;
 gun_flag = 0;
 fill_screen = 1;
 vectors_flag = 0;
 stdin_flag = 1;
 rs_flag = 0;
 rg_flag = 0;
 i_arg =0;
 i_line_offset = 0; //title line y-offset

 gun_thickness = RADIUS_GS;
 gun_radians = 0.65;
 screen_thickness = RADIUS_GS*0.5;

 //set default positions and text sizes
 pos_title[0] = -(MAX_RADIUS);
 pos_title[1] = (MAX_RADIUS + 150.);
 pos_title[2] = 26.; //text size in times-bold
 pos_footnote[0] = -MAX_RADIUS;
 pos_footnote[1] = -(MAX_RADIUS + 50.);
 pos_footnote[2] = 10.; //text size in times-roman
 pos_ev[0] = (MAX_RADIUS*1.)*cos(M_PI*0.25);
 pos_ev[1] = -(MAX_RADIUS*1.2)*sin(M_PI*0.25);
 pos_ev[2] = 20; //text size in times-italicbold
 translate[0] = 0; //text offset x
 translate[1] = 0; //text offset y

 strcpy(vectors_str,"\0");
 // loop over input files
 for (ii=0;ii<5;ii++)
 {
   strcpy(dummystr[ii],"\0");
   do_gs_vectors[ii] = 0;
   do_gs_indices[ii] = 1;
   do_ss_indices[ii] = 1;
   //loop over domains
   for (i_arg=0;i_arg<STRSZ;i_arg++)
     do_ss_vectors[ii][i_arg] = 0;
 }
 strcpy(title_str,"\0");

/*
 Check command line and decode arguments
*/

 if (argc < 2)
 {
  fprintf(stderr,"\n\tsyntax error:\n");
  usage(stderr, argv[0]);
  exit(1);
 }

 for (i_arg = 1; i_arg < argc; i_arg++)
 {
  if (*argv[i_arg] != '-')
  {
   fprintf(stderr,"\n\tsyntax error:\n");
   usage(stderr, argv[0]);
   exit(1);
  }
  else
  {
/* Title */
   if((strncmp(argv[i_arg], "-t", 2) == 0) ||
      (strncmp(argv[i_arg], "--title", 7) == 0))
   {
     if (i_arg+1 < argc)
     {
        i_arg++;
        strcpy(title_str, argv[i_arg]);
        strcat(title_str, "\0");
     }
     for (ii=0;ii<3;ii++)
     {
       if (i_arg+1 < argc)
        if (isdigit(argv[i_arg+1][1]))
        {
          i_arg++;
          pos_title[ii] = atof(argv[i_arg]);
        }
        //printf("%i %.1f %c\n", ii, pos_title[ii],argv[i_arg][1]);
     }
   }
/* Set Electron Energy */
   if((strncmp(argv[i_arg], "--energy", 8) == 0)
       || (strncmp(argv[i_arg], "-e", 2) == 0))
   {
        i_arg++;
        eV = atof(argv[i_arg]);
        ev_flag = 1;
        for (ii=0;ii<3;ii++)
        {
          if (i_arg+1 < argc)
            if(isdigit(argv[i_arg+1][1]))
            {
              i_arg++;
              pos_ev[ii] = atof(argv[i_arg]);
            }
        }
   }
/* Clip spots to edge of MAX_RADIUS */
   if((strncmp(argv[i_arg], "--clip", 6) == 0)
      || (!strncmp(argv[i_arg], "-c", 2)))
   {
     clip_flag = 1;
   }
/* Draw k-space vectors */
   if(strncmp(argv[i_arg], "--vectors", 9) == 0)
   {
     vectors_flag = 1; ii=-1;
     while ((i_arg+1 < argc) && (*argv[i_arg+1] != '-'))
     {
        i_arg++;
        if (ii < 0)
          decode_vectors(SPOT_GS, do_gs_vectors, argv[i_arg], strlen(argv[i_arg]));
        else
          decode_vectors(SPOT_SS, do_ss_vectors[ii],
            argv[i_arg], strlen(argv[i_arg]));
        ii++;
     }
   }
/* Draw a 'screen' */
   if((strncmp(argv[i_arg], "--screen", 8) == 0)
      || (!strncmp(argv[i_arg], "-s", 2)))
   {
     screen_flag = 1;
   }
/* Draw a simulated e-gun */
   if((strncmp(argv[i_arg], "--gun", 5) == 0)
      || (!strncmp(argv[i_arg], "-g", 2)))
   {
     gun_flag =1;
     if (i_arg+1 < argc)
       if(isdigit(argv[i_arg+1][1]))
       {
         // printf("%i of %i:%s\n",i_arg,argc,argv[i_arg]);
         i_arg++;
         gun_radians = atof(argv[i_arg]);
         if (gun_radians > 2*M_PI)
         {
           //asume degrees
           if (gun_radians > 360.)
             gun_radians = (int)(gun_radians) % 360;
           gun_radians *= M_PI/180;
         }
       }
     if (i_arg+1 < argc)
     {
       if(isdigit(argv[i_arg+1][1]))
       {
         // printf("%i of %i:%s\n",i_arg,argc,argv[i_arg]);
         i_arg++;
         gun_thickness = atof(argv[i_arg]);
       }
     }
   }
/* Do not fill spots or screen */
   if(strncmp(argv[i_arg], "--no-fill", 9) == 0)
   {
     fill_screen = 0;
     strcpy(fill_gs, "\0");
     strcpy(fill_ss, "\0");
   }
/* Fill GS spots */
   if(strncmp(argv[i_arg], "--gs-fill", 9) == 0)
   {
     strcpy(fill_gs, "fill");
   }
/* Fill SS spots */
   if(strncmp(argv[i_arg], "--ss-fill", 9) == 0)
   {
     strcpy(fill_ss, "fill");
   }
/* Fill 'screen' */
   if(strncmp(argv[i_arg], "--screen-fill", 13) == 0)
   {
     fill_screen = 1;
   }
/* Allow drawing of SS spots over GS spots */
   if(strncmp(argv[i_arg], "--overlap", 9) == 0)
   {
     overlap_flag = 1;
   }
/* Use colour */
   if(strncmp(argv[i_arg], "--rgb", 5) == 0)
   {
     colour_flag = 1;
   }
/* Use greyscale */
   if(strncmp(argv[i_arg], "--greyscale", 4) == 0)
   {
     colour_flag = -1;
   }
/* Use black only */
   if(strncmp(argv[i_arg], "--black", 7) == 0)
   {
     colour_flag = 0;
   }
/* Open input file(s) */
   if( (strncmp(argv[i_arg], "-i", 2) == 0)
     || (!strncmp(argv[i_arg], "--input", 7)))
   {
    i_arg++;
    stdin_flag = 0;
    ifiles = 0;
    while ((strncmp(argv[i_arg], "-", 1)) && (i_arg < argc))
    {
      if (ifiles >= MAX_INPUT_FILES)
      {
        fprintf(stderr,
            "warning: too many input files (max = %i)!\n",
            MAX_INPUT_FILES);
        fprintf(stderr, "/textra files will be ignored...\n");
      }
      else
      {
        if ((in_streams[ifiles] = fopen(argv[i_arg],"r")) == NULL)
        {
          fprintf(stderr,"error: failed to open '%s'\n",argv[i_arg]);
          exit(1);
        }
        ifiles += 1;
      }
      i_arg++;
    }
   }
/* Open output file */
   if( (strncmp(argv[i_arg], "-o", 2) == 0)
     || (!strncmp(argv[i_arg], "--output", 8)))
   {
    i_arg++;
    if (out_stream != stdout)
        fclose(out_stream);
    strcpy(output_filename, argv[i_arg]);
    if (png_flag)
    {
      // open temporary postscript file then convert later
      if ((out_stream = fopen(PS_TEMP_FILE,"w")) == NULL)
      {
        fprintf(stderr,"error: failed to open temporary '%s'\n",PS_TEMP_FILE);
        exit(1);
      }
    }
    else
    {
      if ((out_stream = fopen(argv[i_arg],"w")) == NULL)
      {
       fprintf(stderr,"error: failed to open '%s'\n",argv[i_arg]);
       exit(1);
      }
    }
   }
/* Do not print indices */
   if(strncmp(argv[i_arg], "-ni", 3) == 0)
   {
    index = 0;
   }
/* Only print GS spot indices */
   if((strncmp(argv[i_arg], "-gi", 3) == 0) ||
      (!strncmp(argv[i_arg], "--index-gs", 10)))
   {
    index = 1;
    for (ii=0;ii<MAX_INPUT_FILES;ii++)
    {
      do_gs_indices[ii] = 1;
      do_ss_indices[ii] = 0;
    }
   }
/* Only print SS spot indices */
   if((strncmp(argv[i_arg], "-si", 3) == 0) ||
   (!strncmp(argv[i_arg], "--index-ss", 10)))
   {
    index = 1;
    for (ii=0;ii<MAX_INPUT_FILES;ii++)
    {
        do_gs_indices[ii] = 0;
        do_ss_indices[ii] = 1;
    }
   }
/* print specified indicies */
   if(!strncmp(argv[i_arg], "--index", 7))
   {
    index = 1;
    for (ii=0;ii<MAX_INPUT_FILES;ii++)
    {
        do_gs_indices[ii] = 0;
        do_ss_indices[ii] = 0;
    }
    ii = -1;
    while ((i_arg+1 < argc) && (*argv[i_arg+1] != '-'))
    {
      i_arg++;
      if (ii < 0)
        decode_vectors(SPOT_GS, do_gs_indices,
          argv[i_arg], strlen(argv[i_arg]));
      else
        decode_vectors(SPOT_GS, do_ss_indices,
          argv[i_arg], strlen(argv[i_arg]));
      ii++;
    }
   }
/* Do not use different symbols */
   if(strncmp(argv[i_arg], "-ns", 3) == 0)
   {
    symbols = 0;
   }
/* Radius of GS spots */
   if(strncmp(argv[i_arg], "-rg", 3) == 0)
   {
    i_arg++;
    spot_gs = (float)atof(argv[i_arg]);
    rg_flag = 1;
   }
/* Radius of SS spots */
   if(strncmp(argv[i_arg], "-rs", 3) == 0)
   {
    i_arg++;
    spot_ss = (float)atof(argv[i_arg]);
    rs_flag = 1;
   }
/* Custom Footer */
    if ((strncmp(argv[i_arg], "-fn", 3) == 0)
        || (strncmp(argv[i_arg], "--footer", 8) == 0)
        || (strncmp(argv[i_arg], "--header", 8) == 0))
    {
      if (strncmp(argv[i_arg], "--header", 8) == 0)
      {
        pos_footnote[0] = -MAX_RADIUS + translate[0];
        pos_footnote[1] = 400 + translate[1];
      }
      if (i_arg+1 < argc)
      {
        i_arg++;
        footnote = argv[i_arg];
        strcat(footnote, argv[i_arg]);
        strcat(footnote, "\0");
      }
      for (ii=0;ii<3;ii++)
      {
        if(i_arg+1 < argc)
        {
          if(isdigit(argv[i_arg+1][1]))
          {
            i_arg++;
            pos_footnote[ii] = atof(argv[i_arg]);
          }
        }
      }
    }
   //Help
   if((strncmp(argv[i_arg], "-h", 2) == 0) ||
      (strncmp(argv[i_arg], "--help", 6) == 0))
   {
     usage(stdout, argv[0]);
     exit(0);
   }
/* Print PATTERN information to stdout */
   if(strncmp(argv[i_arg], "--version", 9) == 0)
   {
     printf("%s\n\n%s\nPlease report bugs to:\n\t%s\n", VERSION, NOTES, BUG_CONTACT);
     exit(0);
   }
  } /* else */
 }  /* for i_arg */

//check footnote string
if (footnote == NULL || strlen(footnote) == 0)
{
  footnote = VERSION;
}

/* write initial lines to output */
draw_initial(out_stream, ifiles, title_str,	pos_title, footnote, pos_footnote,
  screen_flag, screen_thickness, fill_screen, ev_flag, eV, pos_ev,
  clip_flag, vectors_flag);


/* loop over input files */
for(ii=0;ii<ifiles;ii++)
{

  *in_stream = *in_streams[ii];

 fprintf(out_stream, "\n%%=======================================================================\n");
 fprintf(out_stream, "%% BEGIN SECTION %i:\n", ii+1);
 fprintf(out_stream, "%%=======================================================================\n\n");

/* read comment */
 if (stdin_flag)
   printf("Enter comment lines starting with 'c'\n");
 draw_title(in_stream, out_stream, ii, ifiles,
    colour(colour_flag, ii, ifiles, SPOT_GS, 0), &i_line_offset);
 if (stdin_flag)
   printf("(Further comment lines will be ignored)\n");

/*
 GS SPOTS
*/

/* read real space lattice vectors */
 if (stdin_flag)
 {
   printf("Enter vector a1 (x y): ");
   fget_nocomm(line_buffer, in_stream, out_stream);
 }
 sscanf(line_buffer,"%e %e", x_a1, x_a1+1);
 if (stdin_flag) printf("Enter vector a2 (x y): ");
 fget_nocomm(line_buffer, in_stream, out_stream);
 sscanf(line_buffer,"%e %e", x_a2, x_a2+1);

/*
 calculate max length in k-space (radius is the max. k-distance in
 units of the longest rec. lattice vector)
 rescale spot size
*/
 if (stdin_flag) printf("Enter radius: ");
 fget_nocomm(line_buffer, in_stream, out_stream);
 sscanf(line_buffer,"%e", &radius);

 if (ev_flag)
 {
   //calculate radius of Ewald construction at given energy
   long double lambda;
   lambda = sqrt(2*9.11*1.6018*eV)/6.62;
   lambda /= 2*M_PI;
   radius = (float) lambda;
 }

 //scale spot sizes by radius
 if (!rs_flag)
  spot_ss /= radius;
 if (!rg_flag)
  spot_gs /= radius;

 if( (x_a1[0]*x_a1[0] + x_a1[1]*x_a1[1]) > (x_a2[0]*x_a2[0] + x_a2[1]*x_a2[1]))
  radius *= sqrt(x_a1[0]*x_a1[0] + x_a1[1]*x_a1[1]);
 else
  radius *= sqrt(x_a2[0]*x_a2[0] + x_a2[1]*x_a2[1]);

 radius = MAX_RADIUS/ radius;

/* calculate GS vectors */
 a1[0] =  radius*x_a2[1];
 a1[1] = -radius*x_a2[0];
 a2[0] = -radius*x_a1[1];
 a2[1] =  radius*x_a1[0];


 printf("a1: %.1f %.1f\n", a1[0], a1[1]);
 printf("a2: %.1f %.1f\n", a2[0], a2[1]);

/* set draw routines for reciprocal lattice vectors */
 if ((vectors_flag) && (do_gs_vectors[ii]))
    draw_vectors(out_stream, -1, ii, colour(colour_flag, ii, ifiles, SPOT_GS, 0),
      a1, a2, spot_gs, dummystr, SPOT_GS, vectors_str);

 fprintf(out_stream, "%% radius: %.1f\n%%\n", radius);

/* print position of GS spots to output */
 fprintf(out_stream,"%s\n","% GS spots");
 fprintf(out_stream,"%s\n", colour(colour_flag, ii, ifiles, SPOT_GS, 0));
 fprintf(out_stream," %.1f setlinewidth stroke\n", LINE_WIDTH);


/* define max. values */
 imax_1 = 5* (int)( MAX_RADIUS / sqrt(a1[0]*a1[0] + a1[1]*a1[1]) );
 imax_2 = 5* (int)( MAX_RADIUS / sqrt(a2[0]*a2[0] + a2[1]*a2[1]) );


/* write GS spots to output */
if (overlap_flag) {
 for (i1 = -imax_1; i1 < imax_1; i1 ++)
 for (i2 = -imax_2; i2 < imax_2; i2 ++)
 {
  ix = i1 * a1[0] + i2 * a2[0];
  iy = i1 * a1[1] + i2 * a2[1];
  if( (((ix*ix + iy*iy) <= MAX_RADIUS*MAX_RADIUS) || (ev_flag)))
  {
   fprintf(out_stream,
    //" %.1f %.1f %.1f 0 360 arc gsave %.2f setgray fill grestore stroke\n",
    " %.1f %.1f %.1f 0 360 arc gsave %s %s grestore stroke\n",
      ix, iy, spot_gs, colour(colour_flag, ii, ifiles, SPOT_GS, i_dom), fill_gs);
   if ((index) && (do_gs_indices[ii]))
   {
     if (!((round(ix) == 0) && (round(iy) == 0) && (ii > 0)))
     {
       sprintf(text, "%d,%d",i1,i2);
       fprintf(out_stream,"/Times-Roman findfont %.1f scalefont setfont \n",
               spot_gs*2);
       fprintf(out_stream,"%.1f %.1f moveto (%s) show stroke\n",
                       ix - 1.5*spot_gs, iy + spot_gs + 4., text);
     }
   }
  }
 }
}


/*
 SS SPOT
*/

/* loop over domains */
 if (fileno(in_stream) == fileno(stdin)) printf("Enter no. domains: ");
 fget_nocomm(line_buffer, in_stream, out_stream);
 sscanf(line_buffer,"%d", &n_dom);
 printf("%d domains:\n", n_dom);

 fprintf(out_stream,"%s\n","% SS spots");
 fprintf(out_stream," %.1f setlinewidth stroke\n", LINE_WIDTH);

 for(i_dom = 0; i_dom < n_dom; i_dom ++)
 {
/* matrix */
  if (fileno(in_stream) == fileno(stdin))
    printf("Enter domain %i parameters\n(may be rotation 'R<num>', 'S', or a 2x2 Matrix):\n",
      i_dom);
  fget_nocomm(line_buffer, in_stream, out_stream);
  fprintf(out_stream,"%s\n", colour(colour_flag, ii, ifiles, SPOT_SS, i_dom));
  switch (*line_buffer)
  {
   case 'R':
   {
    sscanf(line_buffer+1,"%e", &phi);
    phi *= 0.0174532925;
    det  = x_a1[0]*x_a2[1] - x_a1[1]*x_a2[0];
    aux2 = (x_a1[0]*x_a2[0] + x_a1[1]*x_a2[1]) / det;

    N11 = cos(phi) - aux2*sin(phi);
    N12 = sin(phi) * (x_a1[0]*x_a1[0] + x_a1[1]*x_a1[1]) / det;
    N21 = - sin(phi) * (x_a2[0]*x_a2[0] + x_a2[1]*x_a2[1]) / det;
    N22 = cos(phi) + aux2*sin(phi);

    m11 = M11*N11 + M12*N21;
    m12 = M11*N12 + M12*N22;
    m21 = M21*N11 + M22*N21;
    m22 = M21*N12 + M22*N22;

    M11 = m11; M12 = m12; M21 = m21; M22 = m22;

    break;
   }

   case 'S':
   {
     det  = x_a1[0]*x_a2[1] - x_a1[1]*x_a2[0];
     aux2 = (x_a1[0]*x_a2[1] + x_a1[1]*x_a2[0]) / det;

    if (line_buffer[1] == 'x')
    {
     N11 = aux2;
     N12 = -2 * x_a1[0]*x_a1[1] / det;
     N21 =  2 * x_a2[0]*x_a2[1] / det;
     N22 = -aux2;
    }
    if (line_buffer[1] == 'y')
    {
     N11 = - aux2;
     N12 =  2 * x_a1[0]*x_a1[1] / det;
     N21 = -2 * x_a2[0]*x_a2[1] / det;
     N22 = aux2;
    }

    m11 = M11*N11 + M12*N21;
    m12 = M11*N12 + M12*N22;
    m21 = M21*N11 + M22*N21;
    m22 = M21*N12 + M22*N22;

    M11 = m11; M12 = m12; M21 = m21; M22 = m22;

    break;
   }

   default:
   {
    sscanf(line_buffer,"%e %e", &M11, &M12);
    fget_nocomm(line_buffer, in_stream, out_stream);
    sscanf(line_buffer,"%e %e", &M21, &M22);
    m11 = M11; m12 = M12; m21 = M21; m22 = M22;
    N11 = N22 = 1.; N12 = N21 = 0.;
    break;
   }
  }

/* commensurate or not? */
  comm = !( ( fabs(m11) - (int)(fabs(m11)+0.1) > 0.05) ||
            ( fabs(m12) - (int)(fabs(m12)+0.1) > 0.05) ||
            ( fabs(m21) - (int)(fabs(m21)+0.1) > 0.05) ||
            ( fabs(m22) - (int)(fabs(m22)+0.1) > 0.05) );

/* det of matrix */
  det  = m11*m22 - m12*m21;
  aux1 = 1/det;
  det = fabs(det);

  b1[0] = aux1 *(m22*a1[0] - m21*a2[0]);
  b1[1] = aux1 *(m22*a1[1] - m21*a2[1]);
  b2[0] = aux1 *(m11*a2[0] - m12*a1[0]);
  b2[1] = aux1 *(m11*a2[1] - m12*a1[1]);

/* Print comments to outputs */
  fprintf(out_stream,"%s Domain No. %d\n","%", i_dom+1);
  printf("%d:\tDet: %.1f\n",i_dom+1, det);
  printf("N = (%5.1f %5.1f)\n", N11, N12);
  printf("    (%5.1f %5.1f)\n", N21, N22);
  printf("M = (%5.1f %5.1f)\t(M^-1)t = (%5.2f %5.2f)",
        m11,  m12,              aux1*m22,-aux1*m21);
  printf("\tb1* = (%6.2f)\tb2* = (%6.2f)\n", b1[0]/radius, b2[0]/radius);
  printf("    (%5.1f %5.1f)\t          (%5.2f %5.2f)",
        m21,  m22,             -aux1*m12, aux1*m11);
  printf("\t      (%6.2f)\t      (%6.2f)\n", b1[1]/radius, b2[1]/radius);

/* SS spots */
/* define max. values */
  smax_1 = 5* (int)( MAX_RADIUS / sqrt(b1[0]*b1[0] + b1[1]*b1[1]) );
  smax_2 = 5* (int)( MAX_RADIUS / sqrt(b2[0]*b2[0] + b2[1]*b2[1]) );

/* write SS spot positions  to output */
  for (s1 = -smax_1; s1 < smax_1; s1 ++)
  for (s2 = -smax_2; s2 < smax_2; s2 ++)
  {
   x = s1 * b1[0] + s2 * b2[0];
   y = s1 * b1[1] + s2 * b2[1];
   if( ((x*x + y*y) <= MAX_RADIUS*MAX_RADIUS) || (ev_flag))
   {
    if (symbols)
     draw_spot(out_stream, x, y, spot_ss, i_dom,
        colour(colour_flag, ii, ifiles, SPOT_SS, i_dom), fill_ss);
    else
     draw_spot(out_stream, x, y, spot_ss, 0,
        colour(colour_flag, ii, ifiles, SPOT_SS, i_dom), fill_ss);

    ind_1 = (s1*m22 - s2*m12);
    ind_2 = (s2*m11 - s1*m21);
/*
write indices
*/
/* print domain lattice vectors */

 if ((vectors_flag) && (do_ss_vectors[ii][i_dom]))
    draw_vectors(out_stream, i_dom, ii, colour(colour_flag, ii, ifiles, SPOT_SS, i_dom),
      b1, b2, spot_ss, dummystr, SPOT_SS, vectors_str);


/* check if integral order spot */
    if ((index) && (do_ss_indices[ii]) &&
       ((fabs(ind_1*aux1) - (int)(fabs(ind_1*aux1)+0.1) > 0.05) ||
    (fabs(ind_2*aux1) - (int)(fabs(ind_2*aux1)+0.1) > 0.05) ))
    {
    fprintf(out_stream,"/Times-Roman findfont %.1f scalefont setfont \n",
       spot_ss*2);

     if (!comm)
     {
/* incommensurate */
      sprintf(text, "%.2f,%.2f", ind_1*aux1, ind_2*aux1);
      fprintf(out_stream,"%.1f %.1f moveto (%s) show stroke\n",
                          x - 4*spot_ss, y + spot_ss + 4., text);
     }
     else
     {
/* commensurate (fractions) */

      ind_1_int = (int) (ind_1*1.01);
      ind_2_int = (int) (ind_2*1.01);
      det_int =   (int) (det*1.01);

      switch (nice_frac(&ind_1_int,&det_int))
      {
       case(0):
       {
        sprintf(text, "0, ");
        break;
       }
       case(1):
       {sprintf(text, "%d, ", ind_1_int);
        break;
       }
       case(2):
       {sprintf(text, "%d/%d, ", ind_1_int, det_int);
        break;
       }
      }

      det_int =   (int) (det*1.01);
      switch (nice_frac(&ind_2_int,&det_int))
      {
       case(0):
       {
        sprintf(text2, "0");
        break;
       }
       case(1):
       {sprintf(text2, "%d", ind_2_int);
        break;
       }
       case(2):
       {sprintf(text2, "%d/%d", ind_2_int, det_int);
        break;
       }
      }

/*
      if (fabs(ind_1) < 0.01*aux1)
       sprintf(text, "0, %d/%d", (int)(ind_2*1.01), (int)(det*1.01));
      else if (fabs(ind_2) < 0.01*aux1)
       sprintf(text, "%d/%d, 0", (int)(ind_1*1.01), (int)(det*1.01));
      else
       sprintf(text, "%d/%d,%d/%d", (int)(ind_1*1.01), (int)(det*1.01),
                                   (int)(ind_2*1.01), (int)(det*1.01));
*/
      fprintf(out_stream,"%.1f %.1f moveto (%s%s) show stroke\n",
                          x - 3*spot_ss, y + spot_ss + 2., text, text2);
     }
    }
   }
  }
/*
 If the Mii are not integer, the superstructure is incommensurate.
 In this case the multple scattering spots must be calculated
 separately
*/
/*
  if( ( fabs(m11) - (int)(fabs(m11)+0.1) > 0.05) ||
      ( fabs(m12) - (int)(fabs(m12)+0.1) > 0.05) ||
      ( fabs(m21) - (int)(fabs(m21)+0.1) > 0.05) ||
      ( fabs(m22) - (int)(fabs(m22)+0.1) > 0.05) )
*/
  if (!comm)
  {
   printf("incommensurate\n");
/*
 write multiple scattering SS spots to output
*/
   fprintf(out_stream,"%s Mult scatt. \n","%");
   for (i1 = -imax_1; i1 < imax_1; i1 ++)
   for (i2 = -imax_2; i2 < imax_2; i2 ++)
   {
    if( (i1 != 0) || (i2 != 0) )
    {
     ix = i1 * a1[0] + i2 * a2[0];
     iy = i1 * a1[1] + i2 * a2[1];
     if( (ix*ix + iy*iy) <= MAX_RADIUS*MAX_RADIUS)
     {
      for (s1 = -smax_1; s1 < smax_1; s1 ++)
      for (s2 = -smax_2; s2 < smax_2; s2 ++)
      {
       x = ix + s1 * b1[0] + s2 * b2[0];
       y = iy + s1 * b1[1] + s2 * b2[1];
       if( ((x*x + y*y) <= MAX_RADIUS*MAX_RADIUS) &&
       ((s1 != 0) || (s2 != 0)) )
        fprintf(out_stream,
         //" %.1f %.1f %.1f 0 360 arc gsave %.2f setgray fill grestore stroke\n",
         " %.1f %.1f %.1f 0 360 arc gsave %s %s grestore stroke\n",
         x, y, spot_ss, colour(colour_flag, ii, ifiles, SPOT_SS, i_dom), fill_ss);
      } /* for s1/2 */
     }  /* if < MAX_RADIUS */
    }   /* if != 0  */
   }    /* for i1/2 */
  }     /* if fabs .. */
  else printf("commensurate\n");
 } /* for i_dom */


/* GS added after SS complete so as not to overlap (if desired) */
if (overlap_flag != 1) {
 /* write GS spots to output */
 for (i1 = -imax_1; i1 < imax_1; i1 ++)
 for (i2 = -imax_2; i2 < imax_2; i2 ++)
 {
  ix = i1 * a1[0] + i2 * a2[0];
  iy = i1 * a1[1] + i2 * a2[1];
  if( (ix*ix + iy*iy) <= MAX_RADIUS*MAX_RADIUS || (ev_flag == 1) )
  {
   fprintf(out_stream,"%s\n", colour(colour_flag, ii, ifiles, SPOT_GS, 0));
   fprintf(out_stream,
    " %.1f %.1f %.1f 0 360 arc gsave %s %s grestore stroke\n",
      ix, iy, spot_gs, colour(colour_flag, ii, ifiles, SPOT_GS, 0), fill_gs);
   if ((index) && (do_gs_indices[ii]))
   {
     if (!((round(ix) == 0) && (round(iy) == 0) && (ii > 0)))
     {
       sprintf(text, "%d,%d",i1,i2);
       fprintf(out_stream,"/Times-Roman findfont %.1f scalefont setfont \n",
               spot_gs*2);
       fprintf(out_stream,"%.1f %.1f moveto (%s) show stroke\n",
                       ix - 1.5*spot_gs, iy + spot_gs + 4., text);
     }
   }
  }
 }
}

 fclose(in_stream);
}

/* final drawing strokes */
  draw_final(out_stream, gun_flag, gun_thickness, gun_radians,
    screen_thickness, screen_flag, vectors_flag, vectors_str,
    ifiles, ev_flag, eV, title_str);
  fclose(out_stream);

  if (png_flag)
  {
    #ifdef USE_MAGICK
    magick_ps2png("tmp.ps");
    #endif
  }
  return 0;
} //end of main

/**********************************************************************************/
/* final drawing strokes */
int draw_final(FILE *out_stream, int gun_flag, float gun_thickness,
    float gun_radians, float screen_thickness, int screen_flag,
    int vectors_flag, char *vectors_str, int ifiles, int ev_flag,
    float eV, char *title_str)
{
  int ii;

  fprintf(out_stream, "\n%%=======================================================================\n\n");
  if (gun_flag)
   draw_gun(out_stream, gun_thickness*0.5, gun_thickness*3,
            gun_radians, screen_thickness, screen_flag);
  if (vectors_flag)
    fprintf(out_stream, "%s\n", vectors_str);
  if (screen_flag)
    draw_screen(out_stream, screen_thickness, 0);

  fprintf(out_stream,"grestore\n");
  fprintf(out_stream,"%%Print Titles\n");
  for (ii=0;ii<ifiles;ii++)
  {
    fprintf(out_stream,"Title%i ",ii);
  }
  fprintf(out_stream,"\n");
  if (ev_flag)
    fprintf(out_stream, "add_eV\n");
  if (*title_str != '\0')
    fprintf(out_stream, "title\n");
  fprintf(out_stream,"footnote\n");
  fprintf(out_stream,"\nshowpage\n");
  fprintf(out_stream,"%%EOF");
  return 0;
}

/*========================================================================*/

/* write initial lines to output */
int draw_initial(FILE *out_stream, int ifiles, char *title_str,
    float *pos_title, char *footnote, float *pos_footnote, int screen_flag,
    float screen_thickness, int fill_screen, int ev_flag,
    float eV, float *pos_ev, int clip_flag, int vectors_flag)
{
 fprintf(out_stream,"%s \n","%!PS-Adobe-3.0");
 fprintf(out_stream,"%s %s\n","%%Creator:",VERSION);
 fprintf(out_stream,"%s \n","%%BoundingBox: 50 150 550 650");
 fprintf(out_stream,"%s \n","%%EndComments");
 fprintf(out_stream,"%%=======================================================================\n");
 fprintf(out_stream,"%%Initial setup:\n");
 fprintf(out_stream,"%%=======================================================================\n");

 if (ifiles-1)
   fprintf(out_stream,"%%%%Page: %s %i\n", "Composite_Input", 1);
 else
   fprintf(out_stream,"%%%%Page: %s %i\n", "Single_Input", 1);
 fprintf(out_stream," %.1f %.1f translate gsave %%center page as origin\n",
    OFF_W, OFF_H);
 if (*title_str != '\0')
 {
    fprintf(out_stream,
        "/title {%%add custom title\n/Times-Bold findfont %.1f scalefont setfont\n",
        pos_title[2]);
    fprintf(out_stream,
        "%s %.1f %.1f moveto (%s) show\n} def\n",
        PSBLACK, pos_title[0], pos_title[1], title_str);
 }
 fprintf(out_stream,"/footnote {%%add page footnote\n");
 fprintf(out_stream,"/Times-Roman findfont %.1f scalefont setfont\n",
    pos_footnote[2]);
 fprintf(out_stream,"%s %.1f %.1f moveto (%s) show\n} def\n",
   PSBLACK, pos_footnote[0], pos_footnote[1], footnote );
 if (screen_flag)
   draw_screen(out_stream, screen_thickness, fill_screen);
 if (ev_flag)
 {
 fprintf(out_stream,"/add_eV {%%add electron energy label\n");
 fprintf(out_stream,"/Times-ItalicBold findfont 20 scalefont setfont\n");
 fprintf(out_stream,"%s %.1f %.1f moveto (%.0feV) show\n} def\n",
    PSBLACK, pos_ev[0], pos_ev[1], eV );
 }
 if (clip_flag)
   fprintf(out_stream,"%s %.1f 0 moveto 0 0 %.1f 0 360 arc clip\n",
     PSWHITE, MAX_RADIUS+(screen_thickness-1), MAX_RADIUS+(screen_thickness-1));
   fprintf(out_stream,
     "0. setlinewidth 1 1 1 setrgbcolor 204.0 0 moveto 0 0 %.1f 0 360 arc stroke\n",
     MAX_RADIUS+(screen_thickness-1));
 if(vectors_flag)
     add_PSvectors(out_stream);
  return 0;
}

/*========================================================================*/

/* write title routine to output */
int draw_title(FILE *in_stream, FILE *out_stream, int ii, int ifiles,
        char *colour, int *i_line_offset)
{
 int step;
 int lineno;

 lineno = 0;

 fget_nocomm(line_buffer, in_stream, out_stream);

 if (ifiles > 1)
   step = 25;
 else
   step = -25;

 /* read all c-comment lines and output accordingly */
 while (line_buffer[0] == 'c')
 {
  fprintf(out_stream,"/Title%i {\n", ii);
  fprintf(out_stream,"/Times-Roman findfont 20 scalefont setfont\n");
  fprintf(out_stream,"%s",colour);
  fprintf(out_stream," %.1f %.1f moveto\n",
      -MAX_RADIUS-50, MAX_RADIUS + 5 + *i_line_offset);
  *i_line_offset += step;
  if (!print_ps_substitute(out_stream, line_buffer+1))
  {
    fprintf(out_stream,"(%s) show\n", line_buffer+1 );
  }
  fget_nocomm(line_buffer, in_stream, out_stream);
  fprintf(out_stream,"} def\n");
  lineno++;
 }
 if (!lineno) fprintf(out_stream,"/Title%i {} def %%dummy\n", ii);

 return 0;

}

/*========================================================================*/

/* convert PostScript to PNG using ImageMagick */
int magick_ps2png(char *filename_in, char *filename_out)
{
#ifdef USE_MAGICK
MagickWand *mw = NULL;
MagickWandGenesis();
mw = NewMagickWand();
MagickReadImage(mw,filename_in);
MagickWriteImage(mw,filename_out);
DestroyMagickWand(mw);
MagickWandTerminus();
#endif
return 0;
}

/*========================================================================*/

/* draw screen */
int draw_screen(FILE *out_stream, float edge_thickness, int fill)
{

    fprintf(out_stream,"%% Screen\n");
    fprintf(out_stream,"0 0 moveto %.1f setlinewidth stroke\n", edge_thickness);
    fprintf(out_stream,"0 0 %.1f 0 360 arc ", MAX_RADIUS+(edge_thickness*0.5));
    fprintf(out_stream,"%s stroke\n", EDGE_COLOUR );
    if(fill)
      fprintf(out_stream,"0 0 %.1f 0 360 arc %s fill stroke\n",
        MAX_RADIUS+(edge_thickness*0.5), SCREEN_COLOUR);
    return 0;
}

/*========================================================================*/

/* draw electron gun */
int draw_gun(FILE *out_stream, float gun_thickness, float gun_radius,
            float position_radians, float edge_thickness, int screen_flag)
{

    fprintf(out_stream,"%% Electron gun\n");
    fprintf(out_stream,"%s\n", GUN_COLOUR);
    fprintf(out_stream,"newpath\n%.1f %.1f moveto %.1f setlinewidth\n",
      gun_thickness, -gun_thickness, gun_thickness);
    fprintf(out_stream,"%.1f %.1f lineto %.1f %.1f lineto\n%.1f %.1f lineto\n",
      -(1.005*MAX_RADIUS*cos(3.14182*(position_radians+0.035))),
      -(1.005*MAX_RADIUS*sin(3.14182*(position_radians+0.035))),
      -(1.005*MAX_RADIUS*cos(3.14182*(position_radians-0.035))),
      -(1.005*MAX_RADIUS*sin(3.14182*(position_radians-0.035))),
      -gun_thickness, gun_thickness);
    fprintf(out_stream,"closepath\n%s fill stroke\n", GUN_COLOUR);
    fprintf(out_stream,"0 0 %.1f 0 360 arc 0 0 0 setrgbcolor fill stroke\n",
      gun_radius);

    return 0;
}

/*========================================================================*/

int draw_spot(FILE *out_stream, float x, float y,
        float spot_size, int shape, char *colour, char *fill)
/*
 draw spots in different shapes:
  0: circle
  1: square
  2: triangle
*/
{
int delta;

 switch(shape)
 {
  case(0):
  {
     fprintf(out_stream,
        " %.1f %.1f %.1f 0 360 arc",
        x, y, spot_size);
     if (strncmp(fill, "fill", 4) == 0)
       fprintf(out_stream," %s %s", colour, fill);
     fprintf(out_stream," stroke\n");
   break;
  }
  case(1):
  {
/* square */
   spot_size /= 1.4;
   fprintf(out_stream, "newpath\n");
   fprintf(out_stream,"%.1f %.1f moveto %.1f %.1f lineto \n",
   x-spot_size, y-spot_size, x-spot_size, y+spot_size);
   fprintf(out_stream, "%.1f %.1f lineto %.1f %.1f lineto\n",
   x+spot_size, y+spot_size, x+spot_size, y-spot_size);
   fprintf(out_stream, "closepath");
   if (strncmp(fill, "fill", 4) == 0) {
     fprintf(out_stream, " %s %s", colour, fill);
   }
   fprintf(out_stream, " stroke\n");
   break;
  }
  case(2):
  {
/* up triangle */
    delta = spot_size * 0.866;
    fprintf(out_stream, "newpath\n");
    fprintf(out_stream,"%.1f %.1f moveto %.1f %.1f lineto \n",
        x, y + spot_size, x - delta, y - (0.5* spot_size));
    fprintf(out_stream, "%.1f %.1f lineto \n closepath",
        x + delta, y - (0.5* spot_size));
    if (strncmp(fill, "fill", 4) == 0 )
        fprintf(out_stream, " %s %s", colour, fill);
    fprintf(out_stream, " stroke\n");
    break;
  }
  case(3):
  {
/* down triangle */
    delta = spot_size * 0.866;
    fprintf(out_stream, "newpath\n");
    fprintf(out_stream,"%.1f %.1f moveto %.1f %.1f lineto \n",
        x, y - spot_size, x - delta, y + (0.5* spot_size));
    fprintf(out_stream, "%.1f %.1f lineto \n closepath",
        x + delta, y + (0.5 * spot_size));
    if (strncmp(fill, "fill", 4) == 0 )
        fprintf(out_stream, " %s %s", colour, fill);
    fprintf(out_stream, " stroke\n");
   break;
  }
  case(4):
  {
/* diamond */
    fprintf(out_stream, "newpath\n");
    fprintf(out_stream,"%.1f %.1f moveto %.1f %.1f lineto \n",
        x-spot_size, y, x, y+spot_size);
    fprintf(out_stream, "%.1f %.1f lineto %.1f %.1f lineto\n",
        x+spot_size, y, x, y-spot_size);
    fprintf(out_stream, " closepath");
    if (strncmp(fill, "fill", 4) == 0 )
        fprintf(out_stream, " %s %s", colour, fill);
    fprintf(out_stream, " stroke\n");
    break;
  }
  case(5):
  {
/* hexagon */
    delta = spot_size * 0.866;
    fprintf(out_stream, "newpath\n");
    fprintf(out_stream,"%.1f %.1f moveto %.1f %.1f lineto \n",
        x, y - spot_size, x - delta, y - (0.5*spot_size));
    fprintf(out_stream, "%.1f %.1f lineto %.1f %.1f lineto\n",
        x - delta, y + (0.5*spot_size), x, y + spot_size);
    fprintf(out_stream, "%.1f %.1f lineto %.1f %.1f lineto \n closepath",
        x + delta, y + (0.5*spot_size), x + delta, y - (0.5*spot_size));
    if (strncmp(fill, "fill", 4) == 0 )
        fprintf(out_stream, " %s %s", colour, fill);
    fprintf(out_stream, " stroke\n");
   break;
  }
 }  /* switch */
 return 0;
}

/*========================================================================*/

/* draw reciprocal space vectors to PS file */
int draw_vectors(FILE *out_stream, int i_dom, int ii, char *colour, float *a1, float *a2,
        float spot_radius, char dummystr[][STRSZ], float spot, char *vectors_str)
{
  int compare[2];
  char dom_str[STRSZ];
  char v_type;

  strcpy(dom_str, "\0");
  compare[0] = 0;
  compare[1] = 1;
  v_type = 'a';

  if (spot == SPOT_SS)
  {
    sprintf(dom_str, "_%idom", i_dom);
    compare[0] += 3;
    compare[1] += 3;
    v_type = 'b';
  }

  //do comparisons with previous & write new vectors if neccessary
  sprintf(dummystr[compare[1]],
    "%.1f %.1f %.1f %.1f", a1[0], a1[1], a2[0], a2[1]);
  if (strcmp(dummystr[compare[0]], dummystr[compare[1]]))
  {

    fprintf(out_stream,"%%\n%% Lattice vectors\n");

    fprintf(out_stream, "/%c1_%i%s {\ngsave %s\n", v_type, ii, dom_str, colour);
    fprintf(out_stream,
      "0 0 %.1f %.1f %.1f arrow\n", a1[0], a1[1], spot_radius*0.5);

    if (v_type == 'a')
      sprintf(dummystr[2], "%i", 1);
    else
      sprintf(dummystr[2], "%i,%i", 1, i_dom);

    fprintf(out_stream,
      "0 0 %.1f %.1f (%c) (%s) %.1f %.1f rotatedlabel\n",
      a1[0], a1[1], v_type, dummystr[2], spot_radius*2.5, spot_radius*1.25);
    fprintf(out_stream, "grestore\n} def\n");

    fprintf(out_stream, "/%c2_%i%s {\ngsave %s\n", v_type, ii, dom_str, colour);
    fprintf(out_stream,
      "0 0 %.1f %.1f %.1f arrow\n", a2[0], a2[1], spot_radius*0.5);

    if (v_type == 'a')
      sprintf(dummystr[2], "%i", 2);
    else
      sprintf(dummystr[2], "%i,%i", 2, i_dom);

    fprintf(out_stream,
      "0 0 %.1f %.1f (%c) (%s) %.1f %.1f rotatedlabel\n",
      a2[0], a2[1], v_type, dummystr[2], spot_radius*2.5, spot_radius*1.25);
    fprintf(out_stream, "grestore\n} def\n");

    sprintf(dummystr[2], "\n%c1_%i%s %c2_%i%s",
      v_type, ii, dom_str, v_type, ii, dom_str);

    strcat(vectors_str, dummystr[2]);

  }

  sprintf(dummystr[compare[0]], "%.1f %.1f %.1f %.1f", a1[0], a1[1], a2[0], a2[1]);

  return 0;
}

/*========================================================================*/

int nice_frac(int *zahler, int *nenner)

/*
 make nicer fractions
 return values:
 0: zero
 1: interger number;
 2: fraction;
*/

{
int i, ggt, number;

 if (*zahler == 0)
 return (0);

 if (*nenner < 0) number = - *nenner/2;
 else             number =   *nenner/2;

 for (ggt = 1, i = 2; i <= number; i++)
 {
  if( (*zahler % i == 0 ) && (*nenner % i == 0) ) ggt = i;
 }
 *zahler /= ggt;
 *nenner /= ggt;

 if (*zahler % *nenner == 0)
 {
  *zahler /= *nenner;
  return(1);
 }
 else
 return(2);
}

/*========================================================================*/

/* colour selection */
char *colour(int colour_flag, int ifile, int nfiles, int spot_type, int i_dom) {
 if (!spot_type) //GS spots = 0
 {
  if (nfiles > 0) {
    switch (colour_flag)
    {
      case -1: //greyscale
        return greyscale[ifile % (NUM_GREYS-1)];
        break;

      case 0: //no colour
        return PSBLACK;
        break;

      case 1:
        return file_colours[ifile % 5][spot_type];
        break;

      default:
        return PSBLACK;
        break;
    }
  } else {
    return PSBLACK; //black if 1st set of GS spots
  }

 } else //SS spots
 {
  if(nfiles == 0)
  {
    switch (colour_flag)
    {
      case -1: //greyscale
        return greyscale[i_dom % (NUM_GREYS-1)];
        break;

      case 0: //no colour
        return PSBLACK;
        break;

      case 1:
        return colours[i_dom % (NUM_COLOURS-1)];
        break;

      default:
        return PSBLACK;
        break;
    }
  } else {
    switch (colour_flag)
    {
      case -1: //greyscale
        return greyscale[ifile % (NUM_GREYS-1)];
        break;

      case 0: //no colour
        return PSBLACK;
        break;

      case 1:
        return file_colours[ifile % 5][spot_type];
        break;

      default:
        return PSBLACK;
        break;
    }
  }
 }
 return PSBLACK; //default if no colour selected
}

/*========================================================================*/

/* add lattice vector postscript subroutines */
void add_PSvectors(FILE *out_stream)
{
  /* print section banner */
  fprintf(out_stream,"%%=======================================================================\n");
  fprintf(out_stream,"%%Lattice vector subroutines:\n");
  fprintf(out_stream,"%%=======================================================================\n");

  /* add text angle PS subroutine */
  fprintf(out_stream,"/textangle { %%returns correct angle for text\n");
  fprintf(out_stream,"dup 90 gt { %%>90\n");
  fprintf(out_stream,"	dup 180 gt \n");
  fprintf(out_stream,"	{ %%>180\n");
  fprintf(out_stream,"		dup 270 gt \n");
  fprintf(out_stream,"			{dup pop dup 360 sub} %%>270\n");
  fprintf(out_stream,"			{dup pop dup 180 sub} %%> 180\n");
  fprintf(out_stream,"			ifelse\n");
  fprintf(out_stream,"	}\n");
  fprintf(out_stream,"	{dup pop dup 180 sub} %%90 < x < 180\n");
  fprintf(out_stream,"	ifelse\n");
  fprintf(out_stream,"}\n");
  fprintf(out_stream,"{dup pop dup}\n");
  fprintf(out_stream,"ifelse\n");
  fprintf(out_stream,"} def\n");
  fprintf(out_stream,"%%\n");

  /* add rotated text PS subroutine */
  fprintf(out_stream,"/positiontext { %%calculate text offset\n");
  fprintf(out_stream,"2 dict begin\n");
  fprintf(out_stream,"/sf exch def\n");
  fprintf(out_stream,"/y1 exch def\n");
  fprintf(out_stream,"/x1 exch def \n");
  fprintf(out_stream,"y1 0 gt \n");
  fprintf(out_stream,"{ %%y +ve\n");
  fprintf(out_stream,"	x1 0 gt %%x +ve?  \n");
  fprintf(out_stream,"	{x1 sf sub y1 0.95 mul sf add moveto} %%x +ve\n");
  fprintf(out_stream,"	{\n");
  fprintf(out_stream,"	  x1 0 lt %%x -ve?  \n");
  fprintf(out_stream,"	  {x1 1.25 mul y1 1.1 mul 5 add moveto} %%x -ve\n");
  fprintf(out_stream,"	  {x1 5 sf mul sub y1 0.95 mul 5 sf mul add moveto} %%special case x=0\n");
  fprintf(out_stream,"	  ifelse\n");
  fprintf(out_stream,"	}\n");
  fprintf(out_stream,"	ifelse\n");
  fprintf(out_stream,"}\n");
  fprintf(out_stream,"{\n");
  fprintf(out_stream,"	y1 0 lt\n");
  fprintf(out_stream,"	{ %%y -ve\n");
  fprintf(out_stream,"	  x1 0 gt %%x +ve?  \n");
  fprintf(out_stream,"	  {x1 sf 0.5 mul add y1 0.65 mul sf 2 mul add moveto} %%x +ve\n");
  fprintf(out_stream,"	  {\n");
  fprintf(out_stream,"	    x1 0 lt %%x -ve?  \n");
  fprintf(out_stream,"	    {x1 1.30 mul y1 0.8 mul moveto} %%x -ve\n");
  fprintf(out_stream,"	    {x1 5 sf mul sub y1 0.95 mul 5 add moveto} %%special case x=0\n");
  fprintf(out_stream,"	    ifelse\n");
  fprintf(out_stream,"	  }\n");
  fprintf(out_stream,"	  ifelse\n");
  fprintf(out_stream,"	}\n");
  fprintf(out_stream,"    { %%********* special case: y=0 ***********%%\n");
  fprintf(out_stream,"	  x1 0 gt %%x +ve?  \n");
  fprintf(out_stream,"	  {x1 sf mul 6 sub y1 0.95 mul 5 sf mul add moveto} %%x +ve\n");
  fprintf(out_stream,"	  {\n");
  fprintf(out_stream,"	    x1 0 lt %%x -ve?  \n");
  fprintf(out_stream,"	    {x1 sf 5 mul sub y1 0.95 mul 5 sf mul add moveto} %%x -ve\n");
  fprintf(out_stream,"	    if\n");
  fprintf(out_stream,"	  }\n");
  fprintf(out_stream,"	  ifelse\n");
  fprintf(out_stream,"	}\n");
  fprintf(out_stream,"    ifelse\n");
  fprintf(out_stream,"}\n");
  fprintf(out_stream,"ifelse\n");
  fprintf(out_stream,"} def\n");
  fprintf(out_stream,"\n");
  fprintf(out_stream,"%%\n");

  /* add rotate label subroutine */
  fprintf(out_stream,"/rotatedlabel { %%adds rotated label with supscript text\n");
  fprintf(out_stream,"8 dict begin\n");
  fprintf(out_stream,"/subfontsize exch def\n");
  fprintf(out_stream,"/fontsize exch def\n");
  fprintf(out_stream,"/subtext exch def\n");
  fprintf(out_stream,"/text exch def\n");
  fprintf(out_stream,"/y1 exch def\n");
  fprintf(out_stream,"/x1 exch def\n");
  fprintf(out_stream,"/y0 exch def\n");
  fprintf(out_stream,"/x0 exch def\n");
  fprintf(out_stream,"gsave %%save coordinate system - important! \n");
  fprintf(out_stream,"x1 x0 sub 2 div 0.8 mul y1 y0 sub 2 div moveto %%move to label pos\n");
  fprintf(out_stream,"currentpoint fontsize 10 div positiontext\n");
  fprintf(out_stream,"y1 y0 sub x1 x0 sub atan textangle rotate %%rotate for text\n");
  fprintf(out_stream,"/Times-Bold findfont fontsize scalefont setfont text show\n");
  fprintf(out_stream,"currentpoint translate gsave %%translate for easy subscript\n");
  fprintf(out_stream,"y1 y0 sub x1 x0 sub atan textangle -1 mul rotate %%rotate back \n");
  fprintf(out_stream,"0 subfontsize 8 div 0 subfontsize 5 div sub moveto %%offset y for subscript\n");
  fprintf(out_stream,"y1 y0 sub x1 x0 sub atan textangle rotate %%rotate for subscript text\n");
  fprintf(out_stream,"/Times-Bold findfont subfontsize scalefont setfont subtext show\n");
  fprintf(out_stream,"y1 y0 sub x1 x0 sub atan textangle -1 mul rotate %%rotate back \n");
  fprintf(out_stream,"grestore 0 fontsize subfontsize div 2 add moveto %%offset y for superscript\n");
  fprintf(out_stream,"/Times-Bold findfont fontsize 0.8 mul scalefont setfont (*) show\n");
  fprintf(out_stream,"grestore %%restore original coordinate system\n");
  fprintf(out_stream,"} def\n");
  fprintf(out_stream,"%%\n");

  /* add arrowhead PS subroutine */
  fprintf(out_stream,"/arrowhead {%% stack: s x1 y1, current point: x0 y0\n");
  fprintf(out_stream,"gsave\n");
  fprintf(out_stream,"currentpoint %% s x1 y1 x0 y0\n");
  fprintf(out_stream,"4 2 roll exch %% s x0 y0 y1 x1\n");
  fprintf(out_stream,"4 -1 roll exch %% s y0 y1 x0 x1\n");
  fprintf(out_stream,"sub 3 1 roll %% s x1-x2 y0 y1\n");
  fprintf(out_stream,"sub exch %% s y0-y1 x1-x2\n");
  fprintf(out_stream,"atan rotate %% rotate over arctan((y0-y1)/(x1-x2))\n");
  fprintf(out_stream,"dup scale %% scale by factor s\n");
  fprintf(out_stream,"-7 2 rlineto 1 -2 rlineto -1 -2 rlineto\n");
  fprintf(out_stream,"closepath fill %% fill arrowhead\n");
  fprintf(out_stream,"grestore\n");
  fprintf(out_stream,"newpath\n");
  fprintf(out_stream,"} def\n");
  fprintf(out_stream,"%%\n");

  /* add arrow PS subroutine */
  fprintf(out_stream,"/arrow {\n");
  fprintf(out_stream,"5 dict begin\n");
  fprintf(out_stream,"/arrowsize exch def\n");
  fprintf(out_stream,"/y1 exch def\n");
  fprintf(out_stream,"/x1 exch def\n");
  fprintf(out_stream,"/y0 exch def\n");
  fprintf(out_stream,"/x0 exch def\n");
  fprintf(out_stream,"gsave \n");
  fprintf(out_stream,"arrowsize setlinewidth\n");
  fprintf(out_stream,"x0 y0 moveto x1 0.95 mul y1 0.95 mul lineto stroke\n");
  fprintf(out_stream,"x1 y1 moveto arrowsize x0 y0 arrowhead\n");
  fprintf(out_stream,"grestore\n");
  fprintf(out_stream,"} def\n%%\n");

}

/* decode input string to determine which vectors to draw

   Note: more than 10 input files is not currently supported as the conversion
   is done using single characters (i.e. "0"-"9")
*/
int decode_vectors(float spot_type, int *do_vectors, char *argv, int str_length)
{
  int pos, jj, val[2], imax;
  char str[2][STRSZ];
  char ichar[2];

  pos = 0;
  val[0] = 0;
  val[1] = 0;
  ichar[1] = '\0';

  if (spot_type == SPOT_GS)
    imax = MAX_INPUT_FILES;
  if (spot_type == SPOT_SS)
    imax = STRSZ;

    while ((argv[pos] != '\0') && (pos < str_length))
    {
        strcpy(str[0], "\0");
        strcpy(str[1], "\0");

        while (isdigit(argv[pos]) && (pos < str_length))
        {
          ichar[0] = argv[pos];
          strcat(str[0], ichar);
          pos++;
        }
        val[0] = abs(atoi(str[0]));

        if ((argv[pos] == ',') || (argv[pos] == '\0'))
          if (val[0] < imax)
          {
            do_vectors[val[0]] = 1;
          }

        if (argv[pos] == '-')
        {
          pos++;
          while (isdigit(argv[pos]) && (pos < str_length))
          {
            ichar[0] = argv[pos];
            strcat(str[1], ichar);
            pos++;
          }
          val[1] = abs(atoi(str[1])); jj=0;
          if ((val[0] < imax) && (val[1] < imax))
            while (jj <= val[1])
            {
              do_vectors[jj] = 1;
              jj++;
            }
        }

        if (argv[pos] == '*')
          for (jj=val[0];jj<imax;jj++)
            do_vectors[jj] = 1;

        pos++;
    }

/*
currentpoint translate 0 5 sub 0 moveto
y1 y0 sub x1 x0 sub atan textangle rotate %rotate for text
/Times-Bold findfont fontsize scalefont setfont text show
currentpoint translate %translate for easy subscript
y1 y0 sub x1 x0 sub atan textangle -1 mul rotate %rotate back
0 subfontsize 8 div 0 subfontsize 5 div sub moveto %offset y for subscript
y1 y0 sub x1 x0 sub atan textangle rotate %rotate for subscript text
/Times-Bold findfont subfontsize scalefont setfont subtext show
y1 y0 sub x1 x0 sub atan textangle -1 mul rotate %rotate back
currentpoint translate 0 4 moveto %offset y for superscript
y1 y0 sub x1 x0 sub atan textangle rotate %rotate for superscript text
/Times-Bold findfont fontsize 0.8 mul scalefont setfont (*) show*/


  return 0;
}

/*========================================================================*/
