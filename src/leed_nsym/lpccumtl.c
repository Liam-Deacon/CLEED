/************************************************************************
 * <FILENAME>
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *11.07.03 
  file contains function:

  leed_par_cumulative_tl(mat Tmat, mat tl_0, real ux, real uy, real uz, 
             real energy, int l_max_t, int l_max_0)

 Calculate non-diagonal temperature dependent atomic scattering matrix
 according to the cumulants expansion (P. de Andres).

CHANGES:
GH/16.09.00 - Creation - copy from pctemtl and cumulants (PdA)
GH/22.09.00 - output is -kappa * Tmat (according to the definitions in 
              the P.DeAndres / D.A. King paper)
GH/23.09.00 - Convergence test is proportional to number of matrix elements
GH/03.10.00 - bug fix in set up of T_n (T=0): use RMATEL, IMATEL
GH/11.07.03 - bug fix in output of T_mat for T=0: multiply with (-kappa)

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>

#include "leed.h"


#define NITER 1000
/* #define CONV_TEST 0.00000390625 */  /* to be multiplied by (l_max+1)^2 */
#define CONV_TEST 0.000001        /* to be multiplied by (l_max+1)^2 */

static int n_call = 0;
static int last_l = -1;

static mat Mx = NULL,   My = NULL,   Mz = NULL;
static mat MxMx = NULL, MyMy = NULL, MzMz = NULL;

mat leed_par_cumulative_tl(mat Tmat, mat tl_0, real ux, real uy, real uz, 
             real energy, int l_max_t, int l_max_0)

/************************************************************************

 Calculate non-diagonal temperature dependent atomic scattering matrix
 according to the cumulants expansion (P. de Andres).
 
 INPUT:

  mat Tmat - (input) The function writes the output nondiagonal temperature
           dependent scattering matrix into its first argument. 
           If Tmat is NULL, the structure will be created.

  mat tl_0 - (input) array containing scattering factors 
           sin(delta_l) * exp( i* delta_l) for T = 0. tl_0 will be stored 
           before Tmat is calculated, therefore tl_0 and Tmat can be equal.
           
  real ux, uy, uz - (input) root mean square of the anisotropic vibrational 
           displacements at the given temperature: <(dx)^2>(T).
  real energy - (input) energy (real part) in atomic units.

  int l_max_0 - (input) max l quantum number of input scattering factors.
  int l_max_t - (input) max required l quantum number of output scattering 
           matrix.

 RETURN VALUES:

  mat Tmat - non-diagonal temperature dependent scattering matrix multiplied
        with kappa (see below).
  NULL  if any error occured (and EXIT_ON_ERROR is not defined)

 DESIGN:

 see: P. de Andres, D. A. King, "Anisotropic and Anharmonic effects through
      the t-matrix for Low-Energy Electron Diffraction (TMAT V1.1)",
      Comp. Phys. Comm., sect. 2.4. the equation numbers refer to this paper.
      
      The output matrix (Tmat) is the one described in the above paper
      multiplied by kappa = sqrt(2E)

*************************************************************************/
{

int l1, l2, m1, m2, lm1, lm2;
int iaux, i_el;
int i_iter;
int l_max_2;                   /* matrix dimension */

real relerr_r, relerr_i;
real ux2, uy2, uz2;
real kappa;

real pref, conv_test;

mat tl_aux;                    /* backup of original scattering factors */
mat T_n, T_acc;

/* Ms */
mat MxMxTn, MxTnMx, TnMxMx;
mat MyMyTn, MyTnMy, TnMyMy;
mat MzMzTn, MzTnMz, TnMzMz;

/* 
  preset variables 
  l_max_2 = matrix dimension
  kappa = R_sqrt(2.* energy);
*/

 l_max_2 = (l_max_t+1)*(l_max_t+1);
 kappa = R_sqrt(2.* energy);
 ux2 = ux*ux;
 uy2 = uy*uy;
 uz2 = uz*uz;
 
#ifdef CONTROL
 fprintf(STDCTR,"(leed_par_cumulative_tl): Enter function: \n");
 fprintf(STDCTR,
         "\t(ux, uy, uz) = (%.3f, %.3f, %.3f) [au]; energy = %.3f H; lmax_t = %d, lmax_0 = %d\n",
         ux, uy, uz, energy, l_max_t, l_max_0);
 matshow(tl_0);
#endif

/*************************************************************************
  Backup original scattering factors
  and allocate output and dummy arrays.
*************************************************************************/

 tl_aux = T_n = T_acc = NULL;
 tl_aux = matcop(tl_aux, tl_0);

 T_n   = matalloc(T_n,   l_max_2, l_max_2, NUM_COMPLEX);
 T_acc = matalloc(T_acc, l_max_2, l_max_2, NUM_COMPLEX);

/*************************************************************************
  Make sure that l_max_0 is not greater than l_max_t.
*************************************************************************/

 if(l_max_0 > l_max_t)
 {
   l_max_0 = l_max_t;
#ifdef WARNING
   fprintf(STDWAR,
   "* warning (leed_par_cumulative_tl): input phase shifts are only used up to l_max = %d\n",
   l_max_0);
#endif
 }
 else if(l_max_0 < l_max_t)
 {
#ifdef WARNING
   fprintf(STDWAR,
   "* warning (leed_par_cumulative_tl): input phase shifts exist only up to l_max_0 = %d, for higher l (up to l_max = %d) they are set to zero.\n",
   l_max_0, l_max_t);
#endif
   ;
 }

/*************************************************************************
  Set up zero order t matrix t^(0), write to T_n:
     t^(0) = - (1./kappa) * Sin[phs_(l)] Exp[i phs_(l)]
*************************************************************************/

/* lm1, lm2, are counters in natural order */
 for(lm1 = 1, l1 = 0; l1 <= l_max_0; l1 ++)
   for(m1 = -l1; m1 <= l1; m1 ++, lm1 ++)
     for(lm2 = 1, l2 = 0; l2 <= l_max_0; l2 ++)
       for(m2 = -l2; m2 <= l2; m2 ++, lm2 ++)
         if((l1 == l2) && (m1 == m2) ) 
         {
           RMATEL(lm1, lm2, T_n) = - tl_aux->rel[l1+1] / kappa;
           IMATEL(lm1, lm2, T_n) = - tl_aux->iel[l1+1] / kappa;
         }
 
#ifdef CONTROL_X
 fprintf(STDCTR,"(leed_par_cumulative_tl): Tmat(T=0): \n");
 matshowabs(T_n);
#endif

/*************************************************************************
  If T = 0, i.e. all ux, uy, and uz are zero, we are allready done.
  return T_n * (-kappa).
*************************************************************************/

 if( (ux < GEO_TOLERANCE) && (uy < GEO_TOLERANCE) && (uz < GEO_TOLERANCE) )
 {
   Tmat = matcop(Tmat, T_n);
#ifdef WARNING
 fprintf(STDWAR,"(leed_par_cumulative_tl): All displacements are zero: return Tmat(T=0)\n");
#endif

   iaux = Tmat->cols * Tmat->rows;
   for(i_el = 1; i_el <= iaux; i_el ++)
   {
     Tmat->rel[i_el] *= -kappa;
     Tmat->iel[i_el] *= -kappa;
   } /* for i_el */

   return (Tmat);
 }

/*************************************************************************
  Prepare matrices for iteration:
   - Check if Mx, etc. have to be calculated.
   - allocate T_n etc.
   - set T_n etc to their start values.
*************************************************************************/

 if( (n_call == 0) || (last_l != l_max_t) )
 {

#ifdef CONTROL
   fprintf(STDCTR,"(leed_par_cumulative_tl): calculate Mx, etc. for l_max = %d\n", l_max_t);
#endif
   pc_mk_ms( &Mx, &My, &Mz, &MxMx, &MyMy, &MzMz, l_max_t);
 }

#ifdef CONTROL_X
 fprintf(STDCTR,"(leed_par_cumulative_tl): Mx: \n");
 matshow(Mx);
 fprintf(STDCTR,"(leed_par_cumulative_tl): My: \n");
 matshow(My);
 fprintf(STDCTR,"(leed_par_cumulative_tl): Mz: \n");
 matshow(Mz);
#endif

 MxMxTn = MxTnMx = TnMxMx = NULL;

 MxMxTn  =  matalloc(MxMxTn, l_max_2, l_max_2, NUM_COMPLEX);
 MxTnMx  =  matalloc(MxTnMx, l_max_2, l_max_2, NUM_COMPLEX);
 TnMxMx  =  matalloc(TnMxMx, l_max_2, l_max_2, NUM_COMPLEX);

 MyMyTn = MyTnMy = TnMyMy = NULL;

 MyMyTn  =  matalloc(MyMyTn, l_max_2, l_max_2, NUM_COMPLEX);
 MyTnMy  =  matalloc(MyTnMy, l_max_2, l_max_2, NUM_COMPLEX);
 TnMyMy  =  matalloc(TnMyMy, l_max_2, l_max_2, NUM_COMPLEX);

 MzMzTn = MzTnMz = TnMzMz = NULL;

 MzMzTn  =  matalloc(MzMzTn, l_max_2, l_max_2, NUM_COMPLEX);
 MzTnMz  =  matalloc(MzTnMz, l_max_2, l_max_2, NUM_COMPLEX);
 TnMzMz  =  matalloc(TnMzMz, l_max_2, l_max_2, NUM_COMPLEX);

/*************************************************************************
  Iteration:
    Tmat = S_n T(n)
    T(n+1) = - kappa^2 / (2(n+1))  *
             S_(a=xyz) [ua^2 ( Ma*Ma*T(n) + T(n)*Ma*Ma - 2*Ma*T(n)*Ma]
*************************************************************************/

/* Tmat(T=0) is start value of T_acc for recurrence (i_iter = 0) */
 T_acc = matcop(T_acc, T_n); 

 conv_test = CONV_TEST * l_max_2 * l_max_2;
 relerr_r = relerr_i = 2*conv_test;
 for(i_iter = 1;
     (i_iter < NITER) && ( (relerr_r > conv_test) || (relerr_i > conv_test) );
     i_iter ++)
 {
  /*
    Calculate products of T_n and Mxyz for later use.
    T_n will be overwiritten with Tmat(n+1) afterwards.
  */

      MxTnMx = matmul(MxTnMx, T_n, Mx);
      MxTnMx = matmul(MxTnMx, Mx, MxTnMx);

      MxMxTn = matmul(MxMxTn, MxMx,T_n);
      TnMxMx = matmul(TnMxMx, T_n,MxMx);
   
#ifdef CONTROL_X
      if (i_iter < 4)
      {
      fprintf(STDCTR, "(leed_par_cumulative_tl): MxMxTn(%d):\n", i_iter-1);
      matshow(MxMxTn);
      fprintf(STDCTR, "(leed_par_cumulative_tl): MxTnMx(%d):\n", i_iter-1);
      matshow(MxTnMx);
      fprintf(STDCTR, "(leed_par_cumulative_tl): TnMxMx(%d):\n", i_iter-1);
      matshow(TnMxMx);
      }
#endif

      MyTnMy = matmul(MyTnMy, T_n, My);
      MyTnMy = matmul(MyTnMy, My, MyTnMy);
   
      MyMyTn = matmul(MyMyTn, MyMy,T_n);
      TnMyMy = matmul(TnMyMy, T_n,MyMy);

      MzTnMz = matmul(MzTnMz, T_n, Mz);
      MzTnMz = matmul(MzTnMz, Mz, MzTnMz);
   
      MzMzTn = matmul(MzMzTn, MzMz,T_n);
      TnMzMz = matmul(TnMzMz, T_n,MzMz);

#ifdef CONTROL_X
      if (i_iter < 4)
      {
      fprintf(STDCTR, "(leed_par_cumulative_tl): MzMzTn(%d):\n", i_iter-1);
      matshow(MzMzTn);
      fprintf(STDCTR, "(leed_par_cumulative_tl): MzTnMz(%d):\n", i_iter-1);
      matshow(MzTnMz);
      fprintf(STDCTR, "(leed_par_cumulative_tl): TnMzMz(%d):\n", i_iter-1);
      matshow(TnMzMz);
      }
#endif

/* from here on replace T(n) by T(n+1) */

      pref = - kappa * kappa / i_iter;
      iaux = T_n->cols * T_n->rows;

      for(i_el = 1; i_el <= iaux; i_el ++)
      {
/* Eq. 35 */
        T_n->rel[i_el] =  
         pref * (
         ux2*(MxMxTn->rel[i_el] + TnMxMx->rel[i_el] - 2.*(MxTnMx->rel[i_el])) +
         uy2*(MyMyTn->rel[i_el] + TnMyMy->rel[i_el] - 2.*(MyTnMy->rel[i_el])) +
         uz2*(MzMzTn->rel[i_el] + TnMzMz->rel[i_el] - 2.*(MzTnMz->rel[i_el])) );

        T_n->iel[i_el] =  
         pref * (
         ux2*(MxMxTn->iel[i_el] + TnMxMx->iel[i_el] - 2.*(MxTnMx->iel[i_el])) +
         uy2*(MyMyTn->iel[i_el] + TnMyMy->iel[i_el] - 2.*(MyTnMy->iel[i_el])) +
         uz2*(MzMzTn->iel[i_el] + TnMzMz->iel[i_el] - 2.*(MzTnMz->iel[i_el])) );

/* Eq. 34 */
        T_acc->rel[i_el] += T_n->rel[i_el];
        T_acc->iel[i_el] += T_n->iel[i_el];
      }

/* 
   Calculate relative errors for im. and real parts 
   and check convergence of each element.
*/
      relerr_r = relerr_i = 0.;
      iaux = T_acc->cols * T_acc->rows;

      for(i_el = 1; i_el <= iaux; i_el ++)
      {
        if( ! IS_EQUAL_REAL(T_acc->rel[i_el], 0.)) 
        {
          relerr_r += R_fabs(T_n->rel[i_el] / T_acc->rel[i_el]);
        }
        if( ! IS_EQUAL_REAL(T_acc->iel[i_el], 0.))
        {
          relerr_i += R_fabs(T_n->iel[i_el] / T_acc->iel[i_el]);
        }
      } /* for i_el */
      
#ifdef CONTROL
     fprintf(STDCTR,
     "(leed_par_cumulative_tl): iteration No %d: rel. errors: (%.3e, %.3e) <> %.3e\n", 
     i_iter, relerr_r, relerr_i, conv_test);
#endif

 } /* for i_iter */

 if (i_iter == NITER)
 {
#ifdef ERROR
   fprintf(STDERR,
   "*** error (leed_par_cumulative_tl): No convergence after %d iterations\n", i_iter);
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(NULL);
#endif
 }

 Tmat = matcop(Tmat, T_acc);

/* Tmat -> Tmat * kappa */

 iaux = Tmat->cols * Tmat->rows;
 for(i_el = 1; i_el <= iaux; i_el ++)
 {
   Tmat->rel[i_el] *= -kappa;
   Tmat->iel[i_el] *= -kappa;
 } /* for i_el */

/* 
   Prepare returning:
   - update n_call and last_l 
   - free matrices
*/

 n_call ++;
 last_l = l_max_t;

 matfree(tl_aux);
 matfree(T_n); 
 matfree(T_acc);

 matfree(MxMxTn); matfree(MxTnMx); matfree(TnMxMx);
 matfree(MyMyTn); matfree(MyTnMy); matfree(TnMyMy);
 matfree(MzMzTn); matfree(MzTnMz); matfree(TnMzMz);

#ifdef CONTROL
 fprintf(STDCTR,"(leed_par_cumulative_tl): End of function \n");
#endif

 return (Tmat);

} /* end of function leed_par_cumulative_tl */
