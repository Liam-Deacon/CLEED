/*********************************************************************
 *                          LBMSET.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/26.08.94 - Creation
 *********************************************************************/

/*! @file
 *
 * Contains leed_beam_set() which finds beams belonging to a certain beam set.
 */

#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "leed.h"

/*!
 * Finds the beams belonging to a certain beam set ("set") and write them to the
 * list \p p_beams_out .
 *
 * \param[out] p_beams_out Pointer to the list of beams included in the beam set.
 * The list will be terminated by #F_END_OF_LIST in the
 * structure element @leed_beam::k_par .
 *
 * \param[in] beams_in Pointer to list of all beams used throughout the energy
 * loop. The list must be terminated by #F_END_OF_LIST in the
 * structure element @leed_beam::k_par .
 *
 * \param set set for which the beams are extracted.
 *
 * \return number of beams in the list pointed to by \p p_beams.
 * \retval -1 if failed (not implemented)
 * \warning The program will exit if memory cannot be allocated for
 * \p p_beams_out
 */
int leed_beam_set(leed_beam ** p_beams_out, leed_beam * beams_in, size_t set)
{
  size_t i_beams_in, i_beams_out;
  size_t n_beams_out;

  leed_beam *beams_out;

  /*
   * Check input array beams_in for max. number of beams
   * and allocate memory for the beam list p_beams_out.
   */
  for( i_beams_in = 0, n_beams_out = 0;
       ! IS_EQUAL_REAL((beams_in + i_beams_in)->k_par, F_END_OF_LIST);
       i_beams_in ++)
  {
    if( (beams_in + i_beams_in)->set == set) n_beams_out ++;
  }

  n_beams_out ++;

  if (*p_beams_out == NULL)
  {
    beams_out = *p_beams_out = (leed_beam *)
        calloc(n_beams_out, sizeof(leed_beam));
  }
  else
  {
    leed_beam *tmp_p_beams_out = (leed_beam *)
         realloc(*p_beams_out, n_beams_out*sizeof(leed_beam));
    if (tmp_p_beams_out == NULL)
    {
      ERROR_MSG("could not reallocate %u blocks for '*p_beams_out'"
                " at address %p\n", n_beams_out*sizeof(leed_beam), 
                (void*) *p_beams_out);
      exit(ENOMEM);
    }
    else
    {
      beams_out = *p_beams_out = tmp_p_beams_out;
    }
  }
  if(beams_out == NULL)
  {
    ERROR_MSG("allocation error.\n");
    ERROR_RETURN(LEED_ALLOCATION_ERROR);
  }

  /*
   * Copy those beams in list beams_in which belong to the required beam
   * set into list beams_out.
   * - loop over beam indices.
   */
 
  CONTROL_MSG(CONTROL, "%d beams in beam set %d:\n", n_beams_out-1, set);

  i_beams_out = 0;
  for(i_beams_in = 0;
      ! IS_EQUAL_REAL((beams_in + i_beams_in)->k_par, F_END_OF_LIST);
      i_beams_in ++)
  {
    if( (beams_in + i_beams_in)->set == set)
    {
      memcpy( beams_out+i_beams_out, beams_in+i_beams_in, sizeof(leed_beam) );

      CONTROL_MSG(CONTROL, "beam%3d [set%d]: (%6.2f, %6.2f)\n", i_beams_out,
              (beams_out + i_beams_out)->set,
              (beams_out + i_beams_out)->ind_1,
              (beams_out + i_beams_out)->ind_2);

      i_beams_out ++;
    } /* if */
  } /* for i_beam */

  /* Set k_par of the last element of the list to the terminating value. */
  (beams_out + i_beams_out)->k_par = F_END_OF_LIST;

  return((int)i_beams_out);
}  /* end of function leed_beam_set */
