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
 *16.09.00 
  file contains function:

  leed_par_mktl_nd(mat *p_tl, leed_phs_t *phs_shifts, int l_max, real energy)

 Calculate atomic scattering factors for a given energy.

CHANGES:
GH/20.08.94 - Creation
GH/18.07.95 - temperature dependent phase shifts.
GH/03.05.00 - read non-diagonal t-matrix
GH/16.09.00 - calculate non-diagonal t-matrix.

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "leed.h"


mat * leed_par_mktl_nd(mat *p_tl, leed_phs_t *phs_shifts, int l_max, real energy)

/************************************************************************

 Update matrix tl containing the atomic scattering factors for all types
 of atoms used in the calculation.

 if phs_shifts->t_type is equal to T_DIAG, a diagonal t-matrix is produced,
 if phs_shifts->t_type is equal to T_NOND, a non-diagonal t-matrix is produced.
 
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

 FUNCTION CALLS:

  - leed_par_temp_tl
  - inp_tmat_nd

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
 fprintf(STDCTR,"(leed_par_mktl_nd): energy = %.2f H, n_set = %d, l_max = %d\n", 
                energy /*HART*/, n_set, l_max);
#endif

 if(p_tl == NULL)
 {
   p_tl = (mat *)malloc(n_set * sizeof(mat));
   for(i_set = 0; i_set < n_set; i_set ++) p_tl[i_set] = NULL;
 }
 
/*********************************************************
   Calculate tl (diagonal or non-diagonal scattering matrix
   for each set of phase shifts.
*********************************************************/

 for(i_set = 0; i_set < n_set; i_set ++)
 {

   ptr = phs_shifts + i_set;
   l_set_1 = ptr->lmax + 1;

  /*********************************************************
  check for unknown t_type
  *********************************************************/

   if( (ptr->t_type != T_DIAG) && (ptr->t_type != T_NOND) )
   {
#ifdef ERROR
     fprintf(STDERR,
      "*** error (leed_par_mktl_nd): t_type %d has no valid value for set No. %d\n",
      ptr->t_type, i_set);
     fprintf(STDERR,"\t => exit\n");
#endif
#ifdef EXIT_ON_ERROR
     exit(1);
#else
     free(p_tl);
     return(NULL);
#endif
   } /* neither T_DIAG nor T_NOND */

#ifdef CONTROL_X
   fprintf(STDCTR,
           "(leed_par_mktl_nd):i_set = %d, lmax(set) = %d, neng = %d, t_type = %d\n", 
           i_set, l_set_1 - 1, ptr->neng, ptr->t_type);
#endif

#ifdef CONTROL
   fprintf(STDCTR,"(leed_par_mktl_nd):  d %d: (%s)\n", i_set, ptr->input_file);
#endif

   p_tl[i_set] = matalloc(p_tl[i_set], l_set_1, 1, NUM_COMPLEX);


/* Check if energy is inside the energy range, exit if too small */

   if(energy < ptr->eng_min)
   {

  /*********************************************************
    Abort for too low energies
  *********************************************************/

#ifdef ERROR
     fprintf(STDERR,
"*** error (leed_par_mktl_nd): %.1f H is lower than the min. energy for set No. %d\n",
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
"* warning (leed_par_mktl_nd): %.2f H is higher than max. energy for set No. %d\n",
       energy, i_set);
     fprintf(STDWAR,"\t => calculate extrapolated phase shift values\n");
#endif

     i_eng = ptr->neng-1;
     for(l = 0; l < l_set_1; l ++)
     {
       delta =  ptr->pshift[i_eng*l_set_1 + l] -
       ( ( ptr->pshift[i_eng*l_set_1 + l] - 
           ptr->pshift[(i_eng - 1)*l_set_1 + l] )
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
     if(ptr->t_type == T_DIAG)
     {
       leed_par_temp_tl(p_tl[i_set], p_tl[i_set], ptr->dr[0], energy, l_max, ptr->lmax);
#ifdef CONTROL_X
       fprintf(STDCTR, "(leed_par_mktl_nd): after leed_par_temp_tl, dr[0] = %.3f A^2:\n", 
               ptr->dr[0]*BOHR*BOHR);
       matshowabs(p_tl[i_set]);
#endif
     } /* T_DIAG */
     else if(ptr->t_type == T_NOND)
     {
       leed_par_cumulative_tl(p_tl[i_set], p_tl[i_set], 
                ptr->dr[1], ptr->dr[2], ptr->dr[3], energy, l_max, ptr->lmax);
#ifdef CONTROL_X
       fprintf(STDCTR, "(leed_par_mktl_nd): non-diag. Tlm for set %d:\n", i_set);
       matshowabs(p_tl[i_set]);
#endif
     } /* T_NOND */

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
     if(ptr->t_type == T_DIAG)
     {
       leed_par_temp_tl(p_tl[i_set], p_tl[i_set], ptr->dr[0], energy, l_max, ptr->lmax);
#ifdef CONTROL_X
       fprintf(STDCTR, "(leed_par_mktl_nd): after leed_par_temp_tl, dr[0] = %.3f A^2:\n", 
             ptr->dr[0]*BOHR*BOHR);
       matshowabs(p_tl[i_set]);
#endif
     } /* T_DIAG */
     else if(ptr->t_type == T_NOND)
     {
       leed_par_cumulative_tl(p_tl[i_set], p_tl[i_set], 
                ptr->dr[1], ptr->dr[2], ptr->dr[3], energy, l_max, ptr->lmax);

#ifdef CONTROL_X
       fprintf(STDCTR, "(leed_par_mktl_nd): non-diag. Tlm for set %d:\n", i_set);
       matshowabs(p_tl[i_set]);
#endif
     } /* T_NOND */

   } /* else (in the right energy range) */

 }   /* for i_set */

 return(p_tl);

}  /* end of function leed_par_mktl_nd */
