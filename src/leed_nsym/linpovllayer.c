/*********************************************************************
 *                        LINPOVLLAYER.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/25.07.95 - Creation (copy from leed_inp_bul_layer)
 *   GH/03.05.00 - include copying of t_type.
 *   LD/28.03.14 - removed MIN_DIST redefinition
 *********************************************************************/

/*! \file
 *
 * Sorts which atoms belong in each crystallographic layer for the surface
 * atoms.
 */

#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#include "stddef.h"
#include "leed.h"
#include "leed_def.h"


/*!
 * Decides which atoms belong to which layer by performing the following:
 * - groups atoms to composite layers if necessary
 * - determines inter-layer vectors
 *
 * \param[in,out] par Pointer to structure containing all geometrical and
 * non-geometrical parameters of the bulk except atom positions and types.
 * Output will be written to structure element layers.
 * \param[in,out] atom_list Pointer to structure containing all geometrical and
 * non-geometrical parameters of the atoms to be grouped (see "leed_def.h").
 * The list of atoms must be ordered according to their z coordinate,
 * with smallest z first. \p atom_list will be modified during the execution
 * of this function.
 * \return number of layers for the surface atoms.
 * \note The function cannot handle the case when all layer distances are
 * smaller than #MIN_DIST. In this case the bulk must be modelled as one
 * composite layer.
 */
int leed_inp_overlayer(leed_crystal *par, leed_atom *atom_list)
{
  size_t i;
  int i_c, i_d;
  size_t n_atoms, i_atoms;
  size_t i_layer = 0;

  real b1[3], b2[3];       /* lattice vectors: 1=x, 2=y 0 is not used */
  real orig[4];            /* keeps trac of the position relative to the
                              origin of the coordinate system */

  real *vec;               /* intermediate storage for inter layer vectors */
  size_t *no_of_atoms;     /* intermediate storage for number of atoms in layer */

  real faux;
  real vaux[4];

  #ifdef CONTROL
  fprintf(STDCTR, "(leed_inp_overlayer): "
      "entering leed_inp_overlayer MIN_DIST= %.3f, n_atoms = %d\n",
      MIN_DIST*BOHR, par->n_atoms);
  #endif

  /* predefine some often used variables */
  n_atoms = par->n_atoms;
  b1[1] = par->b[1]; b1[2] = par->b[3];
  b2[1] = par->b[2]; b2[2] = par->b[4];

  /* Allocate memory for intermediate storage vectors vec and no_of_atoms:
   * max. number of layers = number of atoms */
  vec = (real *) malloc( (3*(n_atoms+1) + 1) * sizeof(real) );
  no_of_atoms = (size_t *) malloc(n_atoms * sizeof(size_t) );
 
  /* i_layer indicates the layer which an atom belongs to; it will eventually
   * be the total number of layers.
   *
   * orig keeps track of the position of the first atom with respect
   * to the overall coordinate system.
   *
   * vaux is used to store the z-position of the topmost atom in the layer.
   */
  no_of_atoms[i_layer] = 1;
  atom_list[0].layer = i_layer;

  orig[3] = atom_list[0].pos[3];
  orig[1] = orig[2] = 0.;

  vaux[3] = atom_list[0].pos[3];
  vaux[1] = vaux[2] = 0.;

  for(i_c=1; i_c<=3; i_c++)
  {
    *(vec + 3*i_layer + i_c) = 0.;
  }

  atom_list[0].pos[3] = 0.;
 
  for(i_atoms=1; i_atoms<n_atoms; i_atoms++)
  {

    #ifdef CONTROL
    fprintf(STDCTR, "(leed_inp_overlayer): pos: %.4f %.4f %.4f dist: %.4f\n",
              atom_list[i_atoms].pos[1]*BOHR,
              atom_list[i_atoms].pos[2]*BOHR,
              atom_list[i_atoms].pos[3]*BOHR,
              R_fabs(atom_list[i_atoms-1].pos[3] + vaux[3] -
                        atom_list[i_atoms].pos[3])*BOHR);
    #endif

    if( R_fabs(atom_list[i_atoms-1].pos[3]+vaux[3] - atom_list[i_atoms].pos[3])
        > MIN_DIST )
    {
      /* New layer:
       * - check, whether the previous layer contains only one atom
       *   (no_of_atoms[i_layer] == 1). If so, reset vec and
       *   atom_list[i_atoms-1].pos.
       * - set up new inter layer vector (vec[3*i_layer + i_c]);
       * - set up new origin of the layer (vaux);
       * - increase i_layer;
       */
      #ifdef CONTROL
      fprintf(STDCTR, "(leed_inp_overlayer): new layer, no_of_atoms[%d] = %d\n",
                      i_layer, no_of_atoms[i_layer]);
      #endif

      /* set up new inter-layer vector (vec) */
      *(vec + 3*i_layer + 1) = 0.;
      *(vec + 3*i_layer + 2) = 0.;
      *(vec + 3*i_layer + 3) = atom_list[i_atoms].pos[3] -
                               atom_list[i_atoms-1].pos[3] - vaux[3];

      /* set up new origin of the layer (vaux) */
      vaux[3] = atom_list[i_atoms].pos[3];

      /* check, whether the previous layer contains only one atom */
      if(no_of_atoms[i_layer] == 1)
      {
        if(i_layer == 0)
        {
          orig[1] = atom_list[i_atoms-1].pos[1];
          orig[2] = atom_list[i_atoms-1].pos[2];
        }
        else
        {
          *(vec + 3*(i_layer-1) + 1) += atom_list[i_atoms-1].pos[1];
          *(vec + 3*(i_layer-1) + 2) += atom_list[i_atoms-1].pos[2];
        }
       
        *(vec + 3*i_layer + 1) = - (atom_list[i_atoms-1].pos[1]);
        *(vec + 3*i_layer + 2) = - (atom_list[i_atoms-1].pos[2]);

        atom_list[i_atoms-1].pos[1] = 0.;
        atom_list[i_atoms-1].pos[2] = 0.;

      } /* if (no_of_atoms[i_layer] == 1) */

      i_layer ++;
      no_of_atoms[i_layer] = 0;

    }  /* if R_fabs ... */

    atom_list[i_atoms].layer = i_layer;
    no_of_atoms[i_layer] ++;

    atom_list[i_atoms].pos[3] -= vaux[3];

  }  /* for i_atoms (loop through atoms) */

  /* Check the last layer to see whether it contains only one atom
   * (no_of_atoms[i_layer] == 1).
   * - If so, reset vec and atom_list[n_atoms-1].pos.
   */
  #ifdef CONTROL_X
  fprintf(STDCTR, "(leed_inp_overlayer): no_of_atoms[%d] = %d\n",
                     i_layer, no_of_atoms[i_layer]);
  #endif

  if(no_of_atoms[i_layer] == 1)
  {
    if(i_layer == 0)
    {
      orig[1] = atom_list[n_atoms-1].pos[1];
      orig[2] = atom_list[n_atoms-1].pos[2];
    }
    else
    {
      *(vec + 3*(i_layer-1) + 1) += atom_list[n_atoms-1].pos[1];
      *(vec + 3*(i_layer-1) + 2) += atom_list[n_atoms-1].pos[2];
    }

    atom_list[n_atoms-1].pos[1] = 0.;
    atom_list[n_atoms-1].pos[2] = 0.;
  }

  /* - set the last inter-layer vector (points nowhere)
   * - update several parameters
   */
  vec[3*i_layer + 1] = vec[3*i_layer + 2] = vec[3*i_layer + 3] = 0.;
  atom_list[n_atoms].type = I_END_OF_LIST;

  i_layer ++;
  par->n_layers = i_layer;

  /* Ensure that all inter-layer vectors are the shortest possible. */
  #ifdef CONTROL_X
  for(i=0; i<i_layer; i++)
  {
    fprintf(STDCTR, "(leed_inp_overlayer): vec_org: %.4f %.4f %.4f \n",
            *(vec+3*i+1) *BOHR, *(vec+3*i+2) *BOHR, *(vec+3*i+3) *BOHR);
  }
  #endif

  for(i=0; i< i_layer; i++)
  {
    vaux[1] = *(vec+3*i + 1);
    vaux[2] = *(vec+3*i + 2);
    faux = SQUARE(vaux[1]) + SQUARE(vaux[2]);

    for(i_c = -1; i_c <= 1; i_c ++)
    {
      for(i_d = -1; i_d <= 1; i_d ++)
      {
        if( ( SQUARE(vaux[1] + i_c*b1[1] + i_d*b2[1]) +
              SQUARE(vaux[2] + i_c*b1[2] + i_d*b2[2]) ) < faux )
        {
          *(vec+3*i+1) = vaux[1] + i_c*b1[1] + i_d*b2[1];
          *(vec+3*i+2) = vaux[2] + i_c*b1[2] + i_d*b2[2];
        }
      } /* for i_d */
    } /* for i_c */
  } /* for i */

  #ifdef CONTROL_X
  for(i=0; i< i_layer; i++)
  {
    fprintf(STDCTR, "(leed_inp_overlayer): vec_mod: %.4f %.4f %.4f \n",
            *(vec+3*i+1) *BOHR, *(vec+3*i+2) *BOHR, *(vec+3*i+3) *BOHR);
  }
  #endif

  /* Allocate array "layers" and copy all relevant information from
   * atom_list, vec and no_of_atoms.
   *
   * The z order of layers will be the same as the order of atom_list
   * (smallest z, i.e. deepest layer, first)
   */

  /* Allocate */
  #ifdef CONTROL
  fprintf(STDCTR, "(leed_inp_overlayer): "
            "overlayer atoms split up into %u layers\n", i_layer);
  #endif

  if( (par->layers = (leed_layer*)
      malloc(i_layer * sizeof(leed_layer)) ) == NULL)
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (leed_inp_overlayer): "
       "unable to allocate memory for overlayer\n");
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(-1);
    #endif
  }

  /* Copy */
  for(i=0; i< i_layer; i++)
  {
    /* i refers to index in vec, no_of_atoms, and atom_list */

    /*
     * Write to par->layers[i]:
     * - number of atoms (from no_of_atoms),
     * - periodicity indicator (always zero for overlayer)
     * - lattice vectors (from par->b) and area of unit cell (superstructure)
     */
    par->layers[i].n_atoms = no_of_atoms[i];
    par->layers[i].periodic = 0;

    for(i_c = 1; i_c < 5; i_c ++)
    {
      par->layers[i].a_lat[i_c] = par->b[i_c];
    }
    par->layers[i].rel_area = par->rel_area_sup;

    /*
     * vec_to_next points to the next layer (i+1) except for the topmost
     * layer (i_layer) where it points nowhere (already taken care of in
     * vec[]
     */
    for(i_c = 1; i_c <= 3; i_c ++)
    {
      par->layers[i].vec_to_next[i_c] = vec[3*i + i_c];
    }

    /*
     * vec_from_last points from the last layer (i-1) to the current layer
     * except for the bottom-most layer (0) where it points from the origin
     * of the coordinate system to the layer (= orig[i_c])
     */
    if(i == 0)
    {
      for(i_c = 1; i_c <= 3; i_c ++)
      {
        par->layers[i].vec_from_last[i_c] = orig[i_c];
      }
    }
    else
    {
      for(i_c = 1; i_c <= 3; i_c ++)
      {
        par->layers[i].vec_from_last[i_c] = vec[3*(i-1) + i_c];
      }
    }

    /*
     * Allocate structure element atoms in layer and copy the appropriate
     * entries from list atom_list into par->layers[i].atoms
     */
    #ifdef CONTROL_X
    fprintf(STDCTR, "(leed_inp_overlayer): no_of_atoms[%d] = %d\n",
            i, no_of_atoms[i]);
    fprintf(STDCTR, "(leed_inp_overlayer): par->layers[%d].n_atoms = %d\n",
            i, par->layers[i].n_atoms);
    #endif

    par->layers[i].atoms = (leed_atom*)
        malloc(no_of_atoms[i] * sizeof(leed_atom));

    for( i_d = 0, i_atoms = 0; i_atoms < n_atoms; i_atoms ++)
    {
      if(atom_list[i_atoms].layer == i)
      {
        #ifdef CONTROL_X
        fprintf(STDCTR, "(leed_inp_overlayer): i_d = %d, i_atoms = %d\n",
                i_d, i_atoms);
        #endif

        par->layers[i].atoms[i_d].layer = i;
        par->layers[i].atoms[i_d].type = atom_list[i_atoms].type;
        par->layers[i].atoms[i_d].t_type = atom_list[i_atoms].t_type;
       
        par->layers[i].atoms[i_d].pos[1] = atom_list[i_atoms].pos[1];
        par->layers[i].atoms[i_d].pos[2] = atom_list[i_atoms].pos[2];
        par->layers[i].atoms[i_d].pos[3] = atom_list[i_atoms].pos[3];

        par->layers[i].atoms[i_d].dwf = atom_list[i_atoms].dwf;

       i_d ++;
      }
    } /* for i_atoms */

    if(i_d != par->layers[i].n_atoms)
    {
      #ifdef ERROR
      fprintf(STDERR, "*** error (leed_inp_overlayer): "
              "the numbers of atoms in layer %d do not match\n", i);
      #endif

      #ifdef EXIT_ON_ERROR
      exit(1);
      #else
      return(-1);
      #endif
    }
  } /* for i (loop over layers) */

  /* clean up */
  free(vec);
  free(no_of_atoms);

  #ifdef CONTROL
  fprintf(STDCTR, "(leed_inp_overlayer): "
      "leaving leed_inp_overlayer, return value = %d\n", i_layer);
  #endif

  return(i_layer);
} /* end of function leed_inp_overlayer */
