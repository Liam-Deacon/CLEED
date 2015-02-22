/*********************************************************************
 *                        LPCUPDATEND.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/20.01.95 - include function leed_par_mktl
 *                 (structure var_str has changed)
 *   GH/20.09.95 - optional variable vi (structure var_str has changed).
 *               - use leed_par_mktl_nd
 *********************************************************************/

/*! \file
 *
 * Implements leed_par_update_nd() function to update all parameters that
 * change during the energy loop.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <strings.h>

#include "leed.h"

#ifndef VI_START            /* should be defined in "leed_def.h" */
#define VI_START (100./HART)
#endif


/*!
 * Updates all parameters that change during the energy loop.
 *
 * Energy
 * ------
 *
 * Real part of energy (v_par->eng_r) is set to
 *  E = (vacuum energy) - (real part of opt. potential)
 *
 * Imaginary part of energy. v_par->eng_i is set to imaginary part of
 * optical potential which is defined as:
 *
 * v_par->eng_r <  VI_START : prefactor
 * v_par->eng_r >= VI_START : prefactor * (E / VI_START) ^ (exponent)
 *
 * prefactor = v_par->vi_pre,
 * exponent  = v_par->vi_exp.
 *
 * k_in
 * ----
 *
 * |k_in|   = sin(theta_in) * sqrt( 2*(vacuum energy) )
 * k_in(x) = cos(phi_in) * |k_in|
 * k_in(y) = sin(phi_in) * |k_in|
 *
 * \param[in,out] v_par Pointer to structure containing all the parameters
 * that change during the energy loop (for details see leed_def.h ).
 * The parameter structure must exist and must be preset already.
 * \param[in] phs_shifts Pointer to the phase shift structures (will be handed
 * to leed_par_mktl_nd() function).
 * \param energy New vacuum energy value.
 * \return C style return code indicating function success.
 * \retval 0 if successful.
 * \see leed_par_mktl_nd()
 *
 */
int leed_par_update_nd(leed_var *v_par, leed_phase *phs_shifts, real energy)
{
  real faux_r;

  /* Set new energy */
  v_par->eng_v = energy;
  v_par->eng_r = energy - v_par->vr;

  if (v_par->eng_r < VI_START)
    v_par->eng_i = v_par->vi_pre;
  else
  {
    faux_r = cleed_real_log(v_par->eng_r / VI_START) * v_par->vi_exp;
    v_par->eng_i = v_par->vi_pre * cleed_real_exp(faux_r);
  }

  /* Determine k_in */
  faux_r = cleed_real_sin(v_par->theta) * cleed_real_sqrt(2*v_par->eng_v);
  v_par->k_in[0] = faux_r;
  v_par->k_in[1] = faux_r * cleed_real_cos(v_par->phi);
  v_par->k_in[2] = faux_r * cleed_real_sin(v_par->phi);

  CONTROL_MSG(CONTROL, "new energy: Evac = %.2f; (Er, Ei) = (%.2f, %.2f) eV\n"
             "             k_in = (%.3f, %.3f) A-1\n",
             v_par->eng_v*HART, v_par->eng_r*HART, v_par->eng_i*HART,
             v_par->k_in[1]/BOHR, v_par->k_in[2]/BOHR);
  CONTROL_MSG(CONTROL_X, "k_in = \t(%.2f, %.2f)\n",
              v_par->k_in[1], v_par->k_in[2]);

  /* Update phase shifts (leed_par_mktl_nd) */
  v_par->p_tl = leed_par_mktl_nd(v_par->p_tl, phs_shifts,
                                 v_par->l_max, v_par->eng_r);

  return(0);
}  /* end of function leed_par_update */
