/*********************************************************************
 *													leed_beam.c
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
 * \file leed_beam.c
 * \author Liam Deacon
 * \date 19 Mar 2015
 */

#include "leed.h"
#include <stdlib.h>

void leed_beam_free(leed_beam *beams) {
  if (beams->eout_b_r != NULL) {
    free(beams->eout_b_r);
    beams->eout_b_r = NULL;
  }
  if (beams->eout_b_i != NULL) {
    free(beams->eout_b_i);
    beams->eout_b_i = NULL;
  }
  if (beams->eout_s_r != NULL) {
      free(beams->eout_s_r);
      beams->eout_s_r = NULL;
  }
  if (beams->eout_s_i != NULL) {
    free(beams->eout_s_i);
    beams->eout_s_i = NULL;
  }
  if (beams->ein_b_r != NULL) {
    free(beams->ein_b_r);
    beams->ein_b_r = NULL;
  }
  if (beams->ein_b_i != NULL) {
    free(beams->ein_b_i);
    beams->ein_b_i = NULL;
  }
  if (beams->ein_s_r != NULL) {
    free(beams->ein_s_r);
    beams->ein_s_r = NULL;
  }
  if (beams->ein_s_i != NULL) {
    free(beams->ein_s_i);
    beams->ein_s_i = NULL;
  }
}
