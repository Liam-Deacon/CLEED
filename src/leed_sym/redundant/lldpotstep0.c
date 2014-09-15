/*********************************************************************
  GH/15.03.95 
  file contains functions:

  leed_ld_potstep0        (15.03.95)
     Calculate only the first column of the reflection matrix R+- (i.e. the
     amplitudes of the backscattered beams) for a (super) layer "a" vec_ab 
     away from a square potential step by layer doubling.

     Reflection at the potential step is ignored!

Changes:
 GH/15.03.95 - Creation

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>

#include "leed.h"


/*======================================================================*/
/*======================================================================*/

mat leed_ld_potstep0 ( mat Rpm1_ab, mat Rpm_a,
                leed_beam_t *beams, real eng_vac, real *vec_ab )

/************************************************************************

   Calculate only the first column of the reflection matrix R+- (i.e. the
   amplitudes of the backscattered beams) for a (super) layer "a" (can be 
   the complete bulk) vec_ab away from a square potential step 
   (z(a) < z(pot. step)) - Reflection at the potential step is ignored!

   This will produce output like in the VHT package.

   

 INPUT:

   mat Rpm1 - (output) fist column of the reflection matrix (+-)

   mat Rpm_a - (input) reflection matrix (+-) of the "lower" layer "a".

   beam_str *beams - (input) information about beams.
                  used: k_r, k_i, k_par.
   real eng_vac - vacuum energy (just for compatibility with leed_ld_potstep).
   real *vec_ab - (input) vector pointing from the origin of layer a to
                  the potential step. The usual convention for vectors is 
                  used (x = 1, y = 2, z = 3).

   z(a) < z(b) => vec_ab[3] > 0 (otherwise no convergence!)

 DESIGN:

   The first column of Rpm_a is multiplied with the propagator for
   the (00) beam towards the layer a (-) and the respective propagators
   away from the layer after scattering (+).

 RETURN VALUES:

   mat Rpm1 - fist column of the reflection matrix (+-) (not necessarily 
              equal to the first argument).

*************************************************************************/
{
int k,l;
int n_beams;                       /* total number of beams */

real faux_r, faux_i;
real *ptr_r, *ptr_i, *ptr_end;

mat Maux;                          /* temp. storage space */


 Maux = NULL;

/*************************************************************************
  Check arguments:
*************************************************************************/

/*************************************************************************
 - Allocate memory and set up propagator Maux. 

   Maux(k) = P-(00) * P+(k)

   Maux = exp[-i *( k_x0*v_ab_x + k_y0*v_ab_y - k_z0*v_ab_z) ] *
        exp[ i *( k_x*v_ab_x  + k_y*v_ab_y  + k_z*v_ab_z)  ]
      = exp[ i *( (k_x-k_x0)*v_ab_x + (k_y-k_y0)*v_ab_y + (k_z0+k_z)*v_ab_z)]

 - Multiply the first column of Rpm_a with the propagators to the potential
   step.
 - Multiply with the factor sqrt(cos(out)/cos(in))

*************************************************************************/
 n_beams = Rpm_a->cols;
 Maux = matalloc(NULL, n_beams, 1, NUM_COMPLEX );

 for(k = 1, l = 1; k <= n_beams; k ++, l+= n_beams)
 {

   faux_r = ((beams+k-1)->k_r[1] - beams->k_r[1]) * vec_ab[1] +
            ((beams+k-1)->k_r[2] - beams->k_r[2]) * vec_ab[2] + 
            ((beams+k-1)->k_r[3] + beams->k_r[3]) * vec_ab[3];
   faux_i = ((beams+k-1)->k_i[3] + beams->k_i[3]) * vec_ab[3];
   
   cri_expi(Maux->rel+k, Maux->iel+k, faux_r, faux_i);

/* sqrt(cos(out)/cos(in)): */
   faux_r = 2 * eng_vac - 
            SQUARE((beams+k-1)->k_r[1]) - SQUARE((beams+k-1)->k_r[2]);
   faux_i = 2 * eng_vac - SQUARE(beams->k_r[1]) - SQUARE(beams->k_r[2]);
   faux_r = R_sqrt(faux_r/faux_i);
   faux_r = R_sqrt(faux_r);
   cri_mul(Maux->rel+k, Maux->iel+k, faux_r, 0., Maux->rel[k], Maux->iel[k]);

   cri_mul(Maux->rel+k, Maux->iel+k,
           Rpm_a->rel[l], Rpm_a->iel[l], Maux->rel[k], Maux->iel[k]);

 }
 
/*************************************************************************
 - Write the result to the output pointer
 - Free temporary storage space Maux
 - Return.
*************************************************************************/

 Rpm1_ab = matcop(Rpm1_ab, Maux);
 matfree(Maux);

 return(Rpm1_ab);
}
 
/*======================================================================*/
/*======================================================================*/
