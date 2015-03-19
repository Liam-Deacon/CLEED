/*********************************************************************
 *                        CLEED_NSYM.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/05.10.00 - early return option (-e).
 *   LD/27.03.14 - added OMP parallel for loop for energy and inverted
 *                 (use '-D_USE_OPENMP' & '-fopenmp' flags when compiling)
 *   LD/02.04.14 - added '--help', '-h' & '-V' options for usage and info,
 *                 respectively (added functions usage() & info() )
 *********************************************************************/

/*! \file
 *
 * Main program for LEED calculations (only for bravaislayer() )
 * using non-symmetrised code.
 *
 */

#include "leed.h"

/*!
 * Perform a LEED calculation for anisotropic vibrations (general case)
 */
int main(int argc, char *argv[])
{
  /* decode and check arguments */
  leed_args *args = leed_args_parse(argc, argv);

  /* read input parameters and assign to model */
  leed_model *model = leed_model_init(args);

  /* perform calculations for energy loop */
  leed_model_calculate_energies(model);
  
  /* clean up */
  leed_model_free(model);
  leed_args_free(args);

  /* set exit status explicitly  */
  exit(0);
  return(0);

} /* end of main */
