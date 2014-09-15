/*********************************************************************
  GH/13.09.95 
  file contains functions:

  c_bess             (13.09.95)

       Calculate all spherical Bessel functions J_l up to l = l_max for 
       a given complex argument (using Miller's algorithm).

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>

#include "mat.h"
#include "qm.h"

/*
#define CONTROL
#define CONTROL_X
*/
#define WARNING
#define ERROR

#define EXIT_ON_ERROR


#ifdef REAL_IS_FLOAT
#define ACC 30.0                 /* Accuracy for float */
#else
#define ACC 40.0                 /* Accuracy for double */
#endif


/*======================================================================*/
/*======================================================================*/

mat c_bess ( mat Jl, real z_r, real z_i, int l_max )

/************************************************************************

 Calculate all orders of the spherical Bessel function J_l up to l = l_max 
 for a given complex argument z (z_r + i*z_i) using Miller's device.
 
 INPUT: 

 mat Jl    - output: Bessel function in natural order (l = 0,1, ..., 
             see below)
 real z_r, z_i - real and imaginary part of the argument.
 int l_max - max angular momentum for output.

 DESIGN:

 The spherical Bessel functions J_l ( 0 <= l <= l_max) are calculated 
 using the recurrence formula:

  J_l (z) = (2*l-1)/z J_(l-1)(z) - J_(l-2)(z)

 for l < |z|. And an approximation (Miller's device cf. Abramowitz and 
 Stegun p. 452) for the remaining l_max > l > |z|. In any case, the first
 two Jl are calculated exactly:
  
  J_0(z) = sin(z)/z * (i);
  J_1(z) = sin(z)/z^2 - cos(z)/z;

 whereby
  
  sin(z) = -i/2 ( exp(iz) - exp(-iz) ) 
         = sin(z_r)*cosh(z_i) + i*cos(z_r)*sinh(z_i)
  cos(z) =  1/2 ( exp(iz) + exp(-iz) )
         = cos(z_r)*cosh(z_i) - i*sin(z_r)*sinh(z_i)

 Miller's device is described in some detail in the "Handbook of 
 mathematical functions" by Abramowitz and Stegun (p.452) and in Num. Rec.
 (p. 181/ p. 235). The start index is chosen as 

  l_start =  l_max + (int) R_sqrt(ACC*l_max)

 Where ACC determines the accuracy of the approximation (Num. Rec.: 40.0
 for double precision).
 
 RETURN VALUE:

 NULL, if failed

 Jl (may be different from input parameter). The storage scheme for Jl
 is in the natural order:

 l      0  1  2  3  4  ...
 index  1  2  3  4  5  ...

 I.e. index(l) = l + 1. 

*************************************************************************/
{
int l, l_int, l_start;                      /* quantum number l */
int iaux;

real faux_r, faux_i;
real z_inv_r, z_inv_i;
real z_2_r, z_2_i, pref_r, pref_i;
real sin_r, sin_i, cos_r, cos_i;

real *F_r, *F_i;

int j,jsum,m;
real ax,bj,bjm,bjp,sum,tox,ans;

 if (l_max < 1) l_max = 1;   /* we need at least that much storage */

/*
  Allocate memory for Jl
*/
 Jl = matalloc( Jl, (l_max+1), 1, NUM_COMPLEX );

/************************************************************************
 z == 0.:
 All Jl except J0 (= 1.) are zero.
************************************************************************/
 if( ( z_r == 0.) && ( z_i == 0.) )
 {
   Jl->rel[1] = 1.;
   Jl->iel[1] = 0.;
   
   for(l = 1 ; l <= l_max; l++ )
   {
     Jl->rel[l+1] = Jl->iel[l+1] = 0.;
   }
   return(Jl);
 }

/************************************************************************
 z != 0.:
 Use Miller's device to calculate Jl up to l_max.
************************************************************************/

 /*
  Some often used values:
  z_inv = 1/z
  z_2   = z^2
  sin(z), cos(z)
*/
 cri_div(&z_inv_r, &z_inv_i, 1., 0., z_r, z_i); 
 cri_mul(&z_2_r, &z_2_i, z_r, z_i, z_r, z_i);

/* faux_r/i are used for cos/sin(z_r) */
 if(z_r == 0.)
 { faux_r = 1.; faux_i = 0.; }
 else
 { faux_r = R_cos(z_r); faux_i = R_sin(z_r); }

/* pref_r/i are used for cosh/sinh(z_i) */
 if(z_i == 0.)
 { pref_r = 1.; pref_i = 0.; }
 else
 { pref_r = R_cosh(z_i); pref_i = R_sinh(z_i); }

/* sin(z) = sin(z_r)*cosh(z_i) + i*cos(z_r)*sinh(z_i) */
 sin_r = faux_i*pref_r;
 sin_i = faux_r*pref_i;

/* cos(z) = cos(z_r)*cosh(z_i) + i*sin(z_r)*sinh(z_i) */
 cos_r = faux_r*pref_r;
 cos_i = -faux_i*pref_i;

#ifdef CONTROL_X
 fprintf(STDCTR,"(c_bess-m): sin(z) = (%.3e,%.3e), cos(z) = (%.3e,%.3e)\n", 
         sin_r, sin_i, cos_r, cos_i);
#endif
 
/************************************************************************
  J0 and J1 are calculated from the exact formula:
  J0(z) = 1/z * sin(z)
  J1(z) = J0(z) * 1/z  - 1/z * cos(z) = 1/z * (J0(z) - cos(z))
************************************************************************/
 
/* J0(x) */
 cri_mul(Jl->rel+1, Jl->iel+1, z_inv_r, z_inv_i, sin_r, sin_i);

/* J1(x) */

/* (J0(z) - cos(z)) */
 faux_r = Jl->rel[1] - cos_r; 
 faux_i = Jl->iel[1] - cos_i;

/* 1/z * (J0(z) - cos(z)) */
 cri_mul(Jl->rel+2, Jl->iel+2, z_inv_r, z_inv_i, faux_r, faux_i);
 
#ifdef CONTROL
 fprintf(STDCTR,"(c_bess-m): J(%d) = (%.3e,%.3e)\n", 
                0, Jl->rel[1], Jl->iel[1]);
 fprintf(STDCTR,"(c_bess-m): J(%d) = (%.3e,%.3e)\n", 
                1, Jl->rel[2], Jl->iel[2]);
#endif

/************************************************************************
 loop over l:

 up to l_int = (int) (|z|) Jl is calculated according to the recurrence
 relation:

  Jl (z) = (2*l-1)/z Jl-1(z) - Jl-2(z)

 for higher values the above equation diverges.

 Jl is stored in Jl->r/iel[l+1]
************************************************************************/

 l_int = (int) cri_abs(z_r, z_i);
 l_int = MIN(l_int, l_max);

/* calculate Jl by upwards recurrence */
 for(l = 2; l <= l_int; l++)
 {
   faux_r = (2*l - 1) * z_inv_r;
   faux_i = (2*l - 1) * z_inv_i;

   cri_mul(Jl->rel+l+1, Jl->iel+l+1,
           faux_r, faux_i, Jl->rel[l], Jl->iel[l]);
   Jl->rel[l+1] -= Jl->rel[l-1];
   Jl->iel[l+1] -= Jl->iel[l-1];

#ifdef CONTROL
   fprintf(STDCTR,"(c_bess-m): (r)J(%d) = (%.3e,%.3e)\n",
           l, Jl->rel[l+1], Jl->iel[l+1]);
#endif
 }   /* for l */

/************************************************************************
  The remaining Jl are calculated according to Miller's formula 

 - Downwards recurrence from an l_start =  l_max + (int) R_sqrt(ACC*n)
    Fl_start   = 0.
    Fl_start-1 = 1.
    Fl-1 (z) = (2*l+1)/z Fl(z) - Fl+1(z)
   
 - Store coefficients for l in F_r/i[l].
 
  
************************************************************************/

 if(l_int < l_max)
 {
   l_start = l_max + (int) R_sqrt(ACC*l_max);
   F_r = (real *)malloc( (l_start+2) * sizeof(real) );
   F_i = (real *)malloc( (l_start+2) * sizeof(real) );

   F_r[l_start+1] = 0.; F_i[l_start+1] = 0.;
   F_r[l_start]   = 1.; F_i[l_start]   = 0.;
 
   for (l = l_start-1; l>0; l--) 
   {
     faux_r = (2*l + 1) * z_inv_r;
     faux_i = (2*l + 1) * z_inv_i;

     cri_mul(F_r+l, F_i+l, faux_r, faux_i, F_r[l+1], F_i[l+1]);
     F_r[l] -= F_r[l+2];
     F_i[l] -= F_i[l+2];
   }

/* calculate normalizing factor J0 / F0 */
   cri_div(&pref_r, &pref_i, Jl->rel[1], Jl->iel[1], F_r[1], F_i[1]);

   l_int = MAX(2, l_int+1);
   for(l = l_int; l <= l_max; l++)
   {
     cri_mul(Jl->rel+l+1, Jl->iel+l+1, pref_r, pref_i, F_r[l+1], F_i[l+1]);
#ifdef CONTROL
     fprintf(STDCTR,"(c_bess-m): (m)J(%d) = (%.3e,%.3e)\n",
             l, Jl->rel[l+1], Jl->iel[l+1]);
#endif
   }
#ifdef CONTROL
     fprintf(STDCTR,"(c_bess-m): l_start = %d, pref = (%.3e,%.3e)\n", 
             l_start, pref_r, pref_i);
#endif
   free(F_r);
   free(F_i);
 }  /* if l_int < l_max */

 return(Jl);

} /* end of function c_bess */

/*======================================================================*/
/*======================================================================*/
