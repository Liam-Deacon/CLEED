/**********************************************************************
  GH/02.09.97
  WB/26.02.98
  LD/13.03.14
  file contains function:

  leed_update_phase
   Update the number of phase shifts.
  leed_leed_inp_phase
   Read phase shifts from an input file and store them.
  gen_phase
   Create phase shift input files in iterative manner for newest model.
Changes:

  GH/04.07.94 - Creation
  GH/19.01.95 - add eng_max and eng_min
  GH/08.08.95 - update i_phase
  GH/02.09.97 - Set input path by environment variable CLEED_PHASE
                (makes PHASE_PATH obsolete)
  WB/26.02.98 - Correct control output
  LD/13.03.14 - Auto-generation of phase shifts using phsh.py
               (Check for environment variable CSEARCH_PHSH and generate
                iterative phase shifts, otherwise default to using original 
                phase shift files)

 *********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#include "leed.h"
#include "leed_def.h"

static int i_phase = 0;      /* number of atom types */

int leed_leed_inp_phase( char * phaseinp, real * dr, leed_phase **p_phs_shifts )

/*********************************************************************

  Read phase shifts from an input file and store them.

 INPUT:

  char * phaseinp (input)  Either full path name of phase shift file 
         if starting with "/" or tag for type of atom (will be expanded
         into a full filename by using CLEED_PHASE and extension ".phs").

  real * dr (input) displacement vector for thermic vibrations

  leed_phase **p_phs_shifts (output) phase shifts.

 DESIGN:

  The phase shifts in the input file must be for increasing energies.
  The storage scheme is:



 *********************************************************************/
{
  FILE *inp_stream;

  char filename[STRSZ];
  char linebuffer[STRSZ];
  char eng_type[STRSZ];

  leed_phase *phs_shifts;

  int i;
  int i_str, i_eng;

  int n_eng, lmax, nl;          /* n_eng = No. of energies to be read
                                 * lmax = max. quantum number;
                                 * nl = No. of phase shifts = lmax + 1 */
  real eng_scale;

  /***********************************************************************
  Create name of input file:
  - If phaseinp is a full path (starting with '/'), this is used as 
    name of the input file.
  - Otherwise the filename is put together from CLEED_PHASE, phaseinp
    and the extension '.phs'.
  - Check whether environment variable CLEED_PHASE exists at all.
   ***********************************************************************/

  if( *phaseinp != '/')
  {
    if(getenv("CLEED_PHASE") == NULL)
    {
      ERROR_MSG("environment variable CLEED_PHASE not defined\n");
      exit(1);
    }
    sprintf(filename, "%s/%s.phs",getenv("CLEED_PHASE"),phaseinp);
  }
  else
    sprintf(filename, "%s",phaseinp);

  if(i_phase > 0)
  {
    /*
  Compare filename and dr with previous phaseshifts. Return the 
  corresponding phase shift number if the same combination has already 
  been read.
     */
    for(i=0; i< i_phase; i++)
      if( (!strcmp( (*p_phs_shifts + i)->input_file, filename) )        &&
          ( R_fabs(dr[1] - (*p_phs_shifts + i)->dr[1]) < GEO_TOLERANCE ) &&
          ( R_fabs(dr[2] - (*p_phs_shifts + i)->dr[2]) < GEO_TOLERANCE ) &&
          ( R_fabs(dr[3] - (*p_phs_shifts + i)->dr[3]) < GEO_TOLERANCE )  )
      {
        return(i);
        break;
      }
    i_phase ++;
    *p_phs_shifts = (leed_phase *)realloc(
        *p_phs_shifts, (i_phase + 1) * sizeof(leed_phase) );
  }
  else
  {
    i_phase ++;
    *p_phs_shifts = (leed_phase *) malloc( 2 * sizeof(leed_phase) );
  }

  /* Terminate list of phase shifts */

  (*(p_phs_shifts) + i_phase)->lmax = I_END_OF_LIST;


  phs_shifts = *(p_phs_shifts) + i_phase-1;

  /* write dr to phs_shifts */
  for(i=0; i<=3; i++) phs_shifts->dr[i] = dr[i];

  /********************************************************************
  Open and Read input file for a new set of phase shifts
   ********************************************************************/

#ifdef CONTROL
  fprintf(STDCTR,"(leed_leed_inp_phase): Reading file \"%s\", i_phase = %d\n",
      filename, i_phase-1);
#endif

  /*
  Open input file.
  Copy the full filename into phs_shifts->input_file.
   */

  if( (inp_stream = fopen(filename, "r")) == NULL)
  {
    ERROR_MSG("could not open file \"%s\"\n", filename);
    exit(1);
  }

  strcpy(phs_shifts->input_file, strdup(filename));

  /********************************************************************
   * Read the first line of the input file which contains the number of
   * energies to be read in (n_eng) and the maximum phase shift quantum
   * number (lmax).
   ********************************************************************/

  /* read comment lines */
  while( *fgets(linebuffer, STRSZ, inp_stream) == '#');

  if ( linebuffer == NULL)     /* EOF found */
  {
    ERROR_MSG("unexpected EOF found while reading file \"%s\"\n", filename);
    exit(1);
  }
  else if( sscanf(linebuffer, "%d %d %s", &n_eng, &lmax, eng_type) < 2)
  {
    ERROR_MSG("improper input line in file \"%s\":\n%s", filename, linebuffer);
    exit(1);
  }

  /*
  Define energy scale according to eng_type. The default is 
  input in Hartree units (27.18 eV)
   */
  if( !strncmp(eng_type,"eV",2) || !strncmp(eng_type,"EV",2) )
  {
    eng_scale = 1./HART;
#ifdef CONTROL
    fprintf(STDCTR,"(leed_leed_inp_phase): Energy input in eV\n");
#endif
  }
  else if( !strncmp(eng_type,"Ry",2) || !strncmp(eng_type,"RY",2) )
  {
    eng_scale = 2./HART;
#ifdef CONTROL
    fprintf(STDCTR,"(leed_leed_inp_phase): Energy input in Rydberg (13.59 eV)\n");
#endif
  }
  else
  {
    eng_scale = 1.;
#ifdef CONTROL
    fprintf(STDCTR,"(leed_leed_inp_phase): Energy input in Hartree (27.18 eV)\n");
#endif
  }

  /********************************************************************
   * Read energies and phase shifts.
   * Find max and min energy
   * NB: The search for blank or '-' after reading each number is needed
   * because of the FORTRAN format used for the VHT input files which does
   * not have any blank character between negative numbers.
   ********************************************************************/
  phs_shifts->lmax = lmax;
  nl = lmax + 1;

  phs_shifts->energy = (real *)malloc( n_eng * sizeof(real) );
  phs_shifts->pshift = (real *)malloc( n_eng * nl * sizeof(real) );


  for( i_eng = 0;
      (i_eng < n_eng) && (fgets(linebuffer, STRSZ, inp_stream) != NULL);
      i_eng ++)
  {
#ifdef CLEED_REAL_IS_DOUBLE
    sscanf(linebuffer, "%le", phs_shifts->energy+i_eng);
#endif
#ifdef CLEED_REAL_IS_FLOAT
    sscanf(linebuffer, "%e", phs_shifts->energy+i_eng);
#endif
    phs_shifts->energy[i_eng] *= eng_scale;

    if (i_eng == 0)
      phs_shifts->eng_min = phs_shifts->energy[i_eng];
    else
      phs_shifts->eng_max = phs_shifts->energy[i_eng];

    if( fgets(linebuffer, STRSZ, inp_stream) != NULL)
    {
      for( i_str = 0, i = 0; i<nl; i++)
      {
#ifdef CLEED_REAL_IS_DOUBLE
        sscanf(linebuffer + i_str, "%le", phs_shifts->pshift+i_eng*nl+i);
#endif
#ifdef CLEED_REAL_IS_FLOAT
        sscanf(linebuffer + i_str, "%e", phs_shifts->pshift+i_eng*nl+i);
#endif
        while((linebuffer[i_str] == ' ') || (linebuffer[i_str] == '-')) i_str ++;
        while((linebuffer[i_str] != ' ') && (linebuffer[i_str] != '-')) i_str ++;
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

#ifdef CONTROL
  fprintf(STDCTR,"(leed_leed_inp_phase): Number of energies = %d, lmax = %d\n",
      phs_shifts->n_eng, phs_shifts->lmax);
  fprintf(STDCTR,"\n\t  E(H)");
  for(i=0; i<nl; i++) fprintf(STDCTR,"\t  l=%2d",i);
  fprintf(STDCTR,"\n\n");

  for(i_eng = 0; i_eng < phs_shifts->n_eng; i_eng ++)
  {
    fprintf(STDCTR,"\t%7.4f", phs_shifts->energy[i_eng]);
    for(i=0; i<nl; i++)
    {
      if( ! IS_EQUAL_REAL(phs_shifts->pshift[i_eng*nl+i], 0.0))
        fprintf(STDCTR,"\t%7.4f", phs_shifts->pshift[i_eng*nl+i]);
      else
        fprintf(STDCTR,"\t   --  ");
    }
    fprintf(STDCTR,"\n");
  }
  fprintf(STDCTR,"\n");
#endif

  if(phs_shifts->n_eng != n_eng)
  {
    WARNING_MSG("EOF found before reading all phase shifts:\n"
        "     expected energies: %3d, found: %3d, file: %s\n",
        n_eng, i_eng+1, filename);
  }

  return(i_phase - 1);
} /* end of function leed_leed_inp_phase */
