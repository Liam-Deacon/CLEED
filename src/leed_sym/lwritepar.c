/************************************************************************
 * <FILENAME>
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *08.08.95 
  file contains function:

  leed_write_par
 
Changes:

GH/08.08.95 - Creation (copy from leed_read_overlayer).

*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "leed.h"

#ifdef ERROR
#ifdef EXIT_ON_ERROR
#define ERR_MESS0(x)   fprintf(STDERR,x); exit(1)
#define ERR_MESS1(x,y) fprintf(STDERR,x,y); exit(1)
#else
#define ERR_MESS0(x)   fprintf(STDERR,x); return(-1)
#define ERR_MESS1(x,y) fprintf(STDERR,x,y); return(-1)
#endif
#else
#ifdef EXIT_ON_ERROR
#define ERR_MESS0(x)   exit(1)
#define ERR_MESS1(x,y) exit(1)
#else
#define ERR_MESS0(x)   return(-1)
#define ERR_MESS1(x,y) return(-1)
#endif
#endif


/********************************************************************/

int leed_write_par(leed_crystal *bulk_par,
              leed_phase   *phs_shifts,
              leed_var   *par,
              leed_energy   *eng,
              leed_beam  *beams,
              FILE* file)

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

   Write parameters in the aboce order to the file specified.

  FUNCTION CALLS:

  RETURN VALUES:

    tot_size = number of bytes that have been written if ok.
    -1 if failed (and EXIT_ON_ERROR is not defined)

*********************************************************************/
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
 #ifdef ERROR
   ERR_MESS0("*** error (leed_write_par): "
    "output error while writing bulk parameters\n");
#endif
 }
 tot_size += sizeof(leed_crystal) * 1;

/* layers */
 if( fwrite(bulk_par->layers, sizeof(leed_layer), 
            bulk_par->nlayers, file) != (unsigned int) bulk_par->nlayers )
 {
 #ifdef ERROR
   ERR_MESS0("*** error (leed_write_par): "
    "output error while writing bulk layers\n");
#endif
 }
 tot_size += sizeof(leed_layer) * bulk_par->nlayers;

/* atoms */
 for(i = 0; i < (unsigned int) bulk_par->nlayers; i ++)
 {
   if( fwrite( (bulk_par->layers + i)->atoms, 
               sizeof(leed_atom), 
               (bulk_par->layers + i)->natoms, file) 
       != (unsigned int) (bulk_par->layers + i)->natoms )
   {
   #ifdef ERROR
     ERR_MESS1("*** error (leed_write_par): "
        "output error while writing atoms of bulk layer %d\n", i);
   #endif
   }
   tot_size += sizeof(leed_atom) * (bulk_par->layers + i)->natoms;
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
#ifdef ERROR
   ERR_MESS0("*** error (leed_write_par): "
    "output error while writing No. of phase shifts.\n");
#endif
 }
 tot_size += sizeof(int) * 1;

/* parameters (including terminating set) */
 if( fwrite(phs_shifts, sizeof(leed_phase), n_phs+1, file) != n_phs+1 )
 {
#ifdef ERROR
   ERR_MESS0("*** error (leed_write_par): "
    "output error while writing phase shift parameters\n");
#endif
 }
 tot_size += sizeof(leed_phase) * (n_phs+1);

 for( i = 0; i < n_phs; i ++)
 {

/* energies */
   number = (phs_shifts + i)->neng;
   if( fwrite( (phs_shifts + i)->energy, sizeof(real), number, file) != number)
   {
#ifdef ERROR
     ERR_MESS1("*** error (leed_write_par): "
        "output error while writing phase shift energies (%d)\n", i);
#endif
   }
   tot_size += sizeof(real) * number;

/* phase shifts */
   number = (phs_shifts + i)->neng * ( (phs_shifts + i)->lmax + 1);
   if( fwrite( (phs_shifts + i)->pshift, sizeof(real), number, file) != number)
   {
#ifdef ERROR
     ERR_MESS1("*** error (leed_write_par): "
        "output error while writing phase shifts (%d)\n", i);
#endif
   }
   tot_size += sizeof(real) * number;

/* write length of file name */
   number = strlen( (phs_shifts + i)->input_file ) + 1;
   if( fwrite( &number, sizeof(int), 1, file) != 1 )
   {
#ifdef ERROR
     ERR_MESS1("*** error (leed_read_par): "
        "output error while writing ph. shift file name (%d)\n", i);
#endif
   }
   tot_size += sizeof(int) * 1;

/* write file name */
   if( fwrite( (phs_shifts + i)->input_file, sizeof(char), number, file) 
       != number)
   {
#ifdef ERROR
     ERR_MESS1("*** error (leed_write_par): "
       "output error while writing phase shifts input file (%d)\n", i);
#endif
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
#ifdef ERROR
   ERR_MESS0("*** error (leed_write_par): "
    "output error while writing control parameters\n");
#endif
 }
 tot_size += sizeof(leed_var) * 1;

/************************************************************************
  Write energy parameters to file.
   - parameters (eng, eng_str)
*************************************************************************/

/* parameters */
 if( fwrite(eng, sizeof(leed_energy), 1, file) != 1 )
 {
 #ifdef ERROR
   ERR_MESS0("*** error (leed_write_par): "
    "output error while writing energy parameters\n");
#endif
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
#ifdef ERROR
   ERR_MESS0("*** error (leed_write_par): "
    "output error while writing No. of beams.\n");
#endif
 }
 tot_size += sizeof(int) * 1;

/* beam list */
 if( fwrite(beams, sizeof(leed_beam), number, file) != number )
 {
#ifdef ERROR
   ERR_MESS0("*** error (leed_write_par): "
    "output error while writing beam list\n");
#endif
 }
 tot_size += sizeof(leed_beam) * number;

#ifdef CONTROL
 fprintf(STDCTR,"(leed_write_par): %d bytes written\n", tot_size);
#endif

/************************************************************************
  Write total number of bytes to file for control reasons
*************************************************************************/

 if( fwrite(&tot_size, sizeof(int), 1, file) != 1 )
 {
#ifdef ERROR
   ERR_MESS0("*** error (leed_write_par): "
    "output error while writing control number\n");
#endif
 }
 tot_size += sizeof(int) * 1;

 return(tot_size);
}
