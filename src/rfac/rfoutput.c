/*********************************************************************
 *                           RFOUTPUT.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1994.08.15 - creation
 *********************************************************************/

/*! \file rfoutput.c
 *
 * Produces output of rfactors to files or stdout.
 */

#include <stdio.h>
#include <strings.h>

#include "rfac.h"

/*
#define CONTROL
*/

static int n_call = 0;

/*!
 * \fn rf_output
 *
 * \brief General output routine
 *
 * The amount and the format of output depend on the argument flags
 * "all_groups" and "out_file", respectively. The latter specifies the
 * output path:
 *  - NULL, "-", "stdout" direct the output to stdout
 *   (format: geometry - all R factors + ).
 * - "single"   creates a single file for each R factor
 *   (format: geometry - single R factor).
 * - <file name> directs the output to one file
 *   (format: geometry - all R factors + ).
 *
 * If the "all_groups" flag is set, the average R factor and the R
 * factors of each group of beams (identified by  different group_id's)
 * will be displayed. Otherwise only the average R factor of all groups
 * is displayed.
 *
 * For "single" output only average R factors can be displayed.
 *
 * \param[in] *args pointer to program parameters held in \ref rfac_args struct.
 *
 * \param[in] *rf pointer to list of R factors (average and for each group ID).
 *
 * \param i_geo, i_shift  offsets in list args.p_geo and args.p_shift,
 * respectively.
 *
 * The output is either to stdout or files.
 *
 * \note: if args.out_file is NULL, then output will be written to stdout.
 *
 */
int rf_output(rfac_args *args, rfac *rf, size_t i_geo, size_t i_shift)
{
  int i_id;
  int i_str, n_str;

  char rfversion[STRSZ];
  char title[STRSZ];

  FILE *out_stream[N_RFACTORS];

  rf_version(rfversion);                /* current program version */

  /* make title line */
  #ifdef CONTROL
  if (n_call == 0)
  {
    switch(args->r_switch.ek)
    {
      case E_AXIS:
        strcpy (title, "\n>R FACTORS - I(E):\n\nGEO  ID  ");
        break;

      case K_AXIS:
        strcpy (title, "\n>R FACTORS - I(k):\n\nGEO  ID  ");
        break;
    }

    if (args->r_switch.r_1 == true) strcat(title, "  R_1  ");
    if (args->r_switch.r_2 == true) strcat(title, "  R_2  ");
    if (args->r_switch.r_b == true) strcat(title, "  R_b1   R_b2 ");
    if (args->r_switch.r_g == true) strcat(title, "  R_g  ");
    if (args->r_switch.r_p == true) strcat(title, "  R_p  ");

    strcat (title, " RANGE    RR    RATIO   SHIFT\n");
  }
  #endif

  if ( (args->out_file == NULL) || strncmp(args->out_file, "sing", 4) )
  {
    if ( (args->out_file == NULL) ||
       !strncmp(args->out_file, "stdo", 4) ||
       (args->out_file[0] == '-') )
    {
      out_stream[0] = stdout;
      n_str = 0;
    }
    else
    {
      if (n_call == 0)
      {
        out_stream[0] = fopen(args->out_file, "w");
        printf(">rf_output: writing output to \"%s\"\n", args->out_file);
        fprintf(out_stream[0], "# %s\n", rfversion);
        rf_ctr2out(args->ctr_file, out_stream[0]);
        title[0] = '#';
      }
      else
      {
        out_stream[0] = fopen(args->out_file, "a");
      }

      n_str = 1;

    }

    #ifdef CONTROL
    if (n_call == 0) fprintf(out_stream[0],title);
    fprintf(out_stream[0], "%3d  AV  ", args->p_geo[i_geo]+1);
    #endif

    /* print R factors */
    if (args->r_switch.r_1) fprintf(STDOUT,"%.6f ", rf[0].r_1);
    if (args->r_switch.r_2) fprintf(STDOUT,"%.6f ", rf[0].r_2);
    if (args->r_switch.r_b)
     fprintf(STDOUT,"%.6f %.6f ", rf[0].r_b1, rf[0].r_b2);
    if (args->r_switch.r_g) fprintf(STDOUT,"%.6f ", rf[0].r_g);
    if (args->r_switch.r_p) fprintf(STDOUT,"%.6f ", rf[0].r_p);

    /*
     * print:
     * - energy range
     * - RR factor
     * - shift
    */
    fprintf(STDOUT, "\t%6.1f  %.3f %4.1f\n", rf[0].e_range, rf[0].rr,
            args->p_shift[i_shift] );

    if (args->all_groups)
    {
      for(i_id = 1; rf[i_id].group_id != I_END_OF_LIST ; i_id ++)
      {
        fprintf(out_stream[0], "    %3d  ",rf[i_id].group_id);

        if (args->r_switch.r_1) fprintf(out_stream[0], "%.4f ", rf[i_id].r_1);
        if (args->r_switch.r_2) fprintf(out_stream[0], "%.4f ", rf[i_id].r_2);
        if (args->r_switch.r_b)
        {
          fprintf(out_stream[0], "%.4f %.4f ", rf[i_id].r_b1, rf[i_id].r_b2);
        }
        if (args->r_switch.r_g) fprintf(out_stream[0], "%.4f ", rf[i_id].r_g);
        if (args->r_switch.r_p) fprintf(out_stream[0], "%.4f ", rf[i_id].r_p);

        fprintf(out_stream[0], "%6.1f  %.3f %.3e %4.1f\n", rf[i_id].e_range,
                rf[i_id].rr, rf[i_id].i_ratio, args->p_shift[i_shift] );
      } /* for */

      fprintf(out_stream[0],"\n");

    } /* all_groups */

  }  /* not single */

  else /* write each r factor to a single file */
  {
    i_str = 0;
    if (n_call == 0)
    {
      if (args->r_switch.r_1)
      {
        out_stream[i_str] = fopen("rf.r1", "w");
        fprintf(out_stream[i_str], "# %s (Geometry vs. R1-factor)\n", rfversion);
        rf_ctr2out(args->ctr_file, out_stream[0]);
        fprintf(out_stream[i_str],"%d %e\n", args->p_geo[i_geo]+1, rf[0].r_1);
        i_str ++;
      }
      if (args->r_switch.r_2)
      {
        out_stream[i_str] = fopen("rf.r2", "w");
        fprintf(out_stream[i_str], "# %s (Geometry vs. R2-factor)\n", rfversion);
        rf_ctr2out(args->ctr_file, out_stream[0]);
        fprintf(out_stream[i_str],"%d %e\n", args->p_geo[i_geo]+1, rf[0].r_2);
        i_str ++;
      }
      if (args->r_switch.r_b)
      {
        out_stream[i_str] = fopen("rf.rb1","w");
        fprintf(out_stream[i_str], "# %s (Geometry vs. Rb1-factor)\n", rfversion);
        rf_ctr2out(args->ctr_file, out_stream[0]);
        fprintf(out_stream[i_str],"%d %e\n", args->p_geo[i_geo]+1, rf[0].r_b1);
        i_str ++;
        out_stream[i_str] = fopen("rf.rb2","w");
        fprintf(out_stream[i_str], "# %s (Geometry vs. Rb2-factor)\n", rfversion);
        rf_ctr2out(args->ctr_file, out_stream[0]);
        fprintf(out_stream[i_str],"%d %e\n", args->p_geo[i_geo]+1, rf[0].r_b2);
        i_str ++;
      }
      if (args->r_switch.r_g)
      {
        out_stream[i_str] = fopen("rf.rg", "w");
        fprintf(out_stream[i_str], "# %s (Geometry vs. Georgie's R-factor)\n",
               rfversion);
        rf_ctr2out(args->ctr_file, out_stream[0]);
        fprintf(out_stream[i_str],"%d %e\n", args->p_geo[i_geo]+1, rf[0].r_g);
        i_str ++;
      }
      if (args->r_switch.r_p)
      {
        out_stream[i_str] = fopen("rf.rp", "w");
        fprintf(out_stream[i_str], "# %s (Geometry vs. Pendry's-factor)\n",
               rfversion);
        rf_ctr2out(args->ctr_file, out_stream[0]);
        fprintf(out_stream[i_str],"%d %e\n", args->p_geo[i_geo]+1, rf[0].r_p);
        i_str ++;
      }

    }   /* if n_call = 0 */
    else  /* n_call != 0 */
    {
      if (args->r_switch.r_1)
      {
        out_stream[i_str] = fopen("rf.r1", "a");
        fprintf(out_stream[i_str], "%d %e\n", args->p_geo[i_geo]+1, rf[0].r_1);
        i_str ++;
      }
      if (args->r_switch.r_2)
      {
        out_stream[i_str] = fopen("rf.r2", "a");
        fprintf(out_stream[i_str], "%d %e\n", args->p_geo[i_geo]+1, rf[0].r_2);
        i_str ++;
      }
      if (args->r_switch.r_b)
      {
        out_stream[i_str] = fopen("rf.rb1", "a");
        fprintf(out_stream[i_str], "%d %e\n", args->p_geo[i_geo]+1, rf[0].r_b1);
        i_str ++;
        out_stream[i_str] = fopen("rf.rb2", "a");
        fprintf(out_stream[i_str], "%d %e\n", args->p_geo[i_geo]+1, rf[0].r_b2);
        i_str ++;
      }
      if (args->r_switch.r_g)
      {
        out_stream[i_str] = fopen("rf.rg", "a");
        fprintf(out_stream[i_str], "%d %e\n", args->p_geo[i_geo]+1, rf[0].r_g);
        i_str ++;
      }
      if (args->r_switch.r_p)
      {
        out_stream[i_str] = fopen("rf.rp", "a");
        fprintf(out_stream[i_str], "%d %e\n", args->p_geo[i_geo]+1, rf[0].r_p);
        i_str ++;
      }

    }    /* if n_call != 0 */

    n_str = i_str;

  }     /* single */

  for (i_str=0; i_str < n_str; i_str ++) fclose(out_stream[i_str]);
  n_call ++;

  return(0);
}      /* end */
