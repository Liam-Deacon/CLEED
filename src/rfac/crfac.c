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

#include "rfac.h"

static inline const char* get_rfactor_name(rfactor_type type) {
  switch (type)
  {
    case RP_FACTOR: {return "Rp"; break;}
    case R1_FACTOR: {return "R1"; break;}
    case R2_FACTOR: {return "R2"; break;}
    case RB_FACTOR: {return "Rb1"; break;}
    default: break;
  }
  return "Unknown RFactor type";
}


static void print_rfactor(rfactor_type type, real r_min, real rr,
                          real s_min, real e_range) {
  const char *rf = get_rfactor_name(type);
#if CONTROL
  fprintf(STDCTR, "%s = ", rf);
  fprintf(STDCTR, "%.6f, RR = %.6f (shift = %4.1f, eng. overlap = %.1f)\n",
          r_min, rr, s_min, e_range);
#endif
  fprintf(STDOUT, "%.6f %.6f %.2f %.2f\t\t#  ", r_min, rr, s_min, e_range);
  fprintf(STDCTR, "%s  RR  shift  range\n", rf);
}


#if LIBRARY_BUILD == 1
int rfac_main(int argc, char *argv[])   /* compile as library function */
#else
int main(int argc, char *argv[])        /* compile as standalone program */
#endif
/**********************************************************************
 * Program calculates different R factors.
 **********************************************************************/
{
  real r_min, s_min, e_range;
  real rr;

  /* main structures */
  rfac_args *args = NULL;       /* program parameters from argument list */
  rfac_ivcur *iv_cur = NULL;   /* input data */

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
  rfac_ivcur_process(iv_cur, args->vi);

  r_min = rfac_rmin(iv_cur, args, &r_min, &s_min, &e_range);
  rr = cleed_real_sqrt(args->vi * 8. / e_range);

  print_rfactor(args->r_type, r_min, rr, s_min, e_range);

  free(args);

  return(RFAC_SUCCESS);
} /* end of main */
