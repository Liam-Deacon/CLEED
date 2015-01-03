/*********************************************************************
 *                          SRCKROT.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1995.08.21 - Creation (copy from srmkinp.c)
 *   GH/1995.09.27 - More comments
 *   LD/2014.10.17 - Added doxygen markup
 *********************************************************************/

/*!
 * \file
 * \author Georg Held <g.held@reading.ac.uk>
 * \brief Check rotation symmetry
 *
 * \note The defines \c CONTROL_X and \c ERROR produce additional terminal
 * output when added to the list of definitions during compilation.
 *
 * \todo fix \c GEO_TOLERANCE \c #undef
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "csearch.h"

#undef  GEO_TOLERANCE
#define GEO_TOLERANCE  0.001

/*!
 * Check for rotation symmetry
 *
 * Two atoms at positions \f$ x_i \f$ and \f$ x_j \f$ are symmetry equivalent,
 * if:
 *
 * \f[ x_i = R_\phi \times (x_j - \vec{r_{axis}}) + \vec{r_{axis}}
 * + n_1 \vec{b_1} + n_2 \vec{b_2} \f]
 *
 * where
 * \f$ b_1\f$ & \f$ b_2 \f$ are the overlayer lattice vectors and
 * \f$ n_1 \f$ & \f$ n_2 \f$ are integer numbers,
 * \f$ \vec{r_{axis}} \f$ is the position of the rotation axis,
 * \f$ R_phi \f$ is the rotation matrix for \f$ \phi = \frac{2 \pi n}{n_rot} \f$ .
 *
 * The rotational matrix \f$ R_{\phi} \f$ is given by:
 * \f$ R[1]: cos(\phi) \f$
 * \f$ R[2]: -sin(\phi) \f$
 * \f$ R[3]: sin(\phi) \f$
 * \f$ R[4]: cos(\phi) \f$
 *
 * With \f$ \vec{B} = (b_1, b_2) \f$ it follows:
 *
 * \f[ B^{-1} \times (R_\phi -1) \times axis + B^{-1} \times
 * (x_j - R_\phi x_i) = integer vector. \f]
 *
 * This condition is tested in this function for all pairs of atoms that
 * could qualify to be symmetry equivalent.
 *
 * \param[in,out] atoms pointer to #search_atom \c struct which contains
 * information about initial atom positions, types, etc. The function will
 * modify the following structure elements:
 *  - \c x
 *  - \c y
 *  - \c z_par (conversion between parameter list and atomic coordinates)
 *  - \c ref
 *  - \c n_ref (references to symmetry equivalent atoms).
 *
 * \warning The following structure elements (for each atom) must be
 * assigned before entering the function:
 *  - \c x, \c y, \c z (initial atom positions)
 *  - \c type (atom type)
 *
 * \param[in,out] search pointer #search \c struct containing information
 * about symmetries, etc. The function will update the structure element
 * \c n_par (number of parameters).
 *
 * \note input is needed from the following structure elements:
 *  - \c b_lat     (lattice vectors)
 *  - \c z_only    (0 for xyz search, 1 for z search only)
 *  - \c rot_deg   (degree of rotational symmetry)
 *  - \c rot_axis  (x,y of rotational axis)
 *  - \c mir_point (x,y of point in mirror plane)
 *  - \c mir_dir   (x,y of direction of mirror plane)
 *
 * \note The program exits with code #SR_SYMMETRY_ERROR if the number of
 * atoms do not match the specified degree of rotational symmetry
 *
 * \return number of parameters in search.
 */
size_t sr_ckrot(search_atom *atoms, search *search )
{
  size_t i_atoms, j_atoms, n_atoms;
  size_t i_rot;
  size_t i_par, n_par;

  real R_1[5], R_2[5], R_3[5], R_4[5], R_5[5];
  real B_1[5], BR[5];
  real *R_n[7];                    /* max degree of rot. symmetry is 6 */

  real axis[3];
  real xi, yi, xj, yj;
  real faux;

/***********************************************************************
 * Find number of atoms in list atoms
 ***********************************************************************/

  for(n_atoms = 0; (atoms + n_atoms)->type != I_END_OF_LIST; n_atoms ++)
  { ; }

/***********************************************************************
 * Symmetry:
 *************************************************************************/

  if(search->rot_deg <= 1)
  {
    for(i_atoms = 0; i_atoms != n_atoms; i_atoms ++)
    {
      if( (atoms + i_atoms)->ref == U_END_OF_LIST )
      {
        /* First and only occurence of atom i => reference to itself */
        atoms[i_atoms].ref  = i_atoms;
        atoms[i_atoms].n_ref = 1;
      }
    }
  }
  else
  {
   
    /* Create B^-1 (used for all rotations) */
    faux = search->b_lat[1]*search->b_lat[4] - search->b_lat[2]*search->b_lat[3];
    faux = 1./faux;

    B_1[1] =  faux * search->b_lat[4];
    B_1[2] = -faux * search->b_lat[2];
    B_1[3] = -faux * search->b_lat[3];
    B_1[4] =  faux * search->b_lat[1];

    /* Create rot. matries R_n
     * R_n = ( 1: cos(n*phi)  2: -sin(n*phi) )
     *       ( 3: sin(n*phi)  4:  cos(n*phi) )
     */
    switch(search->rot_deg)
    {
      case(2):
      {
        R_1[1] = R_1[4] = -1.;
        R_1[2] = R_1[3] = 0.;

        R_n[1] = R_1;
        R_n[2] = NULL;

        break;

      } /* search->rot_deg = 2 */

      case(3):
      {
        faux = 2.*PI / search->rot_deg;

        R_1[1] = R_1[4] = R_cos(faux);
        R_1[2] = - ( R_1[3] = R_sin(faux) );

        /* 2*phi = -phi */
        R_2[1] = R_2[4] = R_1[1];
        R_2[2] = - R_1[2];
        R_2[3] = - R_1[3];

        R_n[1] = R_1;
        R_n[2] = R_2;
        R_n[3] = NULL;

        break;

      } /* search->rot_deg = 3 */
     
      case(4):
      {
        /* 90 deg */
        R_1[1] = R_1[4] = 0.;
        R_1[2] = - ( R_1[3] = 1.);

        /* 180 deg */
        R_2[1] = R_2[4] = -1.;
        R_2[2] = R_2[3] = 0.;

        /* 270 deg = -90 deg */
        R_3[1] = R_3[4] = 0.;
        R_3[2] = - ( R_3[3] = -1.);

        R_n[1] = R_1;
        R_n[2] = R_2;
        R_n[3] = R_3;
        R_n[4] = NULL;

        break;

      }  /* search->rot_deg = 4 */

      case(6):
      {
        faux = 2.*PI / search->rot_deg;

        R_1[1] = R_1[4] = R_cos(faux);
        R_1[2] = - ( R_1[3] = R_sin(faux) );

        /* 5*phi = -phi */
        R_5[1] = R_5[4] = R_1[1];
        R_5[2] = - R_1[2];
        R_5[3] = - R_1[3];

        /* 2*phi */
        faux = PI / search->rot_deg;
        R_2[1] = R_2[4] = R_cos(faux);
        R_2[2] = - ( R_2[3] = R_sin(faux) );

        /* 4*phi = -2*phi */
        R_4[1] = R_4[4] = R_2[1];
        R_4[2] = - R_2[2];
        R_4[3] = - R_2[3];

        /* 3*phi = 180 deg */
        R_3[1] = R_3[4] = -1.;
        R_3[2] = R_3[3] = 0.;

        R_n[1] = R_1;
        R_n[2] = R_2;
        R_n[3] = R_3;
        R_n[4] = R_4;
        R_n[5] = R_5;
        R_n[6] = NULL;

        break;

      } /* search->rot_deg = 6 */

    }  /* switch */

    /* Scan through all rotations and atoms and
     * find symmetry related atoms */

    for(i_atoms = 0; i_atoms < n_atoms; i_atoms ++)
    {
      if( (atoms + i_atoms)->ref == U_END_OF_LIST )
      {
        /* First occurence of atom i => reference to itself */

#       if CONTROL_X
        fprintf(STDCTR, "\n");
        CONTROL_MSG(CONTROL_X, "atom: %d ->\trot No n1 n2", i_atoms);
#       endif

        (atoms + i_atoms)->ref  = i_atoms;
        (atoms + i_atoms)->n_ref = 1;
  
        xi = (atoms + i_atoms)->x;
        yi = (atoms + i_atoms)->y;

        for(i_rot = 1; R_n[i_rot] != NULL; i_rot ++)
        {
          /* BR = (B^-1) * R_n[i_rot] */
          BR[1] =  B_1[1] * R_n[i_rot][1] + B_1[2] * R_n[i_rot][3];
          BR[2] =  B_1[1] * R_n[i_rot][2] + B_1[2] * R_n[i_rot][4];
          BR[3] =  B_1[3] * R_n[i_rot][1] + B_1[4] * R_n[i_rot][3];
          BR[4] =  B_1[3] * R_n[i_rot][2] + B_1[4] * R_n[i_rot][4];

          axis[1] = (B_1[1] - BR[1]) * search->rot_axis[1] +
                    (B_1[2] - BR[2]) * search->rot_axis[2];
          axis[2] = (B_1[3] - BR[3]) * search->rot_axis[1] +
                    (B_1[4] - BR[4]) * search->rot_axis[2];

          /* First check if atom i is mapped to itself by rotation
           * (j_atoms = i_atoms) */
          faux = B_1[1]*xi + B_1[2]*yi - (BR[1]*xi + BR[2]*yi + axis[1]);
          faux -= R_nint(faux);
          if( fabs(faux) < GEO_TOLERANCE)
          {
            faux = B_1[3]*xi + B_1[4]*yi - (BR[3]*xi + BR[4]*yi + axis[2]);
            faux -= R_nint(faux);

            if( fabs(faux) < GEO_TOLERANCE)
            {
              (atoms + i_atoms)->n_ref = 0;
            } /* atoms i is symmetry equivalent to itself */

          } /* if fabs ... */

          for(j_atoms = i_atoms+1; j_atoms < n_atoms; j_atoms ++)
          {
            if( ( (atoms + j_atoms)->ref == U_END_OF_LIST ) &&
                ( fabs( (atoms + i_atoms)->z - (atoms + j_atoms)->z )
                 < GEO_TOLERANCE) &&
               ( strcmp((atoms + i_atoms)->name, (atoms + j_atoms)->name)
                 == 0)  )
            {
              xj = (atoms + j_atoms)->x;
              yj = (atoms + j_atoms)->y;

              faux = B_1[1]*xj + B_1[2]*yj - (BR[1]*xi + BR[2]*yi + axis[1]);
          
#             if CONTROL_X
              fprintf(STDCTR, "\n\t\t\t %d %d %.3f, ", i_rot, j_atoms, faux);
#             endif

              faux -= R_nint(faux);
              if( fabs(faux) < GEO_TOLERANCE)
              {
                faux = B_1[3]*xj + B_1[4]*yj - (BR[3]*xi + BR[4]*yi + axis[2]);

#               if CONTROL_X
                fprintf(STDCTR, " %.3f ",  faux);
#               endif

                faux -= R_nint(faux);
                if( fabs(faux) < GEO_TOLERANCE)
                {
                  /* - Copy reference of atom i into j
                   * - increment n_ref(i)
                   *      (number of atoms which are equivalent to i)
                   * - set n_ref(j) to i_rot;
                   */
                  (atoms + j_atoms)->ref = (atoms + i_atoms)->ref;
                  (atoms + j_atoms)->n_ref = i_rot;
                  (atoms + i_atoms)->n_ref ++;
                } /* atoms i and j are symmetry equivalent */

              } /* if fabs ... */

            } /* if ref ... */

          } /* for j_atoms */

        } /* for i_rot */

      } /* if (atoms + i_atoms)->ref == I_END_OF_LIST */

    } /* for i_atoms */

  }  /* search->rot_deg > 1 */

/************************************************************************
 * Count search parameters and allocate memory for xyz_par
 * Check assignment of atoms
 *************************************************************************/

  for(i_atoms = 0, n_par = 0; i_atoms != n_atoms; i_atoms ++)
  {
    if( (atoms + i_atoms)->ref == i_atoms )
    {
      if( (atoms + i_atoms)->n_ref == 0 ) n_par += 1;
      else if((atoms + i_atoms)->n_ref == search->rot_deg )
      {
       n_par += ((search->z_only)? 1 : 3);
      }
      else
      {
        ERROR_MSG("number of atoms does not match with symmetry (%d)\n", i_atoms);
        exit(SR_SYMMETRY_ERROR);
      }

    }  /* if ref == i_atoms */

  }  /* for i_atoms */


  for(i_atoms = 0; i_atoms != n_atoms; i_atoms ++)
  {
    if(!search->z_only)
    {
      (atoms+i_atoms)->x_par = (real *)calloc( (n_par+1), sizeof(real) );
      (atoms+i_atoms)->y_par = (real *)calloc( (n_par+1), sizeof(real) );
    }
    (atoms+i_atoms)->z_par = (real *)calloc( (n_par+1), sizeof(real) );
  }

  CONTROL_MSG(CONTROL_X, "No of parameters in search: %d\n", n_par);

/************************************************************************
 * Identify search parameters
 *************************************************************************/

  for(i_atoms = 0, i_par = 1; i_atoms != n_atoms; i_atoms ++)
  {
    if( (atoms + i_atoms)->ref == i_atoms )
    {

      /* x/y variation if atom is not equivalent to itself & xy search required */
      if( ((atoms + i_atoms)->n_ref == search->rot_deg) && (!search->z_only) )
      {
        (atoms+i_atoms)->x_par[i_par] = 1.;
        i_par ++;
        (atoms+i_atoms)->y_par[i_par] = 1.;
        i_par ++;
      }
      (atoms+i_atoms)->z_par[i_par] = 1.;
      i_par ++;

      /* Find other atoms which are equivalent to atom i */
      for(j_atoms = i_atoms+1; j_atoms < n_atoms; j_atoms ++)
      {
        if( (atoms + j_atoms)->ref == (atoms + i_atoms)->ref )
        {
          if(!search->z_only)
          {
            i_rot = (atoms + j_atoms)->n_ref;
            (atoms+j_atoms)->x_par[i_par - 3] = R_n[i_rot][1];
            (atoms+j_atoms)->x_par[i_par - 2] = R_n[i_rot][2];
            (atoms+j_atoms)->y_par[i_par - 3] = R_n[i_rot][3];
            (atoms+j_atoms)->y_par[i_par - 2] = R_n[i_rot][4];
          }
          (atoms+j_atoms)->z_par[i_par - 1] = 1.;
        }

      }  /* for j_atoms */

    }  /* if ref == i_atoms */

  }  /* for i_atoms */

# if CONTROL_X
  CONTROL_MSG(CONTROL_X, "%d parameters, atoms:\n", n_par);
  for(i_atoms = 0; i_atoms != n_atoms; i_atoms ++)
  {
    fprintf(STDCTR, "\n%d \"%s\" (%6.3f, %6.3f, %6.3f) ref: %d n_ref: %d",
            i_atoms, (atoms + i_atoms)->name,
            (atoms + i_atoms)->x, (atoms + i_atoms)->y, (atoms + i_atoms)->z,
            (atoms + i_atoms)->ref, (atoms + i_atoms)->n_ref);
    fprintf(STDCTR, "\n\tr_min: %6.3f", (atoms + i_atoms)->r_min);

    if(!search->z_only)
    {
      fprintf(STDCTR, "\n\tx_par: ");
      for(i_par = 1; i_par <= n_par; i_par ++)
      {
        fprintf(STDCTR, "%6.3f, ", (atoms+i_atoms)->x_par[i_par]);
      }
      fprintf(STDCTR, "\n\ty_par: ");
      for(i_par = 1; i_par <= n_par; i_par ++)
      {
        fprintf(STDCTR, "%6.3f, ", (atoms+i_atoms)->y_par[i_par]);
      }
    } /* if z_only */
    fprintf(STDCTR, "\n\tz_par: ");
    for(i_par = 1; i_par <= n_par; i_par ++)
    {
      fprintf(STDCTR, "%6.3f, ", (atoms+i_atoms)->z_par[i_par]);
    }
    fprintf(STDCTR, "\n");
  }
# endif /* CONTROL_X */

/************************************************************************
 * return number of search parameters
 *************************************************************************/
 search->n_par = n_par;

 return(n_par);
}
