/********************************************************************
GH/12.09.95
file contains function:

   real cr_rmin( struct crivcur *iv_cur, struct crargs *args,
                real *p_r_min, real *p_s_min, real *p_e_range)

 Calculate R factor and find minimum with respect to shift

Changes:
GH/30.08.95 - Creation
GH/12.09.95 - Output of IV curves for the best overlap
  
********************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>          /* needed for sqrt */

#include "crfac.h"          /* specific definitions etc. */

/*
#define CONTROL
#define SHIFT_DE
*/
#define WARNING
#define ERROR

#define SHIFT_DE

real cr_rmin( struct crivcur *iv_cur, struct crargs *args,
              real *p_r_min, real *p_s_min, real *p_e_range)

/********************************************************************
 Calculate R factor and find minimum with respect to shift.

INPUT:

  struct crivcur *iv_cur - (input) Data structure containing all 
          essential data such as expt. and theor. IV curves weight etc.

  struct crargs args - (input) argument list:
          - imaginary part of the optical potential.
          - s_ini, s_fin, s_step,
          - r_type.

DESIGN:


RETURN VALUE: 
  min. Rp, if successful.

********************************************************************/
{

int i_list, n_list;
int i_leng, n_leng;

real faux;
real shift;
real e_range, norm, rfac;

real *eng, *e_int, *t_int;

char linebuffer[STRSZ];
char r_name[STRSZ];
FILE *out_stream;

/********************************************************************
  Find number of IV curves
  Allocate storage space for cr_mklist.
********************************************************************/
 rfac = 0.;
 n_leng = 0;
 for(n_list = 0; iv_cur[n_list].group_id != I_END_OF_LIST; n_list ++)
 { 
#ifdef SHIFT_DE
   faux = ((iv_cur+n_list)->exp_list + (iv_cur+n_list)->exp_leng -1)->energy  -
          ((iv_cur+n_list)->exp_list)->energy;
   n_leng = MAX(n_leng, (int)(faux/args->s_step) );
   faux = ((iv_cur+n_list)->the_list + (iv_cur+n_list)->the_leng -1)->energy  -
          ((iv_cur+n_list)->the_list)->energy;
   n_leng = MAX(n_leng, (int)(faux/args->s_step) );
#else
   n_leng = MAX(n_leng, (iv_cur+n_list)->exp_leng);
   n_leng = MAX(n_leng, (iv_cur+n_list)->the_leng); 
#endif
 }

#ifdef CONTROL
   fprintf(STDCTR,
   "(cr_rmin): start of function, n_list = %d, n_leng = %d\n",
   n_list, n_leng);
#endif

 eng   = (real *)malloc( n_leng * sizeof(real)*13);
 e_int = (real *)malloc( n_leng * sizeof(real)*13);
 t_int = (real *)malloc( n_leng * sizeof(real)*13);

/********************************************************************
  Scan through shift and find min. R factor
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
     n_leng = cr_mklide(eng, e_int, t_int, args->s_step, shift,
              (iv_cur+i_list)->exp_list, (iv_cur+i_list)->exp_leng,
              (iv_cur+i_list)->the_list, (iv_cur+i_list)->the_leng);
#else
     n_leng = cr_mklist(eng, e_int, t_int, shift,
              (iv_cur+i_list)->exp_list, (iv_cur+i_list)->exp_leng,
              (iv_cur+i_list)->the_list, (iv_cur+i_list)->the_leng);
#endif

     if(n_leng > 1) 
     {
       e_range += faux = (eng[n_leng-1] - eng[0]);
       norm += faux *= (iv_cur+i_list)->weight;

      if(args->r_type == RP_FACTOR)
        faux *= cr_rp(eng, e_int, t_int, args->vi);
      else if (args->r_type == RS_FACTOR)
        faux *= cr_rs(eng, e_int, t_int, args->vi);
      else if (args->r_type == R1_FACTOR)
        faux *= cr_r1(eng, e_int, t_int);
      else if (args->r_type == R2_FACTOR)
        faux *= cr_r2(eng, e_int, t_int);
      else if (args->r_type == RB_FACTOR)
         faux *= cr_rb(eng, e_int, t_int);
       else 
       {
#ifdef ERROR
         fprintf(STDERR,
         "*** error (cr_rmin): invalid R factor selection %d\n", args->r_type);
#endif
         exit(1);
       }
       rfac += faux;
     }
#ifdef WARNING
     else
       fprintf(STDWAR,
       "* warning (cr_rmin): No overlap in IV curve No. %d for shift %.1f eV\n",
       i_list, shift);
#endif
   }  /* for i_list */

   if(IS_EQUAL_REAL(norm, 0.))
   {
#ifdef ERROR
     fprintf(STDERR,
     "*** error (cr_rmin): no overlap for shift %.1f eV\n", shift);
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
 fprintf(STDCTR,"(cr_rmin): r_min = %.6f (shift = %4.1f)\n", 
         *p_r_min, *p_s_min);
#endif

/********************************************************************
  Write IV curves with best agreement to files
********************************************************************/
 
 if(args->iv_out == 1)
 {
   for(i_list = 0; i_list < n_list; i_list ++)
   {

#ifdef SHIFT_DE
     n_leng = cr_mklide(eng, e_int, t_int, args->s_step, *p_s_min,
              (iv_cur+i_list)->exp_list, (iv_cur+i_list)->exp_leng,
              (iv_cur+i_list)->the_list, (iv_cur+i_list)->the_leng);
#else
     n_leng = cr_mklist(eng, e_int, t_int, *p_s_min,
              (iv_cur+i_list)->exp_list, (iv_cur+i_list)->exp_leng,
              (iv_cur+i_list)->the_list, (iv_cur+i_list)->the_leng);
#endif

     

     if(n_leng > 1)
     {

/* calculate R factor for single curve */
       e_range = (eng[n_leng-1] - eng[0]);

       if(args->r_type == RP_FACTOR)
       {
         rfac = cr_rp(eng, e_int, t_int, args->vi);
         strcpy(r_name,"Rp");
       }
       else if (args->r_type == RS_FACTOR)
       {
         rfac = cr_rs(eng, e_int, t_int, args->vi);
         strcpy(r_name,"Rs");
       }
       else if (args->r_type == R1_FACTOR)
       {
         rfac = cr_r1(eng, e_int, t_int);
         strcpy(r_name,"R1");
       }
       else if (args->r_type == R2_FACTOR)
       {
         rfac = cr_r2(eng, e_int, t_int);
         strcpy(r_name,"R2");
       }
       else if (args->r_type == RB_FACTOR)
       {
         rfac = cr_rb(eng, e_int, t_int);
         strcpy(r_name,"Rb");
       }

/* Calculate sum of theor. intensities for normalization */
       norm = 0.;
       for(i_leng = 0; i_leng < n_leng; i_leng++)
         norm += t_int[i_leng];

       if(i_list < 9) sprintf(linebuffer,"%s.0%dt",args->iv_file, i_list+1);
       else           sprintf(linebuffer,"%s.%dt", args->iv_file, i_list+1);
       out_stream = fopen(linebuffer,"w");
#ifdef CONTROL
       fprintf(STDCTR,"(cr_rmin): write to file %s\n", linebuffer);
#endif

       fprintf(out_stream,"# (%.3f, %.3f) - theor. data\n",
           (iv_cur+i_list)->spot_id.index1, (iv_cur+i_list)->spot_id.index2);
       fprintf(out_stream,"# indiv. %s = %.5f\n", r_name, rfac);
       fprintf(out_stream,"# total  %s = %.5f\n", r_name, *p_r_min);
       fprintf(out_stream,"# rel. energy range  = %.3f = (%.1f eV)/(%.1f eV)\n",
           e_range / (*p_e_range), e_range, *p_e_range);
       fprintf(out_stream,"# rel. weight = %.3f\n", (iv_cur+i_list)->weight);
       fprintf(out_stream,"# shift: Eth = Eex - (%.1f eV)\n", *p_s_min);
       for(i_leng = 0; i_leng < n_leng; i_leng++)
         fprintf(out_stream,"%f %e\n", eng[i_leng], t_int[i_leng]);

       fclose(out_stream);

/* Calculate sum of expt. intensities for normalization */
       faux = 0.;
       for(i_leng = 0; i_leng < n_leng; i_leng++)
         faux += e_int[i_leng];

       norm /= faux;

       if(i_list < 9) sprintf(linebuffer,"%s.0%de",args->iv_file, i_list+1);
       else           sprintf(linebuffer,"%s.%de", args->iv_file, i_list+1);
       out_stream = fopen(linebuffer,"w");
#ifdef CONTROL
       fprintf(STDCTR,"(cr_rmin): write to file %s\n", linebuffer);
#endif

       fprintf(out_stream,"# (%.3f, %.3f) - expt. data\n",
           (iv_cur+i_list)->spot_id.index1, (iv_cur+i_list)->spot_id.index2);
       fprintf(out_stream,"# indiv. %s = %.5f\n", r_name, rfac);
       fprintf(out_stream,"# total  %s = %.5f\n", r_name, *p_r_min);
       fprintf(out_stream,"# rel. energy range  = %.3f = (%.1f eV)/(%.1f eV)\n",
           e_range/ (*p_e_range), e_range, *p_e_range);
       fprintf(out_stream,"# rel. weight = %.3f\n", (iv_cur+i_list)->weight);
       for(i_leng = 0; i_leng < n_leng; i_leng++)
         fprintf(out_stream,"%f %e\n", eng[i_leng], e_int[i_leng]*norm);
       
       fclose(out_stream);
     }  /* if n_leng > 1 */
   }  /* for i_list */ 
 }  /* if args->iv_out */
 

/********************************************************************
  Free arrays
********************************************************************/

 free(eng);
 free(e_int);
 free(t_int);

 return (*p_r_min);
}  /* end of function cr_rmin */
