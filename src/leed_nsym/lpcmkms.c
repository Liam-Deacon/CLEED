/*********************************************************************
GH/16.09.00 
  file contains function:

  pc_mk_ms(mat *p_Mx, mat *p_My, mat *p_Mz, 
           mat *p_MxMx, mat *p_MyMy, mat *p_MzMz, int l_max); 

 Compute Ms defined in Fritzsche's paper.

CHANGES:
GH/18.09.00 - Creation

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <strings.h>

#include "leed.h"

#define M_TOLERANCE 1.e-6               /* equal zero */
#define SQRT_4PI3 2.0466534158929771    /* sqrt(4*PI / 3) */
#define SQRT_2_1  0.7071067811865475    /* 1 / sqrt(2)    */

int pc_mk_ms(mat *p_Mx,   mat *p_My,   mat *p_Mz, 
             mat *p_MxMx, mat *p_MyMy, mat *p_MzMz, int l_max)

/************************************************************************

 Calculate non-diagonal temperature dependent atomic scattering matrix
 according to the cumulants expansion (P. de Andres / V. Fritzsche).
 
 INPUT:

  mat * p_Mx, p_My, p_Mz (input/output) 
           The function writes the output matrices into these pointers.
           If tmat is NULL, the mat structure will be created.

  mat * p_Mx, p_My, p_Mz (input/output) 
           The function writes the output matrices into these pointers.
           If tmat is NULL, the mat structure will be created.

  int l_max - (input) max required l quantum number of output matrix.

 RETURN VALUES:

  1 if ok
  - number of violations if erreneous matrix elements have been detected.

 DESIGN:

 see: P. de Andres, D. A. King, "Anisotropic and Anharmonic effects through
      the t-matrix for Low-Energy Electron Diffraction (TMAT V1.1)",
      Comp. Phys. Comm., sect. 2.4. the equation numbers (30 - 32) 
      refer to this paper.

*************************************************************************/
{

int l1, l3, m1, m3;
int i_el, i_count;
int l_max_2;                   /* matrix dimension */

real trace_r, trace_i;

real faux_r, faux_i, pref; 
/* real sq_2_1, sq_4pi3; */
real Mp_r, Mp_i, Mm_r, Mm_i;

/* Ms */
mat Mx, MxMx;
mat My, MyMy;
mat Mz, MzMz;
mat Unity;

#ifdef CONTROL
 fprintf(STDCTR,"(pc_mk_ms): Enter function lmax = %d\n",l_max);
/*
 sq_4pi3 = R_sqrt(4 * PI / 3.);
 sq_2_1  = 1. / R_sqrt(2.);

 fprintf(STDCTR,"(pc_mk_ms):sq_4pi/3 = %f sq_2_1 = %f\n", 
         sq_4pi3 * 10000000000000000., sq_2_1 * 10000000000000000.);
*/
#endif

/*************************************************************************
  call mk_cg_coef to make sure, that all C.G. coefficients are available.
  preset variables 
  l_max_2 = matrix dimension
*************************************************************************/

 mk_cg_coef(2*l_max);

 l_max_2 = (l_max+1)*(l_max+1);

 Mx = *p_Mx; MxMx = *p_MxMx;
 My = *p_My; MyMy = *p_MyMy;
 Mz = *p_Mz; MzMz = *p_MzMz;

 Mx   = matalloc(Mx,   l_max_2, l_max_2, NUM_COMPLEX);
 My   = matalloc(My,   l_max_2, l_max_2, NUM_COMPLEX);
 Mz   = matalloc(Mz,   l_max_2, l_max_2, NUM_COMPLEX);

 Unity = NULL;
 Unity = matalloc(Unity, l_max_2, l_max_2, NUM_COMPLEX);

/*************************************************************************
  Set up Mz, Mp, Mm
(Eq 30)
  Mz(l1,m1,l3,m3) = sqrt(4*PI/3) * i^(l3-l1) (-1)^m3 * cg(l1,m1,1, 0,l3,-m3)
(Eq 31)
  Mp(l1,m1,l3,m3) = sqrt(4*PI/3) * i^(l3-l1) (-1)^m3 * cg(l1,m1,1, 1,l3,-m3)
  Mm(l1,m1,l3,m3) = sqrt(4*PI/3) * i^(l3-l1) (-1)^m3 * cg(l1,m1,1,-1,l3,-m3)

(Eq 32)
  Mx = 1/sqrt(2) * (Mm - Mp)
  My = i/sqrt(2) * (Mm + Mp)

!!! Note: BLM(l1,m1,l2,m3,l3,m3) = cg(l1,m1,l2,-m2,l3,-m3) !!!
*************************************************************************/

#ifdef CONTROL
 fprintf(STDCTR,"(pc_mk_ms): Start computing M_xyz matrices\n");
#endif

 for(i_el = 1, l1 = 0; l1 <= l_max; l1 ++)
   for(m1 = -l1; m1 <= l1; m1 ++)
   {
     for(l3 = 0; l3 <= l_max; l3 ++)
       for(m3 = -l3; m3 <= l3; m3 ++, i_el ++)
       {
         cri_powi(&faux_r, &faux_i, l3 - l1);

/* Eq. 30: m2 = 0, l2 = 1 */
      /* pref = SQRT_4PI3 * M1P(m3) * blm(l1, m1, 1, 0, l3,-m3); */
         pref = SQRT_4PI3 * M1P(m3) * cg(l1, m1, 1, 0, l3, m3);
         Mz->rel[i_el] = pref * faux_r;
         Mz->iel[i_el] = pref * faux_i;

/* Eq. 31: m2 = +/-1, l2 = 1 for blm/cg */
      /* pref = SQRT_4PI3 * M1P(m3) * blm(l1, m1, 1, 1, l3,-m3); */
         pref = SQRT_4PI3 * M1P(m3) * cg(l1, m1, 1,-1, l3, m3);
         Mp_r = pref * faux_r;
         Mp_i = pref * faux_i;

/* Eq. 31: m2 = -/+1, l2 = 1 for blm/cg */
      /* pref = SQRT_4PI3 * M1P(m3) * blm(l1, m1, 1,-1, l3,-m3); */
         pref = SQRT_4PI3 * M1P(m3) * cg(l1, m1, 1, 1, l3, m3);
         Mm_r = pref * faux_r;
         Mm_i = pref * faux_i;

/* Eq 32 */
         Mx->rel[i_el] = SQRT_2_1 * (Mm_r - Mp_r);
         Mx->iel[i_el] = SQRT_2_1 * (Mm_i - Mp_i);

         My->rel[i_el] = - SQRT_2_1 * (Mm_i + Mp_i);
         My->iel[i_el] =   SQRT_2_1 * (Mm_r + Mp_r);

       } /* m3 */
    } /* m1 */
 
#ifdef CONTROL_X
 fprintf(STDCTR,"(pc_mk_ms): Mz: \n");
 matshow(Mz);
 fprintf(STDCTR,"(pc_mk_ms): Mx: \n");
 matshow(Mx);
 fprintf(STDCTR,"(pc_mk_ms): My: \n");
 matshow(My);
#endif

/*************************************************************************
  Create MxMx, MyMy, MzMz
*************************************************************************/

 MxMx = matmul(MxMx, Mx, Mx);
 MyMy = matmul(MyMy, My, My);
 MzMz = matmul(MzMz, Mz, Mz);

#ifdef CONTROL_X
 fprintf(STDCTR,"(pc_mk_ms): MzMz: \n");
 matshowabs(MzMz);
 fprintf(STDCTR,"(pc_mk_ms): MxMx: \n");
 matshowabs(MxMx);
 fprintf(STDCTR,"(pc_mk_ms): MyMy: \n");
 matshowabs(MyMy);
#endif

/*************************************************************************
  Check trace of Sum MxMx + MyMy + MzMz: should be Unity
*************************************************************************/

 i_count = 0;
 trace_r = trace_i = 0.;

 for(i_el = 1, l1 = 0; l1 <= l_max; l1 ++)
   for(m1 = -l1; m1 <= l1; m1 ++)
     for(l3 = 0; l3 <= l_max; l3 ++)
       for(m3 = -l3; m3 <= l3; m3 ++, i_el ++)
       {
         Unity->rel[i_el] = MxMx->rel[i_el] + MyMy->rel[i_el] + MzMz->rel[i_el];
         Unity->iel[i_el] = MxMx->iel[i_el] + MyMy->iel[i_el] + MzMz->iel[i_el];
/* diagonal elements */
         if( (l1 == l3) && ( m1 == m3) )
         {
           trace_r += Unity->rel[i_el];
           trace_i += Unity->iel[i_el];
           
           if( (R_cabs(Unity->rel[i_el] -1., Unity->iel[i_el]) > M_TOLERANCE)
               && (l1 != l_max) )
           {
#ifdef ERROR
             fprintf(STDERR,
"*** error (pc_mk_ms): trace element (%d %d, %d %d) deviates from 1.: (%f,%f)\n",
                     l1,m1, l3, m3, Unity->rel[i_el], Unity->iel[i_el]);
#endif
             i_count ++;
           } /* if */
         } /* if */
/* off-diagonal elements */
         else
         {
           if( ( R_cabs(Unity->rel[i_el], Unity->iel[i_el]) > M_TOLERANCE )
               && (l1 != l_max) && (l3 != l_max) )
           {
#ifdef ERROR
             fprintf(STDERR,
    "*** error (pc_mk_ms): element (%d %d, %d %d) deviates from 0.: (%f,%f)\n",
                     l1,m1,l3,m3, Unity->rel[i_el], Unity->iel[i_el]);
#endif
             i_count ++;
           } /* if */
         } /* if */
       } /* m3 */

#ifdef CONTROL_X
 fprintf(STDCTR,"(pc_mk_ms): MxMx + MyMy + MzMz: \n");
 matshowabs(Unity);
#endif
 if( ! IS_EQUAL_REAL(R_cabs(trace_r, trace_i), 0.))
 {
   faux_r = R_cabs(trace_r - l_max_2, trace_i) / R_cabs(trace_r, trace_i);

#ifdef CONTROL
   fprintf(STDCTR,
   "(pc_mk_ms): rel. error in trace of MxMx + MyMy + MzMz: %.3f\n", faux_r);
#endif
 }

 *p_Mx = Mx; *p_MxMx = MxMx;
 *p_My = My; *p_MyMy = MyMy;
 *p_Mz = Mz; *p_MzMz = MzMz;

#ifdef CONTROL
 fprintf(STDCTR,"(pc_mk_ms): error counts (l < %d): %d\n", l_max, i_count);
#endif
 if(i_count > 0)
   i_count = - i_count;
 else
   i_count = 1;

 return (i_count);

} /* end of function leed_par_cumulative_tl */
