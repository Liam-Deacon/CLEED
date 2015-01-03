/*********************************************************************
 *                       RFRMIN.C
 *
 *  Copyright 1992-2014 Georg Held
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1995.08.30 - Creation
 *   GH/1995.09.12 - Output of IV curves for the best overlap
 ********************************************************************/

/*! \file
 *  \brief Implementation file for rfac_rmin() function.
 *
 *  Calculate R factor and find minimum with respect to shift
 *
 */

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>          /* needed for sqrt */

#include "rfac.h"          /* specific definitions etc. */

#define SHIFT_DE

/*!
 * Calculate R factor and find minimum with respect to shift.
 *
 *
 * \param iv_cur pointer to data structure containing all essential data such
 * as experimental and theoretical IV curves weight etc.
 *
 * \param args pointer to argument list including:
 *  - imaginary part of the optical potential.
 *  - s_ini, s_fin, s_step,
 *  - r_type.
 *
 * \param[out] p_r_min pointer to minimum R Factor.
 * \param[out] p_s_min pointer to minimum energy shift.
 * \param[out] p_e_range pointer to energy range value.
 *
 * \return minimum R factor, if successful.
 */
real rfac_rmin(rfac_ivcur *iv_cur, rfac_args *args,
             real *p_r_min, real *p_s_min, real *p_e_range)
{

  size_t i_list, n_list;
  size_t i_leng, n_leng;

  real faux;
  real shift;
  real e_range, norm, rfac;

  real *eng, *e_int, *t_int;

  char linebuffer[STRSZ];
  char r_name[STRSZ];
  FILE *out_stream;

/********************************************************************
 * Find number of IV curves
 * Allocate storage space for cr_mklist.
 ********************************************************************/

  rfac = 0.;
  n_leng = 0;

  for(n_list = 0; iv_cur[n_list].group_id != END_OF_GROUP_ID; n_list ++)
  {
    #ifdef SHIFT_DE
    faux = ((iv_cur+n_list)->experimental->data +
            (iv_cur+n_list)->experimental->n_eng-1)->energy  -
          ((iv_cur+n_list)->experimental->data)->energy;
    n_leng = MAX(n_leng, (size_t)abs((int)(faux/args->s_step) ));
    faux = ((iv_cur+n_list)->theory->data +
            (iv_cur+n_list)->theory->n_eng -1)->energy  -
          ((iv_cur+n_list)->theory->data)->energy;
    n_leng = MAX(n_leng, (size_t)abs((int)(faux/args->s_step) ));
    #else
    n_leng = MAX(n_leng, (iv_cur+n_list)->exp_leng);
    n_leng = MAX(n_leng, (iv_cur+n_list)->the_leng);
    #endif
  }

  CONTROL_MSG(CONTROL, "start of function, n_list = %d, n_leng = %d\n",
              n_list, n_leng);

  eng   = (real *)malloc( n_leng * sizeof(real)*13);
  e_int = (real *)malloc( n_leng * sizeof(real)*13);
  t_int = (real *)malloc( n_leng * sizeof(real)*13);

/********************************************************************
 * Scan through shift and find minimum R factor
 ********************************************************************/

  *p_r_min = 100.;

  for(shift = args->s_ini; shift <= args->s_fin; shift += args->s_step)
  {
    rfac = 0.;
    norm = 0.;
    e_range = 0.;

    for(i_list = 0; i_list < n_list; i_list ++)
    {

     #ifdef SHIFT_DE
     n_leng = rfac_mklide(eng, e_int, t_int, args->s_step, shift,
              (iv_cur+i_list)->experimental->data,
              (iv_cur+i_list)->experimental->n_eng,
              (iv_cur+i_list)->theory->data,
              (iv_cur+i_list)->theory->n_eng);
     #else
     n_leng = rfac_mklist(eng, e_int, t_int, args->s_step, shift,
              (iv_cur+i_list)->experimental->data,
              (iv_cur+i_list)->experimental->n_eng,
              (iv_cur+i_list)->theory->data,
              (iv_cur+i_list)->theory->n_eng);
     #endif

     if(n_leng > 1) 
     {
       e_range += faux = (eng[n_leng-1] - eng[0]);
       norm += faux *= (iv_cur+i_list)->weight;

       if(args->r_type == RP_FACTOR)
         faux *= rfac_rp(eng, e_int, t_int, args->vi);
       else if (args->r_type == R1_FACTOR)
         faux *= rfac_r1(eng, e_int, t_int);
       else if (args->r_type == R2_FACTOR)
         faux *= rfac_r2(eng, e_int, t_int);
       else if (args->r_type == RB_FACTOR)
         faux *= rfac_rb(eng, e_int, t_int);
       else 
       {
         ERROR_MSG("invalid R factor selection %d\n", args->r_type);
         exit(1);
       }
       rfac += faux;
     }
     else
       WARNING_MSG("No overlap in IV curve No.%d for shift %.1f eV\n",
                   i_list, shift);

   }  /* for i_list */

   if(IS_EQUAL_REAL(norm, 0.))
   {
     ERROR_MSG("no overlap for shift %.1f eV\n", shift);
     exit(1);
   }
   else
   {
     rfac /= norm;

     CONTROL_MSG(CONTROL, "shift = %4.1f, rfac = %.6f range = %.1f\n",
                 shift, rfac, norm);

     if(rfac < *p_r_min)
     { 
       *p_r_min = rfac; 
       *p_s_min = shift;
       *p_e_range = e_range;
     }
   }  /* else (overlap) */
 }  /* for shift ... */

  CONTROL_MSG(CONTROL, "r_min = %.6f (shift = %4.1f)\n", *p_r_min, *p_s_min);

/********************************************************************
 * Write IV curves with best agreement to files
 ********************************************************************/
 
  if(args->iv_file != NULL)
  {
    for(i_list = 0; i_list < n_list; i_list ++)
    {

       #ifdef SHIFT_DE
       n_leng = rfac_mklide(eng, e_int, t_int, args->s_step, *p_s_min,
              (iv_cur+i_list)->experimental->data,
              (iv_cur+i_list)->experimental->n_eng,
              (iv_cur+i_list)->theory->data,
              (iv_cur+i_list)->theory->n_eng);
       #else
       n_leng = rfac_mklist(eng, e_int, t_int, args->s_step, *p_s_min,
                 (iv_cur+i_list)->experimental->data,
                 (iv_cur+i_list)->experimental->n_eng,
                 (iv_cur+i_list)->theory->data,
                 (iv_cur+i_list)->theory->n_eng);
       #endif

       if(n_leng > 1)
       {

         /* calculate R factor for single curve */
         e_range = (eng[n_leng-1] - eng[0]);

         switch(args->r_type)
         {
           case R1_FACTOR:
           {
             rfac = rfac_r1(eng, e_int, t_int);
             strcpy(r_name, "R1");
             break;
           }

           case R2_FACTOR:
           {
             rfac = rfac_r2(eng, e_int, t_int);
             strcpy(r_name, "R2");
             break;
           }

           case RB_FACTOR:
           {
             rfac = rfac_rb(eng, e_int, t_int);
             strcpy(r_name,"Rb");
             break;
           }

           case RP_FACTOR:
           {
             rfac = rfac_rp(eng, e_int, t_int, args->vi);
             strcpy(r_name, "Rp");
             break;
           }

           default:
             break;
         }

         /* Calculate sum of theor. intensities for normalization */
         norm = 0.;
         for(i_leng = 0; i_leng < n_leng; i_leng++)
         {
           norm += t_int[i_leng];
         }

         if(i_list < 9) sprintf(linebuffer, "%s.0%dt", args->iv_file, i_list+1);
         else           sprintf(linebuffer, "%s.%dt" , args->iv_file, i_list+1);

         out_stream = fopen(linebuffer, "w");

         CONTROL_MSG(CONTROL, "write to file '%s'\n", linebuffer);

         fprintf(out_stream, "# (%.3f, %.3f) - theor. data\n",
           (iv_cur+i_list)->spot_id.index1, (iv_cur+i_list)->spot_id.index2);
         fprintf(out_stream, "# indiv. %s = %.5f\n", r_name, rfac);
         fprintf(out_stream, "# total  %s = %.5f\n", r_name, *p_r_min);
         fprintf(out_stream,
                 "# rel. energy range  = %.3f = (%.1f eV)/(%.1f eV)\n",
                 e_range / (*p_e_range), e_range, *p_e_range);
         fprintf(out_stream, "# rel. weight = %.3f\n", (iv_cur+i_list)->weight);
         fprintf(out_stream, "# shift: Eth = Eex - (%.1f eV)\n", *p_s_min);

         for(i_leng = 0; i_leng < n_leng; i_leng++)
           fprintf(out_stream, "%f %e\n", eng[i_leng], t_int[i_leng]);

         fclose(out_stream);

         /* Calculate sum of expt. intensities for normalization */
         faux = 0.;
         for(i_leng = 0; i_leng < n_leng; i_leng++)
         {
           faux += e_int[i_leng];
         }

         norm /= faux;

         if(i_list < 9) sprintf(linebuffer, "%s.0%de", args->iv_file, i_list+1);
         else           sprintf(linebuffer, "%s.%de",  args->iv_file, i_list+1);
         out_stream = fopen(linebuffer, "w");

         CONTROL_MSG(CONTROL, "write to file '%s'\n", linebuffer);

         fprintf(out_stream, "# (%.3f, %.3f) - expt. data\n",
           (iv_cur+i_list)->spot_id.index1, (iv_cur+i_list)->spot_id.index2);
         fprintf(out_stream, "# indiv. %s = %.5f\n", r_name, rfac);
         fprintf(out_stream, "# total  %s = %.5f\n", r_name, *p_r_min);
         fprintf(out_stream, "# rel. energy range  = %.3f = (%.1f eV)/(%.1f eV)\n",
           e_range/ (*p_e_range), e_range, *p_e_range);
         fprintf(out_stream, "# rel. weight = %.3f\n", (iv_cur+i_list)->weight);

         for(i_leng = 0; i_leng < n_leng; i_leng++)
         {
           fprintf(out_stream, "%f %e\n", eng[i_leng], e_int[i_leng]*norm);
         }
       
        fclose(out_stream);

      }  /* if n_leng > 1 */

    }  /* for i_list */

  }  /* if args->iv_out */
 

  /* Free arrays */
  free(eng);
  free(e_int);
  free(t_int);

  return (*p_r_min);
}  /* end of function rfac_rmin */
