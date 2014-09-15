/*********************************************************************
  GH/29.10.92
*********************************************************************/
#include <stdio.h>
#include "rfac.h"

int rf_ctr2out(char * ctr_file, FILE * out_stream)
{

  int i;
  char line_buffer[STRSZ];
  FILE *ctr_stream;

  ctr_stream = fopen(ctr_file, "r");

  fprintf (out_stream,"# Input control file \"%s\":\n", ctr_file);
  for (i=1; fgets(line_buffer, STRSZ, ctr_stream) != NULL; i++)
    fprintf (out_stream,"# %s", line_buffer);
  fprintf (out_stream,"#\n");

  fclose (ctr_stream);
  return(i);
}      /* end of main */
