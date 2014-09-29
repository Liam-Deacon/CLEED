#include <stdio.h>
#include "patt.h"

/* final drawing strokes */ 
int patt_draw_ps_finish(FILE *file_ptr, const drawing_t *drawing)
{
  size_t ii;
  
  fprintf(file_ptr, "\n"
    "%%======================================================================="
    "\n\n");
    
  if (drawing->gun.visible)
  {
    patt_draw_ps_gun(file_ptr, &drawing->gun);
  }
  //if (drawing->show_vectors) fprintf(file_ptr, "%s\n", vectors_str);
  if (drawing->screen.visible)
  {
    patt_draw_ps_screen(file_ptr, &drawing->screen);
  }

  fprintf(file_ptr, "grestore\n");
  fprintf(file_ptr, "%%Print Titles\n");
  for (ii=0; ii<drawing->n_files; ii++)
  {
    fprintf(file_ptr, "Title%i ", ii);
  }
  fprintf(file_ptr, "\n");

  if (drawing->eV.visible) fprintf(file_ptr, "add_eV\n");
  if (drawing->title.label[0] != '\0') fprintf(file_ptr, "title\n");

  fprintf(file_ptr, "footnote\n");
  fprintf(file_ptr, "\nshowpage\n");
  fprintf(file_ptr, "%%EOF");

  return(PATT_SUCCESS);
}
