/*********************************************************************
GH/16.07.02 
  file contains functions:

  leed_ms_tmat_nd_ii

    Create the multiple scattering Matrix for a periodic plane of
    scatterers using a non-diagonal atomic/molecular scattering matrix.

  creation:
GH/11.01.00 - copy from lmstmatii.c
GH/02.05.00 - use transposed of X-matrix: ((1 - Tlm*Gii)^-1)^t
GH/22.09.00
GH/16.07.02 - fix bug cg/gaunt in summation over l3 (Gii)

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>

#include "leed.h"

#ifdef WARNING
#define WARN_LEVEL 1000
#endif

/*======================================================================*/
/*======================================================================*/

mat leed_ms_tmat_nd_ii ( mat Tii, mat Llm, mat Tlm_in, int l_max /* arguments */ )

/************************************************************************

 DESCRIPTION
 
   Create the multiple scattering Matrix [(1 - Tl*Giilm)^-1]*Tl for a
   periodic plane of scatterers.

 INPUT:

   mat Tii  - output: multiple scattering matrix in (l,m)-space.
   mat Llm  - input: lattice sum: 

              Llm =  (-1)^m * 4 PI * Ylm (cos(theta) = 0, phi = 0) * 
                     * sum(R) [ H(1)l(k*|R|) * exp( i(-kin*R + m*phi(R)) ) ]

              (in natural order: (l,m) = (0,0); (-1,1); (0,1); (1,1); ...)

   mat Tlm_in - (input) non-diagonal Atomic scattering matrix (square)
              
              (in natural order: (l,m) = (0,0); (-1,1); (0,1); (1,1); ...)

   int l_max  - max angular momentum quantum number.

 DESIGN:

   * General *

 First create:

   Gii(l1,m1; l2,m2) = 
       S[ (-1)^((l1-l2-l3_min)/2 - m2) * L(l3,m3) * C(l3,m3,l1,m1,l2,m2)]

    L(l3,m3) = lattice sum
    C(l3,m3,l1,m1,l2,m2) = C.G. coefficients.

 Then compute:
   Tii =  Tlm * (1 - Gii * Tlm)^-1

 Multiply each matrix element with i^(l2-l1).


 RETURN VALUE:

Tii = [Tlm (1 - Gii*Tlm)^-1] * i^(l-l')

*************************************************************************/
{
int iaux;
int l1,m1, l2,m2;
int l3,m3;
int l3_min, l3_max;

int i3;
int ilm1, ilm2;

real faux_r, faux_i;
real sum_r, sum_i;
real sign;

mat Tlm;
mat Gii;

/*************************************************************************
Check matrix dimensions of Tlm_in and compatibilities with l_max. 
 - allocate Tii and Tlm.
 - check dimension of Tlm_in:
   increase or reduce if necessary.
   inside the fuction a copy (Tlm) is used.
 - check number of C.G> coefficients
*************************************************************************/

/* Allocate Tii and Tlm.  */

 Tlm = NULL;
 iaux = (l_max + 1)*(l_max + 1);
 Tlm = matalloc( Tlm, iaux, iaux, NUM_COMPLEX);

 if (matcheck (Tlm_in) == 1)
 {
   if ((Tlm_in->num_type) == (MAT_SQUARE | NUM_COMPLEX))
   {
     if (Tlm_in->rows < Tlm->rows)
     /* 
        insert Tlm_in in greater Tlm
     */
     {
#ifdef WARNING
       fprintf(STDWAR,
       "* warning (leed_ms_tmat_nd_ii): increase dimension of Tlm from %d to %d\n",
       Tlm_in->rows, Tlm->rows);
#endif
       Tlm = matins(Tlm, Tlm_in, 1, 1);
     }

     else if (Tlm_in->rows == Tlm->rows)
     {
       Tlm = matcop(Tlm, Tlm_in);
     }

     else if (Tlm_in->rows > Tlm->rows)
     /* 
        Reduce dimesions of Tlm_in.
        Use l for rows and m for columns (1 for Tlm_in, 2 for Tlm).
        ilm counts elements.
     */
     {
#ifdef WARNING
       fprintf(STDWAR,
       "* warning (leed_ms_tmat_nd_ii): reduce dimension of Tlm from %d to %d\n",
       Tlm_in->rows, Tlm->rows);
#endif
       for(ilm1 = 1, ilm2 = 1 , l2 = 1; l2 <= Tlm->rows; l2 ++)
       {
         for(m1 = 1, m2 = 1; m1 <= Tlm_in->cols; m1 ++, ilm1 ++) 
         {
           if ( m2 <= Tlm->cols)
           {
             Tlm->rel[ilm2] = Tlm_in->rel[ilm1];
             Tlm->iel[ilm2] = Tlm_in->iel[ilm1];
             m2 ++;
             ilm2 ++;
           }
         }  /* l2 */
       }  /* m1, m2 */
     } /* else if (size) */
   } /* if num_type */
   else
#ifdef ERROR
     fprintf(STDERR,
     "*** error (leed_ms_tmat_nd_ii): Tlm does not have the right matrix type (square, complex)\n");
#endif
 }
 else
#ifdef ERROR
   fprintf(STDERR,
     "*** error (leed_ms_tmat_nd_ii): Tlm does not point to a matrix\n");
#endif


#ifdef CONTROL
 fprintf(STDCTR,"\n(leed_ms_tmat_nd_ii): Tlm: \n");
/* matshowabs(Tlm); */
#endif

/*************************************************************************
 Allocate
   - C.G. coefficients for G matix.
   - memory for G matrix
 
*************************************************************************/

 if(mk_cg_coef(2*l_max) != 0)
 {
#ifdef WARNING
   fprintf(STDWAR,
     "* warning (leed_ms_tmat_nd_ii): C.G. coefficients had to be recalculated\n");
#endif
   ;
 }

/*************************************************************************
 Loop over (l1,m1),(l2,m2): Set up  - Gii(l1,m1; l2,m2).

 Gii(l1,m1; l2,m2) = 
       S[ (-1)^((l1-l2-l3_min)/2 - m2) * L(l3,m3) * C(l3,m3,l1,m1,l2,m2)]
    L(l3,m3) = lattice sum 
    C(l3,m3,l1,m1,l2,m2) = C.G. coefficients.

  - Gii(l1,m1; l2,m2)  is symmetric under exchange of (l1,m1) and (l2,m2), 
    (under most conditions)
    therefore only (l2,m2) <= (l1,m1) could be calculated which is,
    however not implemented (see comments in m2 loops). 
*************************************************************************/

 Gii = NULL;
 iaux = (l_max + 1)*(l_max + 1);
 Gii = matalloc( Gii, iaux, iaux, NUM_COMPLEX);

 for(l1 = 0, ilm1 = 1; l1 <= l_max; l1 ++)
 {
   for(m1 = -l1; m1 <= l1; m1 ++)
   {
     for(l2 = 0; l2 <= l_max; l2 ++)
     {
       for(m2 = -l2; m2 <= l2; m2 ++, ilm1 ++)
       {

     /*
        Loop over l3 (inner--most loop): calculate the elements of Gii
        - l3_min: l3 >= |m3|, |l2-l1|.
        - Only even (l1 + l2 + l3) lead to nonzero C.G.-coefficients
          => if (l1 + l2 + l3_min) is odd, increment l3_min by 1.
     */

/* m3 = m2 - m1; */
         m3 = m1 - m2;

         l3_min = MAX(abs(m3), abs(l2-l1));
         l3_min += (l1 + l2 + l3_min)%2;
         l3_max = l2+l1;

         sum_r = sum_i = 0.;
         i3 = l3_min*(l3_min + 1) - m3 + 1;

/* sign = M1P( (l2 - l1 - l3_min)/2 - m2); */
         sign = M1P( (l1 - l2 - l3_min)/2 - m2);

         for(l3 = l3_min; l3 <= l3_max; l3 += 2 )
         {
/* faux_r = sign*gaunt(l1, m1, l3, m3, l2,m2); */
           faux_r = sign*cg(l3, m3, l1,m1,l2,-m2);

           sum_r += Llm->rel[i3] * faux_r;
           sum_i += Llm->iel[i3] * faux_r;

         /*
           l3 is incremented by 2
           => l3/2 is incremented by 1; the sign is inverted.
           => i3 is incremented by 2*(l3+1) + 2*(l3+2) = 4*l3 + 6
         */
           sign = -sign;
           i3 += 4*l3 + 6;
         }  /* l3 */

       /*
         Store -i * sum in Gii (to be multiplied with Tlm). The factor i 
         is a correction necessary to get the same results as in XM (VHT). 
         Don't ask why!
       */

         Gii->rel[ilm1] =  sum_i;
         Gii->iel[ilm1] = -sum_r;

       }  /* m2 */
     }  /* l2 */
   }  /* m1 */
 }  /* l1 */

#ifdef CONTROL
 fprintf(STDCTR,"\n(leed_ms_tmat_nd_ii): -Gii: \n");
/* matshowabs(Gii); */
#endif

/*************************************************************************
 Multiply with Tlm from the r.h.s. : -Gii * Tlm
 Store in Tii, Gii not needed anymore.
*************************************************************************/

 Tii = matmul(Tii, Gii, Tlm);
 matfree(Gii);

#ifdef CONTROL
 fprintf(STDCTR,"\n(leed_ms_tmat_nd_ii): -Gii * Tlm: \n");
/* matshowabs(Tii); */
#endif

/*************************************************************************
 Add the identity matrix: (1 - Gii * Tlm )
 Store in Tii
*************************************************************************/
 iaux = Tii->cols*Tii->rows;
 for( ilm1 = 1; ilm1 <=iaux; ilm1 += Tii->rows + 1)
  Tii->rel[ilm1] += 1.;

#ifdef CONTROL
 fprintf(STDCTR,"\n(leed_ms_tmat_nd_ii): (1 - Gii * Tlm ): \n");
#endif

/*************************************************************************
 Matrix inversion: (1 - Gii * Tlm )^-1
 Store in Tii
*************************************************************************/

 Tii = matinv(Tii, Tii);

#ifdef CONTROL
 fprintf(STDCTR,"\n(leed_ms_tmat_nd_ii): (1 - Gii * Tlm )^-1: \n");
#endif
 
/*************************************************************************
  Multiply with Tl from the l.h.s: 
          Tlm * (1 - Gii * Tlm)^-1 
*************************************************************************/

 Tii = matmul(Tii, Tlm, Tii);

/*************************************************************************
  Multiply matrix elements with i^(l2-l1).
  l1 is the index which goes with the Ylm of the outgoing beams (-)
  l2                  -"-                        incoming beams (+)
*************************************************************************/

 for(l1 = 0, ilm1 = 1; l1 <= l_max; l1 ++)
 {
   for(m1 = -l1; m1 <= l1; m1 ++) 
   {
     for(l2 = 0; l2 <= l_max; l2 ++)
     {
       for(m2 = -l2; m2 <= l2; m2 ++, ilm1 ++)
       {
         cri_powi(&faux_r, &faux_i, l2-l1);
         cri_mul(Tii->rel+ilm1, Tii->iel+ilm1, Tii->rel[ilm1], Tii->iel[ilm1], 
                 faux_r, faux_i);
       }  /* m2 */
     }  /* l2 */
   }  /* m1 */
 }  /* l1 */

#ifdef CONTROL
 fprintf(STDCTR, "\n(leed_ms_tmat_nd_ii): Tii = Tlm * (1 - Gii * Tlm)^-1:\n");
#endif

 return(Tii);
} /* end of function leed_ms_tmat_nd_ii */

/*======================================================================*/
/*======================================================================*/
