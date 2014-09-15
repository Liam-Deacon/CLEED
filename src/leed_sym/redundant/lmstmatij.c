/*********************************************************************
  GH/10.07.95 
  file contains functions:

  leed_ms_tmat_ij

   Create the matrix product of a scattering matrix for a single Bravais
   layer "i" Tii and propagator matrix to another lattice "j" Gij used in 
   the combined space method to compute the matrix for scattering between 
   two (or more) periodic planes of scatterers.
Changes:
GH/01.02.95 - Creation

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "leed.h"

/*======================================================================*/

mat leed_ms_tmat_ij ( mat Gij, mat Llm, mat Tii, int l_max /* arguments */ )

/************************************************************************

 DESCRIPTION
 
   Create the matrix product of a scattering matrix for a single Bravais
   layer "i" Tii and propagator matrix to another lattice "j" Gij used in 
   the combined space method to compute the matrix for scattering between 
   two (or more) periodic planes of scatterers.


 INPUT:

   mat Gij  - (output) multiple scattering matrix in (l,m)-space.
   mat Llm  - (input) lattice sum: 

              Llm =  (-1)^m * 4 PI * Ylm (cos(theta) = 0, phi = 0) * 
                     * sum(R) [ H(1)l(k*|R|) * exp( i(-kin*R + m*phi(R)) ) ]

              (in natural order: (l,m) = (0,0); (-1,1); (0,1); (1,1); ...)

   mat Tii  - (input) Scattering matrix for the single periodic layer of type
              "i" (not used at the moment)
              
              Tii is produced by leed_ms_tmat_ii

   int l_max - max angular momentum quantum number.

 DESIGN:

   * General *

   First create -Gij:

 Gij(l1,m1; l2,m2) = -S[ L(l3,m3) * C(l3,m3,l1,m1,l2,m2)]

    L(l3,m3) = lattice sum (created in leed_ms_lsum_ij)
    C(l3,m3,l1,m1,l2,m2) = C.G. coefficients.

   Multiply with Tii from l.h.s.

 FUNCTION CALLS:
  
  cg    - C.G. coefficients
  mat*  - matrix library

 RETURN VALUE:

Gij 

NULL if failed (and EXIT_ON_ERROR is not defined)

*************************************************************************/
{
int iaux;
int l1,m1, l2,m2;
int off_row, off_ij;

int l3,m3;
int l3_min, l3_max;

int i3;

real faux_r, faux_i;
real sign;

/*************************************************************************
 Check the input matrices Llm and Tii
 Make sure that the C.G. coefficients are available 
 (by calling mk_cg_coef)
*************************************************************************/
 if (matcheck(Llm) < 1)
 {
#ifdef ERROR
   fprintf(STDERR," *** error (leed_ms_tmat_ij): improper input for lattice sum\n");
#endif
#ifndef EXIT_ON_ERROR
   return(NULL);
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#endif
 }

 if  (matcheck(Tii) < 1)
 {
#ifdef ERROR
   fprintf(STDERR,
           " *** error (leed_ms_tmat_ij): improper input for atomic scattering\n");
#endif
#ifndef EXIT_ON_ERROR
   return(NULL);
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#endif
 }

 if(mk_cg_coef(l_max) != 0)
 {
#ifdef WARNING
   fprintf(STDWAR,
     " * warning (leed_ms_tmat_ij): C.G. coefficients had to be recalculated\n");
#endif
 }

/* Allocate Gij */

 iaux = (l_max + 1) * (l_max + 1);
 Gij = matalloc(Gij, iaux, iaux, NUM_COMPLEX);

#ifdef CONTROL
 fprintf(STDCTR,"\n(leed_ms_tmat_ij): Llm:\n");
 for(i3 = 1; i3 <= Llm->rows; i3++)
  printf("(%8.5f, %8.5f)", Llm->rel[i3], Llm->iel[i3]);
 printf("\n");
#endif

/*************************************************************************
 Loop over (l1,m1),(l2,m2): Set up  -Gij(l1,m1; l2,m2).

 -Gij(l1,m1; l2,m2) = -S[ Lij(l3,m3) * C(l1,m1,l2,m2,l3,m3)]

    L(l3,m3) = lattice sum 
             = -8 PI * i^(l+1) *
               sum(P) [ Ylm (rj-ri+P) * H(1)l(k*|rj-ri+P|) * exp( i(-kin*P)]
    C(l3,m3,l1,m1,l2,m2) = C.G. coefficients.
*************************************************************************/
 for(l1 = 0, off_row = 0; l1 <= l_max; l1 ++)
 {
   for(m1 = -l1; m1 <= l1; m1 ++, off_row ++ )
   {
/*
     off_row = l1*(l1+1) - m1;
*/
     for(l2 = 0, off_ij = off_row * Gij->cols + 1; l2 <= l_max; l2 ++)
     {
       for(m2 = -l2; m2 <= l2; m2 ++, off_ij ++ )
       {
     /* 
       Loop over l3 (inner-most loop): calculate the elements of Gij

        - l3_min: l3 >= |m3|, |l2-l1|.
        - Only even (l1 + l2 + l3) lead to nonzero C.G.-coefficients
          => if (l1 + l2 + l3_min) is odd, increment l3_min by 1.
          => increment l3 by 2 in the inner loop.
        - The sign (-1)^(m2+1) is due to different definitions of C.G.C's 
          and the "-" in front of the summation.
     */
/*
         off_ij = off_row * Gij->cols + l2*(l2+1) - m2 + 1;
         m3 = m2 - m1;
*/
         m3 = m2 - m1;

         l3_min = MAX(abs(-m3), abs(l2-l1)); 
         l3_min += (l1 + l2 + l3_min)%2;
         l3_max = l2+l1;

         sign = M1P(m2+1);


         off_ij = (l1*(l1+1) - m1) * Gij->cols + l2*(l2+1) - m2 + 1;
         Gij->rel[off_ij] = Gij->iel[off_ij] = 0.;

         i3 = l3_min*(l3_min + 1) - m3 + 1;

         for(l3 = l3_min; l3 <= l3_max; l3 += 2 )
         {

           faux_r = sign*cg(l3,+m3, l2,+m2, l1,-m1);
/*
           printf("%2d %2d %2d %2d %2d %2d : %.5f\n", 
                   l1,m1, l2,m2, l3,m3, faux_r);
*/
           Gij->rel[off_ij] += Llm->rel[i3] * faux_r;
           Gij->iel[off_ij] += Llm->iel[i3] * faux_r;

         /*
           l3 is incremented by 2 
           => i3 is incremented by 2*(l3+1) + 2*(l3+2) = 4*l3 + 6
           sign = -sign;
         */
           i3 += 4*l3 + 6;
         }
       }  /* m2 */
     }  /* l2 */
   }  /* m1 */
 }  /* l1 */

#ifdef CONTROL
 fprintf(STDCTR,"\n(leed_ms_tmat_ij): Gij: \n");
 matshow (Gij);
#endif

/*************************************************************************
 Final matrix multiplication: Gij -> -Tii * Gij
*************************************************************************/
 Gij = matmul(Gij, Tii, Gij);

#ifdef CONTROL
 fprintf(STDCTR,"\n(leed_ms_tmat_ij): Tii*Gij: \n");
 matshow (Gij);
#endif

 return(Gij);
} /* end of function leed_ms_tmat_ij */

/*======================================================================*/
/*======================================================================*/
