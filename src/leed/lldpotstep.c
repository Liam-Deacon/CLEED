/*********************************************************************
 *                          LLDPOTSTEP.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/27.01.95 - Creation
 *********************************************************************/

/*! \file
 *
 * Implements the calculation of the reflection matrix \f$ \textbf{R}^{+-} \f$
 * for a stack of two (super)layers through layer doubling.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "leed.h"

/*!
 * Calculates only the first column of the reflection matrix R+- (i.e. the
 * amplitudes of the backscattered beams) for a stack of one (super) layer
 * "a" (can be the complete bulk) and a square potential step
 * (z(a) < z(pot. step)) by layer doubling:
 *
 * z(a) < z(b) => vec_ab[3] > 0 (otherwise no convergence!)
 *
 * Rab+- = Rb+- + (Tb++ P+ Ra+- P-) * (I - Rb-+ P+ Ra+- P-)^(-1) * Tb--
 *
 * whereby \f$ \textbf{T}_b \f$ and \f$ \textbf{R}_b \f$ are diagonal with:
 *
 * \f[ R  = (k_c - k_v) / (k_c + k_v) \f]
 * \f[ \textbf{T}^+ = 2 k_c / (k_c + k_v)
 * \f[ \textbf{T}^- = 2 k_v / (k_c + k_v)
 *
 * \f$ \vec{k}_c \f$ is the modulus of \f$ \vec{k}_z \f$ inside the crystal
 *  = \f$ \sqrt{2 E_i - k_{par}^2} \f$
 * \f$ \vec{k}_v \f$ is the modulus of \f$ \vec{k}_z \f$ outside the crystal
 * (in vacuum) = \f$ \sqrt{2 E_v - k_{par}^2} \f$
 *
 * \param[out] Rpm1_ab First column of the +- reflection matrix for the
 * "ab" double layer \f$ \textbf{R_{1,ab}^{+-} \f$ .
 * \param[in] Rpm_a Reflection matrix (+-) of the lower layer "a"
 * \f$ \textbf{R}_a^{+-} \f$ .
 * \param[in] beams Pointer to structure containing information about the
 * beams. It uses: @leed_beam::k_r , @leed_beam::k_i and @leed_beam::k_par .
 * \param eng_vac Vacuum energy.
 * \param[in] vec_ab Pointer to vector which points from the origin of layer a
 * to the origin of layer b. The usual indexing convention for vectors is used
 * (x=1, y=2, z=3).
 * \return The first column of the +- reflection matrix
 * \f$ \textbf{R}_{1,ab}^{+-} \f$ , which is not necessarily identical
 * to \p Rpm1_ab
 */
mat leed_ld_potstep ( mat Rpm1_ab, mat Rpm_a,
                leed_beam *beams, real eng_vac, real *vec_ab )
{
  size_t k;
  size_t n_beams, nn_beams;             /* total number of beams */

  real faux_r, faux_i;
  real *ptr_r, *ptr_i, *ptr_end;

  mat kv, Pp, Pm, Maux_a, Maux_b;    /* temp. storage space */
  mat Res;                           /* result will be copied to Rpm_ab */


  Res = kv = Pp = Pm = Maux_a = Maux_b = NULL;

  /*
   * Allocate memory and set up propagators Pp and Pm.
   *
   * Pp = exp[ i *( k_x*v_ab_x + k_y*v_ab_y + k_z*v_ab_z) ]
   * Pm = exp[-i *( k_x*v_ab_x + k_y*v_ab_y - k_z*v_ab_z) ]
   *    = exp[ i *(-k_x*v_ab_x - k_y*v_ab_y + k_z*v_ab_z) ]
   */
  n_beams = Rpm_a->cols;
  nn_beams = n_beams * n_beams;

  Pp = matalloc(NULL, n_beams, 1, NUM_COMPLEX );
  Pm = matalloc(NULL, n_beams, 1, NUM_COMPLEX );

  for( k = 0; k < n_beams; k++)
  {
    faux_r = (beams+k)->k_r[1] * vec_ab[1] +
             (beams+k)->k_r[2] * vec_ab[2] +
             (beams+k)->k_r[3] * vec_ab[3];
    faux_i = (beams+k)->k_i[3] * vec_ab[3];

    cri_expi(Pp->rel+k+1, Pp->iel+k+1, faux_r, faux_i);

    faux_r -= 2 * (beams+k)->k_r[3] * vec_ab[3];

    cri_expi(Pm->rel+k+1, Pm->iel+k+1, -faux_r, faux_i);

  }

  /* Calculate kz in vacuum and store the results in kv. */
  kv = matalloc(kv, n_beams, 1, NUM_COMPLEX );

  for( k = 0; k < n_beams; k++)
  {
    faux_r = 2*eng_vac - SQUARE((beams+k)->k_par);
    if (faux_r >= 0.)
    {
      kv->rel[k+1] = sqrt(faux_r);
      kv->iel[k+1] = 0.;
    }
    else
    {
      kv->rel[k+1] = 0.;
      kv->iel[k+1] = sqrt(-faux_r);
    }
  }
 
  /*
   * Prepare (Ra+- P-) (-> Maux_a):
   * Multiply the k-th column of Ra+- with the k-th element of P-.
   */
  Maux_a = matcopy(Maux_a, Rpm_a);

  for(k = 1; k <= n_beams; k ++)
  {
    faux_r = *(Pm->rel+k);
    faux_i = *(Pm->iel+k);

    ptr_end = Maux_a->rel+nn_beams;
    for (ptr_r = Maux_a->rel+k, ptr_i = Maux_a->iel+k;
         ptr_r <= ptr_end; ptr_r += n_beams,  ptr_i += n_beams)
    {
      cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i);
    }
  }

  /*
   * Calculate I - (Rb-+ P+ Ra+- P-) (-> Maux_b):
   * Multiply the k-th row of Maux_a with the k-th element of P+ and Rpot
   *  R = (kc - kv) / (kc + kv)
   * and add unity (I).
   */
  Maux_b = matcopy(Maux_b, Maux_a);

  for(k = 1; k <= n_beams; k ++)
  {
    cri_div(&faux_r, &faux_i,
           (beams+k-1)->k_r[3] - kv->rel[k], (beams+k-1)->k_i[3] - kv->iel[k],
           (beams+k-1)->k_r[3] + kv->rel[k], (beams+k-1)->k_i[3] + kv->iel[k]);
    cri_mul(&faux_r, &faux_i, Pp->rel[k], Pp->iel[k], faux_r, faux_i);

    ptr_end = Maux_b->rel + k*Maux_b->cols;
    for (ptr_r = Maux_b->rel + (k-1)*Maux_b->cols + 1,
         ptr_i = Maux_b->iel + (k-1)*Maux_b->cols + 1;
         ptr_r <= ptr_end; ptr_r ++,  ptr_i ++)
    {
      cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i);
    }
  }

  /* add unity */
  for(k = 1; k <= nn_beams; k+= Maux_b->cols + 1)
  {
    Maux_b->rel[k] += 1.;
  }

  /*
   * (i) Matrix inversion:
   *      Maux_b = ( I  - (Rb-+ P+ Ra+- P-))^(-1)
   *
   * (ii) Multiply the first row with T--(00) and store the result in Res:
   *      Res = ( I - (Rb-+ P+ Ra+- P-))^(-1)k1 * Tb--(00)
   *     (First allocate Res)
   *
   * (iii) Prepare Res:
   *      Res ->
   *      Ra+- P- * ( I - (Rb-+ P+ Ra+- P-))^(-1) * Tb-- = Maux_a * Res
   */

  /* (i) */
  Maux_b = matinv(Maux_b, Maux_b);

  /* (ii) */
  /* T-(00) = 2*kv / (kv + kc) */
  cri_div(&faux_r, &faux_i,
          2*kv->rel[1], 2*kv->iel[1],
          beams->k_r[3] + kv->rel[1], beams->k_i[3] + kv->iel[1]);

  /* Multiply first column with T-(00) */
  Res = matcol(Maux_b, 1);
  for(k = 1; k <= n_beams; k ++)
  {
    cri_mul(Res->rel+k, Res->iel+k, Res->rel[k], Res->iel[k], faux_r, faux_i);
  }

  /* (iii) */
  Res = matmul(Res, Maux_a, Res);

  /*
   * Complete the computation of the matrix product in R+-:
   *
   *  (i) Multiply with T++ P+, i.e. multiply the elements of Res with
   *      T++ * P+
   * (ii) Add the reflextion of (00) at the potential step:
   *      R+- = (- kc + kv) / (kc + kv)
   */

  /* (i) */
  for(k = 1; k <= n_beams; k ++)
  {
    cri_div(&faux_r, &faux_i,
            2*((beams+k-1)->k_r[3]), 2*((beams+k-1)->k_i[3]),
            (beams+k-1)->k_r[3] + kv->rel[k], (beams+k-1)->k_i[3] + kv->iel[k]);
    cri_mul(&faux_r, &faux_i, Pp->rel[k], Pp->iel[k], faux_r, faux_i);

    cri_mul(Res->rel+k, Res->iel+k, Res->rel[k], Res->iel[k], faux_r, faux_i);
  }

  /* (ii) */
  cri_div(&faux_r, &faux_i,
      - beams->k_r[3] + kv->rel[1], - beams->k_i[3] + kv->iel[1],
      beams->k_r[3] + kv->rel[1], beams->k_i[3] + kv->iel[1]);

  Res->rel[1] += faux_r;
  Res->iel[1] += faux_i;

  /*
   * - Write the result to the output pointer
   * - Free temporary storage space,
   * - Return.
   */
  Rpm1_ab = matcopy(Rpm1_ab, Res);

  matfree(Pp);
  matfree(Pm);
  matfree(kv);
  matfree(Maux_a);
  matfree(Maux_b);
  matfree(Res);

  return(Rpm1_ab);
}
