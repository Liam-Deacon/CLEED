/*********************************************************************
 *                        SRCKGEO.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1995.08.24 - Creation (copy from srevalrf.c)
 *  SRP/2003.04.01 - Modified the code for the angle search (n_par_geo)
 *   LD/1014.07.01 - Modified to use GNU Scientific library structures if
 *                   compiling with 'USE_GSL'
 *********************************************************************/

/*!
 * \file
 * \author Georg Held <g.held@reading.ac.uk>
 * \brief Checks geometry of search.
 *
 * \note To use the GSL interface, ensure \c USE_GSL is defined when
 * compiling.
 */

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#ifdef USE_GSL
#include <gsl/gsl_vector.h>
#endif

#include "csearch.h"

#define GEO_PREF 10.                 /*!< rgeo is 1 if the actual distance
                                      * between two atoms is by 1/GEO_PREF
                                      * smaller than the sum of their min.
                                      * radii */

extern struct search_atom *sr_atoms;
extern struct search *sr_search;
extern char *sr_project;

/*!
 * Perfoms geometry assessment of \p par parameter vector.
 *
 * \param par pointer to parameter vector to evaluate.
 * \return nearest neighbour distance (?)
 */
#ifdef USE_GSL
real sr_ckgeo_gsl(const gsl_vector *par)
#else
real sr_ckgeo(const real *par)
#endif
{
  size_t i_atoms, j_atoms, n_atoms;
  size_t i_par;
  int i1, i2;

  real rgeo;

  real faux;
  real *x, *y, *z;
  real d_min, d_min2;
  real d_x, d_y, d_z;

  /* Set initial values
   * - n_atoms
   * - arrays x, y, and z
   */
  for(n_atoms = 0; (sr_atoms + n_atoms)->type != I_END_OF_LIST; n_atoms ++)
  { ; }

  x = (real *)malloc( n_atoms * sizeof(real) );
  y = (real *)malloc( n_atoms * sizeof(real) );
  z = (real *)malloc( n_atoms * sizeof(real) );

  for(i_atoms = 0; i_atoms < n_atoms; i_atoms ++)
  {
    x[i_atoms] = (sr_atoms + i_atoms)->x;
    y[i_atoms] = (sr_atoms + i_atoms)->y;
    z[i_atoms] = (sr_atoms + i_atoms)->z;

    #ifdef USE_GSL
    for(i_par = 0; i_par < sr_search->n_par_geo; i_par ++)
    {
      faux = gsl_vector_get(par, i_par);
      if(!sr_search->z_only)
      {
        x[i_atoms] += faux * (sr_atoms + i_atoms)->x_par[i_par];
        y[i_atoms] += faux * (sr_atoms + i_atoms)->y_par[i_par];
      }
      z[i_atoms] += faux * (sr_atoms + i_atoms)->z_par[i_par];
    }
    #else
    for(i_par = 1; i_par <= sr_search->n_par_geo; i_par ++)
    {
      if(!sr_search->z_only)
      {
        x[i_atoms] += par[i_par] * (sr_atoms + i_atoms)->x_par[i_par];
        y[i_atoms] += par[i_par] * (sr_atoms + i_atoms)->y_par[i_par];
      }
      z[i_atoms] += par[i_par] * (sr_atoms + i_atoms)->z_par[i_par];
    }
    #endif

  } /* for i_atoms */

  /*
   * Begin geometry assesment
   */
  rgeo = 0.;

  for(i_atoms = 0; i_atoms < n_atoms; i_atoms ++)
  {

    /* Check wether this atom is within the vertical limits (z_min/z_max) */
    if(z[i_atoms] > sr_search->z_max)
    {
      faux = GEO_PREF * (sr_search->z_max - z[i_atoms] ) / sr_search->z_max;
      rgeo += (SQUARE(faux));
    }
    if(z[i_atoms] < sr_search->z_min)
    {
      faux = GEO_PREF * (sr_search->z_min - z[i_atoms] ) / sr_search->z_min;
      rgeo += (SQUARE(faux));
    }

    /* Loop over all other atoms:
     * Check whether the distances between the atoms are within the given
     * limits (r_min)
     */

    for(j_atoms = i_atoms + 1; j_atoms < n_atoms; j_atoms ++)
    {
      d_min = (sr_atoms + i_atoms)->r_min + (sr_atoms + j_atoms)->r_min;
      d_min2 = SQUARE(d_min);
     
      d_z = z[j_atoms] - z[i_atoms];
      d_z *= d_z;
      d_min2 -= d_z;
     
      /* Loop over all surrounding unit cells
       * (add lattice vectors to the atom positions)
       */
      for(i1 = -1; i1 <= 1; i1 ++)
      {
        for(i2 = -1; i2 <= 1; i2 ++)
        {
          d_x = x[j_atoms] - x[i_atoms] +
                  i1 * sr_search->b_lat[1] + i2 * sr_search->b_lat[2];
          d_x *= d_x;
          d_y = y[j_atoms] - y[i_atoms] +
             i1 * sr_search->b_lat[3] + i2 * sr_search->b_lat[4];
          d_y *= d_y;
       
          if(d_min2 - d_x - d_y > 0.)
          {
            faux = GEO_PREF * (d_min - sqrt(d_x + d_y + d_z) ) / d_min;
            rgeo += (SQUARE(faux));
          } /* if d_min2 */

        }  /* for i2 */

      } /* for i1 */

    }  /* for j_atoms */

  }  /* for i_atoms */

  rgeo /= n_atoms;

  return (rgeo);
}
