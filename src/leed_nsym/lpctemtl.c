 /*********************************************************************
GH/19.09.00 
  file contains function:

  leed_par_temp_tl(mat tl_t, mat tl_0,
           real dr2, real energy, int l_max_t, int l_max_0)

 Calculate temperature dependent atomic scattering factors.

CHANGES:
GH/18.07.95 - Creation
GH/13.09.95 - Use the original phase shifts only up to l_max_t
GH/04.05.00 - bug fixed in the summation of l3 (fac_l3)
GH/19.09.00 - change definition of dr2: mean square displacement.

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <strings.h>

#include "leed.h"


mat leed_par_temp_tl(mat tl_t, mat tl_0, 
             real dr2, real energy, int l_max_t, int l_max_0)

/************************************************************************

 Calculate an array of temperature dependent atomic scattering factors.
 
 INPUT:

  mat tl_t - (input) The function writes the output temperature dependent
           scattering factors into its first argument. If tl is NULL,
           the structure will be created.
  mat tl_0 - (input) array containing scattering factors for T = 0. tl_0 
           will be stored before tl_t is calculated, therefore tl_0 can
           be equal to tl_t.
           
  real dr2 - (input) mean square displacement (not isotropic vibrational 
           amplitude) at the given temperature: 
           <(dr)^2>(T) = ux^2 + uy^2 + uz^2 = 3ux^2 
           (since ux^2 = uy^2 = uz^2).
  real energy - (input) energy (real part).

  int l_max_0 - (input) max l quantum number of input scattering factors.
  int l_max_t - (input) max required l quantum number of output scattering 
           factors.

 RETURN VALUES:

  mat tl_t - set of temperature dependent scattering factors
  NULL  if any error occured (and EXIT_ON_ERROR is not defined)

 DESIGN:

  According to formula (23/p. 29) in VHT:

  t(l1,T) = sum(l2,l3) { i^l3 * exp(-4aE) * J(l3)(-4iaE) * t(l2,0) *
                         sqrt[ 4pi (2*l2+1) * (2*l3+1) / (2*ll+1) ] *
                         cg(l3,0; l2,0; l1,0) }

  a = 1/6 <(dr)^2>(T).
  E = E_vac - V0 (kinetic energy inside the crystal).
  J(l2) = bessel function.

*************************************************************************/
{

int l1, l2, l3;
int l3_min, l3_max;

real alpha_4e;
real kappa;
real faux_r, faux_i;
real fac_l1, fac_l2, fac_l3, fac_l12;
mat tl_aux;                    /* backup of original scattering factors */
mat Jl;                        /* Bessel function */

 tl_aux = NULL;
 Jl = NULL;

#ifdef CONTROL
   fprintf(STDCTR,"(leed_par_temp_tl): Enter function: \n");
   fprintf(STDCTR,"\tdr2 = %.3f [au^2], energy = %.3f [H], lmax_t = %d, lmax_0 = %d\n", 
   dr2, energy, l_max_t, l_max_0);
   matshow(tl_0);
#endif

/* 
  backup original scattering factors
  and allocate output array 
*/
 tl_aux = matcop(tl_aux, tl_0);
 tl_t =  matalloc(tl_t, l_max_t+1, 1, NUM_COMPLEX);

/*
  Make sure that l_max_0 is not greater than l_max_t, and that all C.G. 
  coefficients are available (recalculate if necessary).
*/
 if(l_max_0 > l_max_t)
 {
   l_max_0 = l_max_t;
#ifdef WARNING
   fprintf(STDWAR,
   "* warning (leed_par_temp_tl): input phase shifts are only used up to l_max = %d\n",
   l_max_0);
#endif
 } 

 mk_cg_coef(l_max_t+l_max_0);

/******************************************* 
  Calculate often used values:
   alpha_4e = 4 * 1/6 sigma_r * E = 4* 1/6 * 2<(dr)^2>(T) * E;
   i^l3 * Jl3(-i*alpha_4e);
*******************************************/

/* changed GH/19.09.00 from
 alpha_4e = 4./6. * dr2 * energy;
*/
 alpha_4e = 8./6. * dr2 * energy;

 Jl = c_bess(Jl, 0., -alpha_4e, l_max_t+l_max_0);

/* multiply Bessel function with sqrt(4pi) * i^l * exp(-alpha_4e) */

 faux_r = 3.54490770181103205461 * R_exp(-alpha_4e);
 faux_i = 0.;
 for(l1= 1; l1 <= (l_max_t + l_max_0 + 1); l1++)
 {
   cri_mul(Jl->rel+l1, Jl->iel+l1, faux_r, faux_i, Jl->rel[l1],Jl->iel[l1]);
   cri_mul(&faux_r, &faux_i, 0., 1., faux_r, faux_i);
 }
 
/******************************************* 
  Calculate tl_t
*******************************************/

/* loop over output l's */
 for(l1 = 0, fac_l1 = 1.; l1 <= l_max_t; l1 ++, fac_l1 += 2.)
 {
/* loop over input l's */
   for(l2 = 0, fac_l2 = 1.; l2 <= l_max_0; l2 ++, fac_l2 += 2.)
   {
     fac_l12 = fac_l2 / fac_l1;

     l3_min = abs(l2-l1); 
     l3_max = l2+l1;

/* bug fixed */
     for(l3 = l3_min, fac_l3 = l3_min*2. + 1.; 
         l3 <= l3_max; l3 ++, fac_l3 += 2. )
     {
       faux_r = cg(l3,0, l2,0, l1,0);
       faux_r *= R_sqrt(fac_l3*fac_l12);

#ifdef CONTROL_X
       fprintf(STDCTR,"(leed_par_temp_tl): pref (%d %d %d) = %f (%f)\n", 
               l1, l2, l3, faux_r, R_sqrt(fac_l3*fac_l12));
#endif

       cri_mul(&faux_r, &faux_i,
               tl_aux->rel[l2+1], tl_aux->iel[l2+1],
               faux_r * Jl->rel[l3+1], faux_r * Jl->iel[l3+1]);

       tl_t->rel[l1+1] += faux_r;
       tl_t->iel[l1+1] += faux_i;
     }  /* l3 */
   }  /* l2 */
#ifdef CONTROL
   kappa = R_sqrt(2* energy);
   fprintf(STDCTR,
"(leed_par_temp_tl): %d: tl_0=(%7.4f, %7.4f)\ttl_t=(%7.4f, %7.4f)\ttl_t/kappa=(%7.4f, %7.4f)\n", 
                  l1, tl_aux->rel[l1+1], tl_aux->iel[l1+1],
                      tl_t->rel[l1+1], tl_t->iel[l1+1],
                      -(tl_t->rel[l1+1])/kappa, -(tl_t->iel[l1+1])/kappa);
#endif
 }  /* l1 */

 return(tl_t);

}  /* end of function leed_par_temp_tl */
