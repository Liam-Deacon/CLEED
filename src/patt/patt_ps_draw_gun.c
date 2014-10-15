#include <stdio.h>
#include <math.h>
#include "patt.h"


/* draw electron gun */
int patt_draw_ps_gun(FILE *file_ptr, const patt_gun *gun)
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

	return(PATT_SUCCESS);
}
