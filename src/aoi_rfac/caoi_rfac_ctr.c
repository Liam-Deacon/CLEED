/*********************************************************************
ZZ/03.03.08
  file contains functions:

  ctrinp (03.03.08)
     makes .ctr file for each angle of incidence

Changes:
  LD/18.09.2014 - changed lengthctr, ictr and num to type size_t from int

*********************************************************************/

#include "caoi_rfac.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*======================================================================*/

int ctrinp(char *filectr)
{
  char stringctr[STRSIZE];
  char workstr[STRSIZE];

  FILE *fpf;
  FILE *fp;
  size_t lenghtctr = 0;
  size_t ictr;
  size_t num;

  strncpy(stringctr, filectr, STRSIZE);

  if ((fp = fopen(filectr,"r")) == NULL)
  {
	#ifdef ERROR
      fprintf(STDERR, "*** error (caoi_rfac_ctr): "
    		  "could not open output file \"%s\"\n", filectr);
	#endif
    exit(1);
  }

  if (strlen(filectr) > 3) lenghtctr = strlen(filectr) - 4;

/*****************************************************************************
 For N-th  angle pair makes ....ia_N.ctr
******************************************************************************/

  for (ictr = 0; ictr < sa; ictr++)
  {
    rewind(fp);
    strncpy(stringctr, filectr ,STRSIZE);
    sprintf(stringctr+lenghtctr, "ia_%u.ctr", ictr+1);

    if ((fpf=fopen(stringctr,"w")) == NULL)
    {
	  #ifdef ERROR
    	fprintf(STDERR, "*** error (caoi_rfac_ctr): "
    			"could not open output file \"%s\"\n", stringctr);
	  #endif
      exit(1);
    }

    while (fgets(stringctr, STRSIZE, fp))
    {
      if (!strncasecmp(stringctr, "ia:", 3))
        {
          sscanf(stringctr+3, " %u ", &num);

          if ((ictr+1) == num)
          {
            sscanf(stringctr+3, " %u %s", &num,workstr);
            fprintf(fpf, "%s\n", workstr);
          }

        }

      else
      {
        fprintf(fpf,"%s",stringctr);
      }

    } /*while*/

    fclose(fpf); 

  } /*for*/

  fclose(fp);
  
  return(0);
}
