/************************************************************************
 *                        CAOI_RFAC_CTR.C
 *
 *  Copyright 2008-2014 Zhasmina Zheleeva
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Description:  file contains functions
 *  ctrinp()
 *    Makes .ctr file for each angle of incidence
 *
 * Changes:
 *   ZZ/2008.03.03 - creation
 *   LD/18.09.2014 - changed lengthctr, ictr & num to size_t from int
 *********************************************************************/

#include "caoi_rfac.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int ctrinp(const char *filectr)
{
  char stringctr[FILENAME_MAX];
  char workstr[STRSZ];

  FILE *fpf;
  FILE *fp;
  size_t lenghtctr = 0;
  size_t ictr;
  size_t num;

  strncpy(stringctr, filectr, STRSZ);

  if ((fp = fopen(filectr, "r")) == NULL)
  {
    ERROR_MSG("could not open output file \"%s\"\n", filectr);
    exit(1);
  }

  if (strlen(filectr) > 3) lenghtctr = strlen(filectr) - 4;

/*****************************************************************************
 For N-th  angle pair makes ....ia_N.ctr
******************************************************************************/

  for (ictr = 0; ictr < sa; ictr++)
  {
    rewind(fp);
    strncpy(stringctr, filectr, FILENAME_MAX);
    sprintf(stringctr+lenghtctr, "ia_%u.ctr", ictr+1);

    if ((fpf=fopen(stringctr,"w")) == NULL)
    {
      ERROR_MSG("could not open output file \"%s\"\n", stringctr);
      exit(1);
    }

    while (fgets(stringctr, STRSZ, fp))
    {
      if (!strncasecmp(stringctr, "ia:", 3))
        {
          sscanf(stringctr+3, " %u ", &num);

          if ((ictr+1) == num)
          {
            sscanf(stringctr+3, " %u %s", &num, workstr);
            fprintf(fpf, "%s\n", workstr);
          }

        }

      else
      {
        fprintf(fpf, "%s", stringctr);
      }

    } /*while*/

    fclose(fpf); 

  } /*for*/

  fclose(fp);
  
  return(0);
}
