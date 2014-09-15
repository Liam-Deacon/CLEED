#include <stdio.h>
#include "patt.h"

/* draw screen */
int ps_draw_screen(FILE *out_stream, float edge_thickness, int fill)
{

	fprintf(out_stream,"%% Screen\n");
	fprintf(out_stream,"0 0 moveto %.1f setlinewidth stroke\n", 
        edge_thickness);
	fprintf(out_stream,"0 0 %.1f 0 360 arc ", 
        MAX_RADIUS+(edge_thickness*0.5));
	fprintf(out_stream,"%s stroke\n", EDGE_COLOR );
	if(fill)
	  fprintf(out_stream,"0 0 %.1f 0 360 arc %s fill stroke\n",
		MAX_RADIUS+(edge_thickness*0.5), SCREEN_COLOR);
	return 0;
}