/************************************************************************
 * <FILENAME>
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *07.07.03
  file contains functions:

  leed_ms_sym        (03.09.97)
     Calculate scattering matrix for Bravais layer - symmetrised version.

Changes:
GH/04.09.97 - Creation (copied from leed_ms)
WB/21.04.98 - remove the possibility to take old values into acount.
GH/07.07.03 - removed unused variables, make matrices non-static.
*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "leed.h"

/*======================================================================*/

int leed_ms_sym ( mat *p_Tpp, mat *p_Rpm,
                   leed_var *v_par,
                   leed_layer * layer,
                   leed_beam * beams)

/************************************************************************

  Calculate scattering matrix for Bravais layer - symmetrised version.

 INPUT:

   mat * p_Tpp, p_Rpm - (output) pointers to composite layer diffraction 
              matrices in k-space.
              Tpp  k(+) -> k(+) and k(-) -> k(-) (transmission matrix)
              Rpm  k(-) -> k(+) and k(+) -> k(-) (reflection matrix)

   leed_var v_par - (input) parameters which vary during the energy
              loop. Used: k_in    (parallel components of the incoming beam),
                          l_max   (max. l quantum number),
                          tl      (atomic scattering factors),
                          epsilon (smallesti relative wave amplitude to
                                   be considered).

   leed_layer * layer - (input) list containing all relevant
              information about the Bravais layer. Used in this function:

              leed_atom * atoms: scattering properties of the atoms.
              real a_lat[5]: lattice vectors:
                     a_lat[1] = a1_x, a_lat[2] = a2_x,
                     a_lat[3] = a1_y, a_lat[4] = a2_y;
              real rel_area: area of the unit cell relative to 1x1.
              real *eout_b/s: prefactors for outgoing beams:
              real *ein_b/s:  prefactors for incoming beams:

   leed_beam * beams - (input) additional information about the k
              vectors involved.
              Used: k_r, k_i: wave vectors,
                    set:      current beam set,


 DESIGN

  R/T(k'k) = pref_i * |k'><l,m|M(lm,l'm')|l'm'><k| + dkk'

  Ylm =  |k'><l,m| = Ylm(k')     (k_max rows) x ((l_max+1)^2 columns)

 FUNCTION CALLS

  leed_ms_lsum_ii
  leed_ms_tmat_ii

 RETURN VALUES:

  1  upon successful completion

*************************************************************************/
{
int n_beams, i_beams;
int a_type, l_type, i_layer;
int l_max;
int l, m;
int iaux, i_c, i_r;

real pref_i, faux_r, faux_i;
real *ptr_r, *ptr_i;

mat Llm, Gii;
mat Yin_p, Yin_m, Yout;

/*************************************************************************
 Preset often used values: i_layer, l_type, a_type, l_max, n_beams
*************************************************************************/
 
 Llm = NULL, Gii = NULL;
 Yin_p = NULL, Yin_m = NULL, Yout = NULL;

 i_layer = layer->no_of_layer;
 l_type = layer->bulk_over;
 a_type = (layer->atoms)->type;

 l_max = v_par->l_max;

 for(n_beams = 0; 
     ! IS_EQUAL_REAL((beams + n_beams)->k_par, F_END_OF_LIST); n_beams ++);

 if( (l_type != BULK) && (l_type != OVER) )
 {
#ifdef ERROR
   fprintf(STDERR,
   " *** error (leed_ms_sym): unidentified bulk_over type = %d\n", l_type);
#endif
   exit(1);
 }

#ifdef CONTROL_MAT
fprintf(STDCTR, "(leed_ms_sym): layer No = %d, layer type = %d, l_max = %d, ",
         i_layer, l_type, l_max);
fprintf(STDCTR, "No of beams = %d, atom type = %d\n", n_beams, a_type);
#endif

/*************************************************************************
 Check if the current beam set was used in the previous call.
 - if not, recalculate lattice sum, scattering matrix, and spherical 
   harmonics.
 - if yes, reuse lattice sum, scattering matrix, and spherical
   harmonics and recalculate the (lm) scattering matrix only if the
   atom type has changed.
*************************************************************************/

#ifdef CONTROL
 fprintf(STDCTR,"(leed_ms_sym): recalculate lattice sum etc.\n");
#endif

/* calculate lattice sum */
   Llm = leed_ms_lsum_ii ( Llm, beams->k_r[0], beams->k_i[0], beams->k_r,
                      layer->a_lat, 2*l_max, v_par->epsilon );
/* calculate scattering matrix */
   Gii = leed_ms_tmat_ii( Gii, Llm, v_par->p_tl[a_type], l_max);
   Gii = mattrans(Gii, Gii);

/* Yout = Y(k+) */
   Yout  = leed_ms_ymat(Yout, l_max, beams, n_beams);
/* Yin_p: Y*(k+) for transmission matrix. */
   Yin_p = leed_ms_yp_yxp(Yin_p, Yout);
/* Yin_m: Y*(k-) for reflection matrix. */
   Yin_m = leed_ms_yp_yxm(Yin_m, Yout);

  /**********************************************************************
   Loop over k (incoming beams: cols of Yin) and m (rows of Yin):
    - Multiply with factor ein_b/s
      ( ein_b/s = 1/sqrt(n_rot) S exp(-i(m*phi + s*g) )
    - i_beams is (col number - 1) and equal to the index of incoming beams
  **********************************************************************/

   if(l_type == BULK)
   {
     ptr_r = Yin_p->rel + 1;
     ptr_i = Yin_p->iel + 1;
     iaux = 2*l_max+1;

     for(l = 0; l <= l_max; l ++)
       for(m = -l, i_r = i_layer*iaux + l_max +m; m <= l; m ++, i_r ++)
         for(i_beams = 0; i_beams < Yin_p->cols; 
             i_beams ++, ptr_r ++, ptr_i ++ )
         {
           cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i,
                   *((beams+i_beams)->ein_b_r+i_r),
                   *((beams+i_beams)->ein_b_i+i_r) );
         }

     ptr_r = Yin_m->rel + 1;
     ptr_i = Yin_m->iel + 1;
     iaux = 2*l_max+1;

     for(l = 0; l <= l_max; l ++)
       for(m = -l, i_r = i_layer*iaux + l_max +m; m <= l; m ++, i_r ++)
         for(i_beams = 0; i_beams < Yin_m->cols; 
             i_beams ++, ptr_r ++, ptr_i ++ )
         {
           cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i,
                   *((beams+i_beams)->ein_b_r+i_r),
                   *((beams+i_beams)->ein_b_i+i_r) );
         }
   }  /* if BULK, i_beams */

   else
   {

     ptr_r = Yin_p->rel + 1;
     ptr_i = Yin_p->iel + 1;
     iaux = 2*l_max+1;

     for(l = 0; l <= l_max; l ++)
       for(m = -l, i_r = i_layer*iaux + l_max +m; m <= l; m ++, i_r ++)
         for(i_beams = 0; i_beams < Yin_p->cols; 
             i_beams ++, ptr_r ++, ptr_i ++ )
         {
           cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i,
                   *((beams+i_beams)->ein_s_r+i_r),
                   *((beams+i_beams)->ein_s_i+i_r) );
         }


     ptr_r = Yin_m->rel + 1;
     ptr_i = Yin_m->iel + 1;
     iaux = 2*l_max+1;

     for(l = 0; l <= l_max; l ++)
       for(m = -l, i_r = i_layer*iaux + l_max +m; m <= l; m ++, i_r ++)
         for(i_beams = 0; i_beams < Yin_m->cols; 
             i_beams ++, ptr_r ++, ptr_i ++ )
         {
           cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i,
                   *((beams+i_beams)->ein_s_r+i_r),
                   *((beams+i_beams)->ein_s_i+i_r) );
         }
   }  /* else */

  /********************************************************************** 
   Loop over k' (exit beams: rows of Yout): 
    - Multiply with factor i 8 PI^2 / (|k|*A*k'_z)  * eout_b/s
      ( eout_b/s = sqrt(n_rot) S exp(+is*g') )
    - i_beams is (row number - 1) and equal to the index of exit beams 
  **********************************************************************/

   pref_i = 8.*PI*PI / (beams->k_r[0] * layer->rel_area);

   ptr_r = Yout->rel + 1;
   ptr_i = Yout->iel + 1;

   if(l_type == BULK)
     for(i_beams = 0; i_beams < Yout->rows; i_beams ++)
     {
       cri_mul(&faux_r, &faux_i, 0., pref_i, 
               (beams+i_beams)->Akz_r, (beams+i_beams)->Akz_i);
       cri_mul(&faux_r, &faux_i, faux_r, faux_i,
               *((beams+i_beams)->eout_b_r+i_layer), 
               *((beams+i_beams)->eout_b_i+i_layer) );
       for(i_c = 0; i_c < Yout->cols; i_c ++, ptr_r ++, ptr_i ++ )
       {
         cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i); 
       }
     }  /* if BULK, i_beams */
   else
     for(i_beams = 0; i_beams < Yout->rows; i_beams ++)
     {
       cri_mul(&faux_r, &faux_i, 0., pref_i,
               (beams+i_beams)->Akz_r, (beams+i_beams)->Akz_i);
       cri_mul(&faux_r, &faux_i, faux_r, faux_i,
               *((beams+i_beams)->eout_s_r+i_layer),
               *((beams+i_beams)->eout_s_i+i_layer) );
       for(i_c = 0; i_c < Yout->cols; i_c ++, ptr_r ++, ptr_i ++ )
       {
         cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i);
       }
     }  /* else, i_beams */

  /**********************************************************************
   Matrix product Yout * Gii (exit beams)
  **********************************************************************/
   Gii = matmul(Gii, Yout, Gii);
 
 *p_Rpm = matmul(*p_Rpm, Gii, Yin_m);
 *p_Tpp = matmul(*p_Tpp, Gii, Yin_p);

/* Add unscattered wave to transmission matrix */

 iaux = (*p_Tpp)->rows * (*p_Tpp)->cols;
 for(i_c = 1; i_c <= iaux; i_c += (*p_Tpp)->cols + 1)
   (*p_Tpp)->rel[i_c] += 1.;
 
#ifdef CONTROL_MAT
 fprintf(STDCTR, "(leed_ms_sym): Rpm\n");
 matshowabs(*p_Rpm);
 matshow(*p_Rpm);
 fprintf(STDCTR, "(leed_ms_sym): Tpp\n");
 matshowabs(*p_Tpp);
 matshow(*p_Tpp);
#endif

 return(1);
} /* end of function leed_ms_sym */

/*======================================================================*/
