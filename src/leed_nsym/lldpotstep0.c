/*********************************************************************
 *                        LLDPOTSTEP0.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/15.03.95 - Creation
 *********************************************************************/

/*! \file
 *
 * Implements calculation of the first column of the reflection matrix
 * \f$ \textbf{R}_1^{+-} \f$ from a square potential step by layer doubling.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>

#include "leed.h"

/*!
 * Calculates only the first column of the reflection matrix
 * \f$ \textbf{R}^{+-} \f$ (i.e. the amplitudes of the backscattered beams)
 * for a (super) layer "a", which can be the complete bulk, \p vec_ab away
 * from a square potential step (z(a) < z(pot. step))
 *
 * \note Reflection at the potential step is ignored!
 * \note \f$ z(a) < z(b) \f$ => \c vec_ab[3] > 0 (otherwise no convergence!)
 *
 * This will produce output like in the Van Hove-Tong package.
 *
 * The first column of Rpm_a is multiplied with the propagator for
 * the (00) beam towards the layer a (-) and the respective propagators
 * away from the layer after scattering (+).
 *
 * \param[output] Rpm1_ab Pointer to the first column of the +- reflection
 * matrix \f$ \textbf{R}^{+-} \f$
 * \param[in] Rpm_a +- reflection matrix of the "lower" layer a.
 * \param[in] beams pointer to structure containing information about the beams.
 * The member @leed_beam::k_r is used.
 * \param eng_vac vacuum energy, included for compatibility with
 * leed_ld_potstep()
 * \param[in] vec_ab Pointer to vector pointer from the origin of layer a to
 * the potential step. The usual indexing convention for vectors is used
 * (x=1, y=2, z=3).
 * \return Pointer to the first column of the +- reflection matrix
 * \f$ \textbf{R}^{+-} \f$ (this is not necessarily equal to \p Rpm1_ab
 * \retval NULL if function failed.
 */
mat leed_ld_potstep0(mat Rpm1_ab, mat Rpm_a,
                     leed_beam *beams, real eng_vac, real *vec_ab )
{
  size_t k, l;                      /* iterators */
  size_t n_beams;                   /* total number of beams */

  real faux_r, faux_i;              /* complex intermediates */
  mat Maux = NULL;                  /* temp. storage space */

  /*
   * - Allocate memory and set up propagator Maux.
   *
   * Maux(k) = P-(00) * P+(k)
   *
   * Maux = exp[-i *( k_x0*v_ab_x + k_y0*v_ab_y - k_z0*v_ab_z) ] *
   *     exp[ i *( k_x*v_ab_x  + k_y*v_ab_y  + k_z*v_ab_z)  ]
   *   = exp[ i *( (k_x-k_x0)*v_ab_x + (k_y-k_y0)*v_ab_y + (k_z0+k_z)*v_ab_z)]
   *
   * - Multiply the first column of Rpm_a with the propagators to the potential
   *   step.
   * - Multiply with the factor sqrt(cos(out) / cos(in))
   */
  n_beams = Rpm_a->cols;
  if( (Maux = matalloc(NULL, n_beams, 1, NUM_COMPLEX)) == NULL) return NULL;

  for(k = 1, l = 1; k <= n_beams; k ++, l+= n_beams)
  {

    faux_r = ((beams+k-1)->k_r[1] - beams->k_r[1]) * vec_ab[1] +
             ((beams+k-1)->k_r[2] - beams->k_r[2]) * vec_ab[2] +
             ((beams+k-1)->k_r[3] + beams->k_r[3]) * vec_ab[3];
    faux_i = ((beams+k-1)->k_i[3] + beams->k_i[3]) * vec_ab[3];
   
    cri_expi(Maux->rel+k, Maux->iel+k, faux_r, faux_i);

    /* sqrt(cos(out) / cos(in)): */
    faux_r = 2 * eng_vac -
            SQUARE((beams+k-1)->k_r[1]) - SQUARE((beams+k-1)->k_r[2]);
    faux_i = 2 * eng_vac - SQUARE(beams->k_r[1]) - SQUARE(beams->k_r[2]);
    faux_r = R_sqrt(faux_r / faux_i);
    faux_r = R_sqrt(faux_r);
    cri_mul(Maux->rel+k, Maux->iel+k, faux_r, 0., Maux->rel[k], Maux->iel[k]);

    cri_mul(Maux->rel+k, Maux->iel+k,
            Rpm_a->rel[l], Rpm_a->iel[l], Maux->rel[k], Maux->iel[k]);

  } /* for k,l */
 
  /* - Write the result to the output pointer
   * - Free temporary storage space Maux
   * - Return.
   */
  Rpm1_ab = matcopy(Rpm1_ab, Maux);
  matfree(Maux);

  return(Rpm1_ab);
}
