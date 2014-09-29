#include <stdio.h>
#include <strings.h>
#include <cairo.h>
#include <math.h>
#include "spots.h"
#include "patt.h"

int patt_draw(const drawing_t *drawing)
{

  #ifdef _USE_CAIRO
    return(patt_draw_cairo(drawing));
  #else /* _USE_CAIRO */
    if ((drawing->format == PATT_PS_OLD) ||
        (drawing->format == PATT_UNKNOWN_FORMAT))
    {
      return(patt_draw_ps(out_stream, drawing));
    }
  #endif

  return(PATT_FORMAT_ERROR);
}

