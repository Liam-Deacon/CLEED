/*********************************************************************
 *                        LINPPHASEND.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/04.07.94 - Creation
 *   GH/19.01.95 - add eng_max and eng_min
 *   GH/08.08.95 - update i_phase
 *   GH/02.09.97 - Set input path by environment variable CLEED_PHASE
 *                 (makes PHASE_PATH obsolete)
 *   WB/26.02.98 - Correct control output
 *   GH/03.05.00 - extend list of parameters for leed_leed_inp_phase: t_type
 *   GH/15.07.03 - fix bug in energy scaling factor for Ry (was 2./HART, now 2.).
 *********************************************************************/

/*! \file
 *
 * Implements leed_update_phase() function for reading phase shift files into
 * the \c leed program.
 */

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#include "leed.h"
#include "leed_def.h"

#if defined(WIN32) || defined(_WIN32)
static const char *PATH_SEPARATOR = "\\";
#else
static const char *PATH_SEPARATOR = "/";
#endif

static size_t i_phase = 0;      /*!< number of unique phase shifts */
static int ignore_t_type = 0;

/*!
 * Updates the number of phase shifts
 *
 * \param n number i_phase to be set to.
 * \return number of phase shifts
 */
size_t leed_update_phase(size_t n)
{
  i_phase = n;
  return(i_phase);
} /* end of function leed_update_phase */

/*!
 * Reads phase shifts from an input file and stores them internally as
 * #leed_phase objects.
 *
 * \param[in] phaseinp Either full path name of phase shift file or the tag
 * for type of atom (will have the path contained in the \c CLEED_PHASE
 * environment variable prepended and extension ".phs" appended on to each tag).
 * \param dr Pointer to displacement vector for thermal vibrations.
 * \param t_type Matrix type to match (<0 to ignore).
 * \param[out] p_phs_shifts Double pointer to the phase shifts.
 * \return number of phase shifts.
 * \retval -1 on failure and if #EXIT_ON_ERROR is not defined.
 * \note Additional output can be enabled by defining #ERROR_LOG and #WARNING_LOG when
 * compiling.
 * \warning The phase shifts in the input file must be for increasing energies.
 * \todo Check behavior is maintained from previous version.
 */
int leed_inp_phase(const char *phaseinp,
    real *dr, leed_phase **p_phs_shifts, int t_type)
{
  FILE *inp_stream = NULL;

  char filename[STRSZ] = "";
  char linebuffer[STRSZ] = "";
  char eng_type[STRSZ] = "";
  char fmt_buffer[STRSZ] = "%e";

  leed_phase *phs_shifts = NULL;

  size_t i, i_str, i_eng;

  size_t neng, lmax, nl;       /* neng = No. of energies to be read
                                * lmax = max. quantum number;
                                * nl   = No. of phase shifts = lmax + 1
                                */

  real eng_scale;

  /*
   * Create name of input file:
   * - If phaseinp is a full path (starting with '/'), this is used as
   *   name of the input file.
   * - Otherwise the filename is put together from CLEED_PHASE, phaseinp
   *   and the extension '.phs'.
   * - Check whether environment variable CLEED_PHASE exists at all.
   */

  if( (*phaseinp != '/') || !(isalpha(phaseinp[0]) && phaseinp[1] == ':'))
  {
    if(getenv("CLEED_PHASE") == NULL)
    {

      ERROR_MSG("environment variable CLEED_PHASE not defined\n");
      ERROR_RETURN(-1);
    }
    if ((inp_stream = fopen(phaseinp, "r")) == NULL) {
      sprintf(filename, "%s%s%s.phs", getenv("CLEED_PHASE"),
            PATH_SEPARATOR, phaseinp);
    } else {
      strcpy(filename, phaseinp);
      fclose(inp_stream);
    }
  }
  else sprintf(filename, "%s", phaseinp);

  /* set real format specifier */
  sprintf(fmt_buffer, "%%%se", CLEED_REAL_FMT);

  if(i_phase > 0)
  {
    /* Compare filename, dr, and t_type with previous phaseshifts.
     * Return the corresponding phase shift number if the same combination has
     * already been read.
     */
    for(i=0; i < i_phase; i++)
    {
      if( (!strcmp( (*p_phs_shifts + i)->input_file, filename) )         &&
          ( cleed_real_fabs(dr[1] - (*p_phs_shifts + i)->dr[1]) < GEO_TOLERANCE ) &&
          ( cleed_real_fabs(dr[2] - (*p_phs_shifts + i)->dr[2]) < GEO_TOLERANCE ) &&
          ( cleed_real_fabs(dr[3] - (*p_phs_shifts + i)->dr[3]) < GEO_TOLERANCE ) &&
          ( t_type == (*p_phs_shifts + i)->t_type  || t_type < 0))
      {
        return((int)i);
        break;
      }
    }
    i_phase ++;
    CLEED_REALLOC(*p_phs_shifts, (i_phase + 1) * sizeof(leed_phase) );
  }
  else
  {
    i_phase ++;
    CLEED_ALLOC_CHECK(
      *p_phs_shifts = (leed_phase *) calloc( 2, sizeof(leed_phase) ));
  }

  /* Terminate list of phase shifts */
  (*(p_phs_shifts) + i_phase)->lmax = LMAX_TOO_LARGE;

  phs_shifts = *(p_phs_shifts) + i_phase-1;

  /* write dr and t_type to phs_shifts */
  for(i=0; i<=3; i++) phs_shifts->dr[i] = dr[i];
  phs_shifts->t_type = t_type;

  /* Open and Read input file for a new set of phase shifts */
  CONTROL_MSG(CONTROL,
              "Reading file \"%s\", i_phase = %d\n", filename, i_phase-1);

  /* Open input file.
   * Copy the full filename into phs_shifts->input_file.
   */
  if( (inp_stream = fopen(filename, "r")) == NULL)
  {
    ERROR_MSG("could not open file \"%s\" (%s)\n", 
      filename, strerror(errno));
    ERROR_EXIT_RETURN(errno, -1);
  }

  strcpy(phs_shifts->input_file, filename);

  /* Read the first line of the input file which contains the number of
   * energies to be read in (neng) and the maximum phase shift quantum
   * number (lmax). */

  /* read comment lines */
  while( *fgets(linebuffer, STRSZ, inp_stream) == '#');

  if ( linebuffer == NULL)     /* EOF found */
  {
    ERROR_MSG("unexpected EOF found while reading file \"%s\"\n", filename);
    exit(EIO);
  }
  else if( sscanf(linebuffer, "%d %d %s", &neng, &lmax, eng_type) < 3)
  {
    ERROR_MSG("improper input line in file \"%s\":\n%s", filename, linebuffer);
    ERROR_EXIT_RETURN(EIO, -1);
  }

  /* Define energy scale according to eng_type. The default is
   * input in Hartree units (27.18 eV)
   */
  if( !strncmp(eng_type, "eV", 2) || !strncmp(eng_type, "EV", 2) )
  {
    eng_scale = 1./HART;
    CONTROL_MSG(CONTROL, "Energy input in eV\n");
  }
  else if( !strncmp(eng_type, "Ry", 2) || !strncmp(eng_type, "RY", 2) )
  {
    eng_scale = 2.;
    CONTROL_MSG(CONTROL, "Energy input in Rydberg (13.59 eV)\n");
  }
  else
  {
    eng_scale = 1.;
    CONTROL_MSG(CONTROL, "Energy input in Hartree (27.18 eV)\n");
  }
  
  /* Read energies and phase shifts.
   * Find max and min energy
   * NB: The search for blank or '-' after reading each number is needed
   * because of the FORTRAN format used for the VHT input files which does
   * not have any blank character between negative numbers.
   */
  phs_shifts->lmax = lmax;
  nl = lmax + 1;

  CLEED_ALLOC_CHECK(phs_shifts->energy = (real*)calloc(neng, sizeof(real)));
  CLEED_ALLOC_CHECK(phs_shifts->pshift = (real*)calloc(neng*nl, sizeof(real)));
 
 
  for( i_eng = 0;
      (i_eng < neng) && (fgets(linebuffer, STRSZ, inp_stream) != NULL);
       i_eng ++)
  {

    if (sscanf(linebuffer, fmt_buffer, phs_shifts->energy + i_eng) < 1)
    {
      ERROR_MSG("could not read energy value #%u from '%s'\n",
        i_eng, linebuffer);
    }
    phs_shifts->energy[i_eng] *= eng_scale;
   
    if (i_eng == 0)
    {
      phs_shifts->eng_min = phs_shifts->energy[i_eng];
    }
    else
    {
      phs_shifts->eng_max = phs_shifts->energy[i_eng];
    }
   
    if( fgets(linebuffer, STRSZ, inp_stream) != NULL)
    {
      for( i_str = 0, i = 0; i < nl; i++)
      {
        if (sscanf(linebuffer + i_str, fmt_buffer,
               &phs_shifts->pshift[(i_eng*nl)+i]) < 1)
        {
          ERROR_MSG("phaseshift could not be read on line '%s'\n", linebuffer);
          exit(EIO);
        }
        while((linebuffer[i_str] == ' ') || (linebuffer[i_str] == '-')) i_str++;
        while((linebuffer[i_str] != ' ') && (linebuffer[i_str] != '-')) i_str++;
      }
    }
    else
    {
      phs_shifts->energy[i_eng] = 0.;
      phs_shifts->eng_max = phs_shifts->energy[i_eng-1];
      break;
    }
  }
 
  phs_shifts->n_eng = i_eng;

#if CONTROL
  CONTROL_MSG(CONTROL, "Number of energies = %d, lmax = %d\n",
              phs_shifts->n_eng, phs_shifts->lmax);
  fprintf(STDCTR, "\n\t  E(H)");
  for(i=0; i < nl; i++) fprintf(STDCTR, "\t  l=%2d",i);
  fprintf(STDCTR, "\n\n");

  for(i_eng = 0; i_eng < phs_shifts->n_eng; i_eng ++)
  {
    fprintf(STDCTR, "\t%7.4f", phs_shifts->energy[i_eng]);
    for(i=0; i < nl; i++)
    {
      if( ! IS_EQUAL_REAL(phs_shifts->pshift[i_eng*nl+i], 0.0))
      {
        fprintf(STDCTR, "\t%7.4f", phs_shifts->pshift[i_eng*nl+i]);
      }
      else fprintf(STDCTR, "\t   --  ");
    }
    fprintf(STDCTR, "\n");
  }
  fprintf(STDCTR, "\n");
#endif

  if(phs_shifts->n_eng != neng)
  {
    WARNING_MSG("EOF found before reading all phase shifts:\n"
                "     expected energies: %3d, found: %3d, file: %s\n",
                neng, i_eng+1, filename);
  }

  return ((int)i_phase-1);
} /* end of function leed_leed_inp_phase */
