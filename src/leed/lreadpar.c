/************************************************************************
 *                    LREADPAR.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *  GH/08.08.95 - Creation (copy from leed_write_par).
 *
 *********************************************************************/

/*!
 * \file
 *
 * Reads LEED program parameters from file.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "leed.h"

/*!
 * Read all necessary program parameters from a file.
 *
 * \param p_bulk_par pointer to bulk parameters.
 * \param p_phs_shifts pointer to phase shifts.
 * \param p_par other parameters necessary to control the program.
 * \param p_eng energy parameters.
 * \param p_beams all output beams used at the highest energy.
 * \param[inout] file pointer to input file stream.
 *
 * Read parameters in the above order from the file specified.
 *
 * \note The top structures are only allocated if the corresponding pointers are
 * \c NULL. However, for all pointers inside these structures memory will
 * be allocated in any case.
 *
 * \return number of bytes that have been read from input, if okay.
 * \retval -1 if failed (and \c EXIT_ON_ERROR is not defined)
 *
 */
int leed_read_par(
    leed_crystal **p_bulk_par,
    leed_phase **p_phs_shifts,
    leed_var **p_par,
    leed_energy **p_eng,
    leed_beam **p_beams,
    FILE *file)
{
  size_t i;                /* counter, dummy  variables */
  size_t n_phs;
  size_t number;

  size_t tot_size = 0;

  leed_crystal *bulk_par = *p_bulk_par;
  leed_phase *phs_shifts = *p_phs_shifts;
  leed_var *par = *p_par;
  leed_energy *eng = *p_eng;
  leed_beam  *beams = *p_beams;

  /********************************************************************
   * Read bulk parameters from file
   * - allocate if bulk_par = NULL.
   * - parameters (bulk_par, cryst_str)
   * - layers (bulk_par->layers, layer_str)
   * - atoms (bulk_par->layers[i]->atoms, atom_str)
   * - NO COMMENTS !
   *********************************************************************/

  /* allocate memory for bulk_par */
  if( bulk_par == NULL)
  {
    CLEED_ALLOC_CHECK(bulk_par = (leed_crystal *)
        calloc(1, sizeof(leed_crystal)));
  }

  /* read parameters */
  if( fread(bulk_par, sizeof(leed_crystal), 1, file) != 1 )
  {
    ERROR_MSG("input error while reading bulk parameters\n");
  }
  tot_size += sizeof(leed_crystal);

  /* allocate memory for bulk_par->layers */
  number = bulk_par->n_layers;
  CLEED_ALLOC_CHECK(bulk_par->layers = (leed_layer *)
      calloc(number, sizeof(leed_layer) ));

  CONTROL_MSG(CONTROL,"Read parameters for %d bulk layers\n", number);

  /* read layers */
  if( fread(bulk_par->layers, sizeof(leed_layer), number, file)
      != bulk_par->n_layers )
  {
    ERROR_MSG("output error while reading bulk layers\n");
  }
  tot_size += sizeof(leed_layer) * number;

  /* atoms */
  for(i = 0; i < bulk_par->n_layers; i ++)
  {
    /* allocate */
    number = (bulk_par->layers + i)->n_atoms;
    CLEED_ALLOC_CHECK( (bulk_par->layers + i)->atoms =
        (leed_atom *) calloc(number, sizeof(leed_atom) ));

#ifdef CONTROL
    fprintf(STDCTR,"\t%d atoms in layer %d\n", number, i);
#endif

    /* read */
    if( fread( (bulk_par->layers + i)->atoms,
        sizeof(leed_atom), number, file)
        != number )
    {
      ERROR_MSG("output error while reading atoms of bulk layer %d\n", i);
    }
    tot_size += sizeof(leed_atom) * number;
  }

  /* Set comments pointer to NULL */
  CLEED_ALLOC_CHECK(bulk_par->comments = (char **)calloc(1, sizeof(char *)));
  *(bulk_par->comments) = NULL;

  /********************************************************************
   * Read phs_shifts from file
   * - number of sets of phase shifts (n_phs, int)
   * - parameters (phs_shifts, phs_str)
   * - energies (energy, real)
   * - pshift   (pshift, real)
   * - input_file (input_file, char *)
   ********************************************************************/

  /* Read number of sets */
  if( fread(&n_phs, sizeof(int), 1, file) != 1 )
  {
    ERROR_MSG("output error while reading No. of phase shifts.\n");
    ERROR_RETURN(-1);
  }
  tot_size += sizeof(int);

  CONTROL_MSG(CONTROL, "Read %d sets of phase shifts\n", n_phs);

  /* Update number of phase shifts */
  leed_update_phase(n_phs);

  /* Allocate memory for phs_shifts */
  if( phs_shifts == NULL)
  {
    CLEED_ALLOC_CHECK(phs_shifts =
        (leed_phase *) calloc((n_phs + 1), sizeof(leed_phase)));
  }

  /* Read phase shift parameters  (including terminating set) */
  if( fread(phs_shifts, sizeof(leed_phase), n_phs+1, file) != n_phs+1 )
  {
    ERROR_MSG("output error while reading phase shift parameters\n");
    ERROR_RETURN(-1);
  }
  tot_size += sizeof(leed_phase) * (n_phs+1);

  for( i = 0; i < n_phs; i ++)
  {

    /* allocate memory for energies */
    number = (phs_shifts + i)->n_eng;

    CLEED_ALLOC_CHECK( (phs_shifts + i)->energy =
        (real *) calloc(number, sizeof(real) ));

    /* read energies */
    if( fread( (phs_shifts + i)->energy, sizeof(real), number, file) != number)
    {
      ERROR_MSG("input error while reading phase shift energies (%d)\n", i);
      ERROR_RETURN(-1);
    }
    tot_size += sizeof(real) * number;

    /* allocate memory for phase shifts */
    number = (phs_shifts + i)->n_eng * ( (phs_shifts + i)->lmax + 1);

    CLEED_ALLOC_CHECK( (phs_shifts + i)->pshift =
        (real *) calloc(number, sizeof(real) ));

#ifdef CONTROL
    fprintf(STDCTR, "\tset %d: n_eng = %d, lmax = %d, ",
        i, (phs_shifts + i)->n_eng, (phs_shifts + i)->lmax );
#endif

    /* read phase shifts */
    if( fread( (phs_shifts + i)->pshift, sizeof(real), number, file) != number)
    {
      ERROR_MSG("input error while reading phase shifts (%d)\n", i);
      ERROR_RETURN(-1);
    }
    tot_size += sizeof(real) * number;

    /* read length of file name */
    if( fread( &number, sizeof(int), 1, file)!= 1)
    {
      ERROR_MSG("input error while reading ph. shift file name (%d)\n", i);
      ERROR_RETURN(-1);
    }
    tot_size += sizeof(int) * 1;
    /* allocate memory for file name */
    /* read file name */
    if( fread( (phs_shifts + i)->input_file, sizeof(char), number, file)
        != number)
    {
      ERROR_MSG("input error while reading phase shifts input file (%d)\n", i);
    }
    tot_size += sizeof(char) * number;

#ifdef CONTROL
    fprintf(STDCTR, "file name: \"%s\"\n", (phs_shifts + i)->input_file);
#endif

  }  /* for i */

  /************************************************************************
   * Read other parameters from file.
   * - parameters (par, var_str)
   *************************************************************************/

  /* allocate memory for par */
  if( par == NULL)
  {
    CLEED_ALLOC_CHECK(par = (leed_var *) calloc(1, sizeof(leed_var)));
  }

  /* read parameters */
  if( fread(par, sizeof(leed_var), 1, file) != 1 )
  {
    ERROR_MSG("input error while reading control parameters\n");
    ERROR_RETURN(-1);
  }
  tot_size += sizeof(leed_var) * 1;

  /************************************************************************
   * Read energy parameters from file.
   * - parameters (eng, eng_str)
   *************************************************************************/

  /* allocate memory for eng */
  if( eng == NULL)
  {
    CLEED_ALLOC_CHECK(eng = (leed_energy *) calloc(1, sizeof(leed_energy)));
  }

  /* read parameters */
  if( fread(eng, sizeof(leed_energy), 1, file) != 1 )
  {
    ERROR_MSG("input error while reading energy parameters\n");
    ERROR_RETURN(-1);
  }
  tot_size += sizeof(leed_energy) * 1;

  CONTROL_MSG(CONTROL,
      "Read energy parameters: Eini = %.1f, Efin = %.1f, Estp = %.1f\n",
      eng->initial, eng->final, eng->step);


  /************************************************************************
   * Read beam list from file.
   * - beam list (beams, beam_str)
   *************************************************************************/

  /* Read number of beams */
  if( fread(&number, sizeof(int), 1, file) != 1 )
  {
    ERROR_MSG("input error while reading No. of beams.\n");
    ERROR_RETURN(-1);
  }
  tot_size += sizeof(int) * 1;
  CONTROL_MSG(CONTROL, "Read %d beam parameters\n", number-1);

  /* Allocate memory for beam list */
  if( beams == NULL)
  {
    CLEED_ALLOC_CHECK(beams = (leed_beam *)calloc(number, sizeof(leed_beam)));
  }

  /* Read beam list */
  if( fread(beams, sizeof(leed_beam), number, file) != number )
  {
    ERROR_MSG("input error while reading beam list\n");
    ERROR_RETURN(-1);
  }
  tot_size += sizeof(leed_beam) * number;

#ifdef CONTROL
  fprintf(STDCTR, "\t%3d: (%6.3f,%6.3f)\n",
      0, (beams)->ind_1, (beams)->ind_2);
  fprintf(STDCTR, "\t%3d: (%6.3f,%6.3f)\n",
      number-2, (beams+number-2)->ind_1, (beams+number-2)->ind_2);
#endif

  /* Read total number of bytes from file for control reasons */
  if( fread(&number, sizeof(int), 1, file) != 1 )
  {
    ERROR_MSG("input error while reading control number\n");
    ERROR_RETURN(-1);
  }
  if( number != tot_size)
  {
    ERROR_MSG("%d bytes are missing\n", number - tot_size);
    ERROR_RETURN(-1);
  }
  tot_size += sizeof(int) * 1;

  /* Write bulk_par, phs_shifts, par, eng, and beams back to their pointers. */
  *p_bulk_par   = bulk_par;
  *p_phs_shifts = phs_shifts;
  *p_par        = par;
  *p_eng        = eng;
  *p_beams      = beams;

  CONTROL_MSG(CONTROL, "%d bytes read\n", tot_size);

  return(tot_size);
}
