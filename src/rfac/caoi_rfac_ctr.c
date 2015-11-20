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

#include "rfac.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

int rfac_create_control_file_duplicates(const char *control_filename, size_t n)
{
  char fname_ctr[FILENAME_MAX] = "";
  char string_ctr[STRSZ] = "";
  char workstr[STRSZ] = "";

  FILE *fpf = NULL;
  FILE *fp = NULL;
  const size_t length_max = sizeof(string_ctr)-1;
  size_t length_ctr = 0;
  size_t ictr = 0;
  size_t num = 0;

  strncpy(string_ctr, control_filename, length_max);
  string_ctr[length_max] = '\0';

  if ((fp = fopen(control_filename, "r")) == NULL)
  {
    ERROR_MSG("could not open output file \"%s\"\n", control_filename);
    exit(EIO);
  }

  if (strlen(control_filename) > 3)
    length_ctr = strlen(control_filename) - 4;

  /*****************************************************************************
   For N-th  angle pair makes ....ia_N.ctr
   ******************************************************************************/

  for (ictr = 0; ictr < n; ictr++)
  {
    rewind(fp);
    strncpy(fname_ctr, control_filename, sizeof(fname_ctr)-1);
    fname_ctr[sizeof(fname_ctr)-1] = '\0';

    snprintf(workstr, sizeof(workstr), "ia_%u.ctr", ictr);
    if (length_ctr + strlen(workstr) >= sizeof(fname_ctr))
    {
      ERROR_MSG("control filename too long\n");
      exit(ENAMETOOLONG);
    }
    snprintf(fname_ctr+length_ctr,
             sizeof(fname_ctr)-length_ctr,
             "ia_%u.ctr", ictr + 1);

    if ((fpf = fopen(fname_ctr, "w")) == NULL)
    {
      ERROR_MSG("could not open output file \"%s\"\n", fname_ctr);
      exit(EIO);
    }

    while (fgets(string_ctr, STRSZ, fp))
    {
      if (!strncasecmp(string_ctr, "ia:", 3))
      {
        sscanf(string_ctr + 3, " %u ", &num);

        if ((ictr + 1) == num)
        {
          sscanf(string_ctr + 3, " %u %s", &num, workstr);
          fprintf(fpf, "%s\n", workstr);
        }
      }
      else
      {
        fprintf(fpf, "%s", string_ctr);
      }

    } /*while*/

    fclose(fpf);

  } /*for*/

  fclose(fp);

  return (0);
}
