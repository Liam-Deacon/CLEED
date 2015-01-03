/*********************************************************************
 *                      LOUTINT.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/20.01.95 - Creation
 *   GH/11.08.95 - Minor changes
 *********************************************************************/

/*! \file
 *
 * Implements leed_output_intensities() function for intensity output.
 */

#include <malloc.h>
#include <stdio.h>

#include "leed.h"

/*!
 * Prints beam intensities to file \p outfile
 *
 * \param[in] Amp vector containing the beam amplitudes of all beams included
 * at the current energy.
 * \param[in] beams_now Pointer to array of #leed_beam containing information
 * on all beams included at the current energy.
 * \param[in] beams_all Pointer to array of #leed_beam containing information
 * on all beams included at the highest energy
 * \param[in] par
 * \param outfile File pointer to the output file where the intensities are
 * written to.
 * \return number of intensities written to \p outfile
 * \retval -1 if failed (not currently implemented).
 * \see matsqmod()
 */
int leed_output_intensities(const mat Amp, const leed_beam *beams_now,
                            const leed_beam *beams_all, const leed_var *par,
                            FILE *outfile)
{
  size_t i_beams_now = 0, i_beams_all, i_out;

  mat Int = NULL;
  real k_r = R_sqrt(2*par->eng_v);

  /* Calculate intensities as the square of the moduli of the amplitudes. */
  Int = matsqmod(Int, Amp);
 
  /* Print intensities for non-evanescent beams */
#if CONTROL
  CONTROL_MSG(CONTROL, "\t     beam\t  intensity\n\t\t\t== %.2f eV ==\n",
              par->eng_v*HART);
  for (i_beams_now = 0, i_out = 0; i_beams_now < Int->rows; i_beams_now ++)
  {
    if((beams_now + i_beams_now)->k_par <= k_r)
    {
      if(Int->rel[i_beams_now + 1] > INT_TOLERANCE)
      {
        fprintf(STDCTR, "\t\t(%5.2f,%5.2f):\t  %.4e\n",
                (beams_now + i_beams_now)->ind_1,
                (beams_now + i_beams_now)->ind_2,
                Int->rel[i_beams_now + 1]);
      }
      else
      {
        fprintf(STDCTR, "\t\t(%5.2f,%5.2f):\t   < %.0e\n",
                (beams_now + i_beams_now)->ind_1,
                (beams_now + i_beams_now)->ind_2,
                INT_TOLERANCE);
      }
      i_out ++;
    }

    /* also print evanescent beams
   else
     fprintf(STDCTR, "\t\t(%5.2f,%5.2f):\t (evanescent)\n",
             (beams_now + i_beams_now)->ind_1, (beams_now + i_beams_now)->ind_2,
             Int->rel[i_beams_now + 1]);
    */
  }
  fprintf(STDCTR, "\t\t(%d/%d)\n", i_beams_now, i_out);
#endif

#if CONTROL_ALL
  fprintf(STDCTR, "\n(leed_output_int): all beams: \n");

  for (i_beams_all = 0;
      ! IS_EQUAL_REAL((beams_all + i_beams_all)->k_par, F_END_OF_LIST);
      i_beams_all ++)
  {
    fprintf(STDCTR, "\t\t(%5.2f,%5.2f)\n",
            (beams_all + i_beams_all)->ind_1,
            (beams_all + i_beams_all)->ind_2);
  }
  fprintf(STDCTR, "\t\t(%d)\n", i_beams_all);
#endif

  fprintf(outfile, "%.2f ", par->eng_v*HART);

  for(i_beams_all = 0;
      ! IS_EQUAL_REAL((beams_all + i_beams_all)->k_par, F_END_OF_LIST);
      i_beams_all ++)
  {
    for(i_beams_now = 0; i_beams_now < Int->rows; i_beams_now ++)
    {
      if( IS_EQUAL_REAL((beams_all+i_beams_all)->ind_1,
                        (beams_now+i_beams_now)->ind_1) &&
          IS_EQUAL_REAL((beams_all+i_beams_all)->ind_2,
                        (beams_now+i_beams_now)->ind_2)  )
      {
        if((beams_now + i_beams_now)->k_par <= k_r)
        {
          if(Int->rel[i_beams_now + 1] > INT_TOLERANCE)
          {
            fprintf(outfile, "%.6e ", Int->rel[i_beams_now + 1]);
          }
          else fprintf(outfile, "%.6e ",0.);
        }
        else fprintf(outfile, "%.6e ",0.);

        break;
      }  /* if index = index */
    }  /* for i_beams_now */

    if( i_beams_now == Int->rows) fprintf(outfile, "%.6e ", 0.);
  } /* for i_beams_all */

  fprintf(outfile, "\n");

  fflush(outfile);

  return( (int)i_beams_now);

  /* dummy statements - compiler might not catch them due to #ifdef statements
   * and therefore variables need to be set to avoid warnings */
  i_beams_all = i_beams_all * 1;
  i_out = i_out * 1;
}  /* end of function leed_output_int */
