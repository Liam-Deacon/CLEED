/*********************************************************************
  GH/03.09.97
  file contains functions:

  leed_ms            (20.07.95)
     Calculate scattering matrix for Bravais layer.

 Changes:
 GH/20.07.95 - Creation
 GH/31.07.95 - include matrix multiplication Y * Gii *Y (i.e. make
               msltok obsolete).
 GH/03.09.97 - set return value to 1

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>

#include "leed.h"


/*======================================================================*/
/*======================================================================*/

int leed_ms ( mat *p_Tpp, mat *p_Rpm,
               leed_var *v_par,
               leed_layer * layer,
               leed_beam_t * beams)

/************************************************************************

  Calculate scattering matrix for Bravais layer.

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

   leed_beam_t * beams - (input) additional information about the k
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
int l_max;
int i_type;
int iaux, i_c;

real pref_i, faux_r, faux_i;
real *ptr_r, *ptr_i;

static mat Llm = NULL, Gii = NULL;
static mat Yin_p = NULL, Yin_m = NULL, Yout = NULL;

static real old_eng = F_END_OF_LIST;

static int old_set  = I_END_OF_LIST;
static int old_n_beams  = I_END_OF_LIST;
static int old_type = I_END_OF_LIST;
static int old_l_max = I_END_OF_LIST;

/*************************************************************************
 Preset often used values: i_type, l_max, n_beams
*************************************************************************/
 
 i_type = (layer->atoms)->type;

 l_max = v_par->l_max;

 for(n_beams = 0; 
     ! IS_EQUAL_REAL((beams + n_beams)->k_par, F_END_OF_LIST); n_beams ++);

#ifdef CONTROL
 fprintf(STDCTR,"(leed_ms): l_max = %d, No of beams = %d, atom type = %d\n", 
         l_max, n_beams, i_type);
#endif

/*************************************************************************
 Check if the current beam set was used in the previous call.
 - if not, recalculate lattice sum, scattering matrix, and spherical 
   harmonics.
 - if yes, reuse lattice sum, scattering matrix, and spherical
   harmonics and recalculate the (lm) scattering matrix only if the
   atom type has changed.
*************************************************************************/

 if( !(IS_EQUAL_REAL(old_eng, v_par->eng_r)) || 
      (old_set     != beams->set)   ||
      (old_n_beams != n_beams)      ||
      (old_l_max   != l_max)           )
 {
#ifdef CONTROL
 fprintf(STDCTR,"(leed_ms): recalculate lattice sum etc.\n");
#endif

/* First, reset energy, beam_set, and beam_num */
   old_eng = v_par->eng_r;
   old_set = beams->set;
   old_n_beams = n_beams;
   old_l_max = l_max;
   
/* calculate lattice sum */
   Llm = leed_ms_lsum_ii ( Llm, beams->k_r[0], beams->k_i[0], beams->k_r,
                      layer->a_lat, 2*l_max, v_par->epsilon );
/* calculate scattering matrix */
   Gii = leed_ms_tmat_ii( Gii, Llm, v_par->p_tl[i_type], l_max);
   Gii = mattrans(Gii, Gii);

/* Yout = Y(k+) */
   Yout  = leed_ms_ymat(Yout, l_max, beams, n_beams);
/* Yin_p: Y*(k+) for transmission matrix. */
   Yin_p = leed_ms_yp_yxp(Yin_p, Yout);
/* Yin_m: Y*(k-) for reflection matrix. */
   Yin_m = leed_ms_yp_yxm(Yin_m, Yout);

  /********************************************************************** 
   Loop over k' (exit beams: rows of Yout): 
    - Multiply with factor i 8 PI^2 / (|k|*A*k'_z) 
    - i_beams is (row number - 1) and equal to the index of exit beams 
  **********************************************************************/

   pref_i = 8.*PI*PI / (beams->k_r[0] * layer->rel_area);

   ptr_r = Yout->rel + 1;
   ptr_i = Yout->iel + 1;
   for(i_beams = 0; i_beams < Yout->rows; i_beams ++)
   {
     cri_mul(&faux_r, &faux_i, 0., pref_i, 
             (beams+i_beams)->Akz_r, (beams+i_beams)->Akz_i);
     for(i_c = 0; i_c < Yout->cols; i_c ++, ptr_r ++, ptr_i ++ )
     {
       cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i); 
     }
   }  /* i_beams */

  /**********************************************************************
   Matrix product Yout * Gii (exit beams)
  **********************************************************************/
   Gii = matmul(Gii, Yout, Gii);
 }
 else 
 {
  /*************************************************************************
   The current beam set was used already in the previous call: only
   the scattering matrix has to be recalculated if atom type is different.
  *************************************************************************/

   if( old_type != i_type )
   {
#ifdef CONTROL
 fprintf(STDCTR,"(leed_ms): recalculate scattering matrix.\n");
#endif

  /* reset old_type */
     old_type = i_type;

  /* calculate scattering matrix */
     Gii = leed_ms_tmat_ii( Gii, Llm, v_par->p_tl[i_type], l_max);
     Gii = mattrans(Gii, Gii);

  /**********************************************************************
   Matrix product Yout * Gii (exit beams)
  **********************************************************************/
     Gii = matmul(Gii, Yout, Gii);
   }
 }
 
 *p_Rpm = matmul(*p_Rpm, Gii, Yin_m);
 *p_Tpp = matmul(*p_Tpp, Gii, Yin_p);

/* Add unscattered wave to transmission matrix */

 iaux = (*p_Tpp)->rows * (*p_Tpp)->cols;
 for(i_c = 1; i_c <= iaux; i_c += (*p_Tpp)->cols + 1)
   (*p_Tpp)->rel[i_c] += 1.;
 
 return(1);
} /* end of function leed_ms */

/*======================================================================*/
