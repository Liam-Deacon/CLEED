#include <stdio.h>
#include "patt.h"

/* write initial lines to output */
int ps_draw_init(FILE *out_stream, int ifiles, char *title_str,
	float *pos_title, char *footnote, float *pos_footnote, int screen_flag,
	float screen_thickness, int fill_screen, int ev_flag,
	float eV, float *pos_ev, int clip_flag, int vectors_flag)
{
 fprintf(out_stream,
    "%%!PS-Adobe-3.0 \n%%Creator: ");
 fprintf(out_stream, "%s\n", VERSION);
 fprintf(out_stream,
  "%%BoundingBox: 50 150 550 650 \n"
  "%%EndComments \n"
  "%%======================================================================="
  "\n%%Initial setup:\n"
  "%%======================================================================="
  "\n");

 if (ifiles-1)
   fprintf(out_stream,"%%%%Page: %s %i\n", "Composite_Input", 1); 
 else
   fprintf(out_stream,"%%%%Page: %s %i\n", "Single_Input", 1);
 fprintf(out_stream,
    " %.1f %.1f translate gsave %%center page as origin\n",
	OFF_W, OFF_H);
 if (*title_str != '\0')
 {
	fprintf(out_stream,
		"/title {%%add custom title\n"
        "/Times-Bold findfont %.1f scalefont setfont\n",
		pos_title[2]);
	fprintf(out_stream,
		"%s %.1f %.1f moveto (%s) show\n} def\n",
		PSBLACK, pos_title[0], pos_title[1], title_str);
 }
 fprintf(out_stream,"/footnote {%%add page footnote\n");
 fprintf(out_stream,"/Times-Roman findfont %.1f scalefont setfont\n",
	pos_footnote[2]);
 fprintf(out_stream,"%s %.1f %.1f moveto (%s) show\n} def\n",
   PSBLACK, pos_footnote[0], pos_footnote[1], footnote );
 if (screen_flag)
   ps_draw_screen(out_stream, screen_thickness, fill_screen);
 if (ev_flag) 
 {
 fprintf(out_stream,"/add_eV {%%add electron energy label\n"); 
 fprintf(out_stream,"/Times-ItalicBold findfont 20 scalefont setfont\n");
 fprintf(out_stream,"%s %.1f %.1f moveto (%.0feV) show\n} def\n",
	PSBLACK, pos_ev[0], pos_ev[1], eV );
 }
 if (clip_flag)
   fprintf(out_stream,"%s %.1f 0 moveto 0 0 %.1f 0 360 arc clip\n",
     PSWHITE, MAX_RADIUS+(screen_thickness-1), 
     MAX_RADIUS+(screen_thickness-1));
   fprintf(out_stream,
     "0. setlinewidth 1 1 1 setrgbcolor 204.0 0"
     " moveto 0 0 %.1f 0 360 arc stroke\n",
	 MAX_RADIUS+(screen_thickness-1));
 if(vectors_flag)
	 ps_add_vectors(out_stream);
  return 0;
}

/*========================================================================*/