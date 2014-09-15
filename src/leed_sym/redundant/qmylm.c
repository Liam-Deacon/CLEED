/*********************************************************************
  GH/10.08.95 
  file contains functions:

  r_ylm      (15.08.94)

       Calculate all spherical harmonics up to l = l_max for real
       arguments x and phi.

  c_ylm      (15.08.94)

       Calculate all spherical harmonics up to l = l_max for complex
       arguments z and phi(real).
  
  mk_ylm_coef (15.08.94)

       Produce the coefficients needed to calculate spherical harmonics 
       in function ylm.

GH/15.08.94 - Creation
GH/03.04.95 - Calculate Yl-m explicitly in c_ylm.
GH/05.08.95 - mk_ylm_coef is a global function (not static anymore), i.e. 
              it can be called from outside this file.
GH/10.08.95 - WARNING output at the end of mk_ylm_coef.

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>

#include "mat.h"
#include "qm.h"

/*
#define CONTROL_MK
*/
#define WARNING
#define ERROR


#define MEM_BLOCK 256           /* memory block for coef */
#define UNUSED    -1

static real *coef  = NULL;      /* coefficients in the power series of Ylm */

static real *r_pre = NULL;      /* prefactors used to calculate Ylm r/c_ylm */
static real *i_pre = NULL;
static real *r_prec = NULL;     /* prefactors used to calculate Yl-m c_ylm */
static real *i_prec = NULL;

static int l_max_coef = UNUSED;
static int l_max_r = UNUSED;
static int l_max_c = UNUSED;

/*======================================================================*/
/*======================================================================*/

mat r_ylm( mat Ylm, real x, real phi, int l_max )

/************************************************************************

 Calculate all shperical harmonics Ylm up to l = l_max for given real
 arguments x and phi.
 
 input: 

 mat Ylm   - output: spherical harmonics in natural order (see below).
 real x    - first argument: cos(theta)
 real phi  - 2nd argument: phi
 int l_max - max angular momentum for output.

 design:

 The shperical harmonics Ylm are calculated as a power series times
 prefactors. The coefficients of the power series have to be generated
 once and stored in the array coef (function mk_ylm_coef). It is checked
 within the function r_ylm, if this has been done already.
 
 Variables used within the function:
 
  r/i_pre - (dimension: l_max+1) powers of 
            sin(x) * exp(i*phi) = sqrt (1- x*x)*(cos(phi) + i sin(phi) )
            which is the m-dependent prefactor of the spherical harmonics.

  r_pre_l, r_pre_m 
            these variables are either x or 1. The relation holds:
            if r_pre_l/m  = 1(x) then the next time it is x(1). 
            Therefore r_pre_l/m(l/m+1) = 1 + x - r_pre_l/m(l/m).

  coef    - coefficients of the power series generated in mk_ylm_coef.

 output(return value):

 Ylm (may be different from input parameter). The storage scheme for Ylm 
 is in the natural order:

 l      0  1  1  1  2  2  2  2  2  3  3  3  3  3  3  3  4  4 ...
 m      0 -1  0  1 -2 -1  0  1  2 -3 -2 -1  0  1  2  3 -4 -3 ...
 index  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 ...

 I.e. index(l,m) = l*(l+1) + m + 1. Note that, like usually for matrices,
 the first array element Ylm[0] is not occupied.

*************************************************************************/
{
int iaux, off;
int l, m;                      /* quantum numbers l,m */
int lamb;                      /* power of x */

int index;                     /* used to run through coef */

real r_pre_l, r_pre_m;       /* prefactors */
real i_pre_l, i_pre_m;

real faux;
real x_2, sum;


/*
  Allocate memory for Ylm
*/
 iaux = (l_max+1)*(l_max+1);                /* Total number of (l,m) pairs */
 Ylm = matalloc( Ylm, 1, iaux, NUM_COMPLEX );

/*
  Calculate coefficients and allocate memory for prefactors r/i_pre 
  if not done yet or if l_max has changed since last time.
*/
 if ( l_max > l_max_coef ) 
 {
#ifdef WARNING
   if(l_max_coef != UNUSED)
   {
     fprintf(STDWAR,"*** warning (r_ylm): recalculating coefficients: ");
     fprintf(STDWAR,"old l_max: %d, new: %d\n", l_max_coef, l_max);
   }
#endif
   mk_ylm_coef(l_max);
 }

 if ( l_max > l_max_r )
 {
   if (r_pre == NULL) r_pre = (real *) calloc( (l_max+1) , sizeof(real) );
   else       r_pre = (real *) realloc( r_pre, (l_max+1) * sizeof(real) );

   if (i_pre == NULL) i_pre = (real *) calloc( (l_max+1) , sizeof(real) );
   else       i_pre = (real *) realloc( i_pre, (l_max+1) * sizeof(real) );
 }

/*
  Some often used values
*/
 x_2 = x*x;

 r_pre[0] = 1.;
 i_pre[0] = 0.;

 faux = sqrt(1 - x_2);
 r_pre[1] = faux * cos(phi);
 i_pre[1] = faux * sin(phi);

/*
  Y_00:
*/
 Ylm->rel[1] = coef[0];
 Ylm->iel[1] = 0.;

/* 
 loop over l 
*/
 index = 1;
 r_pre_l = 1.;
 for(l = 1; l <= l_max; l++ )
 {
 /* 
   Determine prefactors, 
   write next value into r/i_pre;
   determine offset in Ylm -> off
 */
   r_pre_l = 1. + x - r_pre_l;

   r_pre[l] = (r_pre[l-1]*r_pre[1]) - (i_pre[l-1]*i_pre[1]);
   i_pre[l] = (i_pre[l-1]*r_pre[1]) + (r_pre[l-1]*i_pre[1]);

   off = l*(l+1) + 1;

 /* 
    r_pre_m: for odd (l+m), the lowest power of x is x, for even (l+m), 
    it is 1.
 */
   r_pre_m = r_pre_l;
   for(m = 0; m <= l; m++ )
   {
     sum = 0.;
     iaux = (l+m+1)/2;
     for(lamb = l; lamb >= iaux; lamb--, index++ )
     {
       sum = sum * x_2 + coef[index];
     }

     Ylm->rel[off + m] = r_pre[m] * r_pre_m * sum;
     Ylm->iel[off + m] = i_pre[m] * r_pre_m * sum;
    
     /* -m: (-1)^m */
     if(ODD(m)) 
     { 
       Ylm->rel[off - m] = - Ylm->rel[off + m];
       Ylm->iel[off - m] =   Ylm->iel[off + m];
     }
     else
     {
       Ylm->rel[off - m] =   Ylm->rel[off + m];
       Ylm->iel[off - m] = - Ylm->iel[off + m];
     }

     r_pre_m = 1. + x - r_pre_m;

   } /* m */
 }   /* l */

 return(Ylm);

} /* end of function ylm */

/*======================================================================*/
/*======================================================================*/

mat c_ylm( mat Ylm, real z_r, real z_i, real phi, int l_max )

/************************************************************************

 Calculate all shperical harmonics Ylm up to l = l_max for a given complex
 argument z (z_r, z_i) and real phi.
 
 input: 

 mat Ylm    - output: spherical harmonics in natural order (see below).
 real z_r/i - real and imaginary part of 1st argument: complex cos(theta)
 real phi   - 2nd argument: phi (real)
 int l_max  - max angular momentum for output.

 design (identical to r_ylm):

 The spherical harmonics Ylm are calculated as a power series times
 prefactors. The coefficients of the power series have to be generated
 once and stored in the array coef (function mk_ylm_coef). It is checked
 within the function r_ylm, if this has been done already.

 The definition of the Ylm and Yl-m is according to formula (10,VHT):

 Ylm(th, phi) = 
        (-1)^m [(2l+1)/4pi * (l+m)!/(l-m)!]^(1/2) Plm(cos th) exp( im phi)

 Yl-m(th, phi) = 
               [(2l+1)/4pi * (l+m)!/(l-m)!]^(1/2) Plm(cos th) exp(-im phi)

 Yl-m must be calculated explicitly.

 !!!!!!!!!!
 The assumption of the VHT programs that 
   (Ylm)* = (-1)^m Yl-m
 is not true for complex arguments cos(theta). Therefore Yl-m is calculated
 explicitly. 
 !!!!!!!!!!

 
 Variables used within the function:
 
  r/i_pre - (dimension: l_max+1) powers of 
            sin(x) * exp(i*phi) = sqrt (1- x*x)*(cos(phi) + i sin(phi) )
            which is the m-dependent prefactor of the spherical harmonics.

  r_pre_l, r_pre_m 
            these variables are either x or 1. The relation holds:
            if r_pre_l/m  = 1(x) then the next time it is x(1). 
            Therefore r_pre_l/m(l/m+1) = 1 + x - r_pre_l/m(l/m).

  coef    - coefficients of the power series generated in mk_ylm_coef.

 output(return value):

 Ylm (may be different from input parameter). The storage scheme for Ylm 
 is in the natural order:

 l      0  1  1  1  2  2  2  2  2  3  3  3  3  3  3  3  4  4 ...
 m      0 -1  0  1 -2 -1  0  1  2 -3 -2 -1  0  1  2  3 -4 -3 ...
 index  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 ...

 I.e. index(l,m) = l*(l+1) + m + 1. Note that, like usually for matrices,
 the first array element Ylm[0] is not occupied.

*************************************************************************/
{
int iaux, off;
int l, m;                      /* quantum numbers l,m */
int lamb;                      /* power of x */

int index;                     /* used to run through coef */

real r_pre_l, i_pre_l;       /* prefactors */
real r_pre_m, i_pre_m;

real faux_i, faux_r;
real z2_i, z2_r;
real sum_r, sum_i;


/*
  Allocate memory for Ylm
*/
 iaux = (l_max+1)*(l_max+1);            /* Total number of (l,m) pairs + 1 */
 Ylm = matalloc( Ylm, 1, iaux, NUM_COMPLEX );

/*
  Calculate coefficients and allocate memory for prefactors r/i_pre 
  if not done yet or if l_max has changed since last time.
*/
 if ( l_max > l_max_coef ) 
 {
#ifdef WARNING
   if(l_max_coef != UNUSED)
   {
     fprintf(STDWAR,"*** warning (c_ylm): recalculating coefficients: ");
     fprintf(STDWAR,"old l_max: %d, new: %d\n", l_max_coef, l_max);
   }
#endif
   mk_ylm_coef(l_max);
 }
 
 if ( l_max > l_max_r)
 {
   if (r_pre == NULL) r_pre = (real *) calloc( (l_max+1) , sizeof(real) );
   else       r_pre = (real *) realloc( r_pre, (l_max+1) * sizeof(real) );

   if (i_pre == NULL) i_pre = (real *) calloc( (l_max+1) , sizeof(real) );
   else       i_pre = (real *) realloc( i_pre, (l_max+1) * sizeof(real) );
 }

 if ( l_max > l_max_c)
 {
   if (r_prec == NULL) r_prec = (real *) calloc( (l_max+1) , sizeof(real) );
   else       r_prec = (real *) realloc( r_prec, (l_max+1) * sizeof(real) );

   if (i_prec == NULL) i_prec = (real *) calloc( (l_max+1) , sizeof(real) );
   else       i_prec = (real *) realloc( i_prec, (l_max+1) * sizeof(real) );
 }

/*
  Some often used values
*/
 cri_mul(&z2_r, &z2_i, z_r, z_i, z_r, z_i);              /* z*z */

 r_pre[0] = 1.; i_pre[0] = 0.;
 r_prec[0] = 1.; i_prec[0] = 0.;

 cri_sqrt(&faux_r, &faux_i, (1. - z2_r), - z2_i);        /* sqrt(1 - z*z) */

 sum_r = cos(phi); sum_i = sin(phi);                     /* exp(i*phi) */
 cri_mul(r_pre+1, i_pre+1, faux_r, faux_i, sum_r, sum_i );

 sum_r = cos(-phi); sum_i = sin(-phi);                   /* exp(-i*phi) */
 cri_mul(r_prec+1, i_prec+1, -faux_r, -faux_i, sum_r, sum_i );

/*
  Y_00:
*/

 Ylm->rel[1] = coef[0];
 Ylm->iel[1] = 0.;

/* 
 loop over l 
*/
 index = 1;
 r_pre_l = 1.; i_pre_l = 0.;
 for(l = 1; l <= l_max; l++ )
 {
 /* 
   Write next value into r/i_pre(c);
   Determine prefactors (for odd (l+m), the lowest power of x is x, 
   for even (l+m), it is 1).
   Determine offset in Ylm -> off
 */
   cri_mul(r_pre+l, i_pre+l, r_pre[l-1], i_pre[l-1], r_pre[1], i_pre[1]);
   cri_mul(r_prec+l, i_prec+l, r_prec[l-1], i_prec[l-1], r_prec[1], i_prec[1]);

   r_pre_l = 1. + z_r - r_pre_l;
   i_pre_l =      z_i - i_pre_l;
   r_pre_m = r_pre_l; i_pre_m = i_pre_l;

   off = l*(l+1) + 1;

   for(m = 0; m <= l; m++ )
   {
   /* 
     loop over powers of (z*z)
   */
     sum_r = sum_i = 0.;
     iaux = (l+m+1)/2;
     for(lamb = l; lamb >= iaux; lamb--, index++ )
     {
       faux_r = sum_r;
       sum_r = sum_r*z2_r - sum_i*z2_i + coef[index];
       sum_i = faux_r*z2_i + sum_i*z2_r;
     } /* lamb */

   /*
      The assumption (-1)^m Yl-m = (Ylm)* is not true for complex 
      arguments; therefore Yl-m is calculated explicitly: 
      
        Ylm  = pre[m]  * pre_m * sum
        Yl-m = prec[m] * pre_m * sum

      (the factor (-1)^m is included in prec[m])

   ??? This is not in accordance with the VHT book but with the programs ???
   */

     cri_mul(&faux_r, &faux_i, sum_r, sum_i, r_pre_m, i_pre_m);

     /* +m: */
     cri_mul(Ylm->rel+off+m, Ylm->iel+off+m,
             faux_r, faux_i, r_pre[m], i_pre[m]);

     /* -m: */
     cri_mul(Ylm->rel+off-m, Ylm->iel+off-m,
             faux_r, faux_i, r_prec[m], i_prec[m]);

     r_pre_m = 1. + z_r - r_pre_m;
     i_pre_m =      z_i - i_pre_m;

   } /* m */
 }   /* l */

 return(Ylm);

} /* end of function c_ylm */

/*======================================================================*/
/*======================================================================*/

int mk_ylm_coef(int l_max)

/************************************************************************

 Produce the coefficients needed to calculate spherical harmonics in
 functions r_ylm and c_ylm.

 return value: l_max_coef

*************************************************************************/
{
int i, j;
int iaux;
int index;                   /* index */
int i_mem;                   /* number of memory blocks allocated */

int l, m, lamb;

double *fac;

real pre_0, pre_ll, pre_lm;  /* prefactors */
real sgn;                    /* sign of the coefficients */
 
/* 
  Produce a list of factorials 
*/
 iaux = 2*l_max + 1;
 fac = (double *) malloc( iaux * sizeof(double) );
 
 for (fac[0] = 1. , i = 1; i < iaux; i ++ )
   fac[i] = fac[i-1] * i;

/* 
   Allocate memory for coef.
   (MEM_BLOCK = 512 is enough for l_max <= 15) 
*/

 i_mem = 1;
 if (coef == NULL)
   coef = (real *) calloc(i_mem * MEM_BLOCK , sizeof(real) );
 else
   coef = (real *) realloc( coef, i_mem * MEM_BLOCK * sizeof(real) );

/* 
 loop over l 
 pre_0 is used as sqrt(pi/4) / 2^l
*/
 index = 0;
 pre_0  = sqrt(0.25 / PI);
 for(l = 0; l <= l_max; l++, pre_0 /= 2.)
 {
 /* prefactor pre_ll is common to all m's */
   pre_ll = pre_0 * sqrt(2. * l + 1.);

/* 
  loop over the nonnegative values of m 
*/
   for( m = 0; m <= l; m++) 
   {
   /* prefactor pre_lm is common to all lambdas */
     pre_lm = pre_ll * (real)sqrt( fac[l-m] / fac[l+m] );
/* 
  loop over lambda, i.e. over all powers of x in Legendre's function Plm:
  2*lamb - l - m > 0
  
  The sign starts with (-1)^(2l+m) = (-1)^m.
*/
     sgn = (m%2)?(-1):1;
     iaux = (l+m+1)/2;
     if( index > i_mem * MEM_BLOCK - l - iaux)
     {
       i_mem ++;
       coef = (real *)realloc( coef, i_mem * MEM_BLOCK * sizeof(real) );
#ifdef CONTROL_MK
       fprintf(STDCTR,"(mk_ylm_coef): reallocate coef: %d\n", i_mem);
#endif
     }

     for(lamb = l; lamb >= iaux; lamb--, index++ )
     {
       coef[index] = sgn * pre_lm * 
                     (real) ( fac[2*lamb] / 
                             (fac[lamb] * fac[l - lamb] * fac[2*lamb -l -m]));
#ifdef CONTROL_MK
       fprintf(STDCTR,"l:%2d, m:%2d, lamb:%2d, coef[%2d]:%7.3f\n", 
               l,m,lamb,index, coef[index]);
#endif
       sgn = -sgn;
     } /* lamb */
   }   /* m */
 }     /* l */

#ifdef CONTROL_MK
       fprintf(STDCTR,"end of l,m loop\n");
#endif

 free(fac);
 l_max_coef = l_max;

#ifdef WARNING
/* Write memory size to warning output */
       fprintf(STDCTR,"(mk_ylm_coef): coef[%d] (%d bytes) for l_max = %d\n",
               index, i_mem * MEM_BLOCK * sizeof(real), l_max_coef);
#endif

 return( l_max_coef );

} /* end of function mk_ylm_coef */
/*======================================================================*/
/*======================================================================*/
