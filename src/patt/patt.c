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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <strings.h>
#include <ctype.h>

#include "patt.h"

/*========================================================================*/

int main(int argc, char *argv[])
{

int i;

float translate[2] = {0, 0};        /* x,y offset for text */

char title_str[STRSZ];
char fill_gs[6] = "fill";
char fill_ss[6] = "fill";

size_t i_dom = 0;
size_t n_dom = 0;

float spot_gs = RADIUS_GS;          /* substrate spot size */
float spot_ss = RADIUS_SS;          /* superstructure spot size */
float ix, iy, x, y; 

float radius;
int i_line_offset;
char comm;                          /* commensurate or not */
char text[STRSZ];
char text2[STRSZ];
char dummystr[5][STRSZ];
char vectors_str[STRSZ*8];
char output_filename[STRSZ*2];

char* footnote = NULL;

int do_gs_vectors[MAX_INPUT_FILES];
int do_ss_vectors[MAX_INPUT_FILES][STRSZ];
int do_gs_indices[MAX_INPUT_FILES];
int do_ss_indices[MAX_INPUT_FILES];


/* drawing default options */

int stdin_flag = 1;                 /* input is from stdin pipe by default */
int overlap_flag = 0;               /* overlap GS spots with SS spot strokes */
int color_flag = PATT_COLOR;        /* PATT_COLOR for rgb (default) */
int vectors_flag = 0;               /* draw basis vectors */
int ev_flag = 0;
int rg_flag = 0;
int rs_flag = 0;
int format = PATT_PS;               /* output file format */

/* Preset out_stream */
 out_stream = stdout;
 in_stream = stdin;
 in_streams[0] = stdin;

 i_line_offset = 0;                 /* title line y-offset */
 
 strcpy(vectors_str, "\0");
 
 /* loop over input files */
 for (i=0;i<MAX_INPUT_FILES;i++)
 {
   strcpy(dummystr[i], "\0");
   do_gs_vectors[i] = 0;
   do_gs_indices[i] = 1;
   do_ss_indices[i] = 1;
   
   /* loop over domains */
   for (ii=0;ii<STRSZ;ii++)
	 do_ss_vectors[i][ii] = 0;
     
 }
 strcpy(title_str,"\0");

/*
 Check command line and decode arguments
*/

drawing_t *drawing;
pattern_t *pat = pattern_init();

patt_args(argv, argc, &drawing, &format);

patt_draw(&drawing, format);

return 0;

} /* main */

int patt_draw(FILE *out_stream, drawing_t *drawing, int format)
{
  size_t i;
  FILE *in_stream;
  pattern_t *pat; 
  patt_color_rgb_t *color = PATT_BLACK;
  
  #ifdef _USE_CAIRO
  cairo_surface_t *surface;
  cairo_t *cr;
  
  /* create surface */
  switch (format)
  {
    case PATT_PNG:
      surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 
                                           PATT_PAGE_WIDTH, PATT_PAGE_HEIGHT);
      break;
    
    case PATT_PDF:
      surface = cairo_pdf_surface_create(drawing->output_filename, 
                                         PATT_PAGE_WIDTH, PATT_PAGE_HEIGHT);
      break;
    
    case PATT_SVG:
      surface = cairo_svg_surface_create(drawing->output_filename, 
                                         PATT_PAGE_WIDTH, PATT_PAGE_HEIGHT);
      break;
    
    case PATT_EPS: case PATT_PS: default:
      surface = cairo_ps_surface_create(drawing->output_filename, 
                                        PATT_PAGE_WIDTH, PATT_PAGE_HEIGHT);
      if (format == PATT_EPS) cairo_ps_surface_set_eps(surface, TRUE);
      cairo_ps_surface_dsc_comment (surface, "%%Title: %s", drawing->title.text);
      cairo_ps_surface_dsc_comment (surface, "%%Copyright: Copyright (C) "
                                    "2013-2014 Georg Held & Liam Deacon")
      cairo_ps_surface_dsc_comment (surface, "%%Creator: %s - Version %s", 
                                    PROG, PROG_VERSION);
      break;
  }
  cr = cairo_create(surface);
  
  /* set origin to center of image */
  cairo_translate(cr, PATT_PAGE_WIDTH, PATT_PAGE_HEIGHT);
  
  /* set options */
  cairo_select_font_face(cr, "Serif", CAIRO_FONT_SLANT_NORMAL, 
                         CAIRO_FONT_WEIGHT_NORMAL);

  cairo_set_font_size(cr, 13);
  
  
  /***************************** STRUCTURE LOOP ****************************/
  for (i_file = 0; i_file < drawing->n_files; i_file++)
  {
    pattern_t *pat = pattern_read(drawing->input_files[i_file]);
    
    /* draw substrate structure */
    spots_t *gs_spots = pattern_calculate_substrate_spots(pat);

    switch (drawing->color)
    {
      case PATT_GRAYSCALE_SCHEME:
        color = grays[i_file % NUM_GRAYS];
        break;
      case PATT_COLOR_SCHEME:
        color = colors[i_file % NUM_COLORS][SPOT_GS];
        breal;
      case PATT_BLACK_SCHEME:
        color = PATT_BLACK;
        break;
    }

    /* set spot properties */
    cairo_set_source_rgb(cr, color.red, color.green, color.blue);
    cairo_set_line_width(cr, gs_spots->stroke_width);
    
    for (i_spot = 0; i_spot < gs_spots->n_spots; i_spot++)
    {
      /*! DRAW HERE */      
      patt_draw_cairo_spot(cr, gs_spots->spots[i_spot], 
                           gs_spots->radius, spots_get_shape(gs_spots));
      if (gs_spots.fill == true) cairo_fill(cr);
      if (gs_spots.indexing == true)
      {
        patt_draw_cairo_label(cr, gs_spots->spots[i_spot]);  
      }
    
    }
  
    /* draw superstructure spots */
    if (drawing->color == PATT_COLOR_SCHEME)
    {
      color = colors[i_file % NUM_COLORS][SPOT_SS];
      cairo_set_source_rgb(cr, color.red, color.green, color.blue);
    }
      
    cairo_set_line_width(cr, ss_spots->stroke_width);
    cairo_set_font_size(cr, abs(ss_spots->font_size));
    for (i_domain = 0; i_domain < pat->n_domains; i_domain++)
    {
      spots_t *ss_spots = pattern_calculate_superstructure_spots(pat, i_domain);
      
      spots_set_shape(ss_spots, i_domain);
      ss_spots->font_size = 12;
      ss_spots->radius = RADIUS_SS;
      
      for (i_spot = 0; i_spot < ss_spots->n_spots; i_spot++)
      {
        /*! DRAW HERE */
        patt_draw_cairo_spot(cr, ss_spots->spots[i_spot], 
                           ss_spots->radius, spots_get_shape(ss_spots));
        if (gs_spots.fill == true) cairo_fill(cr);
        if (gs_spots.indexing == true)
        {
          patt_draw_cairo_label(cr, ss_spots->spots[i_spot]);  
        }
      }
      
      spots_free(ss_spots);
    }
  
    spots_free(gs_spots);
    pattern_free(pat);
  
  } /* for i_file */
  
  /* clean up */
  cairo_destroy (cr);
  cairo_surface_finish (surface);
  cairo_surface_destroy (surface);
  
  return 0;
  
  #else /* _USE_CAIRO */
  
  #endif
  
    default:
      /**************** use old PostScript routines ***************************/
      /* write initial lines to output */
      ps_draw_init(out_stream, ifiles, title_str, pos_title, footnote, pos_footnote,
                   screen_flag, screen_thickness, fill_screen, ev_flag, eV, pos_ev,
                   clip_flag, vectors_flag); 
                   
      for (i = 0; i < drawing->nfiles; i++)
      {
      
        /* create section header */
        fprintf(out_stream, "\n"
                "%%======================================================================="
                "\n%% BEGIN SECTION %i:\n"
                "%%======================================================================="
                "\n\n", i+1);
        if (fileno(in_stream) != fileno(stdin))
            in_stream = fopen(drawing->filename[i], "r");
            
        if (in_stream == NULL)
        {
            fprintf(stderr, "error (patt_draw): cannot read file '%s'\n", 
                    drawing->filename[i])
            exit(1);
        }
        
        /* read comment */
        
        
        ps_draw_title(in_stream, out_stream, i, drawing->nfiles,
                      ps_color(color_flag, i, drawing->nfiles, SPOT_GS, 0), 
                      &i_line_offset);
        
        /* set draw routines for reciprocal lattice vectors */
        if ((drawing->show_vectors) && (do_gs_vectors[ii]))
        ps_draw_vectors(out_stream, -1, ii, 
                     ps_color(color_flag, ii, ifiles, SPOT_GS, 0),
                     a1, a2, spot_gs, dummystr, SPOT_GS, vectors_str);
          
        fprintf(out_stream, "%% radius: %.1f\n%%\n", radius);

        /* print position of GS spots to output */
        fprintf(out_stream,"%s\n","% GS spots");
        fprintf(out_stream,"%s\n", ps_color(color_flag, ii, ifiles, SPOT_GS, 0));
        fprintf(out_stream," %.1f setlinewidth stroke\n", LINE_WIDTH);

        
        if (strlen(drawing->eV.text) > 0)
        {
          /* calculate radius of Ewald construction at given energy */
          lambda = sqrt(2*9.11*1.6018*atof(drawing->eV.text))/6.62;
          lambda /= 2*M_PI;
          radius = lambda;
        }
        
        for each GS_spot:
           /* draw spot */
           fprintf(out_stream,
             " %.1f %.1f %.1f 0 360 arc gsave %s %s grestore stroke\n", 
            ix, iy, spot_gs, ps_color(color_flag, ii, ifiles, SPOT_GS, i_dom), 
            fill_gs);
            
            /* draw label */
            sprintf(text, "%d,%d",i1,i2);
            fprintf(out_stream, "/Times-Roman findfont %.1f scalefont setfont \n",
                    spot_gs*2);
            fprintf(out_stream, "%.1f %.1f moveto (%s) show stroke\n", 
                       ix - 1.5*spot_gs, iy + spot_gs + 4., text);
                       
        /* loop over domains */

        fprintf(out_stream, "%s\n","% SS spots");
        ps_set_linewidth(out_stream, LINE_WIDTH);
        
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

          for each domain:
            
          
          
            for each spot:
                if (symbols)
                {
                 ps_draw_spot(out_stream, x, y, spot_ss, i_dom, 
                    ps_color(color_flag, ii, ifiles, SPOT_SS, i_dom), fill_ss);
                }
                else
                {
                 ps_draw_spot(out_stream, x, y, spot_ss, 0,
                    ps_color(color_flag, ii, ifiles, SPOT_SS, i_dom), fill_ss);
                }
                
                       /* check if integral order spot */
        if ((index) && (do_ss_indices[ii]) &&
            ((fabs(ind_1*aux1) - (int)(fabs(ind_1*aux1)+0.1) > 0.05) ||
            (fabs(ind_2*aux1) - (int)(fabs(ind_2*aux1)+0.1) > 0.05) ))
        {
          fprintf(out_stream,"/Times-Roman findfont %.1f scalefont setfont \n",
                  spot_ss*2);
  
          if (!commensurate) 
          {
            /* incommensurate */
           fprintf(out_stream, "%.1f %.1f moveto (%.2f,%.2f) show stroke\n",
                   x - 4*spot_ss, y + spot_ss + 4., ind_1*aux1, ind_2*aux1); 
           printf("incommensurate\n");
           fprintf(out_stream, "%s Mult scatt. \n", "%");
          }
          else
          {
            /* commensurate (fractions) */

            ind_1_int = (int) (ind_1*1.01);
            ind_2_int = (int) (ind_2*1.01);
            det_int =   (int) (det*1.01);

            switch (nice_frac(&ind_1_int, &det_int))
            {
              case(0):
              {
                sprintf(text, "0, ");
                break;
              } 
            
              case(1):
              {
                sprintf(text, "%d, ", ind_1_int);
                break;
              }
            
              case(2):
              {
                sprintf(text, "%d/%d, ", ind_1_int, det_int);
                break;
              }
            }

            det_int = (int) (det*1.01);
            switch (nice_frac(&ind_2_int, &det_int))
            {
              case(0):
              {
                sprintf(text2, "0");
                break;
              }
            
              case(1):
              {
                sprintf(text2, "%d", ind_2_int);
                break;
              }
          
              case(2):
              {
                sprintf(text2, "%d/%d", ind_2_int, det_int);
                break;
              }
            }
      
          fprintf(out_stream,"%.1f %.1f moveto (%s%s) show stroke\n", 
                  x - 3*spot_ss, y + spot_ss + 2., text, text2);
          }
        }
        
            /* print domain lattice vectors */
            if ((vectors_flag) && (do_ss_vectors[ii][i_dom]))
            {
                ps_draw_vectors(out_stream, i_dom, ii, 
                     ps_color(color_flag, ii, ifiles, SPOT_SS, i_dom),
                     b1, b2, spot_ss, dummystr, SPOT_SS, vectors_str);
            }
        
        /* final drawing strokes */ 
          ps_draw_finalize(out_stream, gun_flag, gun_thickness, gun_radians,
            screen_thickness, screen_flag, vectors_flag, vectors_str,
            ifiles, ev_flag, eV, title_str);
          fclose(out_stream);         

        /* clean up */
        if (fileno(in_stream) != fileno(stdin)) 
          fclose(in_stream);
      }
      break;
  }
  
}

int patt_draw_*(drawing_t *drawing)
{
  for (i = 0; i < drawing->nfiles; i++)
  {
  
  
  
  }


}



/****************************************************************************/

/*========================================================================*/







