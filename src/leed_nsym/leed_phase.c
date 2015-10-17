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
