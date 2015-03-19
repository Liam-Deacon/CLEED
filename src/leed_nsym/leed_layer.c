/*********************************************************************
 *													leed_layer.c
 *
 *  Copyright (C) 2015 Liam Deacon <liam.deacon@diamond.ac.uk>
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
 * \file leed_layer.c
 * \author Liam Deacon
 * \date 19 Mar 2015
 */

#include "leed.h"
#include <stdlib.h>

void leed_layer_free(leed_layer *layer) {
  if (layer->atoms != NULL) {
    free(layer->atoms);
    layer->atoms = NULL;
  }
  free(layer);
  layer = NULL;
}
