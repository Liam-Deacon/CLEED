/**********************************************************************
 *                      lwritepar.c
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/08.08.95 - Creation (copy from leed_read_overlayer).
 *********************************************************************/

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "leed_def.h"
#include "cleed_real.h"
#include "gh_stddef.h"

/*********************************************************************
  Write all program parameters to a file.

  INPUT:

   leed_crystal *bulk_par - (input) bulk parameters.
   leed_phase   *phs_shifts - (input) phase shifts.
   leed_var   *par - (input) other parameters necessary to control
              the program.
   leed_energy   *eng - (input) energy parameters.
   leed_beam  *beams - (input) all output beams used at the highest 
              energy.
   FILE* file - (input) pointer to output file.

  DESIGN:

   Write parameters in the above order to the file specified.

  FUNCTION CALLS:

  RETURN VALUES:

    tot_size = number of bytes that have been written if ok.
    -1 if failed (and EXIT_ON_ERROR is not defined)

*********************************************************************/
int leed_write_par(leed_crystal *bulk_par,
              leed_phase   *phs_shifts,
              leed_var   *par,
              leed_energy   *eng,
              leed_beam  *beams,
              FILE* file)
{
  size_t i;                /* counter, dummy  variables */
  size_t n_phs;
  size_t number;
  size_t tot_size;

  tot_size = 0;

  /********************************************************************
  Write bulk parameters to file
   - parameters (bulk_par, cryst_str)
   - layers (bulk_par->layers, layer_str)
   - atoms (bulk_par->layers[i]->atoms, atom_str)
   - NO COMMENTS !
   *********************************************************************/

  /* parameters */
  if( fwrite(bulk_par, sizeof(leed_crystal), 1, file) != 1 )
  {
    ERROR_MSG("output error while writing bulk parameters\n");
    ERROR_RETURN(-1);
  }
  tot_size += sizeof(leed_crystal) * 1;

  /* layers */
  if( fwrite(bulk_par->layers, sizeof(leed_layer),
      bulk_par->n_layers, file) != (unsigned int) bulk_par->n_layers )
  {
    ERROR_MSG("output error while writing bulk layers\n");
    ERROR_RETURN(-1);
  }
  tot_size += sizeof(leed_layer) * bulk_par->n_layers;

  /* atoms */
  for(i = 0; i < bulk_par->n_layers; i ++)
  {
    if( fwrite( (bulk_par->layers + i)->atoms,
        sizeof(leed_atom),
        (bulk_par->layers + i)->n_atoms, file)
        != (size_t) (bulk_par->layers + i)->n_atoms )
    {
      ERROR_MSG("output error while writing atoms of bulk layer %d\n", i);
      ERROR_RETURN(-1);
    }
    tot_size += sizeof(leed_atom) * (bulk_par->layers + i)->n_atoms;
  }

  /********************************************************************
  Write phs_shifts to file
   - number of sets of phase shifts (n_phs, int)
   - parameters (phs_shifts, phs_str)
   - energies (energy, real)
   - pshift   (pshift, real)
   - input_file (input_file, char *)
   ********************************************************************/

  /* Find number of sets of phase shifts.  */
  for(n_phs = 0; (phs_shifts + n_phs)->lmax != I_END_OF_LIST; n_phs ++)
  { ; }

  /* number of sets */
  if( fwrite(&n_phs, sizeof(int), 1, file) != 1 )
  {
    ERROR_MSG("output error while writing No. of phase shifts.\n");
    ERROR_RETURN(-1);
  }
  tot_size += sizeof(int) * 1;

  /* parameters (including terminating set) */
  if( fwrite(phs_shifts, sizeof(leed_phase), n_phs+1, file) != n_phs+1 )
  {
    ERROR_MSG("output error while writing phase shift parameters\n");
    ERROR_RETURN(-1);
  }
  tot_size += sizeof(leed_phase) * (n_phs+1);

  for( i = 0; i < n_phs; i ++)
  {

    /* energies */
    number = (phs_shifts + i)->n_eng;
    if( fwrite( (phs_shifts + i)->energy, sizeof(real), number, file) != number)
    {
      ERROR_MSG("output error while writing phase shift energies (%d)\n", i);
      ERROR_RETURN(-1);
    }
    tot_size += sizeof(real) * number;

    /* phase shifts */
    number = (phs_shifts + i)->n_eng * ( (phs_shifts + i)->lmax + 1);
    if( fwrite( (phs_shifts + i)->pshift, sizeof(real), number, file) != number)
    {
      ERROR_MSG("output error while writing phase shifts (%d)\n", i);
      ERROR_RETURN(-1);
    }
    tot_size += sizeof(real) * number;

    /* write length of file name */
    number = strlen( (phs_shifts + i)->input_file ) + 1;
    if( fwrite( &number, sizeof(int), 1, file) != 1 )
    {
      ERROR_MSG("output error while writing ph. shift file name (%d)\n", i);
      ERROR_RETURN(-1);
    }
    tot_size += sizeof(int) * 1;

    /* write file name */
    if( fwrite( (phs_shifts + i)->input_file, sizeof(char), number, file)
        != number)
    {
      ERROR_MSG("output error while writing phase shifts input file (%d)\n", i);
      ERROR_RETURN(-1);
    }
    tot_size += sizeof(char) * number;

  }

  /************************************************************************
  Write other parameters to file.
   - parameters (par, var_str)
   *************************************************************************/

  /* parameters */
  if( fwrite(par, sizeof(leed_var), 1, file) != 1 )
  {
    ERROR_MSG("output error while writing control parameters\n");
    ERROR_RETURN(-1);
  }
  tot_size += sizeof(leed_var) * 1;

  /************************************************************************
  Write energy parameters to file.
   - parameters (eng, eng_str)
   *************************************************************************/

  /* parameters */
  if( fwrite(eng, sizeof(leed_energy), 1, file) != 1 )
  {
    ERROR_MSG("output error while writing energy parameters\n");
    ERROR_RETURN(-1);
  }
  tot_size += sizeof(leed_energy) * 1;

  /************************************************************************
  Write beam list to file.
   - beam list (beams, beam_str)
   *************************************************************************/

  /* Find number of beams.  */
  for(number = 0; ! IS_EQUAL_REAL((beams + number)->k_par, F_END_OF_LIST); number ++)
  { ; }
  number ++;

  /* number of beams */
  if( fwrite(&number, sizeof(int), 1, file) != 1 )
  {
    ERROR_MSG("output error while writing No. of beams.\n");
    ERROR_RETURN(-1);
  }
  tot_size += sizeof(int) * 1;

  /* beam list */
  if( fwrite(beams, sizeof(leed_beam), number, file) != number )
  {
    ERROR_MSG("output error while writing beam list\n");
    ERROR_RETURN(-1);
  }
  tot_size += sizeof(leed_beam) * number;

  CONTROL_MSG(CONTROL, "%d bytes written\n", tot_size);


  /************************************************************************
  Write total number of bytes to file for control reasons
   *************************************************************************/

  if( fwrite(&tot_size, sizeof(int), 1, file) != 1 )
  {
    ERROR_MSG("output error while writing control number\n");
    ERROR_RETURN(-1);
  }
  tot_size += sizeof(int) * 1;

  return(tot_size);
}
