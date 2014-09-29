/*========================================================================*/

#include <stdio.h>
#include <strings.h>
#include "patt.h"

/* Search string & convert to postscript where needed */ 
int ps_print_substitute(FILE *output, char *str)
{
  int i, j, k, last;
  char substr[STRSZ];
  char prestr[STRSZ];
  char poststr[STRSZ];
  strcpy(poststr, str);

    i = j = k = 0;
    
  if (str[0] == '\0') return(0);

  /* printf("%s\n", str); */
  last=0;
  for (i=0; i<=(int)strlen(str)-1;i++)
  {
    if (str[i] == '$' && str[i+1] == '{' )
    {
      i++; j=0;
      strcpy(substr, "\0");
      strcpy(prestr, "\0");
      fprintf(output,"/Times-Roman findfont 20 scalefont setfont (");
      while (str[i] != '}' && i<=STRSZ-2)
      {
        i++;
        substr[j] = str[i];
        j++;
      }
      /* printf("%i %i %i\n", i, j, last); */

      for (k=last;k<=i-j-2;k++)
      {
       if (str[k] == '(' || str[k] == ')' ||
         str[k] == '{'  || str[k] == '}' )
          fprintf(output, "%c", '\\' );
       if (str[k] == '\0') break;
       fprintf(output,"%c", str[k]);
       prestr[k] = str[k];
      }

      last = i+1;
      fprintf(output, ") show\n");
      for (k=0; k<=NUM_SUBS-1;k++)
      {
        /* if (strncmp(substr, substitutes[k][0], j-1) == 0)
        {
          fprintf(output, "/Symbol findfont 20 scalefont setfont (%s) show\n",
                  substitutes[k][1]);
        } /* if */
      } /* for k */
    } /* if */
	} /* for i */

  fprintf(output, "/Times-Roman findfont 20 scalefont setfont (");
	
  for (k=last+j; k<=STRSZ-1;k++)
	{
	  if ((str+k == NULL) || (str[k] == '\0')) break; 
	  fprintf(output, "%c", str[k]);
  }

  fprintf(output,") show\n");

  return(last);
}
