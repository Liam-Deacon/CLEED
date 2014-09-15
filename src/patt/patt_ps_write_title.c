#include <stdio.h>
#include "patt.h"

/* write title routine to output */
int ps_draw_title(FILE *in_stream, FILE *out_stream, int ii, int ifiles,
		char *color, int *i_line_offset)
{	
  int step;
  int lineno;
 
  lineno = 0;
 
  fget_nocomm(line_buffer, in_stream, out_stream); 
 
  if (ifiles > 1)
    step = 25;
  else
    step = -25;
 
  /* read all c-comment lines and output accordingly */ 
  while (line_buffer[0] == 'c')
  { 
    fprintf(out_stream, "/Title%i {\n", ii);
    fprintf(out_stream, "/Times-Roman findfont 20 scalefont setfont\n");
    fprintf(out_stream, "%s", color);
    fprintf(out_stream, " %.1f %.1f moveto\n", 
            -MAX_RADIUS-50, MAX_RADIUS + 5 + *i_line_offset);
    *i_line_offset += step;
  
    if (!ps_print_substitute(out_stream, line_buffer+1))
      fprintf(out_stream, "(%s) show\n", line_buffer+1 );
  
    fget_nocomm(line_buffer, in_stream, out_stream); 
    fprintf(out_stream, "} def\n");
    
    lineno++;
  }
 
   if (!lineno) 
     fprintf(out_stream, "/Title%i {} def %%dummy\n", ii); 
 
  return 0;
 
}