/*********************************************************************
 *                      LOUTBMLIST.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/20.07.95 - Creation
 *   GH/11.08.95 - write only non-evanescent beams to output. Return value
 *                 is a list of nonevanescent beams at eng->final.
 *********************************************************************/

/*! \file
 *
 * Implements leed_output_beam_list() function to write beam list to file
 * header.
 */

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "leed.h"

/*!
 * Writes header information for beams to output file.
 *
 * Create list \c beams_out
 * ------------------------
 *
 *  First find the maximum parallel vector component of non-evanescent wave
 *  vectors at \f$ e_f \f$ .
 *
 *  \f$ k_{max} = \sqrt{2 e_f} \f$
 *
 *  All beams with parallel momentum < k_max (i.e. \f$ \vec{k}_{par} <
 *  \vec{k}_{max}^2 \f$ ) are included in list beams_out.
 *
 *  \c n_beams is then set to number of output beams.
 *
 * Output to file
 * --------------
 *
 * "#en" energies: initial energy, final energy, energy step
 * "#bn" number of beams
 * "#bi" beam indices: number (starting from 0), 1st index, 2nd index, beam set.
 *
 * \param[in,out] p_beams_out Double pointer to output list of beams.
 * This can be passed into the function as \c NULL if the list should be
 * allocated. The list will be terminated by #F_END_OF_LIST in the structure
 * element @leed_beam::k_par . Note that all structure elements of this list
 * will be copied if they are non-evanescent at energy eng->final.
 * It is assumed, that the elements beams_all->k_par contain the
 * SQUARE OF THE PARALLEL MOMENTUM as created in the function leed_beam_gen() .
 * \param[in] beams_all Pointer to an #leed_beam array for all beams used
 * throughout the energy loop. The list must be terminated by #F_END_OF_LIST in
 * the structure element @leed_beam:k_par .
 * \param[in] eng Pointer to the control parameters for the energy loop.
 * \param outfile File pointer for output stream where the intensities are
 * written to.
 * \return Number of output beams.
 * \retval -1 If unable to (re)allocate memory for \p p_beams_out and
 * #EXIT_ON_ERROR is not defined.
 * \see leed_beam_gen()
 */
int leed_output_beam_list(leed_beam **p_beams_out,
               const leed_beam *beams_all,
               const leed_energy *eng,
               FILE * outfile)
{
  size_t n_beams, n_eng;      /* number of beams and number of energies */
  size_t i_bm_all, i_bm_out;  /* counters for all beams and output beams */

  real k_max;
  real faux;

  leed_beam *beams_out;

  /* Find number of beams in list beams_all (n_beams) and
   * allocate *p_beams_out = beams_out of the same size.
   */
  for(n_beams = 0;
      ! IS_EQUAL_REAL((beams_all + n_beams)->k_par, F_END_OF_LIST);
      n_beams ++)
  { ; }

  #ifdef CONTROL
  fprintf(STDCTR, "(leed_output_beam_list): \tTotal No. of beams %.f eV = %d\n",
                 eng->final*HART, n_beams);
  #endif

  if (*p_beams_out == NULL)
  {
    *p_beams_out = beams_out = (leed_beam *)
                 calloc(n_beams + 1, sizeof(leed_beam));
  }
  else
  {
    *p_beams_out = beams_out = (leed_beam *)
                 realloc(*p_beams_out, (n_beams+1) * sizeof(leed_beam));
  }

  if(beams_out == NULL)
  {
    #ifdef ERROR
    fprintf(STDERR, " *** error (leed_output_beam_list): allocation error.\n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(LEED_ALLOCATION_ERROR);
    #else
    return(-1);
    #endif
  }

  /* Write appropriate beams to beams_out
   *
   * k_max is the square of the maximum parallel vector component of
   *       non-evanescent wave vectors at eng->final.
   * n_beams is set to number of output beams afterwards.
   */
  k_max =  2. * eng->final;

  for(i_bm_all = 0, i_bm_out = 0; i_bm_all < n_beams; i_bm_all ++)
  {
    if( (beams_all + i_bm_all)->k_par <= k_max )
    {
      memcpy( beams_out + i_bm_out,
              beams_all + i_bm_all,
              sizeof(leed_beam) );
      i_bm_out ++;
    }
  }

  /* terminate list beams_out */
  (beams_out + i_bm_out)->k_par = F_END_OF_LIST;
  n_beams = i_bm_out;

  #ifdef CONTROL
  fprintf(STDCTR, "(leed_output_beam_list): \t"
                  "No. of non_evanescent beams = %d\n", n_beams);
  #endif

  /* Write energies, number of beams, and beams to output
   *
   * "#en" energies:
   *       number of energies, initial energy, final energy, energy step.
   * "#bn" number of beams
   * "#bi" beam indices:
   *       number (starting from zero), 1st index, 2nd index, beam set.
   *
   * - Count energy points (Running through the same loop0 as the main
   *   program does avoids inconsistencies due to rounding errors)
   */

  /* energies */
  for(faux = eng->initial, n_eng = 0;
      faux <= eng->final;
      faux += eng->step, n_eng++)
  { ; }

  fprintf(outfile, "#en %d %f %f %f\n", n_eng,
          eng->initial*HART, eng->final*HART, eng->step*HART);

  /* number of beams */
  fprintf(outfile, "#bn %d\n", n_beams);

  /* beam indices */
  for(i_bm_out = 0; i_bm_out < n_beams; i_bm_out ++)
  {
    fprintf(outfile, "#bi %d %f %f %d\n",
                     i_bm_out, 
                     (beams_out+i_bm_out)->ind_1, 
                     (beams_out+i_bm_out)->ind_2,
                     (beams_out+i_bm_out)->set);
  }

  /* flush output file */
  fflush(outfile);

  /* write beams_out back to pointer */
  *p_beams_out = beams_out;

  return(n_beams);
} /* end of function leed_output_beam_list */
