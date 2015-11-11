/************************************************************************
 *                        CAOI_LEED_BSR.C
 *
 *  Copyright 2008-2014 Zhasmina Zheleeva
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   ZZ/2008.03.03 - creation
 *   LD/2014.10.09 - changed number, i_ang & length to size_t
 *********************************************************************/

/*! \file caoi_leed_bsr.c
 *
 * Implementation file for function bsrinp() which makes \c sa
 * ia_i.bsr files for each angle of incidence.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "caoi_leed.h"

/*!
 * Makes sa ia_i.bsr files for each angle of incidence.
 *
 * \param filebsr Root filename to which \p i_ang & .bsr are appended.
 *
 * \warning exits on failure.
 *
 */
void bsrinp(char *filebsr, size_t number)
{
  size_t i_ang;
  size_t num;
  size_t length = 0;

  float ip, it;

  char linebuffer[STRSZ] = "";
  char helpstring[STRSZ] = "";

  FILE *readstream = NULL;
  FILE *writestream = NULL;

  CONTROL_MSG(CONTROL, "sa: %u\n", number);
  ERROR_MSG("%s\n", filebsr);

/***********************************************************************
 Open .bsr file for reading
***********************************************************************/


  if ((readstream = fopen (filebsr, "r")) == NULL)
  {
    ERROR_MSG("can not open file '%s'\n", filebsr);
    exit(EIO);
  }


/***********************************************************************
 Makes sa ia_i.bsr files for each angle of incidence
***********************************************************************/

  if (strlen(filebsr) > 4) length = strlen(filebsr) - 4;
  ERROR_MSG("length = %d\n", length);

  for (i_ang = 0; i_ang < number; i_ang ++)
  {
    ERROR_MSG("i_ang: %u\n", i_ang);
    rewind(readstream);
    strncpy(linebuffer, filebsr, length);
    ERROR_MSG("%s\n", linebuffer);
    sprintf(linebuffer + length, "ia_%u.bsr", i_ang + 1);
    ERROR_MSG("%s\n", linebuffer);

    if ((writestream = fopen(linebuffer, "w")) == NULL)
    {
      ERROR_MSG("can not open file '%s'\n", linebuffer);
      exit(EIO);
    }

    while (fgets(helpstring, STRSZ, readstream))
    {
      if (!strncasecmp(helpstring, "ipt:", 4))
      {
        sscanf(helpstring + 4, " %u", &num);
 
        if (num == (i_ang + 1))
        {
          sscanf(helpstring+4, " %u %f %f", &num, &ip, &it);
          fprintf(writestream, "ip: %.2f\nit: %.2f\n", ip, it);
        }
      }

      else fprintf(writestream, "%s", helpstring);

    } /*while helpstring*/

    fclose(writestream);

  } /*for i_ang*/

 fclose(readstream);

} /*bsrinp*/
