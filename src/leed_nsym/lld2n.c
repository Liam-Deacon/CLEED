/*********************************************************************
 *                          LLD2N.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/21.01.95 - change WARNING_LOG to CONTROL; CONTROL to CONTROL_X
 *   WB/16.04.98 - CONTROL vec_aa
 *********************************************************************/

/*! \file
 *
 * Calculate the layer scattering matrices for a periodic stack layers
 * by Pendry's layer doubling scheme.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "leed.h"

#ifndef LD_TOLERANCE
#define LD_TOLERANCE 0.001
#endif

/*!
 * Calculates the reflection matrix for a periodic stack of layers
 * of the same type "a" by layer doubling.
 *
 * It calculates the transmission and reflection matrices for 2, 4, 8, etc.
 * identical layers iteratively in function leed_ld_2lay() by applying
 * Pendry's layer doubling scheme:
 *
 * \f[ \textbf{T}_{(n+1)}^{++} = (\textbf{T}_n^{++} \textbf{P}^+) \times
 * (\textbf{I} - \textbf{R}_n^{+-} \textbf{P}^- \textbf{R}_n^{-+}
 * \textbf{P}^+)^{-1} \times \textbf{T}_n^{++} \f]
 *
 * \f[ \textbf{R}_{(n+1)}^{-+} = \textbf{R}_n^{-+} + (\textbf{T}_n^{--}
 * \textbf{P}^- \textbf{R}_n^{-+} \textbf{P}^+) \times
 * (\textbf{I} - \textbf{R}_n^{+-} \textbf{P}^- \textbf{R}_n^{-+}
 * \textbf{P}^+)^{-1} \times \textbf{T}_n^{++} \f]
 *
 * \f[ \textbf{T}_{(n+1)}^{--} = (\textbf{T}_n^{--} \textbf{P}^-) \times
 * (\textbf{I} - \textbf{R}_n^{-+} \textbf{P}^+ \textbf{R}_n^{+-}
 * \textbf{P}^-)^{-1} \times \textbf{T}_n^{--} \f]
 *
 * \f[ \textbf{R}_{(n+1)}^{+-} = \textbf{R}_n^{+-} + (\textbf{T}_n^{++}
 * \textbf{P}^+ \textbf{R}_n^{+-} \textbf{P}^-) \times
 * (\textbf{I} - \textbf{R}_n^{-+} \textbf{P}^+ \textbf{R}_n^{+-}
 * \textbf{P}^-)^{-1} \times \textbf{T}_n^{--} \f]
 *
 * \note The moduli of the coefficients of \p Tpp_a (< #LD_TOLERANCE) are used
 * as the convergence criterion.
 *
 * \param[out] Rpm Reflection matrix (-+) \f$ \textbf{R}^{-+} \f$
 * of the stack "aaaa..."
 * \param[in] Tpp_a Transmission matrix (++) \f$ \textbf{T}_a^{++} \f$ of the
 * layer "a".
 * \param[in] Tmm_a Transmission matrix (--) \f$ \textbf{T}_a^{--} \f$ of the
 * layer "a".
 * \param[in] Rpm_a Reflection matrix (+-) of the layer "a",
 * \f$ \textbf{R}_a^{+-} \f$
 * \param[in] Rmp_a Reflection matrix (-+) of the layer "a",
 * \f$ \textbf{R}_a^{-+} \f$
 * \param[in] beams Pointer to structure containing information about the
 * diffracted beams. Uses: @leed_beam::k_r and @leed_beam::k_i
 * \param[in] vec_aa Vector pointing from the origin of one layer "a" to the
 * origin of the next layer "a". The usual indexing convention for vectors is
 * used (x=1, y=2, z=3).
 * \return Reflection matrix (+-) \f$ \textbf{R}^{+-} \f$ of the periodic stack.
 */
mat leed_ld_2n (mat Rpm, mat Tpp_a,  mat Tmm_a,  mat Rpm_a,  mat Rmp_a,
                leed_beam *beams, real *vec_aa )
{
  int i_layer;

  real abs_new;

  mat Tpp, Tmm, Rmp;

  Tpp = Tmm = Rmp = NULL;

  /* Check arguments and copy to internal variables: */
  Tpp = matcopy(Tpp,Tpp_a);
  Tmm = matcopy(Tmm,Tmm_a);
  Rpm = matcopy(Rpm,Rpm_a);
  Rmp = matcopy(Rmp,Rmp_a);

  abs_new = matabs(Tpp);

  CONTROL_MSG(CONTROL_X, "vec between periodic stacks(%.3f %.3f %.3f)\n",
         vec_aa[1] * BOHR,vec_aa[2] * BOHR,vec_aa[3] * BOHR);

  for (i_layer = 1; abs_new >  LD_TOLERANCE; i_layer *= 2)
  /*
   * Tpp^2 (= abs_new^2) is approx. contribution to reflection matrix
   * of electrons backscattered from the last layer.
   */
  {
    leed_ld_2lay( &Tpp, &Tmm, &Rpm, &Rmp,
                   Tpp, Tmm, Rpm, Rmp, Tpp, Tmm, Rpm, Rmp,
                   beams, vec_aa);

    abs_new = matabs(Tpp)/(Tpp->cols*Tpp->rows);
   
    CONTROL_MSG(CONTROL_X, "No. of layers (i_layer) = "
        "%3d, abs_new = %.1e, tol = %.1e\n", i_layer, abs_new, LD_TOLERANCE);
  }

  CONTROL_MSG(CONTROL, "No. of layers included in final iteration: %d;\n"
              "         modulus of transmission matrix: %.0e (tol: %.0e)\n",
              i_layer, abs_new, LD_TOLERANCE);

  /* free memory and return */
  matfree(Tpp);
  matfree(Tmm);
  matfree(Rmp);
 
  return(Rpm);
}
