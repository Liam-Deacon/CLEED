 /********************************************************************
 *                        LPCTEMTL.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/18.07.95 - Creation
 *   GH/13.09.95 - Use the original phase shifts only up to l_max_t
 *   GH/04.05.00 - bug fixed in the summation of l3 (fac_l3)
 *   GH/19.09.00 - change definition of dr2: mean square displacement.
 *********************************************************************/

/*! \file
 *
 * Implements leed_par_temp_tl() function.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <strings.h>

#include "leed.h"

/*!
 * Calculates an array of temperature dependent atomic scattering factors.
 *
 *  According to formula (23/p. 29) in VHT:
 *
 * t(l1,T) = sum(l2,l3) { i^l3 * exp(-4aE) * J(l3)(-4iaE) * t(l2,0) *
 *                        sqrt[ 4pi (2*l2+1) * (2*l3+1) / (2*ll+1) ] *
 *                        cg(l3,0; l2,0; l1,0) }
 *
 * a = 1/6 <(dr)^2>(T).
 * E = E_vac - V0 (kinetic energy inside the crystal).
 * J(l2) = bessel function.
 *
 * \param[in] tl_t The function writes the output temperature dependent
 * scattering factors into its first argument. If \p tl_t is \c NULL ,
 * the structure will be created.
 * \param[in] tl_0 Pointer to matrix containing scattering factors for T = 0.
 * \p tl_0 will be stored before \p tl_t is calculated, therefore \p tl_0 can
 * be equal to \p tl_t.
 * \param dr2 Mean square displacement (not isotropic vibrational amplitude)
 * at the given temperature:
 * <(dr)^2>(T) = ux^2 + uy^2 + uz^2 = 3ux^2
 * (since ux^2 = uy^2 = uz^2).
 * \param energy Real part of the energy.
 * \param l_max_t Maximum l quantum number of input scattering factors.
 * \param l_max_0 Maximum required l quantum number of output scattering factors.
 * \return Matrix for a set of temperature dependent scattering factors.
 * \retval \c NULL if any error occurred (and #EXIT_ON_ERROR is not defined).
 */
mat leed_par_temp_tl(mat tl_t, mat tl_0, 
             real dr2, real energy, size_t l_max_t, size_t l_max_0)
{

  size_t l1, l2, l3;
  size_t l3_min, l3_max;

  real alpha_4e;
  real kappa;
  real faux_r, faux_i;
  real fac_l1, fac_l2, fac_l3, fac_l12;
  mat tl_aux;                    /* backup of original scattering factors */
  mat Jl;                        /* Bessel function */

  tl_aux = NULL;
  Jl = NULL;

  CONTROL_MSG(CONTROL,
      "Enter function: \n\tdr2 = %.3f [au^2], energy = %.3f [H], "
      "lmax_t = %d, lmax_0 = %d\n", dr2, energy, l_max_t, l_max_0);
#if CONTROL
  matshow(tl_0);
#endif

  /* backup original scattering factors and allocate output array */
  tl_aux = matcopy(tl_aux, tl_0);
  tl_t =  matalloc(tl_t, l_max_t+1, 1, NUM_COMPLEX);

  /* Make sure that l_max_0 is not greater than l_max_t, and that all C.G.
   * coefficients are available (recalculate if necessary).
   */
  if(l_max_0 > l_max_t)
  {
    l_max_0 = l_max_t;
    WARNING_MSG("input phase shifts are only used up to l_max = %d\n", l_max_0);
  }

  mk_cg_coef(l_max_t+l_max_0);

  /* Calculate often used values:
   * alpha_4e = 4 * 1/6 sigma_r * E = 4* 1/6 * 2<(dr)^2>(T) * E;
   * i^l3 * Jl3(-i*alpha_4e);
   */

  /* changed GH/19.09.00 from alpha_4e = 4./6. * dr2 * energy; */
  alpha_4e = 8./6. * dr2 * energy;

  Jl = c_bess(Jl, 0., -alpha_4e, l_max_t+l_max_0);

  /* multiply Bessel function with sqrt(4pi) * i^l * exp(-alpha_4e) */
  faux_r = 3.54490770181103205461 * cleed_real_exp(-alpha_4e);
  faux_i = 0.;

  for(l1= 1; l1 <= (l_max_t + l_max_0 + 1); l1++)
  {
    cri_mul(Jl->rel+l1, Jl->iel+l1, faux_r, faux_i, Jl->rel[l1], Jl->iel[l1]);
    cri_mul(&faux_r, &faux_i, 0., 1., faux_r, faux_i);
  }
 
  /* Calculate tl_t */

  /* loop over output l's */
  for(l1 = 0, fac_l1 = 1.; l1 <= l_max_t; l1 ++, fac_l1 += 2.)
  {
    /* loop over input l's */
    for(l2 = 0, fac_l2 = 1.; l2 <= l_max_0; l2 ++, fac_l2 += 2.)
    {
      fac_l12 = fac_l2 / fac_l1;

      l3_min = (size_t)abs((int)l2-(int)l1);
      l3_max = l2+l1;

      /* bug fixed */
      for(l3 = l3_min, fac_l3 = l3_min*2. + 1.;
          l3 <= l3_max; l3 ++, fac_l3 += 2. )
      {
        faux_r = cg((int)l3,0, (int)l2,0, (int)l1,0);
        faux_r *= cleed_real_sqrt(fac_l3*fac_l12);

        CONTROL_MSG(CONTROL_X, "pref (%d %d %d) = %f (%f)\n",
               l1, l2, l3, faux_r, cleed_real_sqrt(fac_l3*fac_l12));

        cri_mul(&faux_r, &faux_i,
               tl_aux->rel[l2+1], tl_aux->iel[l2+1],
               faux_r * Jl->rel[l3+1], faux_r * Jl->iel[l3+1]);

        tl_t->rel[l1+1] += faux_r;
        tl_t->iel[l1+1] += faux_i;
      } /* l3 */
    } /* l2 */

    kappa = cleed_real_sqrt(2* energy);
    CONTROL_MSG(CONTROL, "%d: tl_0=(%7.4f, %7.4f)\ttl_t=(%7.4f, %7.4f)"
                "\ttl_t/kappa=(%7.4f, %7.4f)\n", l1,
                tl_aux->rel[l1+1], tl_aux->iel[l1+1],
                tl_t->rel[l1+1], tl_t->iel[l1+1],
                -(tl_t->rel[l1+1])/kappa, -(tl_t->iel[l1+1])/kappa);
 }  /* l1 */

 return(tl_t);

}  /* end of function leed_par_temp_tl */
