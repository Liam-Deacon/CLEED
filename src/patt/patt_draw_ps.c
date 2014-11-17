/*
 * patt_draw_ps.c
 *
 *  Created on: 24 Sep 2014
 *      Author: Liam Deacon
 */

#include "patt.h"
#include <strings.h>

/*!
 * Draws the LEED pattern using the old postscript backend.
 *
 * \param drawing Pointer to #patt_drawing instance containing all the drawing
 * parameters.
 * \return #patt_error code indicating function success.
 * \retval #PATT_SUCCESS on successful completion of function.
 */
int patt_draw_ps(const patt_drawing *drawing)
{
  size_t i_file, i_spot, i_domain;
  patt_color_rgb color = PATT_BLACK;
  pattern *pat = NULL;
  FILE *f = NULL;
  FILE *out_stream = NULL;
  char title[strlen(drawing->title.label)+10];
  char copyright[strlen(PATT_PROG_COPYRIGHT)+20];
  char program[strlen(PATT_PROG_VERSION)+strlen(PATT_PROG)+20];

  sprintf(title, "%%Title: %s", drawing->title.label);
  sprintf(copyright, "%%Copyright: %s", PATT_PROG_COPYRIGHT);
  sprintf(program, "%%Creator: %s - Version %s", PATT_PROG, PATT_PROG_VERSION);

  /* open output stream */
  if ((out_stream = fopen(drawing->output_filename, "w")) == NULL)
  {
    fprintf(stderr, "***error (patt_draw_ps): "
            "cannot open '%s'\n", drawing->output_filename);
    exit(PATT_WRITE_ERROR);
  }
  /* setup page */
  //PATT_PAGE_WIDTH, PATT_PAGE_HEIGHT);

  /* add header lines */

  /* set origin to middle of image */
  //translate(PATT_PAGE_WIDTH, PATT_PAGE_HEIGHT);

  /* set options */


  /***************************** STRUCTURE LOOP ****************************/
  for (i_file = 0; i_file < drawing->n_files; i_file++)
  {
    if (strlen(drawing->input_files[i_file]))
    {
      if ((f = fopen(drawing->input_files[i_file], "r")) != NULL)
      {
        pat = pattern_read(f);
      }
      else
      {
        fprintf(stderr, "***error (patt_draw_cairo): "
                "cannot open '%s'\n", drawing->input_files[i_file]);
        exit(PATT_READ_ERROR);
      }
    }
    /* draw substrate structure */
    spots *gs_spots = pattern_calculate_substrate_spots(pat);

    switch (drawing->color_scheme)
    {
      case PATT_GRAYSCALE_SCHEME:
        color = grays[i_file % NUM_GRAYS];
        break;
      case PATT_COLOR_SCHEME:
        patt_color_copy(&color, patt_color_from_name(
                                       colors[i_file % NUM_COLORS][SPOT_GS]));
        break;
      case PATT_MONOCHROME_SCHEME: default:
        color = PATT_BLACK;
        break;
    }

    /* set spot properties */
    patt_color_copy(&gs_spots->color, &color);
    //cairo_set_source_rgb(cr, color.red, color.green, color.blue);
    //cairo_set_line_width(cr, gs_spots->stroke_width);

    for (i_spot = 0; i_spot < gs_spots->n_spots; i_spot++)
    {
      /*! DRAW HERE */
      patt_draw_ps_spot(out_stream, &gs_spots->spots[i_spot], gs_spots->radius,
                        spots_get_shape(gs_spots), &gs_spots->color,
                        gs_spots->fill);
      //!TODO: if (gs_spots->fill == true) cairo_fill(cr);
      if (gs_spots->indexing == true)
      {
        patt_draw_ps_label(out_stream, gs_spots, &gs_spots->spots[i_spot]);
      }

    }

    /* draw superstructure spots */
    for (i_domain = 0; i_domain < pat->n_domains; i_domain++)
    {
      spots *ss_spots = pattern_calculate_superstructure_spots(pat, i_domain);

      //cairo_set_line_width(cr, ss_spots->stroke_width);
      //cairo_set_font_size(cr, abs(ss_spots->font_size));

      patt_color_copy(&ss_spots->color, &gs_spots->color);
      spots_set_shape(ss_spots, i_domain);
      ss_spots->font_size = 12;
      ss_spots->radius = RADIUS_SS;

      for (i_spot = 0; i_spot < ss_spots->n_spots; i_spot++)
      {
        /*! DRAW HERE */
        patt_draw_ps_spot(out_stream, &ss_spots->spots[i_spot],
                           ss_spots->radius, ss_spots->shape,
                           &ss_spots->color, ss_spots->fill);
        if (gs_spots->indexing == true)
        {
          patt_draw_ps_label(out_stream, ss_spots, &ss_spots->spots[i_spot]);
        }
      }

      spots_free(ss_spots);
    }

    /* clean up */
    spots_free(gs_spots);
    pattern_free(pat);
    free(f);

  } /* for i_file */

  /* clean up */

  return(PATT_SUCCESS);
}

/*!
 * Adds final postscript to the output file \p file_ptr
 *
 * \param file_ptr File pointer to postscript output e.g. \c stdout or file.
 * \param drawing Pointer to #patt_drawing instance containing all the drawing
 * information.
 * \todo check basis vectors finialisation.
 */
void patt_draw_ps_finish(FILE *file_ptr, const patt_drawing *drawing)
{
  size_t ii;

  fprintf(file_ptr, "\n"
    "%%======================================================================="
    "\n\n");

  if (drawing->gun.visible)
  {
    patt_draw_ps_gun(file_ptr, &drawing->gun);
  }
  //if (drawing->show_vectors) fprintf(file_ptr, "%s\n", vectors_str);
  if (drawing->screen.visible)
  {
    patt_draw_ps_screen(file_ptr, &drawing->screen);
  }

  fprintf(file_ptr, "grestore\n");
  fprintf(file_ptr, "%%Print Titles\n");
  for (ii=0; ii<drawing->n_files; ii++)
  {
    fprintf(file_ptr, "Title%i ", ii);
  }
  fprintf(file_ptr, "\n");

  if (drawing->eV.visible) fprintf(file_ptr, "add_eV\n");
  if (drawing->title.label[0] != '\0') fprintf(file_ptr, "title\n");

  fprintf(file_ptr, "footnote\n");
  fprintf(file_ptr, "\nshowpage\n");
  fprintf(file_ptr, "%%EOF");

}

/*!
 * Draws an electron gun in postscript to the \p file_ptr output file
 *
 * \param file_ptr File pointer to output e.g. \c stdout or file.
 * \param[in] gun Pointer to #patt_gun instance containing information about #
 * the electron gun drawing parameters.
 */
void patt_draw_ps_gun(FILE *file_ptr, const patt_gun *gun)
{
  fprintf(file_ptr, "%% Electron gun\n");
  fprintf(file_ptr, "%s\n", patt_color_get_ps_string(&PATT_GUN_COLOR));
  fprintf(file_ptr, "newpath\n%.1f %.1f moveto %.1f setlinewidth\n",
          gun->stroke_width, -gun->stroke_width, gun->stroke_width);
  fprintf(file_ptr,
          "%.1f %.1f lineto %.1f %.1f lineto\n%.1f %.1f lineto\n",
          -(1.005*MAX_RADIUS*cos(3.14182*(gun->angle+0.035))),
          -(1.005*MAX_RADIUS*sin(3.14182*(gun->angle+0.035))),
          -(1.005*MAX_RADIUS*cos(3.14182*(gun->angle-0.035))),
          -(1.005*MAX_RADIUS*sin(3.14182*(gun->angle-0.035))),
          -gun->stroke_width, gun->stroke_width);
  fprintf(file_ptr, "closepath\n%s fill stroke\n",
          patt_color_get_ps_string(&PATT_GUN_COLOR));
  fprintf(file_ptr, "0 0 %.1f 0 360 arc 0 0 0 setrgbcolor fill stroke\n",
          gun->radius);

    /*!TODO: use edge_thickness & screen_flag */
}

/*!
 * Initializes the postscript file \p file_ptr
 *
 * \param file_ptr File pointer to postscript output e.g. \c stdout or file.
 * \param[in] drawing Pointer to #patt_drawing instance containing all the
 * drawing parameters for setting up the postscript file.
 */
void patt_draw_ps_init(FILE *file_ptr, const patt_drawing *drawing)
{

  fprintf(file_ptr, "%%!PS-Adobe-3.0 \n");
  fprintf(file_ptr, "%%Creator: %s\n", PATT_PROG_VERSION);
  fprintf(file_ptr,
    "%%BoundingBox: 50 150 550 650 \n"
    "%%EndComments \n"
    "%%======================================================================="
    "\n%%Initial setup:\n"
    "%%======================================================================="
    "\n");

  if (drawing->n_files > 1)
  {
    fprintf(file_ptr, "%%%%Page: %s %i\n", "Composite_Input", 1);
  }
  else
  {
    fprintf(file_ptr, "%%%%Page: %s %i\n", "Single_Input", 1);
  }

  fprintf(file_ptr, " %.1f %.1f translate gsave %%center of page as origin\n",
          OFF_W, OFF_H);

  if (drawing->title.label[0] != '\0')
  {
    fprintf(file_ptr, "/title {%%add custom title\n/Times-Bold findfont "
            "%.1f scalefont setfont\n", drawing->title.size);
    fprintf(file_ptr, "%s %.1f %.1f moveto (%s) show\n} def\n",
            patt_color_get_ps_string(&PATT_BLACK),
            drawing->title.x, drawing->title.y, drawing->title.label);
  }
  fprintf(file_ptr, "/footnote {%%add page footnote\n");
  fprintf(file_ptr, "/Times-Roman findfont %.1f scalefont setfont\n",
         drawing->title.size);
  fprintf(file_ptr, "%s %.1f %.1f moveto (%s) show\n} def\n",
          patt_color_get_ps_string(&PATT_BLACK),
          drawing->footnote.x, drawing->footnote.y, drawing->footnote.label);
  if (drawing->screen.visible)
  {
    patt_draw_ps_screen(file_ptr, &drawing->screen);
  }

  if (drawing->eV.visible)
  {
    fprintf(file_ptr,"/add_eV {%%add electron energy label\n");
    fprintf(file_ptr, "/Times-ItalicBold findfont %f scalefont setfont\n",
            drawing->eV.size);
    fprintf(file_ptr,"%s %.1f %.1f moveto (%.0feV) show\n} def\n",
            patt_color_get_ps_string(&PATT_BLACK), drawing->eV.x,
            drawing->eV.y, atof(drawing->eV.label));
  }
  if (drawing->screen.clip)
  {
    fprintf(file_ptr, "%s %.1f 0 moveto 0 0 %.1f 0 360 arc clip\n",
            patt_color_get_ps_string(&PATT_WHITE),
            MAX_RADIUS+(drawing->screen.stroke_width-1),
            MAX_RADIUS+(drawing->screen.stroke_width-1));
    fprintf(file_ptr, "0. setlinewidth 1 1 1 setrgbcolor 204.0 0"
            " moveto 0 0 %.1f 0 360 arc stroke\n",
            MAX_RADIUS+(drawing->screen.stroke_width-1));
  }
  if (drawing->show_vectors) patt_draw_ps_vector_funcs(file_ptr);

}

/*!
 * Draws the Miller index for a given diffraction spot.
 *
 * \param file_ptr
 * \param group
 * \param spot
 */
void patt_draw_ps_label(FILE *file_ptr, const spots *group, const spot *spot)
{
  fprintf(file_ptr, "%.1f %.1f moveto (%s) show stroke\n",
          spot->x - 4*group->radius, spot->y - 4*group->radius, spot->label);
}

/*!
 * Draws a synthetic screen in postscript to \p file_ptr
 *
 * \param file_ptr File pointer for the output e.g. \c stdout or file.
 * \param screen #patt_screen instance containing all parameters for
 * drawing the LEED screen.
 */
void patt_draw_ps_screen(FILE *file_ptr, const patt_screen *screen)
{
  fprintf(file_ptr, "%% Screen\n");
  fprintf(file_ptr, "0 0 moveto %.1f setlinewidth stroke\n",
          screen->stroke_width);
  fprintf(file_ptr, "0 0 %.1f 0 360 arc ",
          MAX_RADIUS+(screen->stroke_width*0.5));
  fprintf(file_ptr, "%s stroke\n", patt_color_get_ps_string(&PATT_EDGE_COLOR));

  if(screen->fill)
  {
    fprintf(file_ptr, "0 0 %.1f 0 360 arc %s fill stroke\n",
            MAX_RADIUS+(screen->stroke_width*0.5),
            patt_color_get_ps_string(&PATT_SCREEN_COLOR));
  }

}

/*!
 * Draws LEED spots in postscript and outputs to \p file_ptr
 *
 * \param file_ptr File pointer for postscript output e.g. \c stdout or a file
 * \param spot Diffraction #spot instance containing spot position and label.
 * \param spot_size Size of drawn spot.
 * \param shape Enum value #patt_shape representing spot shape to draw.
 * \param color Color of drawn spot.
 * \param fill Boolean value of whether or not to fill the spot shape
 * with \p color
 */
void patt_draw_ps_spot(FILE *file_ptr, spot *spot, double spot_size,
                      patt_shape shape, const patt_color_rgb *color, bool fill)
{
  int delta = 0;
  double x = spot->x;
  double y = spot->y;

  switch(shape)
  {

    case(PATT_SQUARE):
    {
      /* square */
      spot_size /= 1.4;
      fprintf(file_ptr, "newpath\n");
      fprintf(file_ptr,"%.1f %.1f moveto %.1f %.1f lineto \n",
      x-spot_size, y-spot_size, x-spot_size, y+spot_size);
      fprintf(file_ptr, "%.1f %.1f lineto %.1f %.1f lineto\n",
      x+spot_size, y+spot_size, x+spot_size, y-spot_size);
      fprintf(file_ptr, "closepath");
      break;
    }

    case(PATT_TRIANGLE_UP):
    {
      /* up triangle */
      delta = spot_size * 0.866;
      fprintf(file_ptr, "newpath\n");
      fprintf(file_ptr,"%.1f %.1f moveto %.1f %.1f lineto \n",
      x, y + spot_size, x - delta, y - (0.5* spot_size));
      fprintf(file_ptr, "%.1f %.1f lineto \n closepath",
      x + delta, y - (0.5* spot_size));
      break;
    }

    case(PATT_TRIANGLE_DOWN):
    {
      /* down triangle */
      delta = spot_size * 0.866;
      fprintf(file_ptr, "newpath\n");
      fprintf(file_ptr,"%.1f %.1f moveto %.1f %.1f lineto \n",
      x, y - spot_size, x - delta, y + (0.5* spot_size));
      fprintf(file_ptr, "%.1f %.1f lineto \n closepath",
      x + delta, y + (0.5 * spot_size));
      break;
    }

    case(PATT_DIAMOND):
    {
      /* diamond */
      fprintf(file_ptr, "newpath\n");
      fprintf(file_ptr,"%.1f %.1f moveto %.1f %.1f lineto \n",
      x-spot_size, y, x, y+spot_size);
      fprintf(file_ptr, "%.1f %.1f lineto %.1f %.1f lineto\n",
      x+spot_size, y, x, y-spot_size);
      fprintf(file_ptr, " closepath");
      break;
    }

    case(PATT_HEXAGON):
    {
      /* hexagon */
      delta = spot_size * 0.866;
      fprintf(file_ptr, "newpath\n");
      fprintf(file_ptr,"%.1f %.1f moveto %.1f %.1f lineto \n",
      x, y - spot_size, x - delta, y - (0.5*spot_size));
      fprintf(file_ptr, "%.1f %.1f lineto %.1f %.1f lineto\n",
      x - delta, y + (0.5*spot_size), x, y + spot_size);
      fprintf(file_ptr, "%.1f %.1f lineto %.1f %.1f lineto \n closepath",
      x + delta, y + (0.5*spot_size), x + delta, y - (0.5*spot_size));
      break;
    }

    case(PATT_CIRCLE): default:
    {
      fprintf(file_ptr, " %.1f %.1f %.1f 0 360 arc", x, y, spot_size);
      break;
    }

  }  /* switch */

  if (fill == true)
  {
    fprintf(file_ptr, " %s fill", patt_color_get_ps_string(color));
  }
  fprintf(file_ptr, " stroke\n");

}


/*!
 * Writes postscript routines for titles to output \p out_stream
 *
 * \param in_stream File pointer to input stream e.g. \c stdin or file.
 * \param out_stream File pointer to output stream e.g. \c stdout or file.
 * \param ii
 * \param ifiles
 * \param color
 * \param i_line_offset
 */
void patt_draw_ps_title(FILE *in_stream, FILE *out_stream, size_t ii,
                        size_t ifiles, char *color, int *i_line_offset)
{
  int step;
  size_t lineno = 0;

  fget_nocomm(line_buffer, in_stream, out_stream);

  if (ifiles > 1) step = 25;
  else step = -25;

  /* read all c-comment lines and output accordingly */
  while (line_buffer[0] == 'c')
  {
    fprintf(out_stream, "/Title%i {\n", ii);
    fprintf(out_stream, "/Times-Roman findfont 20 scalefont setfont\n");
    fprintf(out_stream, "%s", color);
    fprintf(out_stream, " %.1f %.1f moveto\n",
            -MAX_RADIUS-50, MAX_RADIUS + 5 + *i_line_offset);
    *i_line_offset += step;

    if (!ps_print_substitute(out_stream, line_buffer+1))
      fprintf(out_stream, "(%s) show\n", line_buffer+1 );

    fget_nocomm(line_buffer, in_stream, out_stream);
    fprintf(out_stream, "} def\n");

    lineno++;
  }

   if (!lineno) fprintf(out_stream, "/Title%i {} def %%dummy\n", ii);

}

/*!
 * Draws reciprocal space vectors to postscript output file
 *
 * \param file_ptr File pointer to postscript output e.g. \c stdout or file.
 * \param[in] drawing Pointer to #patt_drawing
 */
void patt_draw_ps_vectors(FILE *file_ptr, const patt_drawing *drawing)
{
  size_t i_dom;
  int ii;
  char *color;
  float *a1;
  float *a2;
  float spot_radius;
  char dummystr[10][STRSZ];
  size_t spot;
  char *vectors_str;

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

  /* do comparisons with previous & write new vectors if neccessary */
  sprintf(dummystr[compare[1]],
  "%.1f %.1f %.1f %.1f", a1[0], a1[1], a2[0], a2[1]);
  if (strcmp(dummystr[compare[0]], dummystr[compare[1]]))
  {

    fprintf(file_ptr,"%%\n%% Lattice vectors\n");

    fprintf(file_ptr, "/%c1_%i%s {\ngsave %s\n", v_type, ii, dom_str, color);
    fprintf(file_ptr,
            "0 0 %.1f %.1f %.1f arrow\n", a1[0], a1[1], spot_radius*0.5);

    if (v_type == 'a')
    {
      sprintf(dummystr[2], "%i", 1);
    }
    else
    {
      sprintf(dummystr[2], "%i,%i", 1, i_dom);
    }

    fprintf(file_ptr, "0 0 %.1f %.1f (%c) (%s) %.1f %.1f rotatedlabel\n",
            a1[0], a1[1], v_type, dummystr[2], spot_radius*2.5, spot_radius*1.25);
    fprintf(file_ptr, "grestore\n} def\n");

    fprintf(file_ptr, "/%c2_%i%s {\ngsave %s\n", v_type, ii, dom_str, color);
    fprintf(file_ptr, "0 0 %.1f %.1f %.1f arrow\n", a2[0], a2[1], spot_radius*0.5);

    if (v_type == 'a')
    {
      sprintf(dummystr[2], "%i", 2);
    }
    else
    {
      sprintf(dummystr[2], "%i,%i", 2, i_dom);
    }

    fprintf(file_ptr, "0 0 %.1f %.1f (%c) (%s) %.1f %.1f rotatedlabel\n",
            a2[0], a2[1], v_type, dummystr[2], spot_radius*2.5, spot_radius*1.25);

    fprintf(file_ptr, "grestore\n} def\n");

    sprintf(dummystr[2], "\n%c1_%i%s %c2_%i%s",
            v_type, ii, dom_str, v_type, ii, dom_str);

    strcat(vectors_str, dummystr[2]);

  }

  sprintf(dummystr[compare[0]], "%.1f %.1f %.1f %.1f",
          a1[0], a1[1], a2[0], a2[1]);

}

/*!
 * Creates postscript macros for drawing basis vectors.
 *
 * \param file_ptr File pointer to output e.g. \c stdout or a file
 */
/* add lattice vector postscript subroutines */
void patt_draw_ps_vector_funcs(FILE *file_ptr)
{
  /* print section banner */
  fprintf(file_ptr,
  "%%=======================================================================\n"
  "%%Lattice vector subroutines:\n"
  "%%=======================================================================\n");

  /* add text angle PS subroutine */
  fprintf(file_ptr, "/textangle { %%returns correct angle for text\n");
  fprintf(file_ptr, "dup 90 gt { %%>90\n");
  fprintf(file_ptr, " dup 180 gt \n");
  fprintf(file_ptr, " { %%>180\n");
  fprintf(file_ptr, " dup 270 gt \n");
  fprintf(file_ptr, "     {dup pop dup 360 sub} %%>270\n");
  fprintf(file_ptr, "     {dup pop dup 180 sub} %%> 180\n");
  fprintf(file_ptr, "     ifelse\n");
  fprintf(file_ptr, " }\n");
  fprintf(file_ptr, " {dup pop dup 180 sub} %%90 < x < 180\n");
  fprintf(file_ptr, " ifelse\n");
  fprintf(file_ptr, "}\n");
  fprintf(file_ptr, "{dup pop dup}\n");
  fprintf(file_ptr, "ifelse\n");
  fprintf(file_ptr, "} def\n");
  fprintf(file_ptr, "%%\n");

  /* add rotated text PS subroutine */
  fprintf(file_ptr, "/positiontext { %%calculate text offset\n");
  fprintf(file_ptr, "2 dict begin\n");
  fprintf(file_ptr, "/sf exch def\n");
  fprintf(file_ptr, "/y1 exch def\n");
  fprintf(file_ptr, "/x1 exch def \n");
  fprintf(file_ptr, "y1 0 gt \n");
  fprintf(file_ptr, "{ %%y +ve\n");
  fprintf(file_ptr, " x1 0 gt %%x +ve?  \n");
  fprintf(file_ptr, " {x1 sf sub y1 0.95 mul sf add moveto} %%x +ve\n");
  fprintf(file_ptr, " {\n");
  fprintf(file_ptr, "   x1 0 lt %%x -ve?  \n");
  fprintf(file_ptr, "   {x1 1.25 mul y1 1.1 mul 5 add moveto} %%x -ve\n");
  fprintf(file_ptr, "   {x1 5 sf mul sub y1 0.95 mul 5 sf mul add moveto} %%special case x=0\n");
  fprintf(file_ptr, "   ifelse\n");
  fprintf(file_ptr, " }\n");
  fprintf(file_ptr, " ifelse\n");
  fprintf(file_ptr, "}\n");
  fprintf(file_ptr, "{\n");
  fprintf(file_ptr, " y1 0 lt\n");
  fprintf(file_ptr, " { %%y -ve\n");
  fprintf(file_ptr, "   x1 0 gt %%x +ve?  \n");
  fprintf(file_ptr, "   {x1 sf 0.5 mul add y1 0.65 mul sf 2 mul add moveto} %%x +ve\n");
  fprintf(file_ptr, "   {\n");
  fprintf(file_ptr, "     x1 0 lt %%x -ve?  \n");
  fprintf(file_ptr, "     {x1 1.30 mul y1 0.8 mul moveto} %%x -ve\n");
  fprintf(file_ptr, "     {x1 5 sf mul sub y1 0.95 mul 5 add moveto} %%special case x=0\n");
  fprintf(file_ptr, "     ifelse\n");
  fprintf(file_ptr, "   }\n");
  fprintf(file_ptr, "   ifelse\n");
  fprintf(file_ptr, " }\n");
  fprintf(file_ptr, "    { %%********* special case: y=0 ***********%%\n");
  fprintf(file_ptr, "   x1 0 gt %%x +ve?  \n");
  fprintf(file_ptr, "   {x1 sf mul 6 sub y1 0.95 mul 5 sf mul add moveto} %%x +ve\n");
  fprintf(file_ptr, "   {\n");
  fprintf(file_ptr, "     x1 0 lt %%x -ve?  \n");
  fprintf(file_ptr, "     {x1 sf 5 mul sub y1 0.95 mul 5 sf mul add moveto} %%x -ve\n");
  fprintf(file_ptr, "     if\n");
  fprintf(file_ptr, "   }\n");
  fprintf(file_ptr, "   ifelse\n");
  fprintf(file_ptr, " }\n");
  fprintf(file_ptr, "    ifelse\n");
  fprintf(file_ptr, "}\n");
  fprintf(file_ptr, "ifelse\n");
  fprintf(file_ptr, "} def\n");
  fprintf(file_ptr, "\n");
  fprintf(file_ptr, "%%\n");

  /* add rotate label subroutine */
  fprintf(file_ptr,
      /* string split into segments as max length supported by C90 is 509 */
      "/rotatedlabel { %%adds rotated label with subscript text\n"
      "8 dict begin\n"
      "/subfontsize exch def\n"
      "/fontsize exch def\n"
      "/subtext exch def\n"
      "/text exch def\n"
      "/y1 exch def\n"
      "/x1 exch def\n"
      "/y0 exch def\n"
      "/x0 exch def\n"
      "gsave %%save coordinate system - important! \n"
      "x1 x0 sub 2 div 0.8 mul y1 y0 sub 2 div moveto %%move to label pos\n"
      "currentpoint fontsize 10 div positiontext\n"
      "y1 y0 sub x1 x0 sub atan textangle rotate %%rotate for text\n"
      "/Times-Bold findfont fontsize scalefont setfont text show\n");
  fprintf(file_ptr,
      "currentpoint translate gsave %%translate for easy subscript\n"
      "y1 y0 sub x1 x0 sub atan textangle -1 mul rotate %%rotate back \n"
      "0 subfontsize 8 div 0 subfontsize 5 div sub moveto "
      "%%offset y for subscript\n"
      "y1 y0 sub x1 x0 sub atan textangle rotate %%rotate for subscript text\n"
      "/Times-Bold findfont subfontsize scalefont setfont subtext show\n"
      "y1 y0 sub x1 x0 sub atan textangle -1 mul rotate %%rotate back \n"
      "grestore 0 fontsize subfontsize div 2 add moveto "
      "%%offset y for superscript\n");
  fprintf(file_ptr,
      "/Times-Bold findfont fontsize 0.8 mul scalefont setfont (*) show\n"
      "grestore %%restore original coordinate system\n"
      "} def\n"
      "%%\n");

  /* add arrowhead PS subroutine */
  fprintf(file_ptr, "/arrowhead {%% stack: s x1 y1, current point: x0 y0\n");
  fprintf(file_ptr, "gsave\n");
  fprintf(file_ptr, "currentpoint %% s x1 y1 x0 y0\n");
  fprintf(file_ptr, "4 2 roll exch %% s x0 y0 y1 x1\n");
  fprintf(file_ptr, "4 -1 roll exch %% s y0 y1 x0 x1\n");
  fprintf(file_ptr, "sub 3 1 roll %% s x1-x2 y0 y1\n");
  fprintf(file_ptr, "sub exch %% s y0-y1 x1-x2\n");
  fprintf(file_ptr, "atan rotate %% rotate over arctan((y0-y1)/(x1-x2))\n");
  fprintf(file_ptr, "dup scale %% scale by factor s\n");
  fprintf(file_ptr, "-7 2 rlineto 1 -2 rlineto -1 -2 rlineto\n");
  fprintf(file_ptr, "closepath fill %% fill arrowhead\n");
  fprintf(file_ptr, "grestore\n");
  fprintf(file_ptr, "newpath\n");
  fprintf(file_ptr, "} def\n");
  fprintf(file_ptr, "%%\n");

  /* add arrow PS subroutine */
  fprintf(file_ptr, "/arrow {\n");
  fprintf(file_ptr, "5 dict begin\n");
  fprintf(file_ptr, "/arrowsize exch def\n");
  fprintf(file_ptr, "/y1 exch def\n");
  fprintf(file_ptr, "/x1 exch def\n");
  fprintf(file_ptr, "/y0 exch def\n");
  fprintf(file_ptr, "/x0 exch def\n");
  fprintf(file_ptr, "gsave \n");
  fprintf(file_ptr, "arrowsize setlinewidth\n");
  fprintf(file_ptr, "x0 y0 moveto x1 0.95 mul y1 0.95 mul lineto stroke\n");
  fprintf(file_ptr, "x1 y1 moveto arrowsize x0 y0 arrowhead\n");
  fprintf(file_ptr, "grestore\n");
  fprintf(file_ptr, "} def\n%%\n");

}

/*!
 * Search string and convert to postscript where needed
 *
 * \param output File pointer for output e.g. \c stdout
 * \param[in] str String to modify.
 * \return
 * \todo implement substitution.
 */
int ps_print_substitute(FILE *output, char *str)
{
  int i, j, k, last;
  char substr[STRSZ];
  char prestr[STRSZ];
  char poststr[STRSZ];
  strcpy(poststr, str);

  i = j = k = 0;

  if (str[0] == '\0') return(0);

  /* printf("%s\n", str); */
  last=0;
  for (i=0; i<=(int)strlen(str)-1;i++)
  {
    if (str[i] == '$' && str[i+1] == '{' )
    {
      i++; j=0;
      strcpy(substr, "\0");
      strcpy(prestr, "\0");
      fprintf(output, "/Times-Roman findfont 20 scalefont setfont (");
      while (str[i] != '}' && i<=STRSZ-2)
      {
        i++;
        substr[j] = str[i];
        j++;
      }
      /* printf("%i %i %i\n", i, j, last); */

      for (k=last;k<=i-j-2;k++)
      {
       if (str[k] == '(' || str[k] == ')' ||
           str[k] == '{'  || str[k] == '}' ) fprintf(output, "%c", '\\' );
       if (str[k] == '\0') break;
       fprintf(output, "%c", str[k]);
       prestr[k] = str[k];
      }

      last = i+1;
      fprintf(output, ") show\n");
      for (k=0; k<=NUM_SUBS-1;k++)
      {
        /*!TODO: include PS substitutions */
        if (strncmp(substr, substitutes[k][0], j-1) == 0)
        {
          /* fprintf(output, "/Symbol findfont 20 scalefont setfont (%s) show\n",
                  substitutes[k][1]);
           */
        } /* if */
      } /* for k */
    } /* if */
  } /* for i */

  fprintf(output, "/Times-Roman findfont 20 scalefont setfont (");

  for (k=last+j; k<=STRSZ-1;k++)
  {
    if ((str+k == NULL) || (str[k] == '\0')) break;
    fprintf(output, "%c", str[k]);
  }

  fprintf(output, ") show\n");

  return(last);
}


/*!
 * Sets the linewidth of drawing strokes in the postscript \p file
 *
 * \param file File pointer to postscript output e.g. \c stdout or file
 * \param width Line width of strokes.
 */
void ps_set_linewidth(FILE* file, double width)
{
  fprintf(file, " %.1f setlinewidth stroke\n", width);
}
