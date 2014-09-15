/*********************************************************************
  GH/21.01.95 
  file contains functions:

  leed_ld_2n            (18.01.95)
     Calculate the layer scattering matrices for a periodic stack layers
     by Pendry's layer doubling scheme

 Changes:
 GH/21.01.95 - change WARNING to CONTROL; CONTROL to CONTROL_X
 WB/16.04.98 - CONTROL vec_aa
*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "leed.h"

#ifndef LD_TOLERANCE
#define LD_TOLERANCE 0.001
#endif

/*======================================================================*/
/*======================================================================*/

mat leed_ld_2n (   mat Rpm,
              mat Tpp_a,  mat Tmm_a,  mat Rpm_a,  mat Rmp_a, 
              leed_beam_t *beams, real *vec_aa )

/************************************************************************

   Calculate the reflection matrix for a periodic stack of layers
   of the same type "a" by layer doubling:

 INPUT:

   mat Rmp   - (output) reflection matrix (-+) of the stack "aaaa...".

   mat Tpp_a - (input) transmission matrix (++) of the layer "a".
   mat Tmm_a - (input) transmission matrix (--) of the layer "a".
   mat Rpm_a - (input) reflection matrix (+-) of the layer "a".
   mat Rmp_a - (input) reflection matrix (-+) of the layer "a".

   beam_str *beams - (input) information about beams.
                  used: k_r, k_i.
   real *vec_aa - (input) vector pointing from the origin of one layer a to
                  the origin of the next layer a. The usual convention for 
                  vectors is used (x = 1, y = 2, z = 3).

 DESIGN:

   calculate the transmission and reflection matrices for 2, 4, 8, etc.
   identical layers iteratively in function "leed_ld_2lay" by applying Pendry's 
   layer doubling scheme:

   T(n+1)++ =                (Tn++ P+) * (I - Rn+- P- Rn-+ P+)^(-1) * Tn++
   R(n+1)-+ = Rn-+ + (Tn-- P- Rn-+ P+) * (I - Rn+- P- Rn-+ P+)^(-1) * Tn++

   T(n+1)-- =                (Tn-- P-) * (I - Rn-+ P+ Rn+- P-)^(-1) * Tn--
   R(n+1)+- = Rn+- + (Tn++ P+ Rn+- P-) * (I - Rn-+ P+ Rn+- P-)^(-1) * Tn--


   Use the moduli of the coefficients of Tpp (< LD_TOLERANCE) as convergence 
   criterion

   


 RETURN VALUES:

   Rpm reflection matrix (+-) of the periodic stack.

*************************************************************************/
{
int i_layer;

real abs_new;

mat Tpp, Tmm, Rmp;

 Tpp = Tmm = Rmp = NULL;

/*************************************************************************
  Check arguments and copy to internal variables:
*************************************************************************/

 Tpp = matcop(Tpp,Tpp_a);
 Tmm = matcop(Tmm,Tmm_a);
 Rpm = matcop(Rpm,Rpm_a);
 Rmp = matcop(Rmp,Rmp_a);


 abs_new = matabs(Tpp);

#ifdef CONTROL_X
 fprintf(STDCTR,"(leed_ld_2n):vec between periodic stacks(%.3f %.3f %.3f)\n",
         vec_aa[1] * BOHR,vec_aa[2] * BOHR,vec_aa[3] * BOHR);
#endif

 for (i_layer = 1; abs_new >  LD_TOLERANCE; i_layer *= 2)
     /*
       Tpp^2 (= abs_new^2) is approx. contribution to reflection matrix
       of electrons backscattered from the last layer.
     */
 {
   leed_ld_2lay( &Tpp, &Tmm, &Rpm, &Rmp, 
            Tpp, Tmm, Rpm, Rmp, Tpp, Tmm, Rpm, Rmp,
            beams, vec_aa);

   abs_new = matabs(Tpp)/(Tpp->cols*Tpp->rows); 
   
#ifdef CONTROL_X
   fprintf(STDCTR,
     "(leed_ld_2n): No. of layers (i_layer) = %3d, abs_new = %.1e, tol = %.1e\n",
     i_layer, abs_new, LD_TOLERANCE);
#endif
 }

#ifdef CONTROL
   fprintf(STDCTR,
           "\n(leed_ld_2n): No. of layers included in final iteration: %d;\n",
           i_layer);
   fprintf(STDCTR,"         modulus of transmission matrix: %.0e (tol: %.0e)\n",
           abs_new, LD_TOLERANCE);
#endif

/*
 matshowabs(Rpm);
*/

 matfree(Tpp);
 matfree(Tmm);
 matfree(Rmp);
 
 return(Rpm);
}
 
/*======================================================================*/
/*======================================================================*/
