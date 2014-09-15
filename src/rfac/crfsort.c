/********************************************************************
 GH/30.08.95
 file contains function:

   int cr_lorentz( struct rfivcur *iv_cur, real vi, char *ctr)

  Sort IV curves according to the energy and check if the
  energy values are equidistant.

Changes:
GH/02.10.92 - Creation
GH/30.08.95 - Adaption to CRFAC
********************************************************************/
/*
#define CONTROL
*/

#include <stdio.h>
#include <math.h>

#include "crfac.h"          /* specific definitions etc. */

int cr_sort( struct crivcur *iv_cur)

/*********************************************************************
 Sort experimental and theoretical IV curves according to their 
 energy and check if the energy values are equidistant.

 parameters: iv_cur: data structure containing all essential data
	     such as theor. IV curves. The theoretical IV curves
	     wiil be modified after return.

 return value: 1, if successful.
	       I_FAIL, if failed.
*********************************************************************/
{
int i, j;

real f_aux;

/*********************************************************************
 First: sort theoretical data according to energy values
*********************************************************************/
 for (i = 0; i< iv_cur->the_leng-1; i++)
 {
  for (j = i+1; j< iv_cur->the_leng; j++)
  {
   if(iv_cur->the_list[j].energy < iv_cur->the_list[i].energy)
   {
    /* exchange energy values */
    f_aux = iv_cur->the_list[i].energy;
    iv_cur->the_list[i].energy = iv_cur->the_list[j].energy;
    iv_cur->the_list[j].energy = f_aux;

    /* exchange intensities */
    f_aux = iv_cur->the_list[i].intens;
    iv_cur->the_list[i].intens = iv_cur->the_list[j].intens;
    iv_cur->the_list[j].intens = f_aux;
   }
  }

#ifdef CONTROL
  fprintf(STDCTR,"(cr_sort): %.1f\n", iv_cur->the_list[i].energy);
#endif
 }

/*
 check equidistance
*/
 for (i = 1, iv_cur->the_equidist = 1; i< iv_cur->the_leng-1; i++)
 {
  if (R_fabs ((2*iv_cur->the_list[i].energy - 
	      iv_cur->the_list[i+1].energy - iv_cur->the_list[i-1].energy) ) 
      >  ENG_TOLERANCE ) 
  {
   iv_cur->the_equidist = 0;
  }
 }

#ifdef CONTROL /* print control output if required */
  if(iv_cur->the_equidist)
   fprintf(STDCTR,"(cr_tsort): theor. IV curve is equidistant\n");
  else
   fprintf(STDCTR,"(cr_tsort): theor. IV curve is not equidistant\n");
#endif

 iv_cur->the_first_eng = iv_cur->the_list[0].energy;
 iv_cur->the_last_eng = iv_cur->the_list[iv_cur->the_leng - 1].energy;
 iv_cur->the_sort = 1;

/*********************************************************************
 Now: sort experimental data according to energy values
*********************************************************************/

 for (i = 0; i< iv_cur->exp_leng-1; i++)
 {
  for (j = i+1; j< iv_cur->exp_leng; j++)
  {
   if(iv_cur->exp_list[j].energy < iv_cur->exp_list[i].energy)
   {
    /* exchange energy values */
    f_aux = iv_cur->exp_list[i].energy;
    iv_cur->exp_list[i].energy = iv_cur->exp_list[j].energy;
    iv_cur->exp_list[j].energy = f_aux;

    /* exchange intensities */
    f_aux = iv_cur->exp_list[i].intens;
    iv_cur->exp_list[i].intens = iv_cur->exp_list[j].intens;
    iv_cur->exp_list[j].intens = f_aux;
    fprintf(STDCTR,"(cr_tsort): %i - %i - %f\n", i, j, f_aux);
   }
  }

#ifdef CONTROL
  fprintf(STDCTR,"(cr_sort): %.1f\n", iv_cur->exp_list[i].energy);
#endif
 }

/*
 check equidistance
*/
 for (i = 1, iv_cur->exp_equidist = 1; i< iv_cur->exp_leng-1; i++)
 {
  if (R_fabs ((2*iv_cur->exp_list[i].energy - 
	      iv_cur->exp_list[i+1].energy - iv_cur->exp_list[i-1].energy) ) 
      >  ENG_TOLERANCE ) 
  {
   iv_cur->exp_equidist = 0;
  }
 }

#ifdef CONTROL /* print control output if required */
  if(iv_cur->exp_equidist)
   fprintf(STDCTR,"(cr_tsort): experimental. IV curve is equidistant\n");
  else
   fprintf(STDCTR,"(cr_tsort): experimental. IV curve is not equidistant\n");
#endif

 iv_cur->exp_first_eng = iv_cur->exp_list[0].energy;
 iv_cur->exp_last_eng = iv_cur->exp_list[iv_cur->exp_leng - 1].energy;
 iv_cur->exp_sort = 1;

 return (1);
}
