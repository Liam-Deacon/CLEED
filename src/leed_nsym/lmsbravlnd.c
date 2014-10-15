/*********************************************************************
GH/05.07.03

  file contains functions:

  leed_ms_nd         (20.07.95)
     Calculate scattering matrix for Bravais layer.

 Changes:
 GH/20.07.95 - Creation (leed_ms)
 GH/31.07.95 - include matrix multiplication Y * Tii *Y (i.e. make
               msltok obsolete).
 GH/03.09.97 - set return value to 1
 GH/23.09.00 - extension for non-diagonal atomic scattering matrix.
 GH/05.07.03 - bug fix: update all "old" values at the end of function.

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "leed.h"


/*======================================================================*/

/*======================================================================*/

int leed_ms_nd ( mat *p_Tpp, mat *p_Tmm, mat *p_Rpm, mat *p_Rmp,
                 leed_var *v_par,
                 leed_layer * layer,
                 leed_beam_t * beams)

/************************************************************************

  Calculate scattering matrix for Bravais layer.

 INPUT:

   mat * p_Tpp, p_Tmm, p_Rpm p_Rmp - (output) pointers to Bravais layer 
              diffraction matrices in k-space:
              Tpp  k(+) -> k(+) (transmission matrix)
              Tmm  k(-) -> k(-) (transmission matrix)
              Rpm  k(-) -> k(+) (reflection matrix)
              Rmp  k(+) -> k(-) (reflection matrix)

   leed_var v_par - (input) parameters which vary during the energy
              loop. Used: k_in    (parallel components of the incoming beam),
                          l_max   (max. l quantum number),
                          tl      (atomic scattering factors: either diagonal
                                  (t_type = T_DIAG) or non-diagonal
                                  (t_type = T_NOND)),
                          epsilon (smallest relative wave amplitude to
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
  leed_ms_tmat_nd_ii

  leed_ms_ymat
  leed_ms_yp_ym
  leed_ms_yp_yxp
  leed_ms_yp_yxm

  matmul

 RETURN VALUES:

  1  upon successful completion

*************************************************************************/
{

static int old_set  = I_END_OF_LIST;
static int old_n_beams  = I_END_OF_LIST;
static int old_type = I_END_OF_LIST;
static int old_l_max = I_END_OF_LIST;

static real old_eng = F_END_OF_LIST;

static mat Llm = NULL, Tii = NULL;
static mat Yin_p = NULL, Yin_m = NULL, Yout_p = NULL, Yout_m = NULL;

int n_beams, i_beams;
int l_max;
int i_type, t_type;
int iaux, i_c;

real pref_i, faux_r, faux_i;
real *ptr_r, *ptr_i;

mat Maux;


/*************************************************************************
 Preset often used values: i_type, l_max, n_beams
*************************************************************************/
 
 Maux = NULL;

 t_type = (layer->atoms)->t_type;
 i_type = (layer->atoms)->type;
 l_max = v_par->l_max;

 for(n_beams = 0; 
    ! IS_EQUAL_REAL((beams + n_beams)->k_par, F_END_OF_LIST); 
    n_beams ++);

#ifdef CONTROL
 fprintf(STDCTR,"(leed_ms_nd): l_max = %d, "
            "No of beams = %d, atom type = %d\n", l_max, n_beams, i_type);
#endif

/*************************************************************************
 Check if t_type has the right value
*************************************************************************/

 if( (t_type != T_DIAG) && (t_type != T_NOND) )
 {
#ifdef ERROR
    fprintf(STDERR,
            "*** error (leed_ms_nd): unknown matrix type of t-matrix.\n");
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(0);
#endif
 }

/*************************************************************************
 Check if the current beam set was used in the previous call.
 - if not, recalculate lattice sum, scattering matrix, and spherical 
   harmonics.
 - if yes, reuse lattice sum, scattering matrix, and spherical
   harmonics and recalculate the (lm) scattering matrix only if the
   atom type has changed.
*************************************************************************/

 if( ! ((IS_EQUAL_REAL(old_eng, v_par->eng_r))) || 
        (old_set     != beams->set)   ||
        (old_n_beams != n_beams)      ||
        (old_l_max   != l_max)           )
 {
#ifdef CONTROL
 fprintf(STDCTR,"(leed_ms_nd): recalculate lattice sum etc.\n");
#endif

/* calculate lattice sum */
   Llm = leed_ms_lsum_ii ( Llm, beams->k_r[0], beams->k_i[0], beams->k_r,
                      layer->a_lat, 2*l_max, v_par->epsilon );
/* calculate scattering matrix */
   if(t_type == T_DIAG)
   {
     Tii = leed_ms_tmat_ii( Tii, Llm, v_par->p_tl[i_type], l_max);
     Tii = mattrans(Tii, Tii);
   }
   else if(t_type == T_NOND)
   {
     Tii = leed_ms_tmat_nd_ii( Tii, Llm, v_par->p_tl[i_type], l_max);
   }

/* Yout_p = Y(k+) */
   Yout_p = leed_ms_ymat(Yout_p, l_max, beams, n_beams);
/* Yout_m = Y(k-) */
   Yout_m = leed_ms_yp_ym(Yout_m, Yout_p);


/* Yin_p: Y*(k+) for transmission matrix. */
   Yin_p = leed_ms_yp_yxp(Yin_p, Yout_p);
/* Yin_m: Y*(k-) for reflection matrix. */
   Yin_m = leed_ms_yp_yxm(Yin_m, Yout_p);

  /********************************************************************** 
   Loop over k' (exit beams: rows of Yout_p): 
    - Multiply with factor i 8 PI^2 / (|k|*A*k'_z) 
    - i_beams is (row number - 1) and equal to the index of exit beams 
  **********************************************************************/

   pref_i = 8.*PI*PI / (beams->k_r[0] * layer->rel_area);

   ptr_r = Yout_p->rel + 1;
   ptr_i = Yout_p->iel + 1;
   for(i_beams = 0; i_beams < Yout_p->rows; i_beams ++)
   {
     cri_mul(&faux_r, &faux_i, 0., pref_i, 
             (beams+i_beams)->Akz_r, (beams+i_beams)->Akz_i);
     for(i_c = 0; i_c < Yout_p->cols; i_c ++, ptr_r ++, ptr_i ++ )
     {
       cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i); 
     }
   }  /* i_beams */

   ptr_r = Yout_m->rel + 1;
   ptr_i = Yout_m->iel + 1; 
   for(i_beams = 0; i_beams < Yout_m->rows; i_beams ++)
   { 
     cri_mul(&faux_r, &faux_i, 0., pref_i, 
             (beams+i_beams)->Akz_r, (beams+i_beams)->Akz_i); 
     for(i_c = 0; i_c < Yout_m->cols; i_c ++, ptr_r ++, ptr_i ++ )
     { 
       cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i);
     }
   }  /* i_beams */

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
 fprintf(STDCTR,"(leed_ms_nd): recalculate scattering matrix.\n");
#endif

  /* calculate scattering matrix */
     if(t_type == T_DIAG)
     {
       Tii = leed_ms_tmat_ii( Tii, Llm, v_par->p_tl[i_type], l_max);
       Tii = mattrans(Tii, Tii);
     }
     else if(t_type == T_NOND)
     {
       Tii = leed_ms_tmat_nd_ii( Tii, Llm, v_par->p_tl[i_type], l_max);
#ifdef CONTROL
 fprintf(STDCTR,"(leed_ms_nd): T_NOND \n");
#endif
     }

   } /* if i_type */
 }
 

/**********************************************************************
 Matrix product Yout (exit beams) * Tii * Yin (inc. beams)
**********************************************************************/
 Maux = matmul(Maux, Yout_p, Tii);

 *p_Rpm = matmul(*p_Rpm, Maux, Yin_m);
 *p_Tpp = matmul(*p_Tpp, Maux, Yin_p);

 Maux = matmul(Maux, Yout_m, Tii);

 *p_Tmm = matmul(*p_Tmm, Maux, Yin_m);
 *p_Rmp = matmul(*p_Rmp, Maux, Yin_p);


/**********************************************************************
  Add unscattered wave (identity) to transmission matrices Tpp and Tmm
**********************************************************************/

 iaux = (*p_Tpp)->rows * (*p_Tpp)->cols;
 for(i_c = 1; i_c <= iaux; i_c += (*p_Tpp)->cols + 1)
 {
   (*p_Tpp)->rel[i_c] += 1.;
   (*p_Tmm)->rel[i_c] += 1.;
 }
 
/**********************************************************************
  Update energy, beam_set, and beam_num, l_max, i_type 
**********************************************************************/
   old_eng = v_par->eng_r;
   old_set = beams->set;
   old_n_beams = n_beams;
   old_l_max = l_max;
   old_type = i_type;
   
 return(1);
} /* end of function leed_ms_nd */
/*======================================================================*/
