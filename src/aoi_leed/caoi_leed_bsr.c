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
 * Implementation file for function \ref bsrinp which makes \ref sa
 * ia_i.bsr files for each angle of incidence.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "caoi_leed.h"

/*!
 * \fn bsrinp
 *
 * Makes sa ia_i.bsr files for each angle of incidence.
 *
 * \param *filebsr the root filename to which \p i_ang & .bsr are appended.
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

  char linebuffer[STRSIZE];
  char helpstring[STRSIZE];

  FILE *readstream;
  FILE *writestream;

  printf("*** sa: %u ***\n", number);

  #ifdef DEBUG
    fprintf(STDERR, "***%s***\n", filebsr);
  #endif

/***********************************************************************
 Open .bsr file for reading
***********************************************************************/


  if ((readstream = fopen (filebsr, "r")) == NULL)
  {
    #ifdef ERROR
      fprintf(STDERR, "*** error (caoi_leed_bsr): can not open file %s\n",
              filebsr);
    #endif
    exit(1);
  }


/***********************************************************************
 Makes sa ia_i.bsr files for each angle of incidence
***********************************************************************/

  if (strlen(filebsr) > 4) length = strlen(filebsr) - 4;
  #ifdef DEBUG
    fprintf(STDERR, "***length = %d***\n", length);
  #endif

  for (i_ang = 0; i_ang < number; i_ang ++)
  {
    fprintf(STDERR, "*** i_ang: %u ***\n", i_ang);
    rewind(readstream);
    strncpy(linebuffer, filebsr, length);
    fprintf(STDERR, "***%s***\n", linebuffer);
    sprintf(linebuffer + length, "ia_%u.bsr", i_ang + 1);
    fprintf(STDERR, "***%s***\n", linebuffer);

    if ((writestream = fopen(linebuffer, "w")) == NULL)
    {
      #ifdef ERROR
      fprintf(STDERR,
          "*** error (caoi_leed_bsr): can not open file %s\n", linebuffer);
      #endif
      exit(1);
    }

    while (fgets(helpstring, STRSIZE, readstream))
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
