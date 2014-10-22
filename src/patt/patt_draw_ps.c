/*
 * patt_draw_ps.c
 *
 *  Created on: 24 Sep 2014
 *      Author: Liam Deacon
 */

#include "patt.h"
#include <strings.h>

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

int patt_draw_ps_label(FILE *file_ptr, const spots *group, const spot *spot)
{
  fprintf(file_ptr, "%.1f %.1f moveto (%s) show stroke\n",
          spot->x - 4*group->radius, spot->y - 4*group->radius, spot->label);
  return (PATT_SUCCESS);
}
