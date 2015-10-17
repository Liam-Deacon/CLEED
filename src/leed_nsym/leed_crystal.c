/*********************************************************************
 *													leed_crystal.c
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
 * \file leed_crystal.c
 * \author Liam Deacon
 * \date 19 Mar 2015
 */

#include "leed.h"
#include <stdlib.h>

void leed_crystal_free(leed_crystal *crystal) {
  if (crystal->alpha != NULL) {
    free(crystal->alpha);
    crystal->alpha = NULL;
  }
  if (crystal->m_plane != NULL) {
    free(crystal->m_plane);
    crystal->m_plane = NULL;
  }
  leed_layer_free(crystal->layers);
  for(int i=0; &crystal->comments[i] != NULL; i++) {
    free(&crystal->comments[i]);
  }
  free(crystal);
  crystal = NULL;
}
