/*********************************************************************
 *                          CRFAC.C
 *
 *  Copyright 1992-2014 Georg Held
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
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
#include <stdio.h>

#include "rfac.h"

int main(int argc, char **argv)
/**********************************************************************
 * Program calculates different R factors.
 **********************************************************************/
{
  real r_min, s_min, e_range;
  real rr;

  /* main structures */
  rfac_args *args = NULL;       /* program parameters from argument list */
  rfac_ivcur *iv_cur = NULL;    /* input data */

  const char *rfversion = rfac_version();   /* current program version */

  /* print program version */

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

  rfactor_print(args->r_type, r_min, rr, s_min, e_range);

  free(args);

  return(RFAC_SUCCESS);
} /* end of main */
