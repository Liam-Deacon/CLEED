/*********************************************************************
 *                        LPCMKTL.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014-2016 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *  WB/02.06.98 - copy from outint
 *  WB/02.06.98 - input the faktor 1/rot
 *********************************************************************/

/*!
 * \file
 *
 * Contains output function.
 */

#include <malloc.h>
#include <stdio.h>

#include "leed.h"


/*!
 * Outputs beam intensities \p outfile
 *
 * \param[in] Amp vector containing the beam amplitudes of all beams
 * included at the current energy.
 * \param beams_now all beams included at the current energy.
 * \param beams_all  all beams included at the highest energy.
 * \param[inout] outfile pointer to the output file were the intensities are
 * written to.
 *
 * \return number of intensities written to \p outfile.
 * \retval -1 if failed.
 */
int leed_output_iint_sym(mat Amp, leed_beam *beams_now, leed_beam *beams_all,
    leed_var *par, FILE *outfile)
{

  int i_beams_now, i_beams_all, i_out;
  real  val;
  real k_r = cleed_real_sqrt(2*par->eng_v);
  mat Int = matsqmod(Int, Amp);       /* Calculate intensities as the square
                                         of the moduli of the amplitudes */

  /* Print intensities for non-evanescent beams */
#if CONTROL
  CONTROL_MSG(CONTROL,"\t     beam\t  intensity\n\t\t\t== %.2f eV ==\n",
              par->eng_v*HART);
  for (i_beams_now = 0, i_out = 0; i_beams_now < Int->rows; i_beams_now ++)
  {
    if((beams_now + i_beams_now)->k_par <= k_r)
    {
      if(Int->rel[i_beams_now + 1] > INT_TOLERANCE)
        fprintf(STDCTR,"\t\t(%5.2f,%5.2f):\t  %.4e\n",
            (beams_now + i_beams_now)->ind_1, (beams_now + i_beams_now)->ind_2,
            Int->rel[i_beams_now + 1]/(beams_now + i_beams_now)->n_eqb_s);
      else
        fprintf(STDCTR,"\t\t(%5.2f,%5.2f):\t   < %.0e\n",
            (beams_now + i_beams_now)->ind_1, (beams_now + i_beams_now)->ind_2,
            INT_TOLERANCE);
      i_out ++;
    }
    /*  print also evanescent beams
   else
     fprintf(STDCTR,"\t\t(%5.2f,%5.2f):\t (evanescent)\n",
             (beams_now + i_beams_now)->ind_1, (beams_now + i_beams_now)->ind_2,
             Int->rel[i_beams_now + 1]);
     */
  }
  fprintf(STDCTR,"\t\t(%d/%d)\n",i_beams_now, i_out);
#endif

#ifdef CONTROL_ALL
  fprintf(STDCTR,"\n(leed_output_iint_sym): all beams: \n");

  for (i_beams_all = 0;
      ! IS_EQUAL_REAL((beams_all + i_beams_all)->k_par, F_END_OF_LIST); 
      i_beams_all ++)
  {
    fprintf(STDCTR,"\t\t(%5.2f,%5.2f)\n",
        (beams_all + i_beams_all)->ind_1, (beams_all + i_beams_all)->ind_2);
  }
  fprintf(STDCTR,"\t\t(%d)\n",i_beams_all);
#endif


  fprintf(outfile,"%.2f ", par->eng_v*HART);


  for(i_beams_all = 0;
      ! IS_EQUAL_REAL((beams_all + i_beams_all)->k_par, F_END_OF_LIST);
      i_beams_all ++)
  {
    for(i_beams_now = 0; i_beams_now < Int->rows; i_beams_now ++)
    {
      if( (IS_EQUAL_REAL((beams_all+i_beams_all)->ind_1, (beams_now+i_beams_now)->ind_1)) &&
          (IS_EQUAL_REAL((beams_all+i_beams_all)->ind_2, (beams_now+i_beams_now)->ind_2))  )
      {
        if((beams_now + i_beams_now)->k_par <= k_r)
        {
          if(Int->rel[i_beams_now + 1] > INT_TOLERANCE)
          {
            val = (Int->rel[i_beams_now + 1])/((beams_now + i_beams_now)->n_eqb_s);
#ifdef CONTROL_ALL
            fprintf(STDCTR,"\n(leed_output_iint_sym):neqbs = %d  neqbb = %d beam%d \n",
                (beams_now + i_beams_now)->n_eqb_s,
                (beams_now + i_beams_now)->n_eqb_b,
                i_beams_now);
#endif  
            fprintf(outfile,"%.6e ", val);
          }
          else
            fprintf(outfile,"%.6e ",0.);
        }
        else
          fprintf(outfile,"%.6e ",0.);
        break;
      }  /* if index = index */
    }  /* for i_beams_now */

    if( i_beams_now == Int->rows) fprintf(outfile,"%.6e ",0.);
  } /* for i_beams_all */

  fprintf(outfile,"\n");

  fflush(outfile);
  return(i_beams_now);
}  /* end of function leed_output_int */
