/*********************************************************************
 *                  SRMKINP_MIR.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/30.12.04 - introduce bul for mirrored geometry
 *   GH/22.08.95 - Creation (copy from srmkinp.c)
 *  SRP/31.03.03 - Added a section for the angle search
 *   LD/17.10.14 - Added conditional defines for GSL interface.
 *********************************************************************/

/*!
 * \file srmkinp_mir.c
 * \author Georg Held <g.held@reading.ac.uk>
 * \brief Prepare (*.par) input file for \c cleed_sym program.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if _MSC_VER
#define strncasecmp(x,y,z) _strnicmp((x),(y),(z))
#else
#include <strings.h>
#endif

#include "search.h"

extern search_atom *sr_atoms;
extern search *sr_search;

char line_buffer[STRSZ];

/*!
 * Setup \c *.par input file for \c cleed_sym IV program
 *
 * \param[in] filename \c name of input file.
 * \param[in] par parameter vector for current call.
 * \param iter iteration number in search.
 *
 * \return #search_error
 * \retval #SR_SUCCESS if successful.
 */
int sr_mkinp_mir(const char *filename, const cleed_vector *par, size_t iter)
{
  size_t i_atoms, i_par;
  size_t i_str;

  real x, y, z;
  real theta, phi;

  FILE *iv_par;
  FILE *iv_bul_in;
  FILE *iv_bul_out;
  FILE *iv_bul_mir;

  char inp_file[FILENAME_MAX];

  /* Open IV parameter file */
  iv_par = fopen(filename, "w");

  /* Open IV bulk file for read and write */
  if (strlen(filename) < 4)
  {
    #ifdef EXIT_ON_ERROR
    exit(SR_FILE_IO_ERROR);
    #else
    return(SR_FILE_IO_ERROR);
    #endif
  }

  strncpy(inp_file, filename, FILENAME_MAX);
  i_str = strlen(inp_file)-3;

  sprintf(line_buffer+i_str, "bul");
  iv_bul_in = fopen(line_buffer, "r");

  sprintf(line_buffer+i_str, "bul_org");
  iv_bul_out = fopen(line_buffer, "w");

  sprintf(line_buffer+i_str, "bul_mir");
  iv_bul_mir = fopen(line_buffer, "w");

  /* Write comment lines to IV parameter file */
  fprintf(iv_par, "# overlayer (SEARCH No: %d)\n", iter);

  /* Write all other lines to iv_input */
  for( i_atoms = 0; (sr_atoms + i_atoms)->type != I_END_OF_LIST; i_atoms ++)
  {
    x = (sr_atoms + i_atoms)->x;
    y = (sr_atoms + i_atoms)->y;
    z = (sr_atoms + i_atoms)->z;

    for(i_par = 1; i_par <= (sr_search->n_par_geo); i_par ++)
    {
      if(!sr_search->z_only)
      {
        x += par[i_par] * (sr_atoms + i_atoms)->x_par[i_par];
        y += par[i_par] * (sr_atoms + i_atoms)->y_par[i_par];
      }
      z += par[i_par] * (sr_atoms + i_atoms)->z_par[i_par];
    }

    fprintf(iv_par, "po: %s %f %f %f dr1 %f\n",
                  (sr_atoms + i_atoms)->name,
                  x, y, z, 
                  (sr_atoms + i_atoms)->dr);
  } /* for i_atoms */


  /* angle search */
  if(sr_search->sr_angle)
  {
    phi =   sr_search->phi_0   + par[sr_search->i_par_phi]   * FAC_PHI;
    theta = sr_search->theta_0 + par[sr_search->i_par_theta] * FAC_THETA;
  }
  else
  {
    phi =   sr_search->phi_0;
    theta = sr_search->theta_0;
  }

  /* No negative theta angles are allowed, instead change phi by 180 degrees: */
  if(theta < 0.)
  {
    theta *= -1.;
    phi += 180.;
  }

  /*  Confine the phi angle between 0 and 360 degrees: */
  while(phi > 360.)
  {
    phi -= 360.;
  }
  while(phi < 0.)
  {
    phi += 360.;
  }

  while ( fgets(line_buffer, STRSZ, iv_bul_in) != NULL)
  {
    #ifdef CONTROL_X
    fprintf(STDCTR, "%s", line_buffer);
    #endif

    /* find first non blank character */
    for( i_str = 0;  *(line_buffer+i_str) == ' '; i_str ++);
   
    if( !strncasecmp(line_buffer+i_str, "ip:", 3) )
    {
      fprintf(iv_bul_out, "ip: %.2f\n", phi);
      fprintf(iv_bul_mir, "ip: %.2f\n", 180. - phi);
    }
    else if( !strncasecmp(line_buffer+i_str,"it:",3) )
    {
      fprintf(iv_bul_out, "it: %.2f\n", theta);
      fprintf(iv_bul_mir, "it: %.2f\n", theta);
    }
    else
    {
      fprintf(iv_bul_out, "%s", line_buffer);
      fprintf(iv_bul_mir, "%s", line_buffer);
    }
    
  }

  fclose(iv_bul_in);
  fclose(iv_bul_out);
  fclose(iv_bul_mir);

  fclose(iv_par);

  return(SR_SUCCESS);

}
