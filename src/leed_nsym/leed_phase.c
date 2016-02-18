/*********************************************************************
 *													leed_phase.c
 *
 *  Copyright (C) 2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/19 Mar 2015 - creation
 *********************************************************************/

/*!
 * \file leed_phase.c
 * \author Liam Deacon
 * \date 19 Mar 2015
 */

#include "leed.h"
#include <stdlib.h>

/*!
 * Frees \p phs_shifts from memory.
 */
void leed_phase_free(leed_phase *phs_shifts) {
  if (phs_shifts->energy != NULL) {
    free(phs_shifts->energy);
    phs_shifts->energy = NULL;
  }
  if (phs_shifts->pshift != NULL) {
      free(phs_shifts->pshift);
      phs_shifts->pshift = NULL;
  }
  free(phs_shifts);
  phs_shifts = NULL;
}

/*!
 * Prints phase shift structure \p phsh to \p stream
 *
 * \param stream Pointer to output stream
 *
 * \return String describing the phase shift
 */
const char *leed_phase_print(FILE *stream, const leed_phase * const phsh)
{
  size_t len = 0, max_len, i = 0;
  const size_t nl = phsh ? phsh->lmax + 1 : 1;
  char header[STRSZ] = "";
  static char *str = NULL;

  if (!phsh)
  {
    ERROR_MSG("phase shift cannot be NULL\n")
    return "";
  }

  sprintf(header, "Number of energies = %d, lmax = %d\n\n\t  E(H)",
                 phsh->n_eng, phsh->lmax);
  max_len = len + (20*phsh->n_eng) + (10*phsh->n_eng*nl) + 1;
  CLEED_REALLOC(str, max_len * sizeof(char));

  len += sprintf(str, header);
  while(i < nl && len < max_len-10)
  {
    len += sprintf(str+len, "\t  l=%2d", i);
    i++;
  }
  len += sprintf(str+len, "\n\n");

  size_t i_eng = 0;
  while(i_eng < phsh->n_eng && len < max_len - 10)
  {
    len += sprintf(str+len, "\t%7.4f", phsh->energy[i_eng]);
    i++;
    while(i < nl && len < max_len - 10)
    {
      if(! IS_EQUAL_REAL(phsh->pshift[i_eng*nl+i], 0.0))
        len += sprintf(str+len, "\t%7.4f", phsh->pshift[i_eng*nl+i]);
      else
        len += sprintf(str+len, "\t   --  ");
      i++;
    }
    len += sprintf(str+len, "\n");
    i_eng++;
  }
  len += sprintf(str+len, "\n");

  /* write to stream if valid */
  if (stream)
    fprintf(stream, str);

  return str;
}
