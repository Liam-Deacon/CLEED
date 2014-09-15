/*********************************************************************
  GH/18.09.02 
  file contains functions:

  leed_ms_lsum_ij            (01.02.95)
     Calculate the lattice sum Llm used for the Greens function between 
     two periodic planes of scatterers "i" and "j".

Changes:
GH/01.02.95 - Creation
GH/17.07.95 - Change signs
GH/18.09.02 - change summation boundaries for n1 and n2 so that they comply
              with the general case of dij != 0.

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "leed.h"

#ifdef WARNING
#define WARN_LEVEL 1000
#endif

#ifndef GEO_TOLERANCE           /* should be defined in "leed_def.h" */
#define GEO_TOLERANCE 0.0001
#endif

/*======================================================================*/
/*======================================================================*/

int leed_ms_lsum_ij ( mat *p_Llm_p, mat *p_Llm_m, 
                 real k_r, real k_i, real *k_in, 
                 real *a, real *d_ij, 
                 int l_max, real epsilon )

/************************************************************************

 Calculate the lattice sum Llm used for the Greens function between two
 periodic planes of scatterers "i" and "j".
 Different prefactor from leed_ms_lsum_ii !

 INPUT:

   mat *p_Llm_p, *p_Llm_m - (pointer to output) lattice sums for +/- d_ij 
               (see below).
   real k_r, k_i - (input) real and imag.part of |k|.
   real k_in   - (input) Incident k-vector: k_in[1] = k_in_x, k_in[2] = k_in_y.
   real *a[5]  - (input) Basis vectors of the real 2-dim unit cell:
               a[1] = a1_x, a[2] = a2_x, a[3] = a1_y, a[4] = a2_y;
   real *d_ij[4] - (input) vector pointing from the origin of lattice "j" to
               the origin of lattice "i".
   int l_max    - (input) max l quantum number.
   real epsilon - (input) defines the radius of summation:
               if epsilon < 1. : cut off value for the amplitude of the
                                 wave function.
               if epsilon >= 1.: radius.

 DESIGN:

   * General *

   Calculate the lattice sums Llm_p and Llm_m which are to be multiplied 
   with the Clebsh-Gordan coefficients in order to get the matrix elements 
   of the Greens functions Gij and Gji, resp., for two periodic planes of 
   scatterers (according to p. 50 VHT with modifications).

     Llm_p = -8 PI * kin * i^(l+1) * 
           sum(P) [ Ylm (rj-ri+P) * H(1)l(k*|P + d_ij|) * exp( i(-kin*P)]
     Llm_m = -8 PI * kin * i^(l+1) *
           sum(P) [ Ylm (ri-rj+P) * H(1)l(k*|P - d_ij|) * exp( i(-kin*P)]

     k =     k_r (length of the electron wave vector) +
           i*k_i (damping constant, must be > 0.).
     kin = k-vector of the incoming wave.
     H(1)l Hankel function of the first kind.
     P =   lattice vector (runs over all positions in the plane, 
           i.e. Rz = 0.).

     d_ij = ri - rj = vector between the origins of the two layers "i" 
           and "j".

   If P is a lattice point, -P is automatically also a lattice point; 
   because -(P + d_ij) = -P + -d_ij, the lattice sum Llm_m for -d_ij can
   therefore be evaluated as sum over (-P) along with Llm_p without much 
   additional work using the following identities:
     
     Ylm (-P -d_ij) = Ylm(-cos(theta), phi+PI) = (-1)^l Ylm(P + d_ij);
     H(1)l(k*|-P -d_ij|) = H(1)l(k*|P + d_ij|); 
     exp( i(-kin*(-P)) = exp( i(kin*P) = exp( i(-kin*P)*; 



   * Radius of the Summation (for two layers) *
   
   The radius up to which the lattice points are summed up is determined
   from the damping constant k_i and a lower boundary for the modulus
   of the Hankle function epsilon:

     r_m = - ln(epsilon) / k_i or epsilon (see above)

   r_max be the square of the maximum radius r_m, then for all lattice 
   points within this radius the equation holds:
  
     r_max > (d_x + n1*a1_x + n2*a2_x)^2 + (d_y + n1*a1_y + n2*a2_y)^2 + d_z^2 =
             n1^2 * f1 + n2^2 * f2 + 
             2n1*n2 * f12 + 2n1 * f1d + 2n2 * f2d + fd

   where

     f1  = (a1_x^2 + a1_y^2);
     f2  = (a2_x^2 + a2_y^2);
     f12 = (a1_x*a2_x + a1_y*a2_y);
     f1d = (a1_x*d_x + a1_y*d_y);
     f2d = (a2_x*d_x + a2_y*d_y);
     fd  = (d_x^2 + d_y^2 + d_z^2);

   The solution of the quadratic equation for n2 is:
 
     n2_max/min = -(n1*f12 + f2d)/f2 +/- 
              sqrt( (n1*f12 - f2d)^2 - f2*(n1^2*f1 + 2n1*f1d + fd - r_max) )/f2 

   The boundaries for n1 are given by the condition that the argument of
   the square root must be positive:
     
     n1_max/min = -fb/fa +/- sqrt( fb^2 - 4fafc)/fa

     fa = (f12^2 - f1*f2)
     fb = (f12*f2d - f1d*f2)
     fc = (f2d^2 - fd*f2 + r_max*f2)

 RETURN VALUES:

   0 if failed (and EXIT_ON_ERROR is not defined)
   1 if o.k.

   Llm_p/m (may be different from input parameter). The storage scheme for 
   Llm_p/m is in the natural order:

   l      0  1  1  1  2  2  2  2  2  3  3  3  3  3  3  3  4  4 ...
   m      0 -1  0  1 -2 -1  0  1  2 -3 -2 -1  0  1  2  3 -4 -3 ...
   index  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 ...
 
   I.e. index(l,m) = l*(l+1) + m + 1. Note that, like usually for matrices,
   the first array element Llm_p/m[0] is not occupied.

*************************************************************************/
{
int l,m;                       /* quantum numbers l,m */
int off;                       /* offset in the arrays Ylm and Llm */
int i, iaux;

int n1, n1_min, n1_max;        /* counters for lattice vectors */
int n2, n2_min, n2_max;

real f1, f2, f12;              /* factors used in determining n1, n2 */
real f1d, f2d, fd;
real fa, fb, fc;

real r0_x, r0_y, r_x, r_y, r_z;     /* lattice vectors between the planes */
real p0_x, p0_y, p_x, p_y;          /* lattice vectors in the plane */
real r_max, r_abs, r_phi;
real a1_x, a1_y, a2_x, a2_y;        /* basic lattice vectors */

real faux_r, faux_i;
real fauxp_r, fauxp_i;
real fauxm_r, fauxm_i;
real exp_ikp_i, exp_ikp_r;

mat Llm_p, Llm_m;              /* lattice sums */
mat Hl;                        /* Hankel function */
mat Ylm;                       /* spherical harmonics */
mat pref;                      /* -8*PI * i^(l+1) */

Hl = Ylm = pref = NULL;

Llm_p = *p_Llm_p;
Llm_m = *p_Llm_m;

/*
  Check arguments: k_i
*/

 if( k_i <= 0.)                /* no convergence */
 {
#ifdef ERROR
   fprintf(STDERR," *** error (leed_ms_lsum_ij): damping too small: k_i = %.2e\n",
           k_i);
#endif

#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(0);
#endif
 }

/*
  Allocate memory for Llm_p and Llm_m (and preset all Llm_p with zero).
*/
 iaux = (l_max + 1)*(l_max + 1);
 *p_Llm_p = Llm_p = matalloc( *p_Llm_p, iaux, 1, NUM_COMPLEX );
 *p_Llm_m = Llm_m = matalloc( *p_Llm_m, iaux, 1, NUM_COMPLEX );

/*
  Allocate memory for pref (and preset all Llm_p with zero).
  pref(l+1) = -8 * PI * k_0 * i^(l+1)
  (k_0 = k_r + ik_i)
*/
 iaux = l_max + 1;
 pref = matalloc( pref, iaux, 1, NUM_COMPLEX );
 
 pref->rel[0] = - 8*PI*k_r;
 pref->iel[0] = - 8*PI*k_i;
 for(l = 1; l <= iaux; l ++)
 {
    pref->rel[l] = -pref->iel[l-1];
    pref->iel[l] =  pref->rel[l-1];
 }

/*
  Quantities used to calculate the cut off radius and counters:
*/

 if (epsilon < 1.) r_max = - log(epsilon) / k_i;
 else              r_max = epsilon;

#ifdef WARNING
 if( r_max > WARN_LEVEL)    /* poor convergence */
   fprintf(STDWAR,
     " *** warning (leed_ms_lsum_ij): damping very weak: k_i = %.2e, eps = %.2e\n",
     k_i, epsilon);
#endif

 r_max *= r_max;

 a1_x = a[1]; a1_y = a[3];
 a2_x = a[2]; a2_y = a[4];

/* quantities used to determine the boundaries for counters */
 f1  = a1_x*a1_x + a1_y*a1_y;
 f2  = a2_x*a2_x + a2_y*a2_y;
 f12 = a1_x*a2_x + a1_y*a2_y;
 f1d = a1_x*d_ij[1] + a1_y*d_ij[2];
 f2d = a2_x*d_ij[1] + a2_y*d_ij[2];
 fd  = d_ij[1]*d_ij[1] + d_ij[2]*d_ij[2] + d_ij[3]*d_ij[3];

#ifdef CONTROL
 fprintf(STDCTR,
   "(leed_ms_lsum_ij): a1  = (%.3f,%.3f) A, a2  =  (%.3f,%.3f) A\n", 
     a1_x*BOHR, a1_y*BOHR, a2_x*BOHR, a2_y*BOHR);
 fprintf(STDCTR,
   "              d_ij = (%7.3f,%7.3f,%7.3f) A\n", 
     d_ij[1]*BOHR, d_ij[2]*BOHR, d_ij[3]*BOHR);
 fprintf(STDCTR,
   "              k_in = (%7.3f,%7.3f) A^-1\n", k_in[1]/BOHR, k_in[2]/BOHR);
 fprintf(STDCTR,
   "              eps = %7.5f, k_i = %7.4f A^-1, r_max = %7.3f A\n",
    epsilon, k_i/BOHR, R_sqrt(r_max)*BOHR);
#endif

   
/************************************************************************ 
  Summation over lattice points r = n1*a1 + n2*a2 +/- dij
  k_in != 0, d_ij[3] != 0: General case
************************************************************************/
  
 {
/*
     n1_max/min = -fb/fa +/- sqrt( fb^2 - fafc)/fa

     fa = (f12^2 - f1*f2)
     fb = (f12*f2d - f1d*f2)
     fc = (f2d^2 - fd*f2 + r_max*f2)
*/

   fa = f12 * f12 - f1 * f2;                     /* fa is always < 0. */
   fb = f12*f2d - f1d*f2;
   fc = f2d * f2d - f2 * fd + f2 * r_max;

   faux_i =  fb * fb - fa * fc;
   if (faux_i > 0.) 
     faux_i = R_sqrt( fb * fb - fa * fc ) / fa;
   else 
     faux_i = 0.;                                /* faux_i always <= 0. */

   faux_r = -fb / fa;

   n1_min = (int) R_nint(faux_r + faux_i);
   n1_max = (int) R_nint(faux_r - faux_i);

#ifdef CONTROL
       fprintf(STDCTR, "faux: %f, %f n1_min = %d, n1_max = %d\n", 
               faux_r, faux_i, n1_min, n1_max);
#endif

   for ( p0_x = - (n1_max-0)*a1_x, p0_y = - (n1_max-0)*a1_y, 
         n1 = n1_min; n1 <= n1_max; n1 ++,
         p0_x += a1_x, p0_y += a1_y)
   {

/*
     n2_max/min = -(n1*f12 + f2d)/f2 +/- 
            sqrt( (n1*f12 - f2d)^2 - f2*(n1^2*f1 + 2n1*f1d + fd - r_max) )/f2 
*/

     fb = n1*f12 + f2d;
     fc = f1*n1*n1 + 2*f1d*n1 + fd - r_max;

     faux_r = -fb/f2;
     faux_i = R_sqrt( fb*fb - f2*fc ) / f2;          /* is always > 0. */

     n2_min = (int) R_nint(faux_r - faux_i);
     n2_max = (int) R_nint(faux_r + faux_i);

#ifdef CONTROL
       fprintf(STDCTR, "n1 = %3d,\tn2_min = %3d,\tn2_max = %3d\n", n1, n2_min, n2_max);
#endif

     for ( p_x = p0_x + (n2_min+0)*a2_x, p_y = p0_y + (n2_min+0)*a2_y,
           n2 = n2_min; n2 <= n2_max; n2 ++,
           p_x += a2_x, p_y += a2_y)
     {
       r_x = +d_ij[1] + p_x;
       r_y = +d_ij[2] + p_y;
       r_z = +d_ij[3];
       r_abs = SQUARE(r_x) + SQUARE(r_y) + SQUARE(r_z);

     /* 
        Make sure, r is inside the radius (r_max is square of the max. 
        radius) and the origin is not included in the summation. 
     */
       
#ifdef CONTROL_X
       fprintf(STDCTR, "r_abs = %e, r_max = %e, ", r_abs, r_max);
       fprintf(STDCTR, "n1_max = %d, n2_min = %d, n2_max = %d\n", 
                        n1_max, n2_min, n2_max);
#endif

       if ( (r_abs < r_max) && (r_abs > GEO_TOLERANCE ) )
       {
         r_abs = R_sqrt(r_abs);
         Hl = c_hank1 ( Hl, k_r*r_abs, k_i*r_abs, l_max);

       /* 
          Prepare arguments for Ylm:
            cos(theta) = r_z/r_abs
            phi = arctan(r_y/r_x)
       */
         r_phi = R_atan2(r_y, r_x);
         Ylm = r_ylm(Ylm, r_z/r_abs, r_phi, l_max);

       /* Prepare prefactor exp(-ikP) */
         faux_r = +k_in[1]*p_x + k_in[2]*p_y;
         cri_expi(&exp_ikp_r, &exp_ikp_i, -faux_r, 0.); /* exp(-i*k_in*p) */
       
       /* 
         loops over l and m:
       */
         for(l = 0; l <= l_max; l++ )
         {
           off = l*(l+1) + 1;
  
       /* 
         calculate m-independent prefactors: 
          (-1)^l * -8PI * (i)^(l+1) * Hl * exp(-ikP)  for Llm_p
                   -8PI * (i)^(l+1) * Hl * exp(+ikP)  for Llm_m
       */
           cri_mul(&faux_r, &faux_i, pref->rel[l+1], pref->iel[l+1], 
                   Hl->rel[l+1], Hl->iel[l+1]);

           cri_mul(&fauxp_r, &fauxp_i, faux_r, faux_i, 
                   exp_ikp_r, +exp_ikp_i);              /* for Llm_p */

/*
           faux_r *= M1P(l);
           faux_i *= M1P(l);
*/
           cri_mul(&fauxm_r, &fauxm_i, faux_r, faux_i,
                   exp_ikp_r, -exp_ikp_i);              /* for Llm_m */

           
           for(m = -l; m <= l; m ++)
           {
           /* Hl * exp(-ikp) * Ylm  */
             cri_mul(&faux_r, &faux_i, Ylm->rel[off+m], Ylm->iel[off+m],
                     fauxp_r, fauxp_i);
             Llm_p->rel[off + m] += faux_r*M1P(l+m);
             Llm_p->iel[off + m] += faux_i*M1P(l+m);

             cri_mul(&faux_r, &faux_i, Ylm->rel[off+m], Ylm->iel[off+m],
                     fauxm_r, fauxm_i);
             Llm_m->rel[off + m] += faux_r*M1P(m);
             Llm_m->iel[off + m] += faux_i*M1P(m);
           }  /* m */
         }    /* l */
       }    /* if r < r_max */
     }  /* lattice vectors a2 */
   }    /* lattice vectors a1 */
 }  /* end of k_in != 0 */

 matfree(pref);
 matfree(Hl);
 matfree(Ylm);

 return(1);

} /* end of function leed_ms_lsum_ij */

/*======================================================================*/
/*======================================================================*/
