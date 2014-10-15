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

/*! \file rfrmin.c
 *  \brief Implementation file for rf_rmin function.
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
 * \fn rf_rmin
 *
 * Calculate R factor and find minimum with respect to shift.
 *
 *
 * \param *iv_cur Data structure containing all essential data such
 * as experimental and theoretical IV curves weight etc.
 *
 * \param *args argument list including:
 *         - imaginary part of the optical potential.
 *         - s_ini, s_fin, s_step,
 *         - r_type.
 *
 * \param *p_r_min pointer to
 * \param *p_s_min pointer to
 * \param *p_e_range pointer to
 *
 * \return minimum R factor, if successful.
 */
real rf_rmin( rfac_ivcur *iv_cur, rfac_args *args,
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

  for(n_list = 0; iv_cur[n_list].group_id != I_END_OF_LIST; n_list ++)
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

  #ifdef CONTROL
  fprintf(STDCTR, "(cr_rmin): start of function, n_list = %d, n_leng = %d\n",
          n_list, n_leng);
  #endif

  eng   = (real *)malloc( n_leng * sizeof(real)*13);
  e_int = (real *)malloc( n_leng * sizeof(real)*13);
  t_int = (real *)malloc( n_leng * sizeof(real)*13);

/********************************************************************
 * Scan through shift and find min. R factor
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
     n_leng = rf_mklide(eng, e_int, t_int, args->s_step, shift,
              (iv_cur+i_list)->experimental->data,
              (iv_cur+i_list)->experimental->n_eng,
              (iv_cur+i_list)->theory->data,
              (iv_cur+i_list)->theory->n_eng);
     #else
     n_leng = rf_mklist(eng, e_int, t_int, args->s_step, shift,
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
         faux *= rf_rp(eng, e_int, t_int, args->vi);
       else if (args->r_type == R1_FACTOR)
         faux *= rf_r1(eng, e_int, t_int);
       else if (args->r_type == R2_FACTOR)
         faux *= rf_r2(eng, e_int, t_int);
       else if (args->r_type == RB_FACTOR)
         faux *= rf_rb(eng, e_int, t_int);
       else 
       {

         #ifdef ERROR
         fprintf(STDERR, "*** error (cr_rmin): "
                 "invalid R factor selection %d\n", args->r_type);
         #endif
         exit(1);
       }
       rfac += faux;
     }

     #ifdef WARNING
     else
       fprintf(STDWAR, "* warning (cr_rmin): No overlap in IV curve No."
               "%d for shift %.1f eV\n", i_list, shift);
     #endif

   }  /* for i_list */

   if(IS_EQUAL_REAL(norm, 0.))
   {

     #ifdef ERROR
     fprintf(STDERR, "*** error (cr_rmin): "
             "no overlap for shift %.1f eV\n", shift);
     #endif
     exit(1);
   }
   else
   {
     rfac /= norm;


     #ifdef CONTROL
     fprintf(STDCTR,"(cr_rmin): shift = %4.1f, rfac = %.6f range = %.1f\n",
             shift, rfac, norm);
     #endif

     if(rfac < *p_r_min)
     { 
       *p_r_min = rfac; 
       *p_s_min = shift;
       *p_e_range = e_range;
     }
   }  /* else (overlap) */
 }  /* for shift ... */

  #ifdef CONTROL
  fprintf(STDCTR, "(cr_rmin): r_min = %.6f (shift = %4.1f)\n",
         *p_r_min, *p_s_min);
  #endif

/********************************************************************
 * Write IV curves with best agreement to files
 ********************************************************************/
 
  if(args->iv_out == 1)
  {
    for(i_list = 0; i_list < n_list; i_list ++)
    {

       #ifdef SHIFT_DE
       n_leng = rf_mklide(eng, e_int, t_int, args->s_step, *p_s_min,
              (iv_cur+i_list)->experimental->data,
              (iv_cur+i_list)->experimental->n_eng,
              (iv_cur+i_list)->theory->data,
              (iv_cur+i_list)->theory->n_eng);
       #else
       n_leng = rf_mklist(eng, e_int, t_int, args->s_step, *p_s_min,
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
             rfac = rf_r1(eng, e_int, t_int);
             strcpy(r_name, "R1");
             break;
           }

           case R2_FACTOR:
           {
             rfac = rf_r2(eng, e_int, t_int);
             strcpy(r_name, "R2");
             break;
           }

           case RB_FACTOR:
           {
             rfac = rf_rb(eng, e_int, t_int);
             strcpy(r_name,"Rb");
             break;
           }

           case RP_FACTOR:
           {
             rfac = rf_rp(eng, e_int, t_int, args->vi);
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

         if(i_list < 9) sprintf(linebuffer,"%s.0%dt",args->iv_file, i_list+1);
         else           sprintf(linebuffer,"%s.%dt", args->iv_file, i_list+1);
         out_stream = fopen(linebuffer,"w");

         #ifdef CONTROL
         fprintf(STDCTR, "(cr_rmin): write to file %s\n", linebuffer);
         #endif

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

         if(i_list < 9) sprintf(linebuffer, "%s.0%de",args->iv_file, i_list+1);
         else           sprintf(linebuffer, "%s.%de", args->iv_file, i_list+1);
         out_stream = fopen(linebuffer, "w");

         #ifdef CONTROL
         fprintf(STDCTR, "(cr_rmin): write to file %s\n", linebuffer);
         #endif

         fprintf(out_stream, "# (%.3f, %.3f) - expt. data\n",
           (iv_cur+i_list)->spot_id.index1, (iv_cur+i_list)->spot_id.index2);
         fprintf(out_stream, "# indiv. %s = %.5f\n", r_name, rfac);
         fprintf(out_stream, "# total  %s = %.5f\n", r_name, *p_r_min);
         fprintf(out_stream, "# rel. energy range  = %.3f = (%.1f eV)/(%.1f eV)\n",
           e_range/ (*p_e_range), e_range, *p_e_range);
         fprintf(out_stream, "# rel. weight = %.3f\n", (iv_cur+i_list)->weight);

         for(i_leng = 0; i_leng < n_leng; i_leng++)
         {
           fprintf(out_stream,"%f %e\n", eng[i_leng], e_int[i_leng]*norm);
         }
       
       fclose(out_stream);

     }  /* if n_leng > 1 */

   }  /* for i_list */

 }  /* if args->iv_out */
 

/********************************************************************
 * Free arrays
 ********************************************************************/

  free(eng);
  free(e_int);
  free(t_int);

  return (*p_r_min);
}  /* end of function rf_rmin */
