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
#include "patt_def.h"

/*========================================================================*/

int main(int argc, char *argv[])
{

  char *output_filename = (char*) malloc(MAX_PATH*sizeof(char));

  /* Preset out_stream */
  FILE *out_stream = stdout;
  FILE *in_stream = stdin;

  drawing_t *drawing = &drawing_default;

  patt_args(argc, argv, drawing, output_filename);

  if (strlen(output_filename))
  {
    if ((out_stream = fopen(output_filename, "w")) == NULL)
    {
      fprintf(stderr, "***error (patt_main): "
              "cannot write to '%s'!\n", output_filename);
      exit(PATT_WRITE_ERROR);
    }
  }
  patt_draw(out_stream, (const drawing_t*) &drawing);

  fclose(out_stream);

  return(PATT_SUCCESS);

} /* main */

int patt_draw(FILE *out_stream, const drawing_t *drawing)
{
  
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
  
  #else /* _USE_CAIRO */
  
  #endif
  
  return(PATT_SUCCESS);
}



/****************************************************************************/

/*========================================================================*/







