/*********************************************************************
  GH/18.07.95 
  file contains function:

  leed_par_mktl(mat *p_tl, leed_phs_t *phs_shifts, int l_max, real energy)

 Calculate atomic scattering factors for a given energy.

CHANGES:
GH/20.08.94 - Creation
GH/18.07.95 - temperature dependent phase shifts.

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <strings.h>

#include "leed.h"


mat * leed_par_mktl(mat *p_tl, leed_phs_t *phs_shifts, int l_max, real energy)

/************************************************************************

 Update matrix tl containing the atomic scattering factors for all types
 of atoms used in the calculation.
 
 INPUT:

  mat *p_tl - (input) Array of scattering factor matrices. The function 
           returns its first argument. If tl is NULL, the structure will 
           be created.
  leed_phs_t *phs_shifts  -  phase shifts.
  real energy - new energy (real part)

 RETURN VALUES:

  mat *p_tl - new set of scattering factors
  NULL  if any error occured (and EXIT_ON_ERROR is not defined)

 DESIGN:

*************************************************************************/
{

int l, l_set_1;
int i_set, n_set;
int i_eng, iaux;

real delta;
real faux_r, faux_i;
leed_phs_t *ptr;

/*********************************************************
   Search through list "phs_shifts". 
   - Find number of sets (n_set) and 
   - max. number of l (l_max: > input value if there is one larger 
     set of phase shifts);
   - call mkcg_coeff to ensure all C.G. coefficients are available. 
   - allocate p_tl.
*********************************************************/

 for(n_set = 0; (phs_shifts + n_set)->lmax != I_END_OF_LIST; n_set ++)
 { ; }

#ifdef CONTROL
 fprintf(STDCTR,"(leed_par_mktl): energy = %.2f H, n_set = %d, l_max = %d\n", 
                energy /*HART*/, n_set, l_max);
#endif

 if(p_tl == NULL)
 {
   p_tl = (mat *)malloc(n_set * sizeof(mat));
   for(i_set = 0; i_set < n_set; i_set ++) p_tl[i_set] = NULL;
 }
 
/*********************************************************
   Calculate tl for each set of phase shifts.
*********************************************************/

 for(i_set = 0; i_set < n_set; i_set ++)
 {

   ptr = phs_shifts + i_set;
   l_set_1 = ptr->lmax + 1;

#ifdef CONTROL_X
   fprintf(STDCTR,"(leed_par_mktl):i_set = %d, lmax(set) = %d, neng = %d\n", 
           i_set, l_set_1 - 1, ptr->neng);
#endif

/*
   p_tl[i_set] = matalloc(NULL, l_set_1, 1, NUM_COMPLEX);
*/
   p_tl[i_set] = matalloc(p_tl[i_set], l_set_1, 1, NUM_COMPLEX);


/* Check if energy is inside the energy range, exit if too small */

   if(energy < ptr->eng_min)
   {

/*********************************************************
  Abort for too low energies
*********************************************************/

#ifdef ERROR
     fprintf(STDERR,
  "*** error (leed_par_mktl): %.1f H is lower than the min. energy for set No. %d\n",
       energy, i_set);
     fprintf(STDERR,"\t => exit\n");
#endif
#ifdef EXIT_ON_ERROR
     exit(1);
#else
     free(p_tl);
     return(NULL);
#endif
   } /* if (energy too low) */

   else if(energy >= ptr->eng_max)
   {
/*********************************************************
  Extrapolate for too high energies
*********************************************************/

#ifdef WARNING
     fprintf(STDWAR,
  "* warning (leed_par_mktl): %.2f H is higher than the max. energy for set No. %d\n",
       energy, i_set);
     fprintf(STDWAR,"\t => calculate extrapolated phase shift values\n");
#endif

     i_eng = ptr->neng-1;
     for(l = 0; l < l_set_1; l ++)
     {
       delta =  ptr->pshift[i_eng*l_set_1 + l] -
       ( ( ptr->pshift[i_eng*l_set_1 + l] - ptr->pshift[(i_eng - 1)*l_set_1 + l] )
       / ( ptr->energy[i_eng] - ptr->energy[i_eng - 1] ) ) *
         ( ptr->energy[i_eng] - energy);

       iaux = 1 + l;
       faux_r = R_cos(delta);
       faux_i = R_sin(delta);
       cri_mul(p_tl[i_set]->rel+iaux, p_tl[i_set]->iel+iaux, 
               faux_r, faux_i, faux_i, 0.);
     }

   /*
      Include temperature in atomic scattering factors.
   */
     leed_par_temp_tl(p_tl[i_set], p_tl[i_set], ptr->dr[0], energy, l_max, ptr->lmax);

#ifdef CONTROL
     fprintf(STDCTR, "(leed_par_mktl): after leed_par_temp_tl, dr[0] = %.3f A^2:\n", 
             ptr->dr[0]*BOHR*BOHR);
     matshow(p_tl[i_set]);
#endif

   } /* else if (energy too high) */
   else
   {
/*********************************************************
  If the energy is within the range of phase shifts:
  scan through the energy list and find the right values to 
  interpolate:

  dl(E) = dl(i) - (dl(i) - dl(i-1))/(E(i) - E(i-1)) * ((E(i) - E)
*********************************************************/
     for(i_eng = 0; ptr->energy[i_eng] < energy; i_eng ++)
       ;
     
     for(l = 0; l < l_set_1; l ++)
     {
       delta =  ptr->pshift[i_eng*l_set_1 + l] - 
       ( ( ptr->pshift[i_eng*l_set_1+l] - ptr->pshift[(i_eng - 1)*l_set_1+l] ) 
       / ( ptr->energy[i_eng] - ptr->energy[i_eng - 1] ) ) *
         ( ptr->energy[i_eng] - energy);

       iaux = 1 + l;
       faux_r = R_cos(delta);
       faux_i = R_sin(delta);
       cri_mul(p_tl[i_set]->rel+iaux, p_tl[i_set]->iel+iaux, 
               faux_r, faux_i, faux_i, 0.);
     }

   /*
      Include temperature in atomic scattering factors.
   */
     leed_par_temp_tl(p_tl[i_set], p_tl[i_set], ptr->dr[0], energy, l_max, ptr->lmax);

#ifdef CONTROL
     fprintf(STDCTR, "(leed_par_mktl): after leed_par_temp_tl, dr[0] = %.3f A^2:\n", 
             ptr->dr[0]*BOHR*BOHR);
     matshow(p_tl[i_set]);
#endif

   } /* else (in the right energy range) */

 }   /* for i_set */

 return(p_tl);
}  /* end of function leed_par_mktl */
