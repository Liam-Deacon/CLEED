#include <stdio.h>

void ps_set_linewidth(FILE* file, double width)
{
  fprintf(file, " %.1f setlinewidth stroke\n", width);
}
