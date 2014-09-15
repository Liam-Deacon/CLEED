/*********************************************************************
  GH/03.05.00 
  file contains function:

  leed_inp_bul_layer

Changes:
  GH/01.07.94 - Creation.
  GH/01.03.95 - correct termination of list atoms
  GH/13.07.95 - Preserve the x/y coordinates of atoms in composite 
                layers (the topmost atom is not automatically set
                to be the origin).
  GH/25.07.95 - introduce structure layers.
  GH/14.09.95 - bug fix regarding the last vec_to_next.
  GH/03.05.00 - include copying of t_type.
  LD/28.03.14 - removed MIN_DIST redefinition
  
*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "leed.h"


int leed_inp_bul_layer(leed_cryst_t *par, 
                  leed_atom_t * atom_list, real *a3)

/************************************************************************

 Decide which atoms belong to which layer:
 - group atoms to composite layers if necessary;
 - determine inter layer vectors;

 
 INPUT PARAMETERS:

 leed_cryst_t *par (input, output)
     structure that contains all geometrical and nongeometrical 
     parameters of the bulk except atom positions and types.
     Output will be written to structure element layers.

 leed_atom_t *atom_list (input)
     structure that contains all geometrical and nongeometrical
     parameters of the atoms to be grouped (see "leed_def.h").
     (The list of atoms must be ordered according to their z coordinate,
     LARGEST z first.) It will be modified during the execution
     of this function.

 real *a3
     3rd basis vector of the bulk unit cell.
       
 return value:
     number of layers (i_layer)   if o.k.

 DESIGN:


 NOTE:
 The function cannot handle the case when all layer distances are smaller
 than MIN_DIST. In this case the bulk must be modelled as one composite 
 layer.
 
*************************************************************************/
{
int i,j; 
int i_c, i_d;
int n_atoms, i_atoms;
int i_layer;
int end_per;             /* indicates end of vertical periodicity */

real a1[3], a2[3];       /* 2-dim lattice vectors: 1=x, 2=y 0 is not used */
real orig[4];            /* keeps trac of the position relative to the
                            origin of the coordinate system */

real *vec;               /* intermediate storage for inter layer vectors */
int *no_of_atoms;        /* intermediate storage for number of atoms in layer */

real faux;
real vaux[4];

#ifdef CONTROL
 fprintf(STDCTR,"(leed_inp_bul_layer): entering leed_inp_bul_layer MIN_DIST= %.3f\n", 
         MIN_DIST*BOHR);
#endif
/************************************************************************
  predefine some often used variables
*************************************************************************/

 n_atoms = par->natoms;
 a1[1] = par->a[1]; a1[2] = par->a[3];
 a2[1] = par->a[2]; a2[2] = par->a[4];

#ifdef CONTROL
 fprintf(STDCTR,
 "(leed_inp_bul_layer): n_atoms = %d, a1 = (%.3f,%.3f), a2 = (%.3f,%.3f)\n",
         n_atoms, a1[1]*BOHR, a1[2]*BOHR, a2[1]*BOHR, a2[2]*BOHR);
#endif


/* 
  Allocate memory for intermediate storage vectors vec and no_of_atoms:
  max. number of layers = number of atoms 
*/
 vec = (real *) calloc( (3*(n_atoms+1) + 1), sizeof(real) );
 no_of_atoms = (int *) calloc(n_atoms, sizeof(int) );
 
/************************************************************************
  i_layer indicates the layer which an atom belongs to; it will eventually 
          be the total number of layers.
  orig  keeps track of the position of the first atom with respect 
          to the overall coordinate system.
  vaux    is used to store the z-position of the topmost atom in the layer.
*************************************************************************/

 i_layer = 0;
 no_of_atoms[i_layer] = 1;
 atom_list[0].layer = i_layer;

 orig[1] = 0.;
 orig[2] = 0.;
 orig[3] = atom_list[0].pos[3];

 vaux[3] = atom_list[0].pos[3];
 vaux[1] = vaux[2] = 0.;

 for(i_c=1; i_c<=3; i_c++) 
   *(vec + 3*i_layer + i_c) = 0.;

 atom_list[0].pos[3] = 0.;
 
 for(i_atoms=1; i_atoms<n_atoms; i_atoms++)
 {

#ifdef CONTROL
   fprintf(STDCTR,"(leed_inp_bul_layer): pos: %.4f %.4f %.4f dist: %.4f\n", 
   atom_list[i_atoms].pos[1]*BOHR, atom_list[i_atoms].pos[2]*BOHR, 
   atom_list[i_atoms].pos[3]*BOHR,
   R_fabs(atom_list[i_atoms-1].pos[3]+vaux[3]-atom_list[i_atoms].pos[3])*BOHR);
#endif

   if( R_fabs(atom_list[i_atoms-1].pos[3]+vaux[3] - atom_list[i_atoms].pos[3]) 
        > MIN_DIST )
   {
/*********************************************************************
   New layer:
   - check, weather the previous layer contains only one atom 
     (no_of_atoms[i_layer] == 1). 
     If so, reset vec and atom_list[i_atoms-1].pos.
   - set up new inter layer vector (vec[3*i_layer + i_c]);
   - set up new origin of the layer (vaux);
   - increase i_layer;
**********************************************************************/
#ifdef CONTROL
     fprintf(STDCTR,"(leed_inp_bul_layer): new layer, no_of_atoms[%d] = %d\n", 
                     i_layer, no_of_atoms[i_layer]);
#endif

/* set up new inter layer vector (vec) */
     *(vec + 3*i_layer + 1) = 0.;
     *(vec + 3*i_layer + 2) = 0.;
     *(vec + 3*i_layer + 3) = 
        atom_list[i_atoms].pos[3] - atom_list[i_atoms-1].pos[3] - vaux[3];

/* set up new origin of the layer (vaux) */
     vaux[3] = atom_list[i_atoms].pos[3];

/* check, weather the previous layer contains only one atom */
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
     }

     i_layer ++; 
     no_of_atoms[i_layer] = 0;
   }  /* if R_fabs ... */

   atom_list[i_atoms].layer = i_layer;
   no_of_atoms[i_layer] ++;

   atom_list[i_atoms].pos[3] -= vaux[3];

 }  /* for i_atoms (loop through atoms) */

/************************************************************************
  BULK:
  Set up the last inter layer vector to point from the last layer to the
  first one of the next unit cell.
*************************************************************************/

#ifdef CONTROL
 fprintf(STDCTR,"(leed_inp_bul_layer): setting up bulk layers\n");
#endif

 *(vec+3*i_layer + 1) = orig[1] + a3[1];
 *(vec+3*i_layer + 2) = orig[2] + a3[2];
 *(vec+3*i_layer + 3) = orig[3] + a3[3] 
                           - vaux[3] - atom_list[n_atoms-1].pos[3];;

 if( R_fabs( *(vec + 3*i_layer + 3) ) < MIN_DIST )
 {
   if(i_layer == 0)
   {
#ifdef ERROR
     fprintf(STDERR," *** error (leed_inp_bul_layer): bulk atoms are too close ");
     fprintf(STDERR," (cannot be split into layers).\n");
#endif
#ifdef EXIT_ON_ERROR
     exit(1);
#else
     return(-1);
#endif
   }
   else
   {
#ifdef CONTROL
     fprintf(STDCTR,"(leed_inp_bul_layer): inter layer distance is too short\n");
#endif
/************************************************************************
 If the inter layer distance is too short and the bulk layers can be split
 into layers for layer doubling, merge the last layer with the first one:
  - count number of atoms in incomplete first layer, reallocate
    the equivalent storage space in atom_list.
  - copy the atoms of the first layer into the last layer;
  - set up the new inter layer vector.
*************************************************************************/

     for (j=0; atom_list[j].layer == 0; j++);
     atom_list = ( leed_atom_t *) realloc( 
                 atom_list, (n_atoms+j+2) * sizeof(leed_atom_t) );

   /* 
      The inter layer vector now points from the z coordinate of the frist 
      atom in the last layer (vaux) to the z coordinate of the first atom
      in the first layer translated by a3.
   */
     *(vec + 3*i_layer + 3) += atom_list[n_atoms-1].pos[3];

     for (j=0 ; atom_list[j].layer == 0; j++)
     {
       atom_list[n_atoms + j].layer = i_layer;
       atom_list[n_atoms + j].type = atom_list[j].type;
       atom_list[n_atoms + j].t_type = atom_list[j].t_type;
       for(i_c=1; i_c<=3; i_c++)
       {
         atom_list[n_atoms + j].pos[i_c] = 
         atom_list[j].pos[i_c] + *(vec+3*i_layer + i_c);
       }
       no_of_atoms[i_layer] ++;
     } /* for j */

   /*
      Reset inter layer vector:
      The components of the interlayer vector parallel to the surface 
      are different from the vector between 1st and 2nd layer due to the 
      (possibly) different x/y origins of the layers. 
      The 3rd components are equal.
   */ 
     *(vec + 3*i_layer + 1) += *(vec + 1); 
     *(vec + 3*i_layer + 2) += *(vec + 2); 
     *(vec + 3*i_layer + 3)  = *(vec + 3); 

   /* update n_atoms and start of periodic stacking */
     n_atoms += j;
     end_per = 1;

   } /* else (i_layer != 0) */
 } /* if < MIN_DIST */

 else
 {
/*********************************************************************
 If the first and the last layer are not merged, check the last layer
 weather it contains only one atom (no_of_atoms[i_layer] == 1). 
 - If so, reset vec and atom_list[i_atoms-1].pos. 
**********************************************************************/

   if(no_of_atoms[i_layer] == 1)
   {
     if(i_layer == 0)
     {
       orig[1] = atom_list[0].pos[1];
       orig[2] = atom_list[0].pos[2];

       *(vec + 1) = a3[1];
       *(vec + 2) = a3[2];
       *(vec + 3) = a3[3];
     }
     else
     {
       *(vec + 3*(i_layer-1) + 1) += atom_list[n_atoms-1].pos[1];
       *(vec + 3*(i_layer-1) + 2) += atom_list[n_atoms-1].pos[2];

       *(vec + 3*i_layer + 1) -= (atom_list[n_atoms-1].pos[1]);
       *(vec + 3*i_layer + 2) -= (atom_list[n_atoms-1].pos[2]);
     }

     atom_list[n_atoms-1].pos[1] = 0.;
     atom_list[n_atoms-1].pos[2] = 0.;
   }

   /* update start of periodic stacking */
   end_per = 0;

 }  /* else (> MIN_DIST) */

/* update several parameters */
 par->natoms = n_atoms;
 atom_list[n_atoms].type = I_END_OF_LIST;

 i_layer ++;
 par->nlayers = i_layer;

/************************************************************************
  Make shure that all inter layer vectors are the shortest possible.
*************************************************************************/

#ifdef CONTROL_X
 for(i=0; i< i_layer; i++)
 {
 fprintf(STDCTR,"(leed_inp_bul_layer): vec_org: %.4f %.4f %.4f layer%d\n", 
                 *(vec+3*i+1) *BOHR, 
                 *(vec+3*i+2) *BOHR, 
                 *(vec+3*i+3) *BOHR,i);
 }
#endif

 for(i=0; i< i_layer; i++)
 {
   vaux[1] = *(vec+3*i + 1);
   vaux[2] = *(vec+3*i + 2);
   faux = SQUARE(vaux[1]) + SQUARE(vaux[2]);

   for(i_c = -1; i_c <= 1; i_c ++)
   for(i_d = -1; i_d <= 1; i_d ++) 
   {
     if( ( SQUARE(vaux[1] + i_c*a1[1] + i_d*a2[1]) +
           SQUARE(vaux[2] + i_c*a1[2] + i_d*a2[2]) ) < faux )
     {
       *(vec+3*i+1) = vaux[1] + i_c*a1[1] + i_d*a2[1];
       *(vec+3*i+2) = vaux[2] + i_c*a1[2] + i_d*a2[2];
     }
   } /* for i_c, i_d */
 } /* for i */

#ifdef CONTROL_X
 for(i=0; i< i_layer; i++)
 {
 fprintf(STDCTR,"(leed_inp_bul_layer): vec_mod: %.4f %.4f %.4f \n", 
                 *(vec+3*i+1) *BOHR, 
                 *(vec+3*i+2) *BOHR, 
                 *(vec+3*i+3) *BOHR);
 }
 fprintf(STDCTR,"(leed_inp_bul_layer):i_layer = %d\n", i_layer);
#endif

/************************************************************************
  Allocate array "layers" and copy all relevant information from
  atom_list, vec and no_of_atoms.

  The z order of layers will be the reverse of the order of atom_list
  (smallest z, i.e. deepest layer, first)
*************************************************************************/

/* Allocate */
 
 par->layers = 
       (leed_layer_t *) malloc( (i_layer+1) * sizeof(leed_layer_t) );

 for(i=0; i< i_layer; i++)
 {
/* 
   i refers to index in vec, no_of_atoms, and atom_list
   j refers to index of layers
*/
   j = i_layer - i - 1;

/* 
   Write to par->layers[j]:
   - number of atoms (from no_of_atoms), 
   - periodicity indicator 
   - lattice vectors (from par->a)
   - interlayer vectors ( from - vec[3*i + i_c]
*/
   par->layers[j].natoms = no_of_atoms[i];

   if( (i == 0) && (end_per == 1) )
       par->layers[j].periodic = 0;
   else
     par->layers[j].periodic = 1;

   for(i_c = 0; i_c < 5; i_c ++)
     par->layers[j].a_lat[i_c] = par->a[i_c];

/* for bulk layers 1x1 periodicity is assumed */
     par->layers[j].rel_area = 1.;

   for(i_c = 1; i_c <= 3; i_c ++)
     par->layers[j].vec_from_last[i_c] = - vec[3*i + i_c];

/*
   vec_to_next points to the next layer (j+1) except for the topmost
   layer (i_layer) where it points to the origin of the coordinate 
   system (- orig[i_c])
*/
   if (i > 0)
     for(i_c = 1; i_c <= 3; i_c ++)
       par->layers[j].vec_to_next[i_c] = - vec[3*(i-1) + i_c];
   else
     for(i_c = 1; i_c <= 3; i_c ++)
     {
       par->layers[j].vec_to_next[i_c] = - orig[i_c];
     }

/* 
   Allocate structure element atoms in layer 
   and copy the appropriate entries from list atom_list into 
   par->layers[j].atoms
*/
   par->layers[j].atoms =
     (leed_atom_t *) malloc(no_of_atoms[i] * sizeof(leed_atom_t) );

   for( i_d = 0, i_atoms = 0; i_atoms < n_atoms; i_atoms ++)
   {
     if(atom_list[i_atoms].layer == i)
     {
       par->layers[j].atoms[i_d].layer = j;
       par->layers[j].atoms[i_d].type = atom_list[i_atoms].type;
       par->layers[j].atoms[i_d].t_type = atom_list[i_atoms].t_type;
       
       par->layers[j].atoms[i_d].pos[1] = atom_list[i_atoms].pos[1];
       par->layers[j].atoms[i_d].pos[2] = atom_list[i_atoms].pos[2];
       par->layers[j].atoms[i_d].pos[3] = atom_list[i_atoms].pos[3];

       par->layers[j].atoms[i_d].dwf = atom_list[i_atoms].dwf;

       i_d ++;
     }
   } /* for i_atoms */

   if(i_d != par->layers[j].natoms)
   {
#ifdef ERROR
     fprintf(STDERR,
"*** error (leed_inp_bul_layer): the numbers of atoms in layer %d do not match\n", j);
#endif
#ifdef EXIT_ON_ERROR
       exit(1);
#else
       return(-1);
#endif
   }
 } /* for i (loop over layers) */

 free(vec);
 free(no_of_atoms);

#ifdef CONTROL
 fprintf(STDCTR,
  "(leed_inp_bul_layer): leaving inp_layer, return value = %d\n", i_layer);
#endif

 return(i_layer);
} /* end of function leed_inp_bul_layer */
/*======================================================================*/
