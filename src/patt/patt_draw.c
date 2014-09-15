#include <stdio.h>
#include <strings.h>
#include <cairo.h>
#include <math.h>
#include "spots.h"

void patt_draw_cairo_spot(cairo_t *cr, spot_t spot, 
                          double spot_size, patt_shapes_t shape)
/*
 draw spots in different shapes:
  0: circle
  1: square
  2: triangle
*/
  
{
  double x = spot.x, y = spot.y;
  int delta;

  switch(shape)
  {
    case(PATT_CIRCLE):
    {
	  cairo_arc(cr, x, y, spot_size, 0, 2 * M_PI);
      break;
    }  
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
    cairo_line_to(cr, x+spot_size, y+spot_size);
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
    cairo_line_to(cr, x, x - delta, y + (0.5* spot_size));
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
 }  /* switch */
 return 0;
}

void patt_draw_cairo_label(cairo_t *cr, spot_t *spot)
{
  cairo_text_extents_t extents;
  cairo_text_extents(cr, spot->label, &extents);
  cairo_move_to(cr, spot.x - extents.width/2, spot.y + extents.height/2);
  cairo_show_text(spot->label);
}