/*********************************************************************
 *                          LLD2LAY.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/06.09.94 - Creation
 *********************************************************************/

/*! \file
 *
 * Implementation of the layer doubling calculations.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "leed.h"

/*!
 * Calculates the layer scattering matrices for a stack of two layers
 * "a" and "b" (z(a) < z(b)) by layer doubling:
 *
 *    z(a) < z(b) => vec_ab[3] > 0 (otherwise no convergence!)
 *
 * \f[ T_{ab}^{++} = (T_b^{++} P^+) \times
 *      (I - R_a^{+-} P^- R_b^{-+} P^+)^(-1) \times T_a^{++} \f]
 * \f[ R_{ab}^{-+} = R_a{-+} + (T_a^{--} P^- R_b^{-+} P^+)
 *      \times (I - R_a^{+-} P^- R_b^{-+} P^+)^{-1} \times T_a^{++} \f]
 *
 * \f[ T_{ab}^{--} = (T_a^{--} P^-) \times
 *    (I - R_b^{-+} P_+ R_a^{+-} P_-)^{-1} \times T_b^{--} \f]
 * \f[ R_{ab}^{+-} = R_b^{+-} + (T_b^{++} P^+ R_a^{+-} P^-) \times
 *    (I - R_b^{-+} P^+ R_a^{+-} P^-)^{-1} \times T_b^{--} \f]
 *
 * \param[out] p_Tpp_ab Pointer to transmission matrix (++) of the stack "ab".
 * \param[out] p_Tmm_ab Pointer to transmission matrix (--) of the stack "ab".
 * \param[out] p_Rpm_ab Pointer to reflection matrix (+-) of the stack "ab".
 * \param[out] p_Rmp_ab Pointer to reflection matrix (-+) of the stack "ab".
 * \param[in] Tpp_a Transmission matrix (++) of the single layer "a"
 * \param[in] Tmm_a Transmission matrix (--) of the single layer "a"
 * \param[in] Rpm_a Reflection matrix (+-) of the single layer "a"
 * \param[in] Rmp_a Reflection matrix (-+) of the single layer "a"
 * \param[in] Tpp_b Transmission matrix (++) of the single layer "b"
 * \param[in] Tmm_b Transmission matrix (--) of the single layer "b"
 * \param[in] Rpm_b Reflection matrix (+-) of the single layer "b"
 * \param[in] Rmp_b Reflection matrix (-+) of the single layer "b"
 * \param[in] beams Pointer to structure containing information about
 * the diffracted beams.
 * \param[in] vec_ab pointer to vector directed from the origin of layer a
 * to the origin of layer b. The usual convention for vectors is used (x=1,
 * y=2, z=3).
 *
 * \see matcopy() , matmul() and matinv()
 */
void leed_ld_2lay ( mat *p_Tpp_ab, mat *p_Tmm_ab, mat *p_Rpm_ab, mat *p_Rmp_ab,
              mat Tpp_a,  mat Tmm_a,  mat Rpm_a,  mat Rmp_a, 
              mat Tpp_b,  mat Tmm_b,  mat Rpm_b,  mat Rmp_b,
              leed_beam *beams, real *vec_ab )
{
  int k;
  int n_beams, nn_beams;                   /* total number of beams */

  real faux_r, faux_i;
  real *ptr_r, *ptr_i, *ptr_end;

  static mat Pp = NULL, Pm = NULL, Maux_a = NULL, Maux_b = NULL;
  static mat Tpp_ab = NULL, Tmm_ab = NULL, Rpm_ab = NULL, Rmp_ab = NULL;

/*************************************************************************
  Allocate memory and set up propagators Pp and Pm. 

  Pp = exp[ i *( k_x*v_ab_x + k_y*v_ab_y + k_z*v_ab_z) ]
  Pm = exp[-i *( k_x*v_ab_x + k_y*v_ab_y - k_z*v_ab_z) ]
     = exp[ i *(-k_x*v_ab_x - k_y*v_ab_y + k_z*v_ab_z) ]
*************************************************************************/
  n_beams = Tpp_a->cols;
  nn_beams = n_beams * n_beams;

  Pp = matalloc(NULL, n_beams, 1, NUM_COMPLEX );
  Pm = matalloc(NULL, n_beams, 1, NUM_COMPLEX );

  #ifdef CONTROL
  fprintf(STDCTR, "(leed_ld_2lay): vec_ab(%.2f %.2f %.2f) = vec_from_last\n",
         vec_ab[1] * BOHR, vec_ab[2] * BOHR , vec_ab[3] * BOHR);
  #endif

  for( k = 0; k < n_beams; k++)
  {
    #ifdef CONTROL
    fprintf(STDCTR, "(leed_ld_2lay): %2d: k_r = %6.3f %6.3f %6.3f, k_i = %6.3f;",
           k, (beams+k)->k_r[1], (beams+k)->k_r[2], (beams+k)->k_r[3],
           (beams+k)->k_i[3]);
    #endif

    faux_r = (beams+k)->k_r[1] * vec_ab[1] +
             (beams+k)->k_r[2] * vec_ab[2] +
             (beams+k)->k_r[3] * vec_ab[3];
    faux_i = (beams+k)->k_i[3] * vec_ab[3];

    cri_expi(Pp->rel+k+1, Pp->iel+k+1, faux_r, faux_i);

    faux_r -= 2. * (beams+k)->k_r[3] * vec_ab[3];

    cri_expi(Pm->rel+k+1, Pm->iel+k+1, -faux_r, faux_i);

    #ifdef CONTROL
    fprintf(STDCTR, " Pp = (%6.3f,%6.3f), Pm = (%6.3f,%6.3f)",
           Pp->rel[k+1], Pp->iel[k+1], Pm->rel[k+1], Pm->iel[k+1]);

    cri_mul(&faux_r, &faux_i,
            Pp->rel[k+1], Pp->iel[k+1], Pm->rel[k+1], Pm->iel[k+1]);
    fprintf(STDCTR, ": (%6.3f,%6.3f)\n", faux_r, faux_i);
    #endif
  }
 
  /*
   * Prepare the quantities (Ra+- P-) and  -(Rb-+ P+):
   * Multiply the k-th column of Ra+- / Rb-+ with the k-th element of P-/+.
   */

  Maux_a = matcopy(Maux_a, Rpm_a);
  Maux_b = matcopy(Maux_b, Rmp_b);

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

    faux_r = - *(Pp->rel+k);
    faux_i = - *(Pp->iel+k);

    ptr_end = Maux_b->rel+nn_beams;
    for (ptr_r = Maux_b->rel+k, ptr_i = Maux_b->iel+k;
         ptr_r <= ptr_end; ptr_r += n_beams,  ptr_i += n_beams)
    {
      cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i);
    }

  }

  /*
   * (i) Calculate the quantities
   *   -(Ra+- P- Rb-+ P+) = Maux_a * Maux_b (-> Tpp_ab)  and
   *   -(Rb-+ P+ Ra+- P-) = Maux_b * Maux_a (-> Tmm_ab)
   *   and add unity.
   *
   * (ii) Matrix inversion:
   *    Tpp_ab = ( I  - (Ra+- P- Rb-+ P+))^(-1)
   *    Tmm_ab = ( I  - (Rb-+ P+ Ra+- P-))^(-1)
   *
   * (iii) Multiply with T++_a or T--_b, respectively and store the matrices in
   *    Tpp_ab and Tmm_ab respectively:
   *    Tpp_ab = ( I - (Ra+- P- Rb-+ P+))^(-1) * Ta++
   *    Tmm_ab = ( I - (Rb-+ P+ Ra+- P-))^(-1) * Tb--
   *
   * (iv) Prepare Rpm_ab and Rmp_ab:
   *   Rpm_ab ->
   *      Ra+- P- * ( I - (Rb-+ P+ Ra+- P-))^(-1) * Tb-- = Maux_a * Tmm_ab
   *   Rmp_ab ->
   *    - Rb-+ P+ * ( I - (Ra+- P- Rb-+ P+))^(-1) * Ta++ = Maux_b * Tpp_ab
   *   (the sign will be corrected later on)
   */

  /* (i) */
  Tpp_ab = matmul(Tpp_ab, Maux_a, Maux_b);
  Tmm_ab = matmul(Tmm_ab, Maux_b, Maux_a);

  for(k = 1; k <= nn_beams; k+= Maux_b->cols + 1)
  {
    Tpp_ab->rel[k] += 1.;
    Tmm_ab->rel[k] += 1.;
  }

  #ifdef CONTROL
  fprintf(STDCTR, "(leed_ld_2lay): Tpp_ab = matmul(Tpp_ab, Maux_a, Maux_b)\n");
  matshow(Tpp_ab);
  fprintf(STDCTR, "(leed_ld_2lay): Tmm_ab = matmul(Tmm_ab, Maux_b, Maux_a)\n");
  matshow(Tmm_ab);
  #endif

  /* (ii) */
  Tpp_ab = matinv(Tpp_ab, Tpp_ab);
  Tmm_ab = matinv(Tmm_ab, Tmm_ab);

  /* (iii) */
  Tpp_ab = matmul(Tpp_ab, Tpp_ab, Tpp_a);
  Tmm_ab = matmul(Tmm_ab, Tmm_ab, Tmm_b);

  /* (iv) */
  Rpm_ab = matmul(Rpm_ab, Maux_a, Tmm_ab);
  Rmp_ab = matmul(Rmp_ab, Maux_b, Tpp_ab);

  /*
   * Prepare the quantities Maux_a = (Ta-- P-) and Maux_b = (Tb++ P+):
   * Multiply the k-th column of Ta-- / Tb++ with the k-th element of P-/+.
   */
  Maux_a = matcopy(Maux_a, Tmm_a);
  Maux_b = matcopy(Maux_b, Tpp_b);

  for(k = 1; k <= n_beams; k ++)
  {
    faux_r = *(Pm->rel+k);
    faux_i = *(Pm->iel+k);

    ptr_end=Maux_a->rel+nn_beams;
    for (ptr_r = Maux_a->rel+k, ptr_i = Maux_a->iel+k;
         ptr_r <= ptr_end; ptr_r += n_beams, ptr_i += n_beams)
    {
      cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i);
    }

    faux_r = *(Pp->rel+k);
    faux_i = *(Pp->iel+k);

    ptr_end=Maux_b->rel+nn_beams;
    for (ptr_r = Maux_b->rel+k, ptr_i = Maux_b->iel+k;
         ptr_r <= ptr_end;  ptr_r += n_beams, ptr_i += n_beams)
    {
      cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i);
    }
  } /* for k */

  /*
   * (i) Finish the computation of Tab++ and Tab--:
   *   Tpp_ab = Maux_b * Tpp_ab,
   *   Tmm_ab = Maux_a * Tmm_ab.
   *
   * (ii) Complete the computation of the matrix product in Rab+- and Rab-+:
   *   Rpm_ab = Maux_b * Rpm_ab,
   *   Rmp_ab = Maux_a * Rmp_ab.
   *
   * (iii) Finally add the reflection matrix of a single layer to the matrix
   * product in Rab+- and Rab-+:
   *
   * (The minus sign in Rab-+ is due to the negative sign of the matrix
   *  product).
   */

  /* (i) */
  Tpp_ab = matmul(Tpp_ab, Maux_b, Tpp_ab);
  Tmm_ab = matmul(Tmm_ab, Maux_a, Tmm_ab);

  /* (ii) */
  Rpm_ab = matmul(Rpm_ab, Maux_b, Rpm_ab);
  Rmp_ab = matmul(Rmp_ab, Maux_a, Rmp_ab);

  /* (iii) */

  /* Rpm_ab: */
  ptr_end = Rpm_ab->rel + nn_beams;
  for(ptr_r = Rpm_ab->rel+1, ptr_i = Rpm_b->rel+1;
      ptr_r <= ptr_end; ptr_r ++, ptr_i ++)
  {
    *ptr_r += *ptr_i;
  }

  ptr_end = Rpm_ab->iel + nn_beams;
  for(ptr_r = Rpm_ab->iel+1, ptr_i = Rpm_b->iel+1;
      ptr_r <= ptr_end; ptr_r ++, ptr_i ++)
  {
    *ptr_r += *ptr_i;
  }

  /* Rmp_ab: */
  ptr_end = Rmp_ab->rel + nn_beams;
  for(ptr_r = Rmp_ab->rel+1, ptr_i = Rmp_a->rel+1;
      ptr_r <= ptr_end; ptr_r ++, ptr_i ++)
  {
    *ptr_r = *ptr_i - *ptr_r;
  }

  ptr_end = Rmp_ab->iel + nn_beams;
  for(ptr_r = Rmp_ab->iel+1, ptr_i = Rmp_a->iel+1;
      ptr_r <= ptr_end; ptr_r ++, ptr_i ++)
  {
    *ptr_r = *ptr_i - *ptr_r;
  }

  /*
   * - Free temporary storage space,
   * - Write results to output pointers
   * - Return.
   */

  *p_Tpp_ab = matcopy(*p_Tpp_ab, Tpp_ab);
  *p_Tmm_ab = matcopy(*p_Tmm_ab, Tmm_ab);
  *p_Rpm_ab = matcopy(*p_Rpm_ab, Rpm_ab);
  *p_Rmp_ab = matcopy(*p_Rmp_ab, Rmp_ab);

}
