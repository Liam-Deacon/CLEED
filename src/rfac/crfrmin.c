/********************************************************************
GH/12.09.95
file contains function:

   real cr_rmin( struct crivcur *iv_cur, const struct crargs *args,
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

/**
 * Format output filenames for individual IV curves.
 */
static void cr_format_iv_filename(char *buffer, size_t size,
                                  const char *prefix, int index, char suffix)
{
  if (index < 9) {
    snprintf(buffer, size, "%s.0%d%c", prefix, index + 1, suffix);
  } else {
    snprintf(buffer, size, "%s.%d%c", prefix, index + 1, suffix);
  }
}

static int cr_count_iv_curves(const struct crivcur *iv_cur)
{
  int n_list = 0;

  while (iv_cur[n_list].group_id != I_END_OF_LIST) {
    n_list++;
  }

  return n_list;
}

static int cr_max_overlap_length(const struct crivcur *iv_cur, int n_list,
                                 const struct crargs *args)
{
  int n_leng = 0;

  for (int i_list = 0; i_list < n_list; i_list++) {
#ifdef SHIFT_DE
    real faux = (iv_cur[i_list].exp_list[iv_cur[i_list].exp_leng - 1].energy -
                 iv_cur[i_list].exp_list[0].energy);
    n_leng = MAX(n_leng, (int)(faux / args->s_step));
    faux = (iv_cur[i_list].the_list[iv_cur[i_list].the_leng - 1].energy -
            iv_cur[i_list].the_list[0].energy);
    n_leng = MAX(n_leng, (int)(faux / args->s_step));
#else
    n_leng = MAX(n_leng, iv_cur[i_list].exp_leng);
    n_leng = MAX(n_leng, iv_cur[i_list].the_leng);
#endif
  }

  return n_leng;
}

static int cr_build_overlap(real *eng, real *e_int, real *t_int, real shift,
                            const struct crargs *args,
                            const struct crivcur *curve)
{
#ifdef SHIFT_DE
  return cr_mklide(eng, e_int, t_int, args->s_step, shift,
                   curve->exp_list, curve->exp_leng,
                   curve->the_list, curve->the_leng);
#else
  return cr_mklist(eng, e_int, t_int, shift,
                   curve->exp_list, curve->exp_leng,
                   curve->the_list, curve->the_leng);
#endif
}

static void cr_compute_rfactor(const struct crargs *args, const real *eng,
                               const real *e_int, const real *t_int,
                               real *rfac, const char **label)
{
  const char *name = NULL;

  switch (args->r_type) {
    case RP_FACTOR:
      *rfac = cr_rp(eng, e_int, t_int, args->vi);
      name = "Rp";
      break;
    case RS_FACTOR:
      *rfac = cr_rs(eng, e_int, t_int, args->vi);
      name = "Rs";
      break;
    case R1_FACTOR:
      *rfac = cr_r1(eng, e_int, t_int);
      name = "R1";
      break;
    case R2_FACTOR:
      *rfac = cr_r2(eng, e_int, t_int);
      name = "R2";
      break;
    case RB_FACTOR:
      *rfac = cr_rb(eng, e_int, t_int);
      name = "Rb";
      break;
    default:
#ifdef ERROR
      fprintf(STDERR,
              "*** error (cr_rmin): invalid R factor selection %d\n",
              args->r_type);
#endif
      exit(1);
  }

  if (label) {
    *label = name;
  }
}

static void cr_accumulate_shift(const struct crargs *args,
                                const struct crivcur *iv_cur, int n_list,
                                real shift, real *eng, real *e_int,
                                real *t_int, real *rfac, real *norm,
                                real *e_range)
{
  *rfac = 0.0;
  *norm = 0.0;
  *e_range = 0.0;

  for (int i_list = 0; i_list < n_list; i_list++) {
    const struct crivcur *curve = &iv_cur[i_list];
    const int n_leng = cr_build_overlap(eng, e_int, t_int, shift, args, curve);

    if (n_leng > 1) {
      const real range = eng[n_leng - 1] - eng[0];
      const real weighted = range * curve->weight;
      real r_single = 0.0;

      *e_range += range;
      *norm += weighted;
      cr_compute_rfactor(args, eng, e_int, t_int, &r_single, NULL);
      *rfac += weighted * r_single;
    }
#ifdef WARNING
    else
      fprintf(STDWAR,
              "* warning (cr_rmin): No overlap in IV curve No. %d for shift %.1f eV\n",
              i_list, shift);
#endif
  }
}

static void cr_find_best_shift(const struct crargs *args,
                               const struct crivcur *iv_cur, int n_list,
                               real *eng, real *e_int, real *t_int,
                               real *p_r_min, real *p_s_min, real *p_e_range)
{
  real shift;
  real shift_rfac;
  real shift_norm;
  real shift_range;

  *p_r_min = 100.;

  for(shift = args->s_ini; shift <= args->s_fin; shift += args->s_step)
  {
    cr_accumulate_shift(args, iv_cur, n_list, shift, eng, e_int, t_int,
                        &shift_rfac, &shift_norm, &shift_range);

    if(IS_EQUAL_REAL(shift_norm, 0.))
    {
#ifdef ERROR
      fprintf(STDERR,
      "*** error (cr_rmin): no overlap for shift %.1f eV\n", shift);
#endif
      exit(1);
    }

    shift_rfac /= shift_norm;

#ifdef CONTROL
    fprintf(STDCTR,"(cr_rmin): shift = %4.1f, rfac = %.6f range = %.1f\n",
            shift, shift_rfac, shift_norm);
#endif

    if(shift_rfac < *p_r_min)
    {
      *p_r_min = shift_rfac;
      *p_s_min = shift;
      *p_e_range = shift_range;
    }
  }  /* for shift ... */

#ifdef CONTROL
  fprintf(STDCTR,"(cr_rmin): r_min = %.6f (shift = %4.1f)\n",
          *p_r_min, *p_s_min);
#endif
}

static void cr_write_iv_curves(const struct crivcur *iv_cur, int n_list,
                               const struct crargs *args, real shift,
                               real total_r, real total_range, real *eng,
                               real *e_int, real *t_int)
{
  char linebuffer[STRSZ];

  for (int i_list = 0; i_list < n_list; i_list++) {
    const struct crivcur *curve = &iv_cur[i_list];
    const int n_leng = cr_build_overlap(eng, e_int, t_int, shift, args, curve);

    if (n_leng > 1) {
      const real e_range = eng[n_leng - 1] - eng[0];
      real rfac = 0.0;
      const char *r_name = NULL;
      real the_sum = 0.0;
      FILE *out_stream = NULL;

      cr_compute_rfactor(args, eng, e_int, t_int, &rfac, &r_name);

      for (int i_leng = 0; i_leng < n_leng; i_leng++) {
        the_sum += t_int[i_leng];
      }

      cr_format_iv_filename(linebuffer, sizeof(linebuffer),
                            args->iv_file, i_list, 't');
      out_stream = fopen(linebuffer, "w");
#ifdef CONTROL
      fprintf(STDCTR, "(cr_rmin): write to file %s\n", linebuffer);
#endif

      fprintf(out_stream, "# (%.3f, %.3f) - theor. data\n",
              curve->spot_id.index1, curve->spot_id.index2);
      fprintf(out_stream, "# indiv. %s = %.5f\n", r_name, rfac);
      fprintf(out_stream, "# total  %s = %.5f\n", r_name, total_r);
      fprintf(out_stream, "# rel. energy range  = %.3f = (%.1f eV)/(%.1f eV)\n",
              e_range / total_range, e_range, total_range);
      fprintf(out_stream, "# rel. weight = %.3f\n", curve->weight);
      fprintf(out_stream, "# shift: Eth = Eex - (%.1f eV)\n", shift);
      for (int i_leng = 0; i_leng < n_leng; i_leng++)
        fprintf(out_stream, "%f %e\n", eng[i_leng], t_int[i_leng]);

      fclose(out_stream);

      {
        real exp_sum = 0.0;

        for (int i_leng = 0; i_leng < n_leng; i_leng++) {
          exp_sum += e_int[i_leng];
        }

        the_sum /= exp_sum;
      }

      cr_format_iv_filename(linebuffer, sizeof(linebuffer),
                            args->iv_file, i_list, 'e');
      out_stream = fopen(linebuffer, "w");
#ifdef CONTROL
      fprintf(STDCTR, "(cr_rmin): write to file %s\n", linebuffer);
#endif

      fprintf(out_stream, "# (%.3f, %.3f) - expt. data\n",
              curve->spot_id.index1, curve->spot_id.index2);
      fprintf(out_stream, "# indiv. %s = %.5f\n", r_name, rfac);
      fprintf(out_stream, "# total  %s = %.5f\n", r_name, total_r);
      fprintf(out_stream, "# rel. energy range  = %.3f = (%.1f eV)/(%.1f eV)\n",
              e_range / total_range, e_range, total_range);
      fprintf(out_stream, "# rel. weight = %.3f\n", curve->weight);
      for (int i_leng = 0; i_leng < n_leng; i_leng++)
        fprintf(out_stream, "%f %e\n", eng[i_leng], e_int[i_leng] * the_sum);

      fclose(out_stream);
    }
  }
}

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
real cr_rmin( struct crivcur *iv_cur, const struct crargs *args,
              real *p_r_min, real *p_s_min, real *p_e_range)
{

int n_list;
int n_leng;

real *eng, *e_int, *t_int;
 n_list = cr_count_iv_curves(iv_cur);
 n_leng = cr_max_overlap_length(iv_cur, n_list, args);

#ifdef CONTROL
   fprintf(STDCTR,
   "(cr_rmin): start of function, n_list = %d, n_leng = %d\n",
   n_list, n_leng);
#endif

 eng   = (real *)malloc( n_leng * sizeof(real)*13);
 e_int = (real *)malloc( n_leng * sizeof(real)*13);
 t_int = (real *)malloc( n_leng * sizeof(real)*13);
 cr_find_best_shift(args, iv_cur, n_list, eng, e_int, t_int,
                    p_r_min, p_s_min, p_e_range);

 if(args->iv_out == 1)
 {
   cr_write_iv_curves(iv_cur, n_list, args, *p_s_min, *p_r_min, *p_e_range,
                      eng, e_int, t_int);
 }  /* if args->iv_out */

 free(eng);
 free(e_int);
 free(t_int);

 return (*p_r_min);
}  /* end of function cr_rmin */
