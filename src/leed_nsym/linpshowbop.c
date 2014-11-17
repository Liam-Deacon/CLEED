/************************************************************************
 *                    LINPSHOWBOP.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/26.07.95 - Creation.
 *   GH/03.05.00 - add t_type
 *   GH/05.10.00 - changes in phase shift display
 *********************************************************************/

/*! \file */

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <strings.h>

#include "leed.h"

/*!
 * Prints all the parameters read by leed_inp_read_bul() and
 * leed_read_overlayer()
 *
 * \param bulk_par Pointer to structure containing the bulk model information.
 * \param over_par Pointer to structure containing the surface model information.
 * \param phs_shifts Pointer to structure containing phase shifts information.
 * \see leed_inp_read_bul() and leed_read_overlayer()
 */
void leed_inp_show_beam_op(leed_crystal *bulk_par,
                          leed_crystal *over_par, leed_phase *phs_shifts)
{
  int i, j;                      /* counter variables */

   /* First show general information */
  fprintf(STDOUT, "*****************************"
                  "(leed_inp_show_beam_op)"
                  "****************************\n");
  fprintf(STDOUT, "\t\t\tSUMMARY OF INPUT PARAMETERS\n");
  fprintf(STDOUT, "****************************"
    "******************************************\n");
  fprintf(STDOUT, "potentials:\t%5.2f eV (Vr)\t%5.2f eV (Vi)\n",
         (bulk_par->vr)*HART, (bulk_par->vi)*HART);
  fprintf(STDOUT, "temperature:\t%5.1f K\n", over_par->temp);

  fprintf(STDOUT, "\nbulk 2-dim. unit cell:\n");
  fprintf(STDOUT, "\ta1:  (%7.4f  %7.4f)\n",
      bulk_par->a[1]*BOHR, bulk_par->a[3]*BOHR);
  fprintf(STDOUT, "\ta2:  (%7.4f  %7.4f)\n",
      bulk_par->a[2]*BOHR, bulk_par->a[4]*BOHR);

  fprintf(STDOUT, "\n     reciprocal lattice(1x1): \n");
  fprintf(STDOUT, "\ta1*: (%7.4f  %7.4f) A^-1\n",
          bulk_par->a_1[1]/BOHR, bulk_par->a_1[2]/BOHR);
  fprintf(STDOUT,"\ta2*: (%7.4f  %7.4f) A^-1\n",
          bulk_par->a_1[3]/BOHR, bulk_par->a_1[4]/BOHR);

  fprintf(STDOUT, "\nsuperstructure unit cell:\n");
  fprintf(STDOUT, "\t(%5.2f %5.2f)\tb1:  (%7.4f  %7.4f) A\n",
          bulk_par->m_super[1], bulk_par->m_super[2],
          bulk_par->b[1]*BOHR, bulk_par->b[3]*BOHR);
  fprintf(STDOUT, "\t(%5.2f %5.2f)\tb2:  (%7.4f  %7.4f) A\n",
          bulk_par->m_super[3], bulk_par->m_super[4],
          bulk_par->b[2]*BOHR, bulk_par->b[4]*BOHR);

  fprintf(STDOUT, "\n     reciprocal lattice (superstr.): \n");
  fprintf(STDOUT, "\t(%5.2f %5.2f)\tb1*: (%7.4f  %7.4f) A^-1\n",
          bulk_par->m_recip[1], bulk_par->m_recip[2],
          bulk_par->b_1[1]/BOHR, bulk_par->b_1[2]/BOHR);
  fprintf(STDOUT, "\t(%5.2f %5.2f)\tb2*: (%7.4f  %7.4f) A^-1\n",
          bulk_par->m_recip[3], bulk_par->m_recip[4],
          bulk_par->b_1[3]/BOHR, bulk_par->b_1[4]/BOHR);

  fprintf(STDOUT, "M_trans:\n");
  fprintf(STDOUT, "\t%7.4f  %7.4f\n", bulk_par->m_trans[1], bulk_par->m_trans[2]);
  fprintf(STDOUT, "\t%7.4f  %7.4f\n", bulk_par->m_trans[3], bulk_par->m_trans[4]);

  fprintf(STDOUT, "\ncomments (bulk):\n");

  for( i=0; *(bulk_par->comments + i) != NULL; i++)
  {
    fprintf(STDOUT, "\t%s", *(bulk_par->comments + i));
  }

  fprintf(STDOUT, "\ncomments (overlayer):\n");

  for( i=0; *(over_par->comments + i) != NULL; i++)
  {
    fprintf(STDOUT, "\t%s", *(over_par->comments + i));
  }

  /* Overlayer atoms  (from top to bottom) */
  fprintf(STDOUT, "\noverlayer atoms (%d layers):\n\n", over_par->n_layers);

  for(i= over_par->n_layers - 1; i >= 0; i--)
  {
    if( over_par->layers[i].periodic == 0 ) fprintf(STDOUT,"np:");
    else         fprintf(STDOUT,"p: ");

    fprintf(STDOUT, "\tover_par->layers[%d].natoms = %d\n",
        i, over_par->layers[i].n_atoms);
    for( j = 0; j < over_par->layers[i].n_atoms; j ++)
    {
      fprintf(STDOUT, "\tpos: (%7.4f  %7.4f  %7.4f) A",
              over_par->layers[i].atoms[j].pos[1]*BOHR,
              over_par->layers[i].atoms[j].pos[2]*BOHR,
              over_par->layers[i].atoms[j].pos[3]*BOHR);
      fprintf(STDOUT, "\tlayer No: %d/O atom type: %d\n",
              over_par->layers[i].atoms[j].layer,
              over_par->layers[i].atoms[j].type);
    }
    if(i != 0)
    {
      fprintf(STDOUT, "\n<-\tvec: (%7.4f  %7.4f  %7.4f) A\n\n",
              over_par->layers[i].vec_from_last[1]*BOHR,
              over_par->layers[i].vec_from_last[2]*BOHR,
              over_par->layers[i].vec_from_last[3]*BOHR );
    }
    else
    {
      fprintf(STDOUT, "\n<-\torg: (%7.4f  %7.4f  %7.4f) A\n\n",
              over_par->layers[i].vec_from_last[1]*BOHR,
              over_par->layers[i].vec_from_last[2]*BOHR,
              over_par->layers[i].vec_from_last[3]*BOHR );
    }
  }

  /* Bulk atoms  (from top to bottom) */
  fprintf(STDOUT, "\nbulk atoms (%u layers):\n", bulk_par->n_layers);
 
  fprintf(STDOUT,"\n<-\torg: (%7.4f  %7.4f  %7.4f) A\n\n",
          bulk_par->layers[bulk_par->n_layers-1].vec_to_next[1]*BOHR,
          bulk_par->layers[bulk_par->n_layers-1].vec_to_next[2]*BOHR,
          bulk_par->layers[bulk_par->n_layers-1].vec_to_next[3]*BOHR );

  for(i = bulk_par->n_layers - 1; i >= 0; i--)
  {

    if( bulk_par->layers[i].periodic == 0 ) fprintf(STDOUT, "np:");
    else fprintf(STDOUT, "p: ");

    for( j = 0; j < bulk_par->layers[i].n_atoms; j ++)
    {
      fprintf(STDOUT, "\tpos: (%7.4f  %7.4f  %7.4f) A",
              bulk_par->layers[i].atoms[j].pos[1]*BOHR,
              bulk_par->layers[i].atoms[j].pos[2]*BOHR,
              bulk_par->layers[i].atoms[j].pos[3]*BOHR);
      fprintf(STDOUT, "\tlayer No: %d/B atom type: %d\n",
              bulk_par->layers[i].atoms[j].layer,
              bulk_par->layers[i].atoms[j].type);
    }

    fprintf(STDOUT, "\n<-\tvec: (%7.4f  %7.4f  %7.4f) A\n\n",
            bulk_par->layers[i].vec_from_last[1]*BOHR,
            bulk_par->layers[i].vec_from_last[2]*BOHR,
            bulk_par->layers[i].vec_from_last[3]*BOHR );
  }

  fprintf(STDOUT, "min. bulk layer distance: %7.4f A\n", bulk_par->dmin * BOHR);
  fprintf(STDOUT, "min. over layer distance: %7.4f A\n", over_par->dmin * BOHR);

  /* Show phase shift information */

  /* find max. No of phase shifts */
  j = MAX(bulk_par->n_types, over_par->n_types);

  fprintf(STDOUT, "\n%d different sets of phase shifts used:\n", j);
  for(i = 0; i < j; i ++)
  {
    fprintf(STDOUT,
           "(%d)\t%s \n\t%d energies, lmax = %d, sqrt(<dr^2>_T) = %.3f A, ",
           i,
           (phs_shifts+i)->input_file, 
           (phs_shifts+i)->n_eng,
           (phs_shifts+i)->lmax,
           R_sqrt( (phs_shifts+i)->dr[0] ) * BOHR);

    if ((phs_shifts+i)->t_type == T_DIAG)
    {
      fprintf(STDOUT, "diagonal T\n");
    }
    else if ((phs_shifts+i)->t_type == T_NOND)
    {
      fprintf(STDOUT, "non-diagonal T\n");
    }
   else  fprintf(STDOUT, "unknown T type\n");
  }

  fprintf(STDOUT, "*****************************"
                  "(leed_inp_show_beam_op)"
                  "****************************\n");

  return;
}
