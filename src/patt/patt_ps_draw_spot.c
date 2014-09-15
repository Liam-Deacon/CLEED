#include <stdio.h>
#include <strings.h>

int ps_draw_spot(FILE *out_stream, float x, float y,
		float spot_size, int shape, char *color, char *fill)
/*
 draw spots in different shapes:
  0: circle
  1: square
  2: triangle
*/
{
int delta;

 switch(shape)
 {
  case(0):
  {
	 fprintf(out_stream,
		" %.1f %.1f %.1f 0 360 arc",
		x, y, spot_size);
	 if (strncmp(fill, "fill", 4) == 0) 
	   fprintf(out_stream," %s %s", color, fill);
	 fprintf(out_stream," stroke\n");
   break;
  }  
  case(1):
  {
/* square */
   spot_size /= 1.4;
   fprintf(out_stream, "newpath\n");
   fprintf(out_stream,"%.1f %.1f moveto %.1f %.1f lineto \n",
   x-spot_size, y-spot_size, x-spot_size, y+spot_size);
   fprintf(out_stream, "%.1f %.1f lineto %.1f %.1f lineto\n",
   x+spot_size, y+spot_size, x+spot_size, y-spot_size);
   fprintf(out_stream, "closepath");
   if (strncmp(fill, "fill", 4) == 0) {
     fprintf(out_stream, " %s %s", color, fill);
   }
   fprintf(out_stream, " stroke\n");
   break;
  } 
  case(2):
  {
/* up triangle */
    delta = spot_size * 0.866;
    fprintf(out_stream, "newpath\n");
    fprintf(out_stream,"%.1f %.1f moveto %.1f %.1f lineto \n",
		x, y + spot_size, x - delta, y - (0.5* spot_size));
    fprintf(out_stream, "%.1f %.1f lineto \n closepath",
		x + delta, y - (0.5* spot_size));
    if (strncmp(fill, "fill", 4) == 0 )
		fprintf(out_stream, " %s %s", color, fill);
    fprintf(out_stream, " stroke\n");
    break;
  }
  case(3):
  {
/* down triangle */
    delta = spot_size * 0.866;
    fprintf(out_stream, "newpath\n");
    fprintf(out_stream,"%.1f %.1f moveto %.1f %.1f lineto \n",
		x, y - spot_size, x - delta, y + (0.5* spot_size));
    fprintf(out_stream, "%.1f %.1f lineto \n closepath",
		x + delta, y + (0.5 * spot_size));
	if (strncmp(fill, "fill", 4) == 0 )
		fprintf(out_stream, " %s %s", color, fill);
    fprintf(out_stream, " stroke\n");
   break;
  }
  case(4):
  {
/* diamond */
    fprintf(out_stream, "newpath\n");
    fprintf(out_stream,"%.1f %.1f moveto %.1f %.1f lineto \n",
		x-spot_size, y, x, y+spot_size);
    fprintf(out_stream, "%.1f %.1f lineto %.1f %.1f lineto\n",
		x+spot_size, y, x, y-spot_size);
    fprintf(out_stream, " closepath");
	if (strncmp(fill, "fill", 4) == 0 )
		fprintf(out_stream, " %s %s", color, fill);
    fprintf(out_stream, " stroke\n");
    break;
  }
  case(5):
  {
/* hexagon */
    delta = spot_size * 0.866; 
    fprintf(out_stream, "newpath\n");
    fprintf(out_stream,"%.1f %.1f moveto %.1f %.1f lineto \n",
		x, y - spot_size, x - delta, y - (0.5*spot_size));
    fprintf(out_stream, "%.1f %.1f lineto %.1f %.1f lineto\n",
		x - delta, y + (0.5*spot_size), x, y + spot_size);
    fprintf(out_stream, "%.1f %.1f lineto %.1f %.1f lineto \n closepath",
		x + delta, y + (0.5*spot_size), x + delta, y - (0.5*spot_size)); 
	if (strncmp(fill, "fill", 4) == 0 )
		fprintf(out_stream, " %s %s", color, fill);
    fprintf(out_stream, " stroke\n");
   break; 
  }
 }  /* switch */
 return 0;
}