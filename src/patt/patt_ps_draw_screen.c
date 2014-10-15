#include <stdio.h>
#include "patt.h"

/* draw screen */
int patt_draw_ps_screen(FILE *file_ptr, const patt_screen *screen)
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

	return(PATT_SUCCESS);
}
