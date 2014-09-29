/*
 * patt_draw_cairo.c
 *
 *  Created on: 24 Sep 2014
 *      Author: Liam Deacon
 */

#include "patt.h"
#include "pattern.h"
#include <cairo.h>
#include <cairo-ps.h>
#include <cairo-pdf.h>
#include <cairo-svg.h>
#include <string.h>
#include <math.h>

int patt_draw_cairo(const drawing_t *drawing)
{
  cairo_surface_t *surface;
  cairo_t *cr;
  size_t i_file, i_spot, i_domain;
  patt_color_rgb_t color = PATT_BLACK;
  pattern_t *pat = NULL;
  FILE *f = NULL;

  char *copyright = (char*) malloc((BUFSIZ)*sizeof(char));
  char *tok = NULL;
  char str_buf[BUFSIZ];

  strcpy(copyright, PROG_COPYRIGHT);
  tok = strtok(copyright, "\n");

  /* create surface */
  switch (drawing->format)
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
      if (drawing->format == PATT_EPS) cairo_ps_surface_set_eps(surface, true);

      /*! COMMENTS */

      /* title */
      sprintf(str_buf, "%%Title: %s", drawing->title.label);
      cairo_ps_surface_dsc_comment(surface, str_buf);

      /* copyright */
      while(tok)
      {
        sprintf(str_buf, "%%Copyright: %s", tok);
        tok = strtok(NULL, "\n");
        cairo_ps_surface_dsc_comment(surface, str_buf);
      }

      /* program info */
      sprintf(str_buf, "%%Creator: %s - Version %s", PROG, PROG_VERSION);
      cairo_ps_surface_dsc_comment(surface, str_buf);

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
    spots_t *gs_spots = pattern_calculate_substrate_spots(pat);

    switch (drawing->color_scheme)
    {
      case PATT_GRAYSCALE_SCHEME:
        color = grays[i_file % NUM_GRAYS];
        break;
      case PATT_COLOR_SCHEME:
        patt_color_copy(&color, patt_color_from_name(
                                       colors[i_file % NUM_COLORS][SPOT_GS]));
        break;
      case PATT_BLACK_SCHEME: default:
        color = PATT_BLACK;
        break;
    }

    /* set spot properties */
    patt_color_copy(&gs_spots->color, &color);
    cairo_set_source_rgb(cr, color.red, color.green, color.blue);
    cairo_set_line_width(cr, gs_spots->stroke_width);

    for (i_spot = 0; i_spot < gs_spots->n_spots; i_spot++)
    {
      /*! SETUP */

      /*! DRAW HERE */
      patt_draw_cairo_spot(cr, &gs_spots->spots[i_spot],
                           gs_spots->radius, spots_get_shape(gs_spots));
      if (gs_spots->fill == true) cairo_fill(cr);
      if (gs_spots->indexing == true)
      {
        patt_draw_cairo_label(cr, &gs_spots->spots[i_spot]);
      }

    }

    /* draw superstructure spots */
    for (i_domain = 0; i_domain < pat->n_domains; i_domain++)
    {
      spots_t *ss_spots = pattern_calculate_superstructure_spots(pat, i_domain);

      cairo_set_line_width(cr, ss_spots->stroke_width);
      cairo_set_font_size(cr, abs(ss_spots->font_size));

      patt_color_copy(&ss_spots->color, &gs_spots->color);
      spots_set_shape(ss_spots, i_domain);
      ss_spots->font_size = 12;
      ss_spots->radius = RADIUS_SS;

      for (i_spot = 0; i_spot < ss_spots->n_spots; i_spot++)
      {
        /*! DRAW HERE */
        patt_draw_cairo_spot(cr, &ss_spots->spots[i_spot],
                           ss_spots->radius, spots_get_shape(ss_spots));
        if (gs_spots->fill == true) cairo_fill(cr);
        if (gs_spots->indexing == true)
        {
          patt_draw_cairo_label(cr, &ss_spots->spots[i_spot]);
        }
      }

      spots_free(ss_spots);
    }

    /* clean up */
    spots_free(gs_spots);
    pattern_free(pat);
    fclose(f);

  } /* for i_file */

  /*! EXTRAS */

  /* eV */
  if (drawing->eV.visible)
  {
    patt_draw_cairo_text(cr, &drawing->eV, CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_NORMAL);
  }

  /* screen */
  if (drawing->screen.visible) patt_draw_cairo_screen(cr, &drawing->screen);

  /* electron gun */
  if (drawing->gun.visible) patt_draw_cairo_gun(cr, &drawing->gun);

  if (drawing->footnote.visible)
  {
    patt_draw_cairo_text(cr, &drawing->footnote, CAIRO_FONT_SLANT_NORMAL,
                        CAIRO_FONT_WEIGHT_NORMAL);
  }

  if (drawing->title.visible)
  {
    patt_draw_cairo_text(cr, &drawing->title, CAIRO_FONT_SLANT_NORMAL,
                        CAIRO_FONT_WEIGHT_BOLD);
  }



  /*! FINISH */
  switch (drawing->format)
  {
    case PATT_PNG:
    {
      cairo_surface_write_to_png(surface, drawing->output_filename);
      break;
    }

    case PATT_SVG:
    case PATT_PDF:
    case PATT_EPS:
    case PATT_PS:
    default:
    {
      cairo_show_page(cr);
      cairo_surface_flush(surface);
      break;
    }

  }

  /* clean up */
  cairo_destroy(cr);
  cairo_surface_finish(surface);
  cairo_surface_destroy(surface);

  free(tok);
  free(copyright);

  return(PATT_SUCCESS);
}

void patt_draw_cairo_spot(cairo_t *cr, spot_t *spot,
                          double spot_size, patt_shape_t shape)
/*
 draw spots in different shapes:
  0: circle
  1: square
  2: triangle
*/

{
  double x = spot->x, y = spot->y;
  int delta;

  switch(shape)
  {
    case(PATT_SQUARE):
    {
      /* square */
      spot_size /= 1.4;
      cairo_move_to(cr, x, y);
      cairo_rectangle(cr, x - spot_size, y - spot_size,
                          x + spot_size, y + spot_size);
      break;
    }
    case(PATT_TRIANGLE_UP):
    {
      /* up triangle */
      delta = spot_size * 0.866;
      cairo_new_path(cr);
      cairo_move_to(cr, x, y + spot_size);
      cairo_line_to(cr, x - delta, y - (0.5* spot_size));
      cairo_line_to(cr, x + spot_size, y + spot_size);
      cairo_line_to(cr, x + delta, y - (0.5* spot_size));
      cairo_close_path(cr);
      break;
    }
    case(PATT_TRIANGLE_DOWN):
    {
      /* down triangle */
      delta = spot_size * 0.866;
      cairo_new_path(cr);
      cairo_move_to(cr, x, y - spot_size);
      cairo_line_to(cr, x - delta, y + (0.5* spot_size));
      cairo_line_to(cr, x + delta, y + (0.5 * spot_size));
      cairo_close_path(cr);
      break;
    }
    case(PATT_DIAMOND):
    {
      /* diamond */
      cairo_new_path(cr);
      cairo_move_to(cr, x - spot_size, y);
      cairo_line_to(cr, x, y + spot_size);
      cairo_line_to(cr, x + spot_size, y);
      cairo_line_to(cr, x, y - spot_size);
      cairo_close_path(cr);
      break;
    }
    case(PATT_HEXAGON):
    {
      /* hexagon */
      delta = spot_size * 0.866;
      cairo_new_path(cr);
      cairo_move_to(cr, x, y - spot_size);
      cairo_line_to(cr, x - delta, y - (0.5*spot_size));
      cairo_line_to(cr, x - delta, y + (0.5*spot_size));
      cairo_line_to(cr, x, y + spot_size);
      cairo_line_to(cr, x + delta, y + (0.5*spot_size));
      cairo_line_to(cr, x + delta, y - (0.5*spot_size));
      cairo_close_path(cr);
      break;
    }
    case(PATT_CIRCLE): default:
    {
      cairo_arc(cr, x, y, spot_size, 0, 2 * M_PI);
      break;
    }
  }  /* switch */
}

void patt_draw_cairo_label(cairo_t *cr, const spot_t *spot)
{
  cairo_text_extents_t extents;
  cairo_text_extents(cr, spot->label, &extents);
  cairo_move_to(cr, spot->x - extents.width/2, spot->y + extents.height/2);
  cairo_show_text(cr, spot->label);
}


void patt_draw_cairo_arrow(cairo_t *cr, double x1, double y1, double x2,
                           double y2, double stroke_width, double head_size)
{
  double angle = atan2 (y2 - y1, x2 - x1) + M_PI;
  double arrow_length = sqrt((x2-x1)*(x2-x1)  + (y2-y1)*(y2-y1));
  double head_x1 = x2 + arrow_length * cos(angle - ARROW_ANGLE);
  double head_y1 = y2 + arrow_length * sin(angle - ARROW_ANGLE);
  double head_x2 = x2 + arrow_length * cos(angle + ARROW_ANGLE);
  double head_y2 = y2 + arrow_length * sin(angle + ARROW_ANGLE);

  double original_width = cairo_get_line_width(cr);

  cairo_set_line_width(cr, stroke_width);
  cairo_move_to(cr, x1, y1);
  cairo_line_to(cr, x2, y2);
  cairo_line_to(cr, head_x1, head_y1);
  cairo_line_to(cr, head_x2, head_y2);
  cairo_close_path(cr);

  //cairo_set_source_rgb(cr, );
  cairo_stroke_preserve(cr);

  //cairo_set_source_rgb(cr, );
  cairo_fill(cr);

  /* restore */
  cairo_set_line_width(cr, original_width);

}

void patt_draw_cairo_gun(cairo_t *cr, const gun_t *gun)
{
  patt_color_rgb_t color = gun->color;

  cairo_set_source_rgba(cr, color.red, color.blue, color.green, gun->alpha);
  cairo_set_line_width(cr, gun->stroke_width);
  cairo_move_to(cr, gun->stroke_width, -gun->stroke_width);
  cairo_line_to(cr, -(1.005*MAX_RADIUS*cos(3.14182*(gun->angle+0.035))),
                    -(1.005*MAX_RADIUS*sin(3.14182*(gun->angle+0.035))));
  cairo_line_to(cr, -(1.005*MAX_RADIUS*cos(3.14182*(gun->angle-0.035))),
                    -(1.005*MAX_RADIUS*sin(3.14182*(gun->angle-0.035))));
  cairo_line_to(cr, -gun->stroke_width, gun->stroke_width);
  cairo_close_path(cr);
  cairo_set_source_rgba(cr, color.red, color.blue, color.green, gun->alpha);
  cairo_fill(cr);
  cairo_set_source_rgba(cr, color.red, color.blue, color.green, gun->alpha);
  cairo_arc(cr, 0., 0., gun->radius, 0., 2*M_PI);
  cairo_stroke(cr);
  cairo_fill(cr);
}

void patt_draw_cairo_screen(cairo_t *cr, const screen_t *screen)
{
  cairo_move_to(cr, 0., 0.);
  cairo_set_line_width(cr, screen->stroke_width);
  cairo_set_source_rgba(cr, screen->stroke_color.red, screen->stroke_color.blue,
                        screen->stroke_color.green, screen->alpha);
  cairo_arc(cr, 0., 0., MAX_RADIUS+(screen->stroke_width*0.5), 0., 2*M_PI);
  cairo_stroke(cr);

  if(screen->fill)
  {
    cairo_arc(cr, 0., 0., MAX_RADIUS+(screen->stroke_width*0.5), 0., 2*M_PI);
    cairo_set_source_rgba(cr, screen->fill_color.red, screen->fill_color.blue,
                          screen->fill_color.green, screen->alpha);
    cairo_stroke(cr);
    cairo_fill(cr);
  }
}

void patt_draw_cairo_text(cairo_t *cr, const text_t *text,
                        cairo_font_slant_t slant, cairo_font_weight_t weight)
{
  cairo_save(cr);
  cairo_move_to(cr, text->x, text->y);
  cairo_set_source_rgba(cr, text->color.red, text->color.blue,
                        text->color.green, text->alpha);
  cairo_select_font_face(cr, "Serif", slant, weight);
  cairo_set_font_size(cr, text->size);
  cairo_show_text(cr, text->label);
  cairo_restore(cr);
}
