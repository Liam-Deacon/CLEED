/************************************************************************
 *                        CAOI_RFAC_CTR.C
 *
 *  Copyright 2008-2015 Zhasmina Zheleeva
 *  Copyright 2014-2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*!
 * \file
 *
 * \author Zhasmina Zheleeva
 */

#include "rfac.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

/*!
 * Spawns @p n_ctr copies of the file given by @p control_filename and appends
 * the number followed by '.ctr' e.g. <control_filename>_<i>.ctr
 */
int rfac_create_control_file_duplicates(
    const char *control_filename,
    size_t n_ctr)
{
  char fname_ctr[FILENAME_MAX] = "";
  char fname_workstr[FILENAME_MAX] = "";
  char string_ctr[STRSZ] = "";
  char workstr[STRSZ] = "";

  FILE *fpf = NULL;
  FILE *fp = NULL;
  const size_t length_max = sizeof(string_ctr)-1;
  size_t length_ctr = 0;
  size_t i_ctr = 0;
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

  for (i_ctr = 0; i_ctr < n_ctr; i_ctr++)
  {
    rewind(fp);
    strncpy(fname_ctr, control_filename, sizeof(fname_ctr)-1);
    fname_ctr[sizeof(fname_ctr)-1] = '\0';

    snprintf(fname_workstr, sizeof(fname_workstr)-1, "ia_%u.ctr", i_ctr);
    if (length_ctr + strlen(fname_workstr) >= sizeof(fname_ctr))
    {
      ERROR_MSG("control filename too long\n");
      exit(ENAMETOOLONG);
    }
    snprintf(fname_ctr+length_ctr,
             sizeof(fname_ctr)-length_ctr,
             "ia_%u.ctr", i_ctr + 1);

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

        if ((i_ctr + 1) == num)
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
