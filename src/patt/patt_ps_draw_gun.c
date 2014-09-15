#include <stdio.h>
#include <math.h>
#include "patt.h"


/* draw electron gun */
int ps_draw_gun(FILE *out_stream, float gun_thickness, float gun_radius, 
			float position_radians, float edge_thickness, int screen_flag)
{

	fprintf(out_stream,"%% Electron gun\n");
	fprintf(out_stream,"%s\n", GUN_COLOR);
	fprintf(out_stream,"newpath\n%.1f %.1f moveto %.1f setlinewidth\n", 
	  gun_thickness, -gun_thickness, gun_thickness);
	fprintf(out_stream,
      "%.1f %.1f lineto %.1f %.1f lineto\n%.1f %.1f lineto\n",
	  -(1.005*MAX_RADIUS*cos(3.14182*(position_radians+0.035))),
	  -(1.005*MAX_RADIUS*sin(3.14182*(position_radians+0.035))), 
	  -(1.005*MAX_RADIUS*cos(3.14182*(position_radians-0.035))),
	  -(1.005*MAX_RADIUS*sin(3.14182*(position_radians-0.035))),
	  -gun_thickness, gun_thickness);
	fprintf(out_stream,"closepath\n%s fill stroke\n", GUN_COLOR);
	fprintf(out_stream,"0 0 %.1f 0 360 arc 0 0 0 setrgbcolor fill stroke\n",
	  gun_radius);

    /*!TODO: use edge_thickness & screen_flag */

	return 0;
}