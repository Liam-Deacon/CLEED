#include <stdio.h>
#include "patt.h"

/* write initial lines to output */
int patt_draw_ps_init(FILE *file_ptr, const patt_drawing *drawing)
{

  fprintf(file_ptr, "%%!PS-Adobe-3.0 \n");
  fprintf(file_ptr, "%%Creator: %s\n", PATT_PROG_VERSION);
  fprintf(file_ptr,
    "%%BoundingBox: 50 150 550 650 \n"
    "%%EndComments \n"
    "%%======================================================================="
    "\n%%Initial setup:\n"
    "%%======================================================================="
    "\n");

  if (drawing->n_files > 1)
  {
    fprintf(file_ptr, "%%%%Page: %s %i\n", "Composite_Input", 1);
  }
  else
  {
    fprintf(file_ptr, "%%%%Page: %s %i\n", "Single_Input", 1);
  }

  fprintf(file_ptr, " %.1f %.1f translate gsave %%center of page as origin\n",
          OFF_W, OFF_H);

  if (drawing->title.label[0] != '\0')
  {
    fprintf(file_ptr, "/title {%%add custom title\n/Times-Bold findfont "
            "%.1f scalefont setfont\n", drawing->title.size);
    fprintf(file_ptr, "%s %.1f %.1f moveto (%s) show\n} def\n",
            patt_color_get_ps_string(&PATT_BLACK),
            drawing->title.x, drawing->title.y, drawing->title.label);
  }
  fprintf(file_ptr, "/footnote {%%add page footnote\n");
  fprintf(file_ptr, "/Times-Roman findfont %.1f scalefont setfont\n",
         drawing->title.size);
  fprintf(file_ptr, "%s %.1f %.1f moveto (%s) show\n} def\n",
          patt_color_get_ps_string(&PATT_BLACK),
          drawing->footnote.x, drawing->footnote.y, drawing->footnote.label);
  if (drawing->screen.visible)
  {
    patt_draw_ps_screen(file_ptr, &drawing->screen);
  }

  if (drawing->eV.visible)
  {
    fprintf(file_ptr,"/add_eV {%%add electron energy label\n");
    fprintf(file_ptr, "/Times-ItalicBold findfont %f scalefont setfont\n",
            drawing->eV.size);
    fprintf(file_ptr,"%s %.1f %.1f moveto (%.0feV) show\n} def\n",
            patt_color_get_ps_string(&PATT_BLACK), drawing->eV.x,
            drawing->eV.y, atof(drawing->eV.label));
  }
  if (drawing->screen.clip)
  {
    fprintf(file_ptr, "%s %.1f 0 moveto 0 0 %.1f 0 360 arc clip\n",
            patt_color_get_ps_string(&PATT_WHITE),
            MAX_RADIUS+(drawing->screen.stroke_width-1),
            MAX_RADIUS+(drawing->screen.stroke_width-1));
    fprintf(file_ptr, "0. setlinewidth 1 1 1 setrgbcolor 204.0 0"
            " moveto 0 0 %.1f 0 360 arc stroke\n",
            MAX_RADIUS+(drawing->screen.stroke_width-1));
  }
  if (drawing->show_vectors) patt_draw_ps_vector_funcs(file_ptr);

  return(PATT_SUCCESS);
}
