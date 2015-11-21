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
  size_t sa = 0;
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
 * Read data from files (including multiple angles of incidence)
 * Smooth (and sort) IV curves
 * Prepare cubic spline
 **********************************************************************/
  iv_cur = rfac_ivcur_read(args->ctr_file, args->the_file);

  if ((sa = rfac_get_number_of_angles(args->ctr_file)) == 0)
  {
    rfac_ivcur_process(iv_cur, args->vi);

    r_min = rfac_rmin(iv_cur, args, &r_min, &s_min, &e_range);
    rr = cleed_real_sqrt(args->vi * 8. / e_range);
  }
  else
  {
    /* clone control files for each angle of incidence */
    rfac_create_control_file_duplicates(args->ctr_file, sa);

    char project_name[FILENAME_MAX] = "";
    real average_shift = 0;
    real average_rfactor = 0;
    real average_energy = 0;
    real average_error = 0;
    real sum = 0;
    real sum1 = 0;
    real shift = 0;

    strncpy(project_name, args->ctr_file, sizeof(project_name)-1);
    project_name[sizeof(project_name)-1] = '\0';

    /*
     * Calculate rfactor for each angle of incidence and
     * compute average shift, rfactor and energy range
     */
    for (size_t i=0; i < sa; i++)
    {
      snprintf(args->ctr_file,
               sizeof(args->ctr_file)-1,
               "%s_%u.ctr",
               project_name,
               strrchr(args->ctr_file, '.') - args->ctr_file);

      rfac_ivcur_process(iv_cur, args->vi);

      r_min = rfac_rmin(iv_cur, args, &r_min, &s_min, &e_range);
      rr = cleed_real_sqrt(args->vi * 8. / e_range);

      fprintf(STDERR, "\nR%u = %f\n", i+1, rfac);
      rfactor_print(STDERR, args->r_type, r_min, rr, s_min, e_range);

      average_shift += shift;
      average_rfactor += r_min*e_range;
      sum += e_range;
      sum1 += (1/rr)*(1/rr);

    } /*for*/

    s_min = average_shift/(float)sa;
    r_min  = average_rfactor / sum;
    e_range = sum;
    rr = (real)sqrt(1/sum1);

  } /* else sa > 1 */

  /* print out final rfactor and clean up */
  rfactor_print(STDCTR, args->r_type, r_min, rr, s_min, e_range);

  free(args);

  return(0);
} /* end of main */
