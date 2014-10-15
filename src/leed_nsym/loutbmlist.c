/*********************************************************************
  GH/11.08.95 
  file contains function:

  int leed_output_beam_list(leed_beam_t ** p_beams_out, 
                 leed_beam_t *beams_all,
                 sruct eng_str *eng, 
                 FILE * outfile)

 Write header information to output file.

 Changes:
 
 GH/20.07.95 - Creation
 GH/11.08.95 - write only non-evanescent beams to output. Return value
               is a list of nonevanescent beams at eng->fin.

*********************************************************************/

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "leed.h"


int leed_output_beam_list(leed_beam_t ** p_beams_out, 
               leed_beam_t *beams_all, 
               leed_energy *eng, 
               FILE * outfile)

/************************************************************************

 write header information to output file.
 
 INPUT:

  leed_beam_t ** p_beams_out  - (input) Pointer to output list (can 
            be NULL to indicate that list should be allocated).
            The list will be terminated by "F_END_OF_LIST" in the 
            structure element "k_par".

  leed_beam_t *beams_all  - (input) 
            List of all beams used throughout the energy
            loop. The list must be terminated by "F_END_OF_LIST" in the 
            structure element "k_par".
            
            NOTE:

            All structure elements of this list will be copied if they
            are non-evanescent at energy eng->fin.
            It is assumed, that the elements beams_all->k_par contain the
            SQUARE OF THE PARALLEL MOMENTUM as created in function 
            leed_beam_gen.

  sruct eng_str *eng - (input) control parameters for energy loop:
            eng->ini = first energy,
            eng->fin = last energy,
            eng->stp = energy step.
        
  FILE * outfile - (input) pointer to the output file were the intensities 
            are written to.

 DESIGN:

 *Create list beams_out*

  First find the maximum parallel vector component of non-evanescent wave
  vectors at eng->fin.
 
  k_max = sqrt(2 * eng->fin)

  All beams with parallel momentum < k_max (i.e. k_par < k_max ^2) are 
  included in list beams_out.

  n_beams is then set to number of output beams.

 *Output to file*
 
  "#en" energies: 
        initial energy, final energy, energy step
  "#bn" number of beams
  "#bi" beam indices: 
        number (starting from zero), 1st index, 2nd index, beam set.

 RETURN VALUES:

  int n_beams  number of output beams.
  -1           if failed (not implemented).

*************************************************************************/
{

int n_beams, n_eng;
int i_bm_all, i_bm_out;

real k_max;
real faux;

leed_beam_t *beams_out;

/************************************************************************
  Find number of beams in list beams_all (n_beams) and 
  allocate *p_beams_out = beams_out of the same size.
************************************************************************/

 for(n_beams = 0; 
     ! IS_EQUAL_REAL((beams_all + n_beams)->k_par, F_END_OF_LIST); 
     n_beams ++) { ; }

#ifdef CONTROL
 fprintf(STDCTR,"(leed_output_beam_list): \tTotal No. of beams %.f eV = %d\n",
                 eng->fin*HART, n_beams);
#endif

 if (*p_beams_out == NULL)
   *p_beams_out = beams_out = (leed_beam_t *)
                 calloc(n_beams + 1, sizeof(leed_beam_t));
 else
   *p_beams_out = beams_out = (leed_beam_t *)
                 realloc(*p_beams_out, (n_beams+1) * sizeof(leed_beam_t));

 if(beams_out == NULL)
 {
#ifdef ERROR
   fprintf(STDERR," *** error (leed_output_beam_list): allocation error.\n");
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(-1)
#endif
 }

/************************************************************************
  Write appropriate beams to beams_out
  
  k_max is the square of the maximum pareallel vector component of 
        non-evanescent wave vectors at eng->fin.
  n_beams is set to number of output beams afterwards.
************************************************************************/

 k_max =  2. * eng->fin;

 for(i_bm_all = 0, i_bm_out = 0; i_bm_all < n_beams; i_bm_all ++)
 {
   if( (beams_all + i_bm_all)->k_par <= k_max )
   {
     memcpy( beams_out + i_bm_out, 
             beams_all + i_bm_all, 
             sizeof(leed_beam_t) );
     i_bm_out ++;
   }
 }

/* terminate list beams_out */
 (beams_out + i_bm_out)->k_par = F_END_OF_LIST;
 n_beams = i_bm_out;

#ifdef CONTROL
 fprintf(STDCTR,"(leed_output_beam_list): \tNo. of non_evanescent beams = %d\n",
                  n_beams);
#endif


/************************************************************************
  Write energies, number of beams, and beams to output

  "#en" energies: 
        number of energies, initial energy, final energy, energy step.
  "#bn" number of beams
  "#bi" beam indices: 
        number (starting from zero), 1st index, 2nd index, beam set.

  - Count energy points (Running through the same loop0 as the main
    program does avoids inconsistencies due to rounding errors)
  - 
************************************************************************/

/* energies */
 for(faux = eng->ini, n_eng = 0; 
     faux <= eng->fin; 
     faux += eng->stp, n_eng++)
 { ; }

 fprintf(outfile, "#en %d %f %f %f\n", 
                   n_eng,
                   eng->ini*HART, 
                   eng->fin*HART, 
                   eng->stp*HART);

/* number of beams */
 fprintf(outfile, "#bn %d\n", n_beams);

/* beam indices */
 for(i_bm_out = 0; i_bm_out < n_beams; i_bm_out ++)
 {
   fprintf(outfile, "#bi %d %f %f %d\n", 
                     i_bm_out, 
                     (beams_out+i_bm_out)->ind_1, 
                     (beams_out+i_bm_out)->ind_2,
                     (beams_out+i_bm_out)->set);
 }
/* flush output file */
 fflush(outfile);

/* write beams_out back to pointer */
 *p_beams_out = beams_out;

 return(n_beams);
}  /* end of function leed_output_beam_list */
/************************************************************************/
