/*********************************************************************
  file contains function:

  leed_inp_overlayer_sym

Creation:
  WB/14.07.98 - copy from inpbullayersym.c

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include "leed.h"
#include "leed_def.h"

#define epsilon 0.001

#ifndef MIN_DIST             /* should be defined in "leed_def.h" */
#define MIN_DIST 2.0
#endif

int leed_inp_overlayer_sym(leed_cryst_t *par,leed_atom * atom_list) 

/************************************************************************

 Decide which atoms belong to which layer:
 - group atoms to composite layers if necessary;
 - determine inter layer vectors;

 
 INPUT PARAMETERS:

 leed_cryst_t *par (input, output)
     structure that contains all geometrical and nongeometrical 
     parameters of the overlayer except atom positions and types.
     Output will be written to structure element layers.

 leed_atom *atom_list (input)
     structure that contains all geometrical and nongeometrical
     parameters of the atoms to be grouped (see "leed_def.h").
     (The list of atoms must be ordered according to their z coordinate,
     SMALLEST z first.) It will be modified during the execution
     of this function.

 return value:
     number of layers (i_layer)   if o.k.

 DESIGN:


*************************************************************************/
{
int i; 
int i_c,i_d;
int n_mir,n_rot;
int n_atoms, i_atoms;
int i_layer;
//int end_per;             /* indicates end of vertical periodicity */

real b1x,b1y,b2x,b2y;    /* 2-dim lattice shifttors: 1=x, 2=y 0 is not used */
real orig;         
                  
real *vec;               /* intermediate storage for inter layer vectors */
real *shift;             /* intermediate storage for registry-shifts */
int *no_of_atoms;        /* intermediate storage for number of atoms in layer */

real faux;
real vaux[2];


#ifdef CONTROL
 fprintf(STDCTR,"(leed_inp_overlayer_sym): entering leed_inp_overlayer_sym MIN_DIST= %.3f n_atoms = %d\n", 
         MIN_DIST*BOHR ,  par->natoms);
#endif
/************************************************************************
  predefine some often used variables
*************************************************************************/

 n_atoms = par->natoms;
 n_rot = par->n_rot;
 n_mir = par->n_mir;

 b1x = par->b[1];
 b1y = par->b[3];
 b2x = par->b[2];
 b2y = par->b[4];

#ifdef CONTROL
 fprintf(STDCTR,
 "(leed_inp_overlayer_sym): n_atoms = %d, b1 = (%.3f,%.3f), b2 = (%.3f,%.3f)\n",
         n_atoms, b1x*BOHR, b1y*BOHR, b2x*BOHR, b2y*BOHR);
#endif


/* 
  Allocate memory for intermediate storage vectors vec,shift and no_of_atoms:
  max. number of layers = number of atoms 
*/
   vec = (real *) calloc( (4*(n_atoms) + 1), sizeof(real) ); 
 shift = (real *) calloc( (2*(n_atoms) + 1), sizeof(real) );
 no_of_atoms = (int *) calloc(n_atoms , sizeof(int) );
 
/************************************************************************
  i_layer indicates the layer which an atom belongs to; it will eventually 
          be the total number of layers.
  orig  keeps track of the position of the rotaxis(new origin).
        the origin is set to (0;0;0) 
  vaux  is used to store the z-position of the topmost atom in the layer.
*************************************************************************/

 i_layer = 0;
 no_of_atoms[i_layer] = 1;
 atom_list[0].layer = i_layer;

 orig = atom_list[0].pos[3];

 vaux[0] = atom_list[0].pos[3];
 atom_list[0].pos[3] = 0.;
 
#ifdef CONTROL
 for(i_atoms = 0;i_atoms < n_atoms; i_atoms++)
  fprintf(STDCTR,"(leed_inp_overlayer_sym): old atomlist, atom%d (%f %f)\n",
         i_atoms, atom_list[i_atoms].pos[1]*BOHR,atom_list[i_atoms].pos[2]*BOHR);
#endif

 for(i_atoms=1; i_atoms<n_atoms; i_atoms++)
 {
  if( R_fabs(atom_list[i_atoms-1].pos[3]+vaux[0] - atom_list[i_atoms].pos[3]) 
        > MIN_DIST )
  {
/*********************************************************************
   New layer:
   - check, weather the previous layer contains only one atom 
     (no_of_atoms[i_layer] == 1). 
   - set up new inter layer vector (vec[3*i_layer + i_c]);
   - increase i_layer;
   - set up new origin of the layer;
**********************************************************************/
#ifdef CONTROL
     fprintf(STDCTR,"(leed_inp_overlayer_sym): new layer, no_of_atoms[%d] = %d\n", 
                     i_layer, no_of_atoms[i_layer]);
#endif
 
   if(no_of_atoms[i_layer] != 1)
   {
    *(shift + 2*i_layer + 0) = 0;
    *(shift + 2*i_layer + 1) = 0;

/* set up new inter layer vector  */
    *(vec + 4*i_layer + 1) = 0.;
    *(vec + 4*i_layer + 2) = 0.;
    *(vec + 4*i_layer + 3) =
    atom_list[i_atoms].pos[3] - atom_list[i_atoms-1].pos[3] - vaux[0];
     
   }
   else
   {
    *(shift + 2*i_layer + 0) = - atom_list[i_atoms -1].pos[1];
    *(shift + 2*i_layer + 1) = - atom_list[i_atoms -1].pos[2];

/* set up new inter layer vector  */
    *(vec + 4*i_layer + 1) = 0.;
    *(vec + 4*i_layer + 2) = 0.;
    *(vec + 4*i_layer + 3) =
        atom_list[i_atoms].pos[3] - atom_list[i_atoms-1].pos[3] - vaux[0];

   }

     i_layer ++;
     no_of_atoms[i_layer] = 0;
     vaux[0] = atom_list[i_atoms].pos[3];
     atom_list[i_atoms].pos[3] = 0.;

  }  /* if R_fabs ... */

  /***********************************************************************
   set up new atom position.
   in case of composite layer keep track of the new origin.
  ***********************************************************************/
   atom_list[i_atoms -1].pos[1] = atom_list[i_atoms -1].pos[1];
   atom_list[i_atoms -1].pos[2] = atom_list[i_atoms -1].pos[2];
   no_of_atoms[i_layer]++;
   atom_list[i_atoms].layer = i_layer;
   if(no_of_atoms[i_layer] > 1)
   { 
   atom_list[i_atoms].pos[3] -= vaux[0];
   }
 }  /* for i_atoms (loop through atoms) */

/************************************************************************
  OVER:
  Set up the last inter layer vector to point from the last layer to the
  first one of the next unit cell.
*************************************************************************/

 atom_list[n_atoms -1].pos[1] = atom_list[n_atoms -1].pos[1];
 atom_list[n_atoms -1].pos[2] = atom_list[n_atoms -1].pos[2];

 if(no_of_atoms[i_layer] > 1)
 {
 *(shift + 2*i_layer + 0) = 0;
 *(shift + 2*i_layer + 1) = 0;
 }
 else
 { 
 *(shift + 2*i_layer + 0) = -atom_list[n_atoms -1].pos[1];
 *(shift + 2*i_layer + 1) = -atom_list[n_atoms -1].pos[2];
 }

 *(vec+4*i_layer + 1) = 0.;
 *(vec+4*i_layer + 2) = 0.;
 *(vec+4*i_layer + 3) = 0.;


/****** update several parameters ******/
 par->natoms = n_atoms;
 atom_list[n_atoms].type = I_END_OF_LIST;

 i_layer ++;
 par->nlayers = i_layer;

#ifdef CONTROL_X
 for(i=0; i< i_layer; i++)
 fprintf(STDCTR,"(leed_leed_inp_bul_layer_sym): shift_orig: %.4f %.4f \n",
                 *(shift+2*i+0) *BOHR,
                 *(shift+2*i+1) *BOHR);
 fprintf(STDCTR,"(leed_leed_inp_bul_layer_sym):i_layer = %d\n", i_layer);
#endif

/************************************************************************
  Make shure that all registry shifts are the shortest possible.
*************************************************************************/

 for(i=0; i< i_layer; i++)
 {
   vaux[0] = *(shift+2*i + 0);
   vaux[1] = *(shift+2*i + 1);
   faux = SQUARE(vaux[0]) + SQUARE(vaux[1]);

   for(i_c = -1; i_c <= 1; i_c ++)
   for(i_d = -1; i_d <= 1; i_d ++)
   {
     if( ( SQUARE(vaux[0] + i_c*b1x + i_d*b2x) +
           SQUARE(vaux[1] + i_c*b1y + i_d*b2y) ) < faux )
     {
       *(shift+2*i+0) = vaux[0] + i_c*b1x + i_d*b2x;
       *(shift+2*i+1) = vaux[1] + i_c*b1y + i_d*b2y;
     }
   } /* for i_c, i_d */
 } /* for i */
 
#ifdef CONTROL_X
 for(i=0; i< i_layer; i++)
 fprintf(STDCTR,"(leed_leed_inp_bul_layer_sym): shift_mod: %.4f %.4f \n",
                 *(shift+2*i+0) *BOHR,
                 *(shift+2*i+1) *BOHR);
 fprintf(STDCTR,"(leed_leed_inp_bul_layer_sym):i_layer = %d\n", i_layer);
#endif

/************************************************************************
  Allocate array "layers" and copy all relevant information from
  atom_list, shift and no_of_atoms.
  The z order of layers will be the reverse of the order of atom_list
  (smallest z, i.e. deepest layer, first)
*************************************************************************/
 
 par->layers =
       (leed_layer *) malloc( (i_layer + 1) * sizeof(leed_layer) );

 for(i = 0 ; i < i_layer ; i++)
 {
   par->layers[i].no_of_layer = i;
   par->layers[i].bulk_over = OVER;
   par->layers[i].reg_shift[1] = *(shift+2*i+0); 
   par->layers[i].reg_shift[2] = *(shift+2*i+1);
 }

 for(i=0; i< i_layer; i++)
 {

/********************************************************************* 
   i refers to index in shift, no_of_atoms, and atom_list
   Write to par->layers[j]:
   - number of atoms (from no_of_atoms), 
   - periodicity indicator 
   - lattice vectors (from par->a)
   - interlayer vectors 
*********************************************************************/
   par->layers[i].natoms = no_of_atoms[i];
   par->layers[i].periodic = 0;

   for(i_c = 1; i_c < 5; i_c ++)
     par->layers[i].a_lat[i_c] = par->b[i_c];

     par->layers[i].a_lat[0] = 0;
     par->layers[i].rel_area = par->rel_area_sup;

   for(i_c = 1; i_c <= 4; i_c ++)
     par->layers[i].vec_to_next[i_c] = vec[4 * i + i_c];

/************************************************************************
   vec_from_last points to the previous layer (i-1) except for the deepest
   layer (i_layer) where it points to the origin. 
*************************************************************************/
   if(i == 0)
   {
   par->layers[i].vec_from_last[1] = 0;  
   par->layers[i].vec_from_last[2] = 0;
   par->layers[i].vec_from_last[3] = orig;
   }
   else
   {
    for(i_c = 1; i_c <= 4; i_c ++)
       par->layers[i].vec_from_last[i_c] = vec[4*(i-1) + i_c];
   }
  
/************************************************************************* 
   Allocate structure element atoms in layer 
   and copy the appropriate entries from list atom_list into 
   par->layers[j].atoms
**************************************************************************/
   par->layers[i].atoms =
     (leed_atom *) malloc(par->layers[i].natoms * sizeof(leed_atom) );

   for( i_c = 0, i_atoms = 0; i_atoms < n_atoms ; i_atoms ++)
   {
     if(atom_list[i_atoms].layer == i)
     {
       par->layers[i].atoms[i_c].layer = i;
       par->layers[i].atoms[i_c].type = atom_list[i_atoms].type;
       
       par->layers[i].atoms[i_c].pos[1] = atom_list[i_atoms].pos[1];
       par->layers[i].atoms[i_c].pos[2] = atom_list[i_atoms].pos[2];
       par->layers[i].atoms[i_c].pos[3] = atom_list[i_atoms].pos[3];

       par->layers[i].atoms[i_c].dwf = atom_list[i_atoms].dwf;

       i_c ++;
     }
   } /* for i_atoms */
   if(i_c != par->layers[i].natoms)
   {
#ifdef ERROR
     fprintf(STDERR,
"*** error (leed_inp_overlayer_sym): the numbers of atoms in layer %d do not match\n", i);
#endif
#ifdef EXIT_ON_ERROR
       exit(1);
#else
       return(-1);
#endif
   }
 } /* for i (loop over layers) */

/***********************************************************************
 TEST THE SYMMETRY
************************************************************************/
fprintf(STDOUT,"(leed_inp_overlayer_sym):bulk_over = %d \n",(par->layers + 0)->bulk_over);
 if(n_rot > 1)
    n_rot = leed_check_rotation_sym(par);
 if(n_mir > 0)
    n_mir = leed_check_mirror_sym(par);
 
/**************************************************************************
  and now,
  the useful control of all variables.
**************************************************************************/
#ifdef CONTROL
fprintf(STDCTR,"ROT = %d   MIRRORPLANES %d\n",n_rot,n_mir);
for(i_c = 0 ; i_c < i_layer ; i_c++)
{
fprintf(STDCTR,"\n********************************\n");
fprintf(STDCTR,"overlayer%d:\n",i_c);
fprintf(STDCTR,"=============\n");
fprintf(STDCTR,"regshift (%.3f %.3f)\n",
                   par->layers[i_c].reg_shift[1]*BOHR,
                   par->layers[i_c].reg_shift[2]*BOHR);
fprintf(STDCTR,"vec_from_last (%.3f %.3f %.3f)\n",
                   par->layers[i_c].vec_from_last[1]*BOHR,
                   par->layers[i_c].vec_from_last[2]*BOHR,
                   par->layers[i_c].vec_from_last[3]*BOHR);
fprintf(STDCTR,"vec_to_next (%.3f %.3f %.3f)\n",
                   par->layers[i_c].vec_to_next[1]*BOHR,
                   par->layers[i_c].vec_to_next[2]*BOHR,
                   par->layers[i_c].vec_to_next[3]*BOHR);
fprintf(STDCTR,"periodic: %d \n",par->layers[i_c].periodic);
fprintf(STDCTR,"a_lat( %.3f %.3f %.3f %.3f)\n",
                   par->layers[i_c].a_lat[1]*BOHR,par->layers[i_c].a_lat[2]*BOHR,
                   par->layers[i_c].a_lat[3]*BOHR,par->layers[i_c].a_lat[4]*BOHR);
fprintf(STDCTR,"atom_position:\n");

  for(i = 0 ; i < par->layers[i_c].natoms ; i++)
        fprintf(STDCTR,"\t atom%d :(%.3f %.3f %.3f) type%d\n",i+1,
                             par->layers[i_c].atoms[i].pos[1]*BOHR,
                             par->layers[i_c].atoms[i].pos[2]*BOHR,
                             par->layers[i_c].atoms[i].pos[3]*BOHR,
                             par->layers[i_c].atoms[i].type);
fprintf(STDCTR,"********************************\n\n"); 
}
#endif

/***************************************************************************/

 free(shift);
 free(vec);
 free(no_of_atoms);


#ifdef EXIT
 fprintf(STDCTR,
  "(leed_inp_overlayer_sym): leaving inp_layer, return value = %d\n", i_layer);
 fprintf(STDERR, "********* (LEED): exit now\n");
 fprintf(STDCTR,"(leed_inp_overlayer_sym): at the end of program\n");
 exit(0);
#endif

 return(i_layer);
} /* end of function leed_inp_overlayer_sym */
/*======================================================================*/
