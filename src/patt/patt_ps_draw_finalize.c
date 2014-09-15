#include <stdio.h>
#include "patt.h"

/* final drawing strokes */ 
int ps_draw_finalize(FILE *out_stream, int gun_flag, float gun_thickness,
	float gun_radians, float screen_thickness, int screen_flag,
	int vectors_flag, char *vectors_str, int ifiles, int ev_flag,
	float eV, char *title_str)
{
  int ii;
  
  fprintf(out_stream, "\n"
    "%%======================================================================="
    "\n\n");
    
  if (gun_flag)
   ps_draw_gun(out_stream, gun_thickness*0.5, gun_thickness*3,
			gun_radians, screen_thickness, screen_flag);
  if (vectors_flag)
	fprintf(out_stream, "%s\n", vectors_str);
  if (screen_flag)
	ps_draw_screen(out_stream, screen_thickness, 0);
  
  fprintf(out_stream,"grestore\n");
  fprintf(out_stream,"%%Print Titles\n");
  for (ii=0;ii<ifiles;ii++)
  {
	fprintf(out_stream,"Title%i ",ii);
  }
  fprintf(out_stream,"\n");
  if (ev_flag)
    fprintf(out_stream, "add_eV\n");
  if (*title_str != '\0')
	fprintf(out_stream, "title\n");
  fprintf(out_stream,"footnote\n"); 
  fprintf(out_stream,"\nshowpage\n"); 
  fprintf(out_stream,"%%EOF");
  return 0;
}
