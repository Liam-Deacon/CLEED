/*********************************************************************
 *													leed_var.c
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
 * \file leed_var.c
 * \author Liam Deacon
 * \date 19 Mar 2015
 */

#include "leed.h"

#include <stdlib.h>
#include <stddef.h>

/*!
 * Allocates and initialises a #leed_var structure to heap and initialises its
 * values.
 *
 * \param vr Real part of the optical potential
 * \param vi Imaginary part of the optical potential
 *
 * \return Pointer to initialised #leed_var
 *
 * \warning The member \c p_tl is initialised to \c NULL .
 */


void leed_var_init(leed_var *var_par, const real vr, const real vi)
{
  size_t i_c;

  /* preset values */
  var_par->eng_r = 0.;
  var_par->eng_i = 0.;
  var_par->eng_v = 0.;

  var_par->vi_pre = vi;
  var_par->vi_exp = 0.;
  var_par->vr     = vr;

  for( i_c = 0; i_c <=3; i_c ++) var_par->k_in[i_c] = 0.;

  var_par->p_tl = NULL;

  /* to be read in this function: */
  var_par->theta = var_par->phi = 0.;
  var_par->epsilon = WAVE_TOLERANCE;
  var_par->l_max = 0;
}

/*!
 * Frees a #leed_var structure given by \p var from memory
 */
void leed_var_free(leed_var *var) {
  if(var->p_tl) matarrfree(var->p_tl);
  free(var);
  var = NULL;
}
