/**********************************************************************
  GH/15.07.03
  file contains function:

  leed_update_phase
   Update the number of phase shifts.
  leed_leed_inp_phase_nd
   Read phase shifts from an input file and store them.

Changes:

  GH/04.07.94 - Creation
  GH/19.01.95 - add eng_max and eng_min
  GH/08.08.95 - update i_phase
  GH/02.09.97 - Set input path by environment variable CLEED_PHASE
                (makes PHASE_PATH obsolete)
  WB/26.02.98 - Correct control output
  GH/03.05.00 - extend list of parameters for leed_leed_inp_phase: t_type
  GH/15.07.03 - fix bug in energy scaleing factor for Ry (was 2./HART, now 2.).

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#include "leed.h"
#include "leed_def.h"


#ifndef GEO_TOLERANCE          /* should be defined in "leed_def.h" */
#define GEO_TOLERANCE 0.0001                /* ca. 0.00005 A */
#endif

#ifdef __STRICT_ANSI__
char *strdup(const char *str) /* strdup is not standard C (its POSIX) */
{
    int n = strlen(str) + 1;
    char *dup = malloc(n);
    if(dup) strcpy(dup, str);
    return dup;
}
#endif

static int i_phase = 0;      /* number of atom types */

/********************************************************************/

int leed_update_phase( int n)

/*********************************************************************
  Update the number of phase shifts i_phase

INPUT:
  int n number i_phase will be set to.

RETURN VALUE:
  i_phase (i.e. equal to the argument)

*********************************************************************/
{
  i_phase = n;
  return(i_phase);
} /* end of function leed_update_phase */

/********************************************************************/

int leed_leed_inp_phase_nd( char * phaseinp, real * dr, int t_type, 
                 leed_phs_t **p_phs_shifts )

/*********************************************************************

  Read phase shifts from an input file and store them.

 INPUT:

  char * phaseinp (input)  Either full path name of phase shift file 
         if starting with "/" or tag for type of atom (will be expanded
         into a full filename by using CLEED_PHASE and extension ".phs").
  
  real * dr (input) displacement vector for thermic vibrations
  
  leed_phs_t **p_phs_shifts (output) phase shifts.

 DESIGN:

  The phase shifts in the input file must be for increasing energies.
  The storage scheme is:
  
  

*********************************************************************/
{
FILE *inp_stream;

char filename[STRSZ];
char linebuffer[STRSZ];
char eng_type[STRSZ];

leed_phs_t *phs_shifts;

int i;
int i_str, i_eng;

int neng, lmax, nl;          /* neng = No. of energies to be read
                                lmax = max. quantum number;
                                nl   = No. of phase shifts = lmax + 1
                             */

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
#ifdef ERROR
     fprintf(STDERR,
     " *** error (leed_leed_inp_phase): environment variable CLEED_PHASE not defined\n");
#endif
     exit(1);
   }
   sprintf(filename, "%s/%s.phs",getenv("CLEED_PHASE"),phaseinp);
 }
 else
   sprintf(filename, "%s", phaseinp);

 if(i_phase > 0)
 {
/* 
  Compare filename, dr, and t_type with previous phaseshifts. Return the 
  corresponding phase shift number if the same combination has already 
  been read.
*/
   for(i=0; i< i_phase; i++)
     if( (!strcmp( (*p_phs_shifts + i)->input_file, filename) )         &&
         ( R_fabs(dr[1] - (*p_phs_shifts + i)->dr[1]) < GEO_TOLERANCE ) &&
         ( R_fabs(dr[2] - (*p_phs_shifts + i)->dr[2]) < GEO_TOLERANCE ) &&
         ( R_fabs(dr[3] - (*p_phs_shifts + i)->dr[3]) < GEO_TOLERANCE ) &&
         ( t_type == (*p_phs_shifts + i)->t_type ) 
       )
     {
       return(i); 
       break;
     }
   i_phase ++;
   *p_phs_shifts = (leed_phs_t *)realloc( 
                    *p_phs_shifts, (i_phase + 1) * sizeof(leed_phs_t) );
 }
 else
 {
   i_phase ++;
   *p_phs_shifts = (leed_phs_t *) malloc( 2 * sizeof(leed_phs_t) );
 }

/* Terminate list of phase shifts */
  
 (*(p_phs_shifts) + i_phase)->lmax = I_END_OF_LIST;


 phs_shifts = *(p_phs_shifts) + i_phase-1;

/* write dr and t_type to phs_shifts */
 for(i=0; i<=3; i++) phs_shifts->dr[i] = dr[i];
 phs_shifts->t_type = t_type;

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
#ifdef ERROR
   fprintf(STDERR,
  " *** error (leed_leed_inp_phase): could not open file \"%s\"\n",filename);
#endif
   exit(1);
 }

 phs_shifts->input_file = strdup(filename);

/********************************************************************
  Read the first line of the input file which contains the number of 
  energies to be read in (neng) and the maximum phase shift quantum 
  number (lmax).
********************************************************************/

/* read comment lines */
 while( *fgets(linebuffer, STRSZ, inp_stream) == '#');

 if ( linebuffer == NULL)     /* EOF found */
 {
#ifdef ERROR
   fprintf(STDERR,
   " *** error (leed_leed_inp_phase): unexpected EOF found while reading file \"%s\"\n",
   filename);
#endif
   exit(1);
 }
 else if( sscanf(linebuffer, "%d %d %s", &neng, &lmax, eng_type) < 2)
 {
#ifdef ERROR
   fprintf(STDERR,
   " *** error (leed_leed_inp_phase): improper input line in file \"%s\":\n%s",
   filename, linebuffer);
#endif
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
   eng_scale = 2.;
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
  Read energies and phase shifts. 
  Find max and min energy
  NB: The search for blank or '-' after reading each number is needed
  because of the FORTRAN format used for the VHT input files which does
  not have any blank character between negative numbers.
********************************************************************/

 phs_shifts->lmax = lmax;
 nl = lmax + 1;

 phs_shifts->energy = (real *)malloc( neng * sizeof(real) ); 
 phs_shifts->pshift = (real *)malloc( neng * nl * sizeof(real) ); 
 
 
 for( i_eng = 0; 
     (i_eng < neng) && (fgets(linebuffer, STRSZ, inp_stream) != NULL); 
      i_eng ++)
 {
#ifdef REAL_IS_DOUBLE
   sscanf(linebuffer, "%le", phs_shifts->energy+i_eng);
#endif
#ifdef REAL_IS_FLOAT
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
#ifdef REAL_IS_DOUBLE
       sscanf(linebuffer + i_str, "%le", phs_shifts->pshift+i_eng*nl+i);
#endif
#ifdef REAL_IS_FLOAT
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
 
 phs_shifts->neng = i_eng;

#ifdef CONTROL
 fprintf(STDCTR,"(leed_leed_inp_phase): Number of energies = %d, lmax = %d\n",
         phs_shifts->neng, phs_shifts->lmax);
 fprintf(STDCTR,"\n\t  E(H)");
 for(i=0; i<nl; i++) fprintf(STDCTR,"\t  l=%2d",i); 
 fprintf(STDCTR,"\n\n");

 for(i_eng = 0; i_eng < phs_shifts->neng; i_eng ++)
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

#ifdef WARNING
 if(phs_shifts->neng != neng)
 {
   fprintf(STDWAR,
   " *** warning (leed_leed_inp_phase): EOF found before reading all phase shifts:\n");
   fprintf(STDWAR,
   "     expected energies: %3d, found: %3d, file: %s\n", 
   neng, i_eng+1, filename);
 }
#endif

 return(i_phase - 1);
} /* end of function leed_leed_inp_phase */
