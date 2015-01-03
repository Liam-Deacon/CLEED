/*********************************************************************
 *                          CRFAC.C
 *
 *  Copyright 1992-2014 Georg Held
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Description:  Program calculates different R factors.
 *
 * Changes:
 *   GH/1995.08.29 - creation
 *********************************************************************/

/*! \file
 *  \brief Main function for CRFAC program.
 */

#include <math.h>
#include <malloc.h>
#include <strings.h>

#include "gh_stddef.h"
#include "rfac_def.h"
#include "rfac.h"

#if LIBRARY_BUILD == 1
int rfac_main(int argc, char *argv[])   /* compile as library function */
#else
int main(int argc, char *argv[])        /* compile as standalone program */
#endif
/**********************************************************************
 * Program calculates different R factors.
 **********************************************************************/
{

  int i_list;

  real r_min, s_min, e_range;
  real rr;

  /* main structures */
  rfac_args *args;             /* program parameters from argument list */
  rfac_ivcur *iv_cur;          /* input data */

  char rfversion[STRSZ];       /* current program version */

  /* print program version */
  rfac_version(rfversion);

/**********************************************************************
 * Read arguments and set up program parameters (args)
 **********************************************************************/
  args = rfac_rdargs(argc, argv);

/**********************************************************************
 * Read data from files
 **********************************************************************/
  iv_cur = rfac_ivcur_read(args->ctr_file, args->the_file);

/**********************************************************************
 * Smooth (and sort) IV curves
 * Prepare cubic spline
 **********************************************************************/

  for(i_list = 0; iv_cur[i_list].group_id != END_OF_GROUP_ID; i_list ++)
  {
    /* smooth both experimental and theoretical curves */
    rfac_iv_lorentz_smooth(iv_cur[i_list].experimental, args->vi / 2);
    rfac_iv_lorentz_smooth(iv_cur[i_list].theory, args->vi / 2);

    /* prepare cubic spline */
    rfac_iv_spline((&iv_cur[i_list])->experimental);
    rfac_iv_spline((&iv_cur[i_list])->theory);

  }  /* for i_list */
 
  r_min = rfac_rmin(iv_cur, args, &r_min, &s_min, &e_range);
  rr = R_sqrt(args->vi * 8. / e_range);

#if CONTROL
  switch (args->r_type)
  {
    case RP_FACTOR:
      fprintf(STDCTR, "Rp = ");
      break;

    case R1_FACTOR:
      fprintf(STDCTR, "R1 = ");
      break;

    case R2_FACTOR:
      fprintf(STDCTR, "R2 = ");
      break;

    case RB_FACTOR:
      fprintf(STDCTR, "Rb1 = ");
      break;

    default:
      break;
  }
  fprintf(STDCTR, "%.6f, RR = %.6f (shift = %4.1f, eng. overlap = %.1f)\n",
          r_min, rr, s_min, e_range);
#endif

  fprintf(STDOUT, "%.6f %.6f %.2f %.2f\t\t#  ", r_min, rr, s_min, e_range);
  switch (args->r_type)
  {
    case RP_FACTOR:
      fprintf(STDCTR, "Rp  RR  shift  range\n");
      break;

    case R1_FACTOR:
      fprintf(STDCTR, "R1  RR  shift  range\n");
      break;

    case R2_FACTOR:
      fprintf(STDCTR, "R2  RR  shift  range\n");
      break;

    case RB_FACTOR:
      fprintf(STDCTR, "Rb1 RR  shift  range\n");
      break;

    default:
      break;
  }

  free(args);

  return(RFAC_SUCCESS);
} /* end of main */
