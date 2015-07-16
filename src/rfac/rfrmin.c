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

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>          /* needed for sqrt */

#include "rfac.h"          /* specific definitions etc. */

#ifndef SHIFT_DE
#define SHIFT_DE 1
#endif

/*!
 * Returns the energy range of the IV curve.
 *
 * \param iv Pointer to #rfac_iv containing energy data
 * \return Energy range of IV curve.
 * \note Returns zero if either \p iv or \c data member are \c NULL
 * (or \c n_eng data member is 0).
 */
inline real rfac_iv_get_energy_range(const rfac_iv *iv) {
  /* Following statement works because (iv == NULL) is evaluated first */
  if (iv == NULL || iv->data == NULL) return 0.;
  return (iv->data[iv->n_eng-1].energy - iv->data[0].energy);
}

static inline size_t rfac_iv_make_list(
    real *energies, real *expt_intensities,
    real *theory_intensities, real de, real shift,
    rfac_iv_data *experimental_data, size_t experimental_leng,
    rfac_iv_data *theory_data, size_t theory_leng) {
#ifdef SHIFT_DE
  return rfac_mklide(energies, expt_intensities, theory_intensities, de,
        shift, experimental_data, experimental_leng, theory_data, theory_leng);
#else
  return rfac_mklist(energies, expt_intensities, theory_intensities, de,
        shift, experimental_data, experimental_leng, theory_data, theory_leng);
#endif
}

/*!
 * Returns the number of datasets in \p iv_cur array (must be terminated by
 *
 * \param iv_cur Pointer to #rfac_ivcur array, must be terminated by
 * #END_OF_GROUP_ID in \c .group_id member
 * \return Number of datasets.
 */
inline size_t rfac_ivcur_get_number_of_datasets(const rfac_ivcur *iv_cur)
{
  size_t n;
  for(n=0; &iv_cur[n] != NULL && iv_cur[n].group_id != END_OF_GROUP_ID; ++n) {};
  return n;
}

static size_t rfac_ivcur_get_n_leng(const rfac_ivcur *iv_cur, real de) {
  real e_range = 0.;
  size_t n_leng = 0;
  for(size_t i=0;
#if DEBUG
      &iv_cur[i] != NULL &&
#endif
        iv_cur[i].group_id != END_OF_GROUP_ID; i++)
  {
    fprintf(stderr, "i=%i\n", i);
#if SHIFT_DE
    e_range = rfac_iv_get_energy_range(iv_cur[i].experimental);
    n_leng = MAX(n_leng, (size_t)abs((int)(e_range/de)));
    e_range = rfac_iv_get_energy_range(iv_cur[i].theory);
    n_leng = MAX(n_leng, (size_t)abs((int)(e_range/de)));
#else
    n_leng = MAX(n_leng, iv_cur[i].experimental->n_eng);
    n_leng = MAX(n_leng, iv_cur[i].theory->n_eng);
#endif
  }
  return n_leng;
}

/*!
 * Returns the overlap between theoretical and experimental energies for the
 * current IV dataset.
 *
 * \param iv_cur Pointer to the IV curve pair to compare.
 * \return The energy overlap.
 */
static inline real rfac_ivcur_get_overlap(const rfac_ivcur *iv_cur) {
  real overlap = 0.;
#if SHIFT_DE
  overlap =  MIN(iv_cur->experimental->last_eng, iv_cur->theory->last_eng) -
             MAX(iv_cur->experimental->first_eng, iv_cur->theory->first_eng);
#endif
  return overlap;
}

/*!
 * Returns the calculated RFactor fit for a given dataset and calculation type.
 *
 * \param energies Pointer to array of energies terminated with #F_END_OF_LIST
 * \param expt_intensities Pointer to array of experimental intensities.
 * \param theory_intensities Pointer to array of theoretical intensities.
 * \param vi Imaginary part of the optical potential - used in rfac_rp()
 * \param r_type Enum for the type of RFactor to calculate.
 * \return RFactor
 */
static inline real rfactor_calculate(real *energies,
    real *expt_intensities, real *theory_intensities,
    real vi, rfactor_type r_type)
{
  real faux = 0.;

#if DEBUG
  if (energies == NULL ||
        expt_intensities == NULL || theory_intensities == NULL) {
    ERROR_MSG("input(s) are NULL\n");
    exit(EINVAL);
  }
#endif

  switch(r_type) {
    case(RP_FACTOR):
      return rfac_rp(energies, expt_intensities, theory_intensities, vi);
      break;
    case(R1_FACTOR):
      return rfac_r1(energies, expt_intensities, theory_intensities);
      break;
    case(R2_FACTOR):
      return rfac_r2(energies, expt_intensities, theory_intensities);
      break;
    case(RB_FACTOR):
      return rfac_rb(energies, expt_intensities, theory_intensities);
      break;
    default:
      ERROR_MSG("invalid R factor selection %d\n", r_type);
      exit(EINVAL);
  }
  return faux;
}

/*!
 * Returns the sum of the intensities for a given array.
 *
 * \param intensities Array of values to sum over.
 * \param n Number of elements in array.
 * \return Total sum.
 */
static inline real rfac_iv_get_sum(const real *intensities, size_t n)
{
  real sum = 0.;

#if DEBUG
  if (intensities == NULL) {
    ERROR_MSG("intensities are NULL\n");
    exit(EINVAL);
  }
#endif

  for(size_t i = 0; i < n; i++) {
    sum += intensities[i];
  }
  return sum;
}

/*!
 * Writes an IV curve to disk.
 *
 * \param iv_file String for output filepath.
 * \param energies Pointer to array of energy values.
 * \param intensities Pointer to array of intensity values.
 * \param n_leng Number of values in \p energies and \p intensities .
 * \param r_min The minimum RFactor obtained.
 * \param r_type Enum giving the RFactor type.
 * \param spot_id Pointer to #rfac_spot object for spot indexing.
 * \param weight Relative weight of IV curve - should be between 0 and 1.
 * \param shift Relative shift of theoretical curve compared to experimental
 * curve in eV.
 * \param energy_range Energy range of the current IV curve.
 * \param total_energy_range Sum of the energy ranges of all IV curves.
 * \param norm Normalisation factor. If this is < 0 then the curve is assumed
 * to be theoretical.
 */
void rfac_iv_write(const char *iv_file,
    const real *energies, const real *intensities, size_t n_leng,
    real r_fac, real r_min, rfactor_type r_type,
    const rfac_spot *spot_id, real weight, real shift,
    real energy_range, real total_energy_range, real norm)
{
  FILE *out_stream = NULL;
  char r_name[STRSZ];
  char data_type[STRSZ];

  if ((out_stream = fopen(iv_file, "w")) == NULL)
  {
    ERROR_MSG("cannot write to IV file '%s'\n", iv_file);
    exit(EIO);
  }

  strcpy(r_name, rfactor_name(r_type));
  if (norm < 0.)
    strcpy(data_type, "theor.");
  else
    strcpy(data_type, "expt.");

  fprintf(out_stream, "# (%.3f, %.3f) - %s data\n",
          spot_id->index1, spot_id->index2, data_type);
  fprintf(out_stream, "# indiv. %s = %.5f\n", r_name, r_fac);
  fprintf(out_stream, "# total  %s = %.5f\n", r_name, r_min);
  fprintf(out_stream, "# rel. energy range  = %.3f = (%.1f eV)/(%.1f eV)\n",
          energy_range/total_energy_range, energy_range, total_energy_range);
  fprintf(out_stream, "# rel. weight = %.3f\n", weight);
  if (norm < 0.)
  {
    fprintf(out_stream, "# shift: Eth = Eex - (%.1f eV)\n", shift);
    for(size_t i=0; i < n_leng; i++)
      fprintf(out_stream, "%f %e\n", energies[i], intensities[i]);
  }
  else
  {
    for(size_t i=0; i < n_leng; i++)
      fprintf(out_stream, "%f %e\n", energies[i], intensities[i]*norm);
  }

  fclose(out_stream);
}

/*!
 * Writes both theoretical and experimental IV curves to file.
 *
 * \param iv_file_prefix File prefix string for the IV curves.
 * \param iv_cur Array of #rfac_ivcur data.
 * \param n_leng Number of energies in IV data arrays.
 * \param r_min Minimum RFactor for current dataset.
 * \param shift Shift between theory and experiment IV data for current dataset.
 * \param total_energy_range Sum of energy ranges covered by all IV datasets.
 * \param r_type Enum for type of RFactor calculation used.
 * \param vi The imaginary part of the optical potential.
 */
void rfac_ivcur_write(const char *iv_file_prefix,
    const rfac_ivcur *iv_cur, size_t n_leng, real r_min, real shift, real de,
    real total_energy_range, rfactor_type r_type, real vi)
{
  char filename[PATH_MAX], buffer[PATH_MAX];
  real norm, e_range, r_fac;
  size_t n_curves = rfac_ivcur_get_number_of_datasets(iv_cur);
  real eng[n_leng], t_int[n_leng], e_int[n_leng];

  if(iv_file_prefix != NULL && strlen(iv_file_prefix) > 0)
  {
    for(size_t i_curve=0; i_curve < n_curves; i_curve++)
    {
      /* populate eng, t_int and e_int arrays with current IV dataset */
      n_leng = rfac_iv_make_list(eng, e_int, t_int, de, shift,
                                 iv_cur[i_curve].experimental->data,
                                 iv_cur[i_curve].experimental->n_eng,
                                 iv_cur[i_curve].theory->data,
                                 iv_cur[i_curve].theory->n_eng);

      /* check that there are enough data points for comparison */
      if(n_leng > 1)
      {
        /* calculate energy range */
        e_range = (eng[n_leng-1] - eng[0]);

        /* calculate normalisation factor */
        norm = rfac_iv_get_sum(t_int, n_leng) / rfac_iv_get_sum(e_int, n_leng);

        /* calculate R factor for single curve */
        r_fac = rfactor_calculate(eng, e_int, t_int, vi, r_type);

        /* prepare file prefix */
        if(i_curve < 9)
          sprintf(buffer, "%s.0%d", iv_file_prefix, i_curve+1);
        else
          sprintf(buffer, "%s0%d", iv_file_prefix, i_curve+1);

        /* write theoretical IV output */
        sprintf(filename, "%st", buffer);
        CONTROL_MSG(CONTROL, "writing theoretical IV to file '%s'\n", filename);

        rfac_iv_write(filename, eng, t_int, n_leng, r_fac, r_min, r_type,
            &iv_cur[i_curve].spot_id, iv_cur[i_curve].weight,
            shift, e_range, total_energy_range, -1.);

        /* write experimental IV output */
        sprintf(filename, "%se", buffer);
        CONTROL_MSG(CONTROL, "writing experimental IV to file '%s'\n", filename);

        rfac_iv_write(filename, eng, e_int, n_leng, r_fac, r_min, r_type,
            &iv_cur[i_curve].spot_id, iv_cur[i_curve].weight,
            shift, e_range, total_energy_range, norm);

      }  /* if n_leng > 1 */

    }  /* for i_curve */

  }  /* if */

}

void rfac_iv_print_list(const real *eng, const real *t_int, const real *e_int) {
  for (size_t i=0; eng[i] != F_END_OF_LIST; i++) {
    fprintf(stderr, "%d %f %f %f\n", i, eng[i], t_int[i], e_int[i]);
  }
}

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
 * \param[out] r_min_ptr pointer to minimum R Factor.
 * \param[out] s_min_ptr pointer to minimum energy shift.
 * \param[out] e_range_ptr pointer to energy range value.
 *
 * \return minimum R factor, if successful.
 */
real rfac_rmin(rfac_ivcur *iv_cur, rfac_args *args,
             real *r_min_ptr, real *s_min_ptr, real *e_range_ptr)
{
  size_t i_list, n_list = rfac_ivcur_get_number_of_datasets(iv_cur);

  size_t n_leng = rfac_ivcur_get_n_leng(iv_cur, args->s_step);

  real faux = 0.; //rfac_ivcur_get_overlap(iv_cur);
  real shift = 0., e_range = 0., norm = 0., rfac = 0.;

  real eng[n_leng], e_int[n_leng], t_int[n_leng];

  CONTROL_MSG(CONTROL, "start of function, n_list = %d, n_leng = %d\n",
              n_list, n_leng);

  /* Scan through shift and find minimum R factor */
  *r_min_ptr = 100.;
  eng[0] = (real)F_END_OF_LIST;
  for(shift = args->s_ini; shift <= args->s_fin; shift += args->s_step)
  {
    rfac = 0.;
    norm = 0.;
    e_range = 0.;

    for(i_list = 0; i_list < n_list; i_list ++)
    {
      n_leng = rfac_iv_make_list(eng, e_int, t_int, args->s_step, shift,
                                 iv_cur[i_list].experimental->data,
                                 iv_cur[i_list].experimental->n_eng,
                                 iv_cur[i_list].theory->data,
                                 iv_cur[i_list].theory->n_eng);

      rfac_iv_print_list(eng, t_int, e_int);
      exit(0);

     if(n_leng > 1) 
     {
       e_range += faux = (eng[n_leng-1] - eng[0]);
       norm += faux *= iv_cur[i_list].weight;
       faux = rfactor_calculate(eng, e_int, t_int, args->vi, args->r_type);
       rfac += faux;
     }
     else
       WARNING_MSG("No overlap in IV curve No.%d for shift %.1f eV\n",
                   i_list, shift);

    }  /* for i_list */


    if (IS_EQUAL_REAL(norm, 0.))
    {
      ERROR_MSG("no overlap for shift %.1f eV\n", shift);
      exit(1);
    }
    else
    {
      rfac /= norm;

      CONTROL_MSG(CONTROL, "shift = %4.1f, rfac = %.6f range = %.1f\n",
          shift, rfac, norm);

      if(rfac < *r_min_ptr)
      {
        *r_min_ptr = rfac;
        *s_min_ptr = shift;
        *e_range_ptr = e_range;
      }
    }  /* else (overlap) */
  }  /* for shift ... */

  CONTROL_MSG(CONTROL, "r_min = %.6f (shift = %4.1f)\n", *r_min_ptr, *s_min_ptr);

  /* Write IV curves with best agreement to files */
  rfac_ivcur_write(args->iv_file, iv_cur, n_leng, *r_min_ptr, shift,
                   *e_range_ptr, e_range, args->r_type, args->vi);

  return (*r_min_ptr);
}  /* end of function rfac_rmin */
