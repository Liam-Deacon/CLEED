/*********************************************************************
  GH/26.08.94 
  file contains function:

  leed_beam_set

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "leed.h"


/*======================================================================*/

int leed_beam_set(leed_beam_t ** p_beams_out, leed_beam_t * beams_in, int set)

/************************************************************************

 Find the beams belonging to a certain beam set ("set") and write them to a
 list.
 
 INPUT:

  leed_beam_t ** p_beams_out - (output) 
                Pointer to the list of beams included in the beam set.
                The list will be terminated by "F_END_OF_LIST" in the 
                structure element "k_par".

  leed_beam_t * beams_in - (input)
                List of all beams used throughout the energy
                loop. The list must be terminated by "F_END_OF_LIST" in the 
                structure element "k_par".

  int set       (input) set for which the beams are extracted.

 RETURN VALUE:

  int i_beams_out - number of beams in the list pointed to by p_beams.
  -1            if failed (not implemented)

*************************************************************************/
{
int i_beams_in, i_beams_out;
int n_beams_out;

real faux_r, faux_i;
real k_max, k_max_2;
real k_r, k_i;
real k_x, k_y;

leed_beam_t *beams_out, bm_ptr;

/************************************************************************
  Check input array beams_in for max. number of beams
  and allocate memory for the beam list p_beams_out.
*************************************************************************/

 for( i_beams_in = 0, n_beams_out = 0; 
      (beams_in + i_beams_in)->k_par != F_END_OF_LIST; 
      i_beams_in ++) 
   if( (beams_in + i_beams_in)->set == set) n_beams_out ++;

 n_beams_out ++;

 if (*p_beams_out == NULL)
   beams_out = *p_beams_out = (leed_beam_t *)
                 calloc(n_beams_out, sizeof(leed_beam_t));
 else
   beams_out = *p_beams_out = (leed_beam_t *)
                 realloc(*p_beams_out, n_beams_out*sizeof(leed_beam_t));

 if(beams_out == NULL)
 {
#ifdef ERROR
   fprintf(STDERR," *** error (leed_beam_set): allocation error.\n");
#endif
   exit(1);
 }

/*********************************************************
  Copy those beams in list beams_in which belong to the required beam
  set into list beams_out.
  - loop over beam indices.
*********************************************************/ 
 
#ifdef CONTROL
 fprintf(STDCTR,"(leed_beam_set): %d beams in beam set %d:\n", n_beams_out-1, set);
#endif

 i_beams_out = 0;
 for(i_beams_in = 0; (beams_in + i_beams_in)->k_par != F_END_OF_LIST; 
     i_beams_in ++)
 {
   if( (beams_in + i_beams_in)->set == set)
   {
     memcpy( beams_out+i_beams_out, beams_in+i_beams_in, 
             sizeof(leed_beam_t) );

#ifdef CONTROL

 fprintf(STDCTR,"beam%3d [set%d]: (%6.2f, %6.2f)\n", i_beams_out, 
    (beams_out + i_beams_out)->set,
    (beams_out + i_beams_out)->ind_1, (beams_out + i_beams_out)->ind_2);
#endif
     i_beams_out ++;
   } /* if */
 }

/*
  Set k_par of the last element of the list to the terminating value.
*/
 (beams_out + i_beams_out)->k_par = F_END_OF_LIST;

 return(i_beams_out);
}  /* end of function leed_beam_set */

/*======================================================================*/
