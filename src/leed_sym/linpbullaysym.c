/*********************************************************************




















  WB/06.10.00 
  file contains function:
  (copy from inpbullayer.c additional test symmetry and create the 
                           registry shifts)





  leed_inp_bul_layer_sym

Changes:
                              
  WB/13.01.99   change the sign of the origin (orig)
                to set the last interlayer vector.
*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>

#include <strings.h>

#include <stdlib.h>
#include "leed.h"
#include "leed_def.h"

#ifndef MIN_DIST             /* should be defined in "leed_def.h" */
#define MIN_DIST 2.0
#endif

/*!
 * Decides which atoms belong to which layer (based on symmetry):
 * - group atoms to composite layers if necessary
 * - determine inter-layer vectors
 *
 * \param[in,out] par Pointer to structure that contains all geometrical
 * and non-geometrical parameters of the bulk except atom positions and types.
 * Output will be written to structure element layers.
 * \param[in,out] atom_list Pointer to structure that contains all
 * geometrical and non-geometrical parameters of the atoms to be
 * grouped (see "leed_def.h"). The list of atoms must be ordered according to
 * their z coordinate, with LARGEST z first. It will be modified during
 * the execution of this function.
 * \param[in] a3 Pointer to third basis vector of the bulk unit cell.
 * \return Number of layers.
 * \retval -1 if unsuccessful and #EXIT_ON_ERROR is not defined.

 * \warning The function cannot handle the case when all layer distances
 * are smaller than #MIN_DIST. In this case the bulk must be modelled
 * as one composite layer.
 */
int leed_inp_bul_layer_sym(leed_crystal *par, leed_atom * atom_list,
                           const real *a3)
{
int i, j;
int i_c, i_d;




int n_rot, n_mir;
int n_atoms, i_atoms;
int i_layer;
int end_per;             /* indicates end of vertical periodicity */

real a1x,a1y,a2x,a2y;    /* 2-dim lattice shift to rs: 1=x, 2=y 0 is not used */


real orig;              
                          
real *vec;               /* intermediate storage for inter layer vectors */
real *shift;             /* intermediate storage for registry-shifts */
int *no_of_atoms;        /* intermediate storage for number of atoms in layer */




real faux;
real vaux[4];




#ifdef CONTROL
 fprintf(STDCTR,"(leed_inp_bul_layer_sym): entering leed_inp_bul_layer_sym MIN_DIST= %.3f\n",
         MIN_DIST*BOHR);
#endif
/************************************************************************
  predefine some often used variables
*************************************************************************/


 n_atoms = par->natoms;
 n_mir = par->n_mir;
 n_rot = par->n_rot;







 a1x = par->a[1];
 a1y = par->a[3];
 a2x = par->a[2];
 a2y = par->a[4];

#ifdef CONTROL
 fprintf(STDCTR,
 "(leed_inp_bul_layer_sym): n_atoms = %d, a1 = (%.3f,%.3f), a2 = (%.3f,%.3f)\n",

         n_atoms, a1x*BOHR, a1y*BOHR, a2x*BOHR, a2y*BOHR);
#endif


/* 
  Allocate memory for intermediate storage vectors vec, shift and no_of_atoms:
  max. number of layers = number of atoms 
*/
   vec = (real *) calloc( (4*(n_atoms) + 1), sizeof(real) ); 
 shift = (real *) calloc( (2*(n_atoms) + 1), sizeof(real) );
 no_of_atoms = (int *) calloc(n_atoms, sizeof(int) );
 
/************************************************************************
  i_layer indicates the layer which an atom belongs to; it will eventually 
          be the total number of layers.


  vaux  is used to store the z-position of the topmost atom in the layer.
*************************************************************************/


 i_layer = 0;
 no_of_atoms[i_layer] = 1;
 atom_list[0].layer = i_layer;





 orig = atom_list[0].pos[3];

 vaux[3] = atom_list[0].pos[3];
 atom_list[0].pos[3] = 0.;
 
#ifdef CONTROL
  fprintf(STDCTR,"(leed_inp_bul_layer_sym): old atomlist, atom%d (%f %f %f (now %f))\n",0,





          atom_list[0].pos[1]*BOHR,atom_list[0].pos[2]*BOHR,vaux[3]*BOHR,atom_list[0].pos[3]*BOHR);
 for(i_atoms = 1;i_atoms < n_atoms; i_atoms++)

  fprintf(STDCTR,"(leed_inp_bul_layer_sym): old atomlist, atom%d (%f %f %f)\n",
         i_atoms, atom_list[i_atoms].pos[1]*BOHR,atom_list[i_atoms].pos[2]*BOHR,atom_list[i_atoms].pos[3]*BOHR);




#endif











 for(i_atoms=1; i_atoms<n_atoms; i_atoms++)
 {








#ifdef CONTROL
   fprintf(STDCTR,"(leed_inp_bul_layer_sym):for i_atom = %d \n",i_atoms);
   fprintf(STDCTR,"(leed_inp_bul_layer_sym): pos: %.4f %.4f %.4f dist: %.4f\n",
   atom_list[i_atoms].pos[1]*BOHR, atom_list[i_atoms].pos[2]*BOHR, 
   atom_list[i_atoms].pos[3]*BOHR,

   R_fabs(atom_list[(i_atoms - 1)].pos[3]+vaux[3]-atom_list[i_atoms].pos[3])*BOHR);
#endif

  if( R_fabs(atom_list[i_atoms-1].pos[3]+vaux[3] - atom_list[i_atoms].pos[3]) 
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
     fprintf(STDCTR,"(leed_inp_bul_layer_sym): new layer, no_of_atoms[%d] = %d\n",
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
    atom_list[i_atoms].pos[3] - atom_list[(i_atoms-1)].pos[3] - vaux[3];
     
   }
   else
   {
    *(shift + 2*i_layer + 0) = -(atom_list[i_atoms -1].pos[1]);
    *(shift + 2*i_layer + 1) = -(atom_list[i_atoms -1].pos[2]);

/* set up new inter layer vector  */
    *(vec + 4*i_layer + 1) = 0.;
    *(vec + 4*i_layer + 2) = 0.;
    *(vec + 4*i_layer + 3) =
        atom_list[i_atoms].pos[3] - atom_list[i_atoms-1].pos[3] - vaux[3];





   }

     i_layer ++;
     no_of_atoms[i_layer] = 0;
     vaux[3] = atom_list[i_atoms].pos[3];
     atom_list[i_atoms].pos[3] = 0.;





     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
  }  /* if R_fabs ... */


  /***********************************************************************
   set up new atom position.
   in case of composite layer keep track of the new origin.
  ***********************************************************************/

   no_of_atoms[i_layer]++;
   atom_list[i_atoms].layer = i_layer;
   if(no_of_atoms[i_layer] > 1)
   { 
     atom_list[i_atoms].pos[3] -= vaux[3];
   }
 }  /* for i_atoms (loop through atoms) */



/************************************************************************
  BULK:
  Set up the last inter layer vector to point from the last layer to the
  first one of the next unit cell.
*************************************************************************/










 if(no_of_atoms[i_layer] > 1)
 {
/******************************************************************************
 now we set the shift to the rotational axis (must be zero for composite layer)
 and the vector showing from one layer to another.
 for the last point it is important that this vector is showing from the topmost
 atom of the one composite layer to the bottom-most in the next composite layer
 (the coordinate of the last layer is stored in vaux[3])
********************************************************************************/        
   *(shift + 2*i_layer + 0) = 0;
   *(shift + 2*i_layer + 1) = 0;





   *(vec+4*i_layer + 1) = 0.;
   *(vec+4*i_layer + 2) = 0.;
   *(vec+4*i_layer + 3) = a3[3] - atom_list[n_atoms-1].pos[3] - vaux[3] + orig;



 }
 else
 {
/**********************************************************************************
 set the shift to the rotational layer for a bravais layer   
 and set the interlayer vectors (showing to the next layer)
 vaux = the coordinate of to topmost atom in the layer above
 orig = the z-shift of the top most bulk atom to zero
***********************************************************************************/
   *(shift + 2*i_layer + 0) = -atom_list[n_atoms -1].pos[1];
   *(shift + 2*i_layer + 1) = -atom_list[n_atoms -1].pos[2];







   *(vec+4*i_layer + 1) = 0.;
   *(vec+4*i_layer + 2) = 0.;
   *(vec+4*i_layer + 3) = a3[3] - vaux[3] - atom_list[n_atoms-1].pos[3] + orig;
 }

/************************************************************************
 TEST the DISTANCE between the last atom of unit cell and
 the first atom of the next unit cell
*************************************************************************/
 if( R_fabs( *(vec + 4*i_layer + 3) ) < MIN_DIST )
 {
   if(i_layer == 0)
   {
#ifdef ERROR
     fprintf(STDERR," *** error (leed_inp_bul_layer_sym): bulk atoms are too close ");
     fprintf(STDERR," (cannot be split into layers).\n");
#endif
#ifdef EXIT_ON_ERROR
     exit(1);
#else
     return(-1);
#endif


   }

   else /* i_layer != 0 ,but R_abs too small */
   {
#ifdef CONTROL
     fprintf(STDCTR,"(leed_inp_bul_layer_sym): inter layer distance is too short\n");
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

       atom_list = ( leed_atom *) realloc( 
                 atom_list, (n_atoms+j+2) * sizeof(leed_atom) );

   /*******************************************************
    Reset atom_list:
     - add the atoms of the first layer to the last one
     - choose the coordinates relative to the origin in
       the last layer 
   *******************************************************/ 
     for (j=0 ; atom_list[j].layer == 0; j++)
     {
       atom_list[n_atoms + j].layer = i_layer;
       atom_list[n_atoms + j].type = atom_list[j].type;




       for(i_c=1; i_c<=3; i_c++)
       {
         atom_list[n_atoms + j].pos[i_c] = 
         atom_list[j].pos[i_c] + *(vec+4*i_layer + i_c);
       }
       no_of_atoms[i_layer] ++;
     } /* for j */


    /*********************************************************************
      The inter layer vector now points from the z coordinate of the frist
      atom in the last layer (vaux) to the z coordinate of the first atom
      in the first layer translated by a3.
      So you have to add the inter layer vector of the first layer.
    **********************************************************************/

     *(vec + 4*i_layer + 3) += *(vec + 3); /** atom_list[n_atoms -1].pos[3] **/

    /*********************************************
      Reset registry-shift:
      In this case the layer is a composite layer,
      wherefor the shift becomes zero. 
    **********************************************/ 
     *(shift + 2*i_layer + 0) = 0; 
     *(shift + 2*i_layer + 1) = 0; 




   /*** update n_atoms and start of periodic stacking ***/
     n_atoms += j;
     end_per = 1;




   } /* else (i_layer != 0) */
 } /* if < MIN_DIST */

 else /** means that all distances are o.k. **/
 {
   /*** update start of periodic stacking ***/
   end_per = 0;

 }  /* else (> MIN_DIST) */

/****** update several parameters ******/
 par->natoms = n_atoms;
 atom_list[n_atoms].type = I_END_OF_LIST;

 i_layer ++;
 par->nlayers = i_layer;


#ifdef CONTROL_X
 for(i=0; i< i_layer; i++)
   fprintf(STDCTR,"(leed_inp_bul_layer_sym): shift_orig: %.4f %.4f \n",
                 *(shift+2*i+0) *BOHR,
                 *(shift+2*i+1) *BOHR);
 fprintf(STDCTR,"(leed_inp_bul_layer_sym):i_layer = %d\n", i_layer);
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
     if( ( SQUARE(vaux[0] + i_c*a1x + i_d*a2x) +
           SQUARE(vaux[1] + i_c*a1y + i_d*a2y) ) < faux )
     {
       *(shift+2*i+0) = vaux[0] + i_c*a1x + i_d*a2x;
       *(shift+2*i+1) = vaux[1] + i_c*a1y + i_d*a2y;
     }
   } 
 } 
 
#ifdef CONTROL_X
 for(i=0; i< i_layer; i++)
   fprintf(STDCTR,"(leed_inp_bul_layer_sym): shift_mod: %.4f %.4f \n",
                 *(shift+2*i+0) *BOHR,
                 *(shift+2*i+1) *BOHR);
 fprintf(STDCTR,"(leed_inp_bul_layer_sym):i_layer = %d\n", i_layer);
#endif












/************************************************************************
  Allocate array "layers" and copy all relevant information from
  atom_list, shift and no_of_atoms.
  The z order of layers will be the reverse of the order of atom_list
  (smallest z, i.e. deepest layer, first)
*************************************************************************/


 
 par->layers =
       (leed_layer *) malloc( (i_layer+1) * sizeof(leed_layer) );


















 for(i = 0 ; i < i_layer ; i++)
 {
   j = i_layer - i - 1; 
   par->layers[j].no_of_layer = j; 
   par->layers[j].bulk_over = BULK;
   par->layers[j].reg_shift[1] = *(shift+2*i+0); 
   par->layers[j].reg_shift[2] = *(shift+2*i+1);
 }




 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 for(i=0; i< i_layer; i++)


 {
/********************************************************************* 
   i refers to index in shift, no_of_atoms, and atom_list
   j refers to index of layers

*********************************************************************/
   j = i_layer - i - 1;
/********************************************************************* 
   Write to par->layers[j]:
   - number of atoms (from no_of_atoms), 
   - periodicity indicator 
   - lattice vectors (from par->a)
   - interlayer vectors 
*********************************************************************/
   par->layers[j].natoms = no_of_atoms[i];


   if( (i == 0) && (end_per == 1) )
       par->layers[j].periodic = 0;
   else
       par->layers[j].periodic = 1;



   for(i_c = 1; i_c < 5; i_c ++)
     par->layers[j].a_lat[i_c] = par->a[i_c];



     par->layers[j].a_lat[0] = 0;


/* for bulk layers 1x1 periodicity is assumed */
     par->layers[j].rel_area = 1.;

   for(i_c = 1; i_c <= 4; i_c ++)

     par->layers[j].vec_from_last[i_c] =  - vec[4 * i + i_c];


/************************************************************************
   vec_to_next points to the next layer (j+1) except for the topmost
   layer (i_layer) where it points to the origin of the coordinate 



   system (the new origin is the rot.axis ,set to 0/0/0).
*************************************************************************/
   if(i == 0)
   {
     par->layers[j].vec_to_next[1] = 0;  
     par->layers[j].vec_to_next[2] = 0;
     par->layers[j].vec_to_next[3] = - orig;
   }
   else
   {
     for(i_c = 1; i_c <= 4; i_c ++)

       par->layers[j].vec_to_next[i_c] =  - vec[4*(i-1) + i_c];



   }






  
/************************************************************************* 
   Allocate structure element atoms in layer 
   and copy the appropriate entries from list atom_list into 
   par->layers[j].atoms

**************************************************************************/
   par->layers[j].atoms =
     (leed_atom *) malloc(no_of_atoms[i] * sizeof(leed_atom) );

   for( i_c = 0, i_atoms = 0; i_atoms < n_atoms; i_atoms ++)

   {
     if(atom_list[i_atoms].layer == i)

     {
       par->layers[j].atoms[i_c].layer = j;
       par->layers[j].atoms[i_c].type = atom_list[i_atoms].type;
       
       par->layers[j].atoms[i_c].pos[1] = atom_list[i_atoms].pos[1];
       par->layers[j].atoms[i_c].pos[2] = atom_list[i_atoms].pos[2];
       par->layers[j].atoms[i_c].pos[3] = atom_list[i_atoms].pos[3];


       par->layers[j].atoms[i_c].dwf = atom_list[i_atoms].dwf;


       i_c ++;
     }
   } /* for i_atoms */
   if(i_c != par->layers[j].natoms)
   {
#ifdef ERROR
     fprintf(STDERR,
"*** error (leed_inp_bul_layer_sym): the numbers of atoms in layer %d do not match\n", j);
#endif
#ifdef EXIT_ON_ERROR
       exit(1);
#else
       return(-1);
#endif
   }
 } /* for i (loop over layers) */





/***************************************************************************/






 free(shift);
 free(vec);
 free(no_of_atoms);
/***********************************************************************
  test rot.symmetry for composite layer.
  therefor rotate all atoms in the layer
  check if you reach this position .......
************************************************************************/
 if(n_rot > 1)
    n_rot = leed_check_rotation_sym(par);
 if(n_mir > 0)
    n_mir = leed_check_mirror_sym(par);












/**************************************************************************
  and now,
  the useful control of all variables.
**************************************************************************/
#ifdef CONTROL
fprintf(STDCTR,"ROT = %d  MIRRORPLANES %d \n",n_rot,n_mir);
for(i_c = 0 ; i_c < i_layer ; i_c++)

{
  fprintf(STDCTR,"\n*****************************************\n");
  fprintf(STDCTR,"bulklayer%d:\n",i_c);
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
  fprintf(STDCTR,"atom_position: \n");





  for(i = 0 ; i < par->layers[i_c].natoms ; i++)

        fprintf(STDCTR,"\t atom%d :(%.3f %.3f %.3f) type%d \n\n",i+1,
                                 par->layers[i_c].atoms[i].pos[1]*BOHR,
                                 par->layers[i_c].atoms[i].pos[2]*BOHR,
                                 par->layers[i_c].atoms[i].pos[3]*BOHR,
                                 par->layers[i_c].atoms[i].type);



  fprintf(STDCTR,"*****************************************\n");    
}
#endif






#ifdef EXIT
 fprintf(STDCTR,
  "(leed_inp_bul_layer_sym): leaving inp_layer, return value = %d\n", i_layer);
 fprintf(STDERR, "********* (LEED): exit now\n");
 fprintf(STDCTR,"(leed_inp_bul_layer_sym): at the end of program");
 exit(0);
#endif

 return(i_layer);
} /* end of function leed_inp_bul_layer_sym */
