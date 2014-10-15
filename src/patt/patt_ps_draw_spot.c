#include "patt.h"
#include <stdio.h>
#include <strings.h>
#include <stdbool.h>

const char *patt_color_get_ps_string(const patt_color_rgb *color)
{
  char *str = (char*) malloc(sizeof(char)* 1024);
  sprintf(str, "%.6e %.6e %.6e setrgbcolor",
          color->red, color->blue, color->green);
  return(str);
}

int patt_draw_ps_spot(FILE *file_ptr, spot *spot, double spot_size,
                      patt_shape shape, const patt_color_rgb *color, bool fill)
/*
 draw spots in different shapes:
  0: circle
  1: square
  2: triangle
*/
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


  return(PATT_SUCCESS);
}
