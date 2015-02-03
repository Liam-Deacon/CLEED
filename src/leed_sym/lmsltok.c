/*********************************************************************
  GH/05.09.94 
  file contains functions:

  ms_ltok            (06.09.94)
     Transformation of the multiple scattering matrix from angular 
     momentum space into k-space.

 Changes:

 GH/27.01.95 - Remove "-" from pref_i. Prefactor is now: 
               i*8*PI^2 / (|k|*A*kz+)

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "leed.h"

/*======================================================================*/

mat ms_ltok ( mat Mkk, mat Mlm, mat Ylm, mat Yxlm, 
              leed_beam * beams, real rel_area, real unsc)

/************************************************************************
 
  Transformation of the multiple scattering matrix from angular momentum
  space into k-space.

  Mk'k = pre_fac * sum(l,m,l',m') |k'><l,m|M(lm,l'm')|l'm'><k| + dkk'

  Ylm =  |k'><l,m| = Ylm(k')     (k_max rows) x ((l_max+1)^2 columns)
  Yxlm = |k><l',m'| = Y*l'm'(k)  ((l_max+1)^2 rows) x (k_max columns)

  pre_fac = i*8*PI^2 / (|k|*A*kz+)

 INPUT:

   mat Mkk  - output: multiple scattering matrix in k-space.
   mat Mlm  - input: multiple scattering matrix in (l,m)-space.

   mat Ylm  - input: transformation matrix for exit beams (l,m) -> (k) of
              dimension: n_beams x (l_max+1)^2 (e.g. output from leed_ms_ymat)
   mat Yxlm - input: transformation matrix for incoming beams (k) -> (l,m)
              of dimension (l_max+1)^2 x n_beams (e.g. output from ms_ytoyx)

              If either Ykl or Yxkl are NULL, it will be calculated as the
              conjugate transposed matrix from the opposite transformation
              using ms_ytoyx.

   leed_beam * beams - additional information about the k vectors 
              involvedi (in this case: (A kz)^-1).
              The order of beams must be equal to the first dimension of
              Ykl (not checked).

   real unsc - relative intensity of the unscattered beams (either 1. for
              transmission or 0. for reflection)

   real rel_area - area of the unit cell relative to 1x1.


 DESIGN

   Depending on the k-vectors involved in the transformation matrices Ylm 
   and Yxlm, either a reflection (+,- or -,+) or a transmission matrix 
   (+,+ or -,-) will be calculated.

 RETURN VALUES:

   NULL if failed (and EXIT_ON_ERROR is not defined)

   Mkk (may be different from input parameter). The storage scheme for 
   Mkk is defined by the order of Ylm and Yxlm.

*************************************************************************/
{
int iaux;
int i_c, i_r;
real *ptr_r, * ptr_i;

real faux_r, faux_i;
real pref_i;

/********************************************************************** 
 - Check the validity of the input matrices Mkk, Mlm, Ylm, Yxlm
   (Different requirements if FIND_Yx is defined)
 - Check arguments Ylm, Yxlm and create the missing matrix if FIND_Yx
   is defined.
**********************************************************************/

#ifndef FIND_Yx

 if( (matcheck(Mlm)  < 1) ||
     (matcheck(Ylm)  < 1) ||
     (matcheck(Yxlm) < 1) ||
     (matcheck(Mkk)  < 0) )
 {
#ifdef ERROR_LOG
   if(matcheck(Mlm)  < 1)
     fprintf(STDERR," *** error (ms_ltok): invalid input matrix Mlm\n");
   if (matcheck(Ylm)  < 1)
     fprintf(STDERR," *** error (ms_ltok): invalid input matrix Ylm\n");
   if (matcheck(Yxlm) < 1)
     fprintf(STDERR," *** error (ms_ltok): invalid input matrix Yxlm\n");
   if (matcheck(Mkk)  < 0)
     fprintf(STDERR," *** error (ms_ltok): invalid matrix pointer Mkk\n");
#endif
#ifdef EXIT_ON_ERROR
     exit(1);
#else
     return(NULL);
#endif
 }

#else /* FIND_Yx is defined */

 if( (matcheck(Mlm)  < 1) ||
    ((matcheck(Ylm) + (matcheck(Yxlm)) < 1) ||
     (matcheck(Mkk)  < 0) )
 {
#ifdef ERROR_LOG
     fprintf(STDERR," *** error (ms_ltok): invalid input matrices\n");
#endif
#ifdef EXIT_ON_ERROR
     exit(1);
#else
     return(NULL);
#endif
 }

 if(Yxlm == NULL)
 {
   if(Ylm != NULL)
     Yxlm = ms_ytoyx(Yxlm, Ylm);
   else /* Ylm == NULL */
   {
#ifdef ERROR_LOG
     fprintf(STDERR," *** error (ms_ltok): Yxlm = Ylm = NULL\n");
#endif
#ifdef EXIT_ON_ERROR
     exit(1);
#else
     return(NULL);
#endif
   }
 }
 if(Ylm == NULL)  /* for Yxlm == NULL already checked */
   Ylm = ms_ytoyx(Ylm, Yxlm); 

#endif /* FIND_Yx */

/********************************************************************** 
  Calculate the matrix product Ylm(k') * M(lm,l'm') * Yl'm'(k)
**********************************************************************/

/*
 matshowabs(Ylm);
 matshowabs(Mlm);
*/
 Mkk= matmul(Mkk,Ylm,Mlm);
 Mkk= matmul(Mkk,Mkk,Yxlm);
 
#ifdef CONTROL
 fprintf(STDCTR,"(ms_ltok): Ylm * Mlm * Yxlm = \n");
 matshow(Mkk);
#endif
 
/********************************************************************** 
 Loop over k' (exit beams: rows of Mk'k): 
  - Multiply with factor i 8 PI^2 / (|k|*A*k'_z) 
  - i_r is (row number - 1) and equal to the index of exit beams 
**********************************************************************/

 pref_i = 8.*PI*PI / (beams->k_r[0] * rel_area);

 ptr_r = Mkk->rel + 1;
 ptr_i = Mkk->iel + 1;
 for(i_r = 0; i_r < Mkk->rows; i_r ++)
 {
   cri_mul(&faux_r, &faux_i, 0., pref_i, 
           (beams+i_r)->Akz_r, (beams+i_r)->Akz_i);
   for(i_c = 0; i_c < Mkk->cols; i_c ++, ptr_r ++, ptr_i ++ )
   {
     cri_mul(ptr_r, ptr_i, *ptr_r, *ptr_i, faux_r, faux_i); 
   }
 }  /* i_r */

/********************************************************************** 
 Add unscattered beams:
**********************************************************************/
 if( ! IS_EQUAL_REAL(unsc, 0.))
 {
   iaux = Mkk->rows * Mkk->cols;
   for(i_r = 1; i_r <= iaux; i_r += Mkk->cols + 1)
     *(Mkk->rel+i_r) += unsc;
 }
 return(Mkk);

} /* end of function ms_ltok */

/*======================================================================*/
/*======================================================================*/
