/*********************************************************************
 *													srmklog.c
 *
 *  Copyright (C) 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/2014.10.17 - creation
 *********************************************************************/

/*!
 * \file
 * \author Liam Deacon <liam.deacon@diamond.ac.uk>
 * \brief Creates log file and saves initial parameters for search.
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include "search.h"

/*!
 * Creates log file and saves initial parameters for search.
 *
 * \param log_file filename for the search log.
 */
void sr_mklog(const char *log_file)
{

  FILE *log_stream;
  size_t i_atoms, i_par;

  if( (log_stream = fopen(log_file, "w")) == NULL)
  {
    ERROR_MSG("Could not write to log file '%s'\n", log_file);
  }

  fprintf(log_stream, "CSEARCH - version %s\n\n", SEARCH_VERSION);
  fprintf(log_stream, "=> Atoms in search:\n\n");
  for(i_atoms = 0; sr_atoms[i_atoms].type != I_END_OF_LIST; i_atoms ++)
  {
    fprintf(log_stream, "%d \"%s\": (%6.3f, %6.3f, %6.3f) ref: %d n_ref: %d",
            i_atoms,
            sr_atoms[i_atoms].name,
            sr_atoms[i_atoms].x,
            sr_atoms[i_atoms].y,
            sr_atoms[i_atoms].z,
            sr_atoms[i_atoms].ref,
            sr_atoms[i_atoms].n_ref);
    fprintf(log_stream, " r_min: %.3f\n", (sr_atoms + i_atoms)->r_min);

    if(!sr_search->z_only)
    {
      fprintf(log_stream, "x_par:\t");
      for(i_par = 1; i_par <= sr_search->n_par; i_par ++)
      {
        fprintf(log_stream, "%.3f ", sr_atoms[i_atoms].x_par[i_par]);
      }
      fprintf(log_stream, "\ny_par:\t");
      for(i_par = 1; i_par <= sr_search->n_par; i_par ++)
      {
        fprintf(log_stream, "%.3f ", sr_atoms[i_atoms].y_par[i_par]);
      }
      fprintf(log_stream, "\n");
    }
    fprintf(log_stream, "z_par:\t");
    for(i_par = 1; i_par <= sr_search->n_par; i_par ++)
    {
      fprintf(log_stream, "%.3f ", sr_atoms[i_atoms].z_par[i_par]);
    }
    fprintf(log_stream, "\n\n");
  }

  fclose(log_stream);

}
