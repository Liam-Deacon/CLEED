/**********************************************************************
  WB/24.08.98
  file contains functions:

  mssumR_p
    summation over all equivalent beams for the function leed_ms_compl_sym
  mssumR_m
    summation over all equivalent beams for the function leed_ms_compl_sym
 Changes:
 **********************************************************************/

#include <math.h>
#include <stdio.h>

#include "leed.h"

/*!
 * Returns sign of \p x or -1 if \p x = 0
 */
static inline int signof(int x) {
  return (x > 0) - (x <= 0);
}

static inline void
leed_ms_comp_xyp_partial_k_sum_helper(
    real *sum_r, real *sum_i, size_t n_equiv_beams,
    const leed_beam* const beam, const leed_atom* const atom, real m)
{
  real tmp_r = 0., tmp_i = 0.;

  sum_r = 0.;
  sum_i = 0.;
  for(size_t i = 0; i < n_equiv_beams ; i++)
  {
    real faux1_r = beam->k_x_sym[i] * atom->pos[1]
                 + beam->k_y_sym[i] * atom->pos[2]
                 - beam->k_p_sym[i] * m;
    cri_expi(&tmp_r, &tmp_i, faux1_r, 0.);
    sum_r += tmp_r;
    sum_i += tmp_i;
  }
}

static inline void leed_ms_comp_xy_partial_k_sum_helper(
    real *sum_r, real *sum_i, size_t n_equiv_beams,
    const leed_beam* const beam, const leed_atom* const atom)
{
  real tmp_r = 0., tmp_i = 0.;

  sum_r = 0.;
  sum_i = 0.;
  for(size_t i = 0; i < n_equiv_beams ; i++)
  {
    sum_r = beam->k_x_sym[i] * atom->pos[1]
          + beam->k_y_sym[i] * atom->pos[2];
    cri_expi(&tmp_r, &tmp_i, tmp_r, 0.);
    sum_r += tmp_r;
    sum_i += tmp_i;
  }
}


/*!
 *
 */
mat leed_ms_comp_k_sum(mat Maux,leed_beam *beam,leed_atom *atom,
    int type, int l_max, int sign)
{
  real faux_r, faux_i, faux1_r, faux1_i;
  real exp_k3_r, exp_k3_i;
  real *ptr_r, *ptr_i;
  real *ptr1_r, *ptr1_i;

  int iaux, i, l, m;

  size_t k;
  size_t n_equiv_beams;

  sign = signof(sign);

  /* populate k sum for each column in Maux */
  for(k = 0; k < Maux->cols; k ++)
  {
    /* get number of equivalent beams */
    if (type == BULK)
      n_equiv_beams = beam[k].n_eqb_b;
    else
      n_equiv_beams = beam[k].n_eqb_s;

    /* common prefactor exp(+/-i kz * z)/n_eqb_b */
    faux_r = -1*sign*(beam+k)->k_r[3]*atom->pos[3];
    faux_i = -1*sign*(beam+k)->k_i[3]*atom->pos[3];
    cri_expi(&exp_k3_r,&exp_k3_i,faux_r,faux_i);
    exp_k3_r /= cleed_real_sqrt((real)n_equiv_beams);
    exp_k3_i /= cleed_real_sqrt((real)n_equiv_beams);

    ptr_r = Maux->rel + 1 + k;
    ptr_i = Maux->iel + 1 + k;

    /* First: m = 0: */
    leed_ms_comp_xy_partial_k_sum_helper(&faux_r, &faux_i,
        n_equiv_beams, &beam[k], atom);
    cri_mul(&faux_r, &faux_i, exp_k3_r, exp_k3_i, faux_r, faux_i);

    /* offset of first occurence of m = 0 is 0, increment is 2, 4, 6, ... */
    iaux = 0;
    ptr1_r = ptr_r + iaux * Maux->cols;
    ptr1_i = ptr_i + iaux * Maux->cols;
    cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);

    for( i = 2, iaux += i; iaux < Maux->rows ; i += 2, iaux += i)
    {
      ptr1_r = ptr_r + iaux * Maux->cols;
      ptr1_i = ptr_i + iaux * Maux->cols;
      cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);
    }

    /* Now: m != 0: */
    for(l = 1; l <= l_max; l ++)
    {
      m = -l;
      leed_ms_comp_xyp_partial_k_sum_helper(&faux_r, &faux_i,
          n_equiv_beams, &beam[k], atom, m);
      cri_mul(&faux_r, &faux_i, exp_k3_r, exp_k3_i, faux_r, faux_i);

      /* offset of first occurence of -m is m*m,
       * increment is 2|m|+2, 2|m|+4, ..
       */
      iaux = l*l;
      ptr1_r = ptr_r + iaux * Maux->cols;
      ptr1_i = ptr_i + iaux * Maux->cols;
      cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);

      for( i = 2*(l+1), iaux += i; iaux < Maux->rows ; i += 2, iaux += i)
      {
        ptr1_r = ptr_r + iaux * Maux->cols;
        ptr1_i = ptr_i + iaux * Maux->cols;
        cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);
      }

      m = +l;
      leed_ms_comp_xyp_partial_k_sum_helper(&faux_r, &faux_i,
          n_equiv_beams, &beam[k], atom, m);
      cri_mul(&faux_r, &faux_i, exp_k3_r, exp_k3_i, faux_r, faux_i);

      /* offset of first occurence of +m is (m+1)*(m+1) -1,
       * increment is 2|m|+2, 2|m|+4, ..
       */
      iaux = (l+1)*(l+1) - 1;
      ptr1_r = ptr_r + iaux * Maux->cols;
      ptr1_i = ptr_i + iaux * Maux->cols;
      cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);

      for(i = 2*(l+1), iaux += i; iaux < Maux->rows ; i += 2, iaux += i)
      {
        ptr1_r = ptr_r + iaux * Maux->cols;
        ptr1_i = ptr_i + iaux * Maux->cols;
        cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);
      }

    } /*l*/

  } /*k*/

  return(Maux);
} /* end of function leed_ms_comp_k_sum */
