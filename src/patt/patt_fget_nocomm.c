/*========================================================================*/
#include <stdio.h>
#include "patt.h"

char *fget_nocomm(char *buffer, FILE *in_stream, FILE *out_stream)
/* 
 Lines beginning with '#' are interpreted as comments. 
*/
{
  while( (fgets(buffer, STRSZ, in_stream) != NULL) && (*buffer == '#') )
  {
   printf ("%s", buffer);
   fprintf(out_stream,"%s %s","%", buffer);
  }
  if (buffer == NULL) printf("***input error");
 return(buffer);
}