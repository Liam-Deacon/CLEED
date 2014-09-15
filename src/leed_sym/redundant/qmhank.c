/*********************************************************************
  GH/09.08.94 
  file contains functions:

  r_hank1            (09.08.94)

       Calculate all Hankel functions H(1)l up to l = l_max for a given 
       real argument.

  c_hank1            (09.08.94)

       Calculate all Hankel functions H(1)l up to l = l_max for a given 
       complex argument.

  (Tested for PI/2, PI, i and -i.)

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "mat.h"
#include "qm.h"

/*
#define CONTROL
*/
#define CONTROL
#define WARNING
#define ERROR

#define EXIT_ON_ERROR

/*======================================================================*/
/*======================================================================*/

mat r_hank1 ( mat Hl, real x, int l_max )

/************************************************************************

 Calculate all orders of the Hankel function of the first kind H(1)l up to 
 l = l_max for a given real argument x.
 
 Input: 

 mat Hl    - output: Hankel function of the first kind in natural order 
             (l = 0,1, ..., see below)
 real x    - real argument.
 int l_max - max angular momentum for output.

 Design:

 The Hankel functions H(1)l are calculated from the recurrence
 relation:

  H(1)l (x) = (2*l-1)/x H(1)l-1(x) - H(1)l-2(x)

 knowing the exact formula for H(1)0 and H(1)1:
  
  H(1)0(x) = - exp(ix)/x * (i) = sin(x)/x - i*cos(x)/x.
  H(1)1(x) = - exp(ix)/x * (i/x + 1) = H(1)0(x) * (1/x - i).

 Variables used within the function:
 
 Return value:

 NULL, if failed

 Hl (may be different from input parameter). The storage scheme for Hl
 is in the natural order:

 l      0  1  2  3  4  ...
 index  1  2  3  4  5  ...

 I.e. index(l,m) = l + 1. 

*************************************************************************/
{
int l;                        /* quantum number l */

real faux;
real z_inv;
real *ptr_r, *ptr_i;


/*
  Check arguments x and l_max
*/

 if( x == 0.)
 {
#ifdef ERROR
   fprintf(STDERR," *** error (r_hank1): invalid argument x = 0.\n");
#endif

#ifndef EXIT_ON_ERROR
   return(NULL);
#endif

#ifdef EXIT_ON_ERROR
   exit(1);
#endif
 }
 
 if (l_max < 1) l_max = 1;   /* we need at least that much storage */

/*
  Allocate memory for Hl
*/
 Hl = matalloc( Hl, (l_max+1), 1, NUM_COMPLEX );

/* Add the offset of 1 to r/ptr_i */
 ptr_r = Hl->rel+1;
 ptr_i = Hl->iel+1;

/*
  Some often used values
*/
 z_inv = 1./x;
 
/*
  H0 and H1 are calculated from the exact formula:
  H0(x) = sin(x)/x - i*cos(x)/x.
  H1(x) = H0(x) * (1/x - i).
*/
 
 /* H0(x) */
 ptr_r[0] =  z_inv * sin(x);
 ptr_i[0] = -z_inv * cos(x);

 /* H1(x) */
 cri_mul(ptr_r+1, ptr_i+1, ptr_r[0], ptr_i[0], z_inv, -1.);
 

/* 
 loop over l:

  Hl (x) = (2*l-1)/x Hl-1(x) - Hl-2(x)
*/
 for(l = 2; l <= l_max; l++ )
 {
   faux = (2*l - 1) * z_inv;
   cri_mul(ptr_r+l, ptr_i+l, faux, 0., ptr_r[l-1], ptr_i[l-1]);
   ptr_r[l] -= ptr_r[l-2];
   ptr_i[l] -= ptr_i[l-2];
 }   /* l */

 return(Hl);

} /* end of function r_hank1 */

/*======================================================================*/
/*======================================================================*/

mat c_hank1 ( mat Hl, real z_r, real z_i, int l_max )

/************************************************************************

 Calculate all orders of the Hankel function of the first kind H(1)l up to 
 l = l_max for a given complex argument z.
 
 input: 

 mat Hl    - output: Hankel function of the first kind in natural order 
             (l = 0,1, ..., see below)
 real z_r, z_i - real and imaginary part of the argument.
 int l_max - max angular momentum for output.

 design:

 The Hankel functions H(1)l are calculated from the recurrence
 relation:

  H(1)l (z) = (2*l-1)/z H(1)l-1(z) - H(1)l-2(z)

 knowing the exact formula for H(1)0 and H(1)1:
  
  H(1)0(z) = - exp(iz)/z * (i);
  H(1)1(z) = - exp(iz)/z * (i/z + 1) = H(1)0(z) * (1/z - i).

 Variables used within the function:
 
 output(return value):

 NULL, if failed

 Hl (may be different from input parameter). The storage scheme for Hl
 is in the natural order:

 l      0  1  2  3  4  ...
 index  1  2  3  4  5  ...

 I.e. index(l,m) = l + 1. 

*************************************************************************/
{
int l;                        /* quantum number l */

real faux_r, faux_i;
real z_inv_r, z_inv_i;
real *ptr_r, *ptr_i;


/*
  Check arguments z and l_max
*/

 if( ( z_r == 0.) && ( z_i == 0.) )
 {
#ifdef ERROR
   fprintf(STDERR," *** error (r_hank1): invalid argument z = (0., 0.)\n");
#endif

#ifndef EXIT_ON_ERROR
   return(NULL);
#endif

#ifdef EXIT_ON_ERROR
   exit(1);
#endif
 }

 if (l_max < 1) l_max = 1;   /* we need at least that much storage */

/*
  Allocate memory for Hl
*/
 Hl = matalloc( Hl, (l_max+1), 1, NUM_COMPLEX );

/* Add the offset of 1 to r/ptr_i */
 ptr_r = Hl->rel+1;
 ptr_i = Hl->iel+1;

/*
  Some often used values
*/
 cri_div(&z_inv_r, &z_inv_i, 1., 0., z_r, z_i); 
 
/*
  H0 and H1 are calculated from the exact formula:
  H0(z) = - i/z * exp(iz).
  H1(x) = H0(x) * (1/x - i).
*/
 
 /* H0(x) */
 cri_expi(&faux_r, &faux_i, z_r, z_i);
 
 /* -i * z_inv = z_inv_i - i* z_inv_r */
 cri_mul(ptr_r  , ptr_i  , z_inv_i , -z_inv_r , faux_r, faux_i);

 /* H1(x) */
 cri_mul(ptr_r+1, ptr_i+1, ptr_r[0], ptr_i[0], z_inv_r, z_inv_i - 1.);
 

/* 
 loop over l:

  Hl (z) = (2*l-1)/z Hl-1(z) - Hl-2(z)
*/
 for(l = 2; l <= l_max; l++ )
 {
   faux_r = (2*l - 1) * z_inv_r;
   faux_i = (2*l - 1) * z_inv_i;

   cri_mul(ptr_r+l, ptr_i+l, faux_r, faux_i, ptr_r[l-1], ptr_i[l-1]);
   ptr_r[l] -= ptr_r[l-2];
   ptr_i[l] -= ptr_i[l-2];
 }   /* l */

 return(Hl);

} /* end of function c_hank1 */

/*======================================================================*/
/*======================================================================*/
