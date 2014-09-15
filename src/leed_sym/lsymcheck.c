/*********************************************************************
  WB/16.09.98
  file contains function:

  leed_check_rotation_sym
  leed_check_mirror_sym

Changes:
*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "leed.h"
#include "leed_def.h"

#ifndef EPSILON
#define EPSILON 0.001
#endif


/*======================================================================*/

int leed_check_rotation_sym(leed_cryst_t *p_cryst)

{
int i; 
int i_c,i_d;
int i_atoms,n_rot,i_rot;
int ctrol,nsize;

real a1x,a1y,a2x,a2y;    /* 2-dim lattice shifttors: 1=x, 2=y 0 is not used */
real det;                /* Determinante */
                          
real **R_n;              /* Rot.-matrix */
real *vecrot;            
real *integ;             /* in case of rot.sym., it must be an integer */
real *position;          /* intermediate storage for rotated atom positions */

real faux;
real vaux[2];
             

 if(p_cryst->layers[0].bulk_over == OVER)
 {
   a1x = p_cryst->b[1];
   a1y = p_cryst->b[3];
   a2x = p_cryst->b[2];
   a2y = p_cryst->b[4];
 
   nsize = 2*p_cryst->nlayers + 4;
   det = (a1x * a2y - a1y * a2x);
 }

 else
 {
   a1x = p_cryst->a[1];
   a1y = p_cryst->a[3];
   a2x = p_cryst->a[2];
   a2y = p_cryst->a[4];

   det = (a1x * a2y - a1y * a2x);
   nsize = 2*p_cryst->nlayers + 4;
 }
 
#ifdef CONTROL
 if(p_cryst->layers[0].bulk_over == OVER)
   fprintf(STDCTR,"(leed_check_rotation_sym) test the overlayers\n");
 else if(p_cryst->layers[0].bulk_over == BULK)
   fprintf(STDCTR,"(leed_check_rotation_sym) test the bulklayers\n");
 else
   fprintf(STDCTR,"(leed_check_rotation_sym)can not test rotationsymmetry,cause the nature of the layer is unknown!\n");
#endif

  /***** Allocate ************/

vecrot = (real *) malloc(nsize  * sizeof(real));
integ = (real *) malloc(nsize  * sizeof(real));
n_rot = p_cryst->n_rot;

  /*** calculate rot.matrix ***/
   R_n = leed_beam_get_rotation_matrices(n_rot);
   ctrol = 0;

#ifdef CONTROL_X
 for(i_rot = 1; i_rot < n_rot; i_rot ++)
 {
   fprintf(STDCTR,"%d: (%6.3f %6.3f)\n", i_rot, R_n[i_rot][1], R_n[i_rot][2]);
   fprintf(STDCTR,"    (%6.3f %6.3f)\n",        R_n[i_rot][3], R_n[i_rot][4]);
 }
#endif

  /************** TEST ROTATION ********************/

 for(i_rot = 1; i_rot < n_rot; i_rot ++)
 {
   if(ctrol == 0)
   {

    /*** rotate lattice vec ****/
     vecrot[0]= a1x * R_n[i_rot][1] + a1y * R_n[i_rot][2];
     vecrot[1]= a1x * R_n[i_rot][3] + a1y * R_n[i_rot][4]; 
     vecrot[2]= a2x * R_n[i_rot][1] + a2y * R_n[i_rot][2];
     vecrot[3]= a2x * R_n[i_rot][3] + a2y * R_n[i_rot][4];

   /*** rotate reg_shifts *****/
     for(i = 0 ;i < p_cryst->nlayers; i++)
     {
      vecrot[2*i+4] = p_cryst->layers[i].reg_shift[1] * R_n[i_rot][1]
                     +p_cryst->layers[i].reg_shift[2] * R_n[i_rot][2];
      vecrot[2*i+5] = p_cryst->layers[i].reg_shift[1] * R_n[i_rot][3]
                     +p_cryst->layers[i].reg_shift[2] * R_n[i_rot][4];
     }
   /*** build the difference ***/
     for(i = 0 ;i < p_cryst->nlayers; i++)
     {
      vecrot[2*i+4] -= p_cryst->layers[i].reg_shift[1];
      vecrot[2*i+5] -= p_cryst->layers[i].reg_shift[2];
     }

   /**** formula for ...  of lattice vec *************************/
     integ[0] = R_fabs((vecrot[1] * a1x - vecrot[0] * a1y)/det) ;
     integ[1] = R_fabs((vecrot[1] * a2x - vecrot[0] * a2y)/(-det));
     integ[2] = R_fabs((vecrot[3] * a1x - vecrot[2] * a1y)/det) ;
     integ[3] = R_fabs((vecrot[3] * a2x - vecrot[2] * a2y)/(-det));

     for(i = 0;i < p_cryst->nlayers;i++)
     {
      integ[4 + 2 * i] =
      R_fabs((vecrot[2*i+5] * a1x - vecrot[2*i+4] * a1y)/det) ;
      integ[5 + 2 * i] =
      R_fabs((vecrot[2*i+5] * a2x - vecrot[2*i+4] * a2y)/(-det));
     }

   /**** test for integer ****/
     for(i = 0;i < nsize ;i++)
     {
     faux = integ[i] - R_nint(integ[i]);
       if(R_fabs(faux) > EPSILON)
       {
       ctrol = 1;

#ifdef CONTROL
  if(i < 4)  
    fprintf(STDCTR,"(leed_check_rotation_sym): no rotsymmetrie of latticvec ,difn = %e\n",faux);
  else
  {
  if((i % 2) != 0)
    fprintf(STDCTR,"(leed_check_rotation_sym):layer%d have no rotationsymmetry,difn = %e\n"
                                           ,((i-5)/2),faux);
  else
    fprintf(STDCTR,"(leed_check_rotation_sym):layer%d have no rotationsymmetry,difn = %e\n"
                                           ,((i-4)/2),faux);  
  }/* end else */
#endif
   
       }/* if R_fabs ...*/
     }/*for i */
   
   /*** when all o.k. ,reset p_cryst->n_rot ***/
     if(ctrol != 0)
     {   
     p_cryst->n_rot = 1;
     n_rot = 1;
     }
     
   }/* if */
 }/* for */ 


/***********************************************************************
  test rot.symmetry for composite layer.
  therefor rotate all atoms in the layer
  check if you reach this position .......
************************************************************************/
/**************************************
 find the layer with max atoms,
 and allocate storage space
 nsize is the maximum space we need
**************************************/

 nsize = p_cryst->layers[0].natoms;
 for(i = 0 ;i < p_cryst->nlayers ; i++)
 {
   nsize = MAX(p_cryst->layers[i].natoms, nsize);
 }
 nsize = 3 * nsize + 1;

 /***** allocate *****/
 position = (real *) malloc(nsize * sizeof(real));


 for(i = 0 ;i < p_cryst->nlayers ; i++)
 {
  if(p_cryst->layers[i].natoms > 1 && ctrol == 0)
  {
   for(i_rot = 1 ; i_rot < n_rot ; i_rot++)
   {
     for(i_atoms = 0 ; i_atoms < p_cryst->layers[i].natoms ; i_atoms++)
     { 
     position[3*i_atoms] = p_cryst->layers[i].atoms[i_atoms].pos[1] * R_n[i_rot][1] +
                           p_cryst->layers[i].atoms[i_atoms].pos[2] * R_n[i_rot][2];
     position[3*i_atoms+1] = p_cryst->layers[i].atoms[i_atoms].pos[1] * R_n[i_rot][3] +
                             p_cryst->layers[i].atoms[i_atoms].pos[2] * R_n[i_rot][4];
     position[3*i_atoms+2] = p_cryst->layers[i].atoms[i_atoms].pos[3];
     }

   /**** formula for ...  of lattice vec *************************/
     for(i_c = 0 ; i_c < p_cryst->layers[i].natoms ; i_c++)
     {
       if(ctrol == 0)
       {
         for(i_d = 0 ; i_d < p_cryst->layers[i].natoms ; i_d++)
         { 
           if( IS_EQUAL_REAL(position[3*i_c+2], position[3*i_d+2]))
           {
           vaux[0] = position[3*i_c] - p_cryst->layers[i].atoms[i_d].pos[1]; 
           vaux[1] = position[3*i_c+1] - p_cryst->layers[i].atoms[i_d].pos[2];
           integ[0] = R_fabs((vaux[1] * a1x - vaux[0] * a1y)/det) ;
           integ[1] = R_fabs((vaux[1] * a2x - vaux[0] * a2y)/(-det));

           vaux[0] = integ[0] - R_nint(integ[0]);
           vaux[1] = integ[1] - R_nint(integ[1]);
             if(R_fabs(vaux[0]) > EPSILON || R_fabs(vaux[1]) > EPSILON)
             {
             ctrol++ ;
             }
           }
         }/** for i_d **/          
         if(ctrol < p_cryst->layers[i].natoms)
                ctrol = 0;
         else
           fprintf(STDCTR,"no rot.symmetry for atom%d in layer%d\n",i_c,i);   
       }/** if ctrol **/
     }/** for i_c **/
   }/** for i_rot **/  
  }
 }/**for i (layer)**/ 

/*** free rot.matrix ***/
  leed_beam_rotation_matrix_free(R_n);

/*** when all o.k. ,reset p_cryst->n_rot ***/
 if(ctrol == 0)
  p_cryst->n_rot = n_rot;
 else
  p_cryst->n_rot = 1;

/******WARNING if no rot.symmetry ************/
if(p_cryst->n_rot == 1)
  {
#ifdef WARNING
 fprintf(STDWAR,"(leed_check_rotation_sym):******WARNING NO ROTATIONSYMMETRIE\n");
#endif
  }

 free(vecrot);
 free(integ);
 free(position);

 return(p_cryst->n_rot);

} /* end of function leed_check_rotation_sym */
/*======================================================================*/

/**************************************************************************
 WB/17.09.98
 Function : leed_check_mirror_sym

 test the mirrorsymmetry
 INPUT: struct BULK or OVER
 OUTPUT: n_mir i.e. the number of mirrorplanes

**************************************************************************/

int leed_check_mirror_sym(leed_cryst_t *p_cryst)

{
int i;
int i_c,i_d;
int i_atoms,n_mir,i_mir;
int ctrol,nsize;

real a1x,a1y,a2x,a2y;    /* 2-dim lattice shifttors: 1=x, 2=y 0 is not used */
real det;                /* Determinante */

real R_m[5];             /* mirror -matrix*/
real *vecrot;
real *integ;             /* in case of rot.sym., it must be an integer */
/*real *vec;                intermediate storage for inter layer vectors */
real *position;          /* intermediate storage for reflectd atom positions */

real faux;
real vaux[2];

R_m[1] = R_m[2] = R_m[3] = R_m[4] = 1.0;


 if(p_cryst->layers[0].bulk_over == OVER)
 {
   a1x = p_cryst->b[1];
   a1y = p_cryst->b[3];
   a2x = p_cryst->b[2];
   a2y = p_cryst->b[4];

   nsize = 2*p_cryst->nlayers + 4;
   det = (a1x * a2y - a1y * a2x);
 }

 else
 {
   a1x = p_cryst->a[1];
   a1y = p_cryst->a[3];
   a2x = p_cryst->a[2];
   a2y = p_cryst->a[4];

   det = (a1x * a2y - a1y * a2x);
   nsize = 2*p_cryst->nlayers + 4;
 }

#ifdef CONTROL
 if((p_cryst->layers + 0)->bulk_over == OVER)
   fprintf(STDCTR,"(leed_check_mirror_sym) test the overlayers\n");
 else if((p_cryst->layers + 0)->bulk_over == BULK)
   fprintf(STDCTR,"(leed_check_mirror_sym) test the bulklayers\n");
 else
   fprintf(STDCTR,"(leed_check_mirror_sym)can not test mirrorsymmetry,cause the nature of the layer is unknown!\n");
#endif

  /***** Allocate ************/

vecrot = (real *) malloc(nsize  * sizeof(real));
integ = (real *) malloc(nsize  * sizeof(real));
n_mir = p_cryst->n_mir;
ctrol = 0;

  /************** TEST REFLECTION ******************/

 for(i_mir = 0; i_mir < n_mir; i_mir ++)
 {
   if(ctrol == 0)
   {
     R_m[1] = R_cos(2* p_cryst->alpha[i_mir]);
     R_m[2] = R_sin(2* p_cryst->alpha[i_mir]);
     R_m[3] = R_m[2];
     R_m[4] = - R_m[1];

#ifdef CONTROL_X
   fprintf(STDCTR,"%d: (%6.3f %6.3f)\n", i_mir, R_m[1], R_m[2]);
   fprintf(STDCTR,"    (%6.3f %6.3f)\n",        R_m[3], R_m[4]);
#endif  

    
    /*** reflect lattice vec ****/
     vecrot[0]= a1x * R_m[1] + a1y * R_m[2];
     vecrot[1]= a1x * R_m[3] + a1y * R_m[4];
     vecrot[2]= a2x * R_m[1] + a2y * R_m[2];
     vecrot[3]= a2x * R_m[3] + a2y * R_m[4];

   /*** reflect reg_shifts *****/
     for(i = 0 ;i < p_cryst->nlayers; i++)
     {
      vecrot[2*i+4] = p_cryst->layers[i].reg_shift[1] * R_m[1]
                     +p_cryst->layers[i].reg_shift[2] * R_m[2];
      vecrot[2*i+5] = p_cryst->layers[i].reg_shift[1] * R_m[3]
                     +p_cryst->layers[i].reg_shift[2] * R_m[4];
     }
   /*** build the difference ***/
     for(i = 0 ;i < p_cryst->nlayers; i++)
     {
      vecrot[2*i+4] -= p_cryst->layers[i].reg_shift[1];
      vecrot[2*i+5] -= p_cryst->layers[i].reg_shift[2];
     }

   /**** formula for ...  of lattice vec *************************/
     integ[0] = R_fabs((vecrot[1] * a1x - vecrot[0] * a1y)/det) ;
     integ[1] = R_fabs((vecrot[1] * a2x - vecrot[0] * a2y)/(-det));
     integ[2] = R_fabs((vecrot[3] * a1x - vecrot[2] * a1y)/det) ;
     integ[3] = R_fabs((vecrot[3] * a2x - vecrot[2] * a2y)/(-det));

     for(i = 0;i < p_cryst->nlayers;i++)
     {
      integ[4 + 2 * i] =
      R_fabs((vecrot[2*i+5] * a1x - vecrot[2*i+4] * a1y)/det) ;
      integ[5 + 2 * i] =
      R_fabs((vecrot[2*i+5] * a2x - vecrot[2*i+4] * a2y)/(-det));
     }

   /**** test for integer ****/
     for(i = 0;i < nsize ;i++)
     {
     faux = integ[i] - R_nint(integ[i]);
       if(R_fabs(faux) > EPSILON)
       {
       ctrol = 1;

#ifdef CONTROL
  if(i < 4)
    fprintf(STDCTR,"(leed_check_mirror_sym): no mirrorsymmetry for latticvec in case of mirrorplane%d alpha%f  ,difn = %e\n",i_mir+1,p_cryst->alpha[i_mir]*RAD_TO_DEG,faux);
  else
  {
  if((i % 2) != 0)
    fprintf(STDCTR,"(leed_check_mirror_sym):layer%d have no mirrorsymmetry for mirrorplane%d alpha%f  ,difn = %e\n",((i-5)/2),n_mir+1,p_cryst->alpha[i_mir]*RAD_TO_DEG,faux);
  else
    fprintf(STDCTR,"(leed_check_mirror_sym):layer%d have no mirrorsymmetry for mirrorplane%d alpha%f  ,difn = %e\n",((i-4)/2),n_mir+1,p_cryst->alpha[i_mir]*RAD_TO_DEG,faux);
  }/* end else */
#endif

       }/* if R_fabs ...*/
     }/*for i */

   /*** when something wrong ,reset p_cryst->n_mir and exit***/
     if(ctrol != 0)
     {
     p_cryst->n_mir = 0;
     n_mir = 0;
     exit(0);
     }

   }/* if */
 }/* for */

/***********************************************************************
  test mir.symmetry for composite layer.
  therefor reflect all atoms in the layer
  check if you reach this position .......
************************************************************************/
/**************************************
 find the layer with max atoms,
 and allocate storage space
 nsize is the maximum space we need
**************************************/

 nsize = p_cryst->layers[0].natoms;
 for(i = 0 ;i < p_cryst->nlayers ; i++)
 { 
   nsize = MAX(p_cryst->layers[i].natoms, nsize);
 }
 nsize = 3 * nsize + 1;

 /***** allocate *****/
 position = (real *) malloc(nsize * sizeof(real));

 for(i = 0 ;i < p_cryst->nlayers ; i++)
 {
  if(p_cryst->layers[i].natoms > 1 && ctrol == 0)
  {
   for(i_mir = 0 ; i_mir < n_mir ; i_mir++)
   {
     for(i_atoms = 0 ; i_atoms < p_cryst->layers[i].natoms ; i_atoms++)
     {
     position[3*i_atoms] = p_cryst->layers[i].atoms[i_atoms].pos[1] * R_m[1] +
                           p_cryst->layers[i].atoms[i_atoms].pos[2] * R_m[2];
     position[3*i_atoms+1] = p_cryst->layers[i].atoms[i_atoms].pos[1] * R_m[3] +
                             p_cryst->layers[i].atoms[i_atoms].pos[2] * R_m[4];
     position[3*i_atoms+2] = p_cryst->layers[i].atoms[i_atoms].pos[3];
     }

   /**** formula for ...  of lattice vec *************************/
     for(i_c = 0 ; i_c < p_cryst->layers[i].natoms ; i_c++)
     {
       if(ctrol == 0)
       {
         for(i_d = 0 ; i_d < p_cryst->layers[i].natoms ; i_d++)
         {
           if( IS_EQUAL_REAL(position[3*i_c+2], position[3*i_d+2]))
           {
           vaux[0] = position[3*i_c] - p_cryst->layers[i].atoms[i_d].pos[1];
           vaux[1] = position[3*i_c+1] - p_cryst->layers[i].atoms[i_d].pos[2];
           integ[0] = R_fabs((vaux[1] * a1x - vaux[0] * a1y)/det) ;
           integ[1] = R_fabs((vaux[1] * a2x - vaux[0] * a2y)/(-det));

           vaux[0] = integ[0] - R_nint(integ[0]);
           vaux[1] = integ[1] - R_nint(integ[1]);
             if(R_fabs(vaux[0]) > EPSILON || R_fabs(vaux[1]) > EPSILON)
             {

             ctrol++ ;
             }
           }
         }/** for i_d **/
         if(ctrol < p_cryst->layers[i].natoms)
                ctrol = 0;
         else
           fprintf(STDCTR,"(leed_check_mirror_sym)no mirrorsymmetry to mirrorplane%d(alpha %f) for atom%d in layer%d\n",i_mir,p_cryst->alpha[i_mir],i_c,i);
       }/** if ctrol **/
     }/** for i_c **/
   }/** for i_mir **/
  }
 }/**for i (layer)**/


/*** when all o.k. ,reset p_cryst->n_rot ***/
 if(ctrol == 0)
  p_cryst->n_mir = n_mir;
 else
  p_cryst->n_mir = 0;

/******WARNING if no rot.symmetry ************/
if(p_cryst->n_mir == 0)
  {
#ifdef WARNING
 fprintf(STDWAR,"(leed_check_mirror_sym):******WARNING NO MIRRORSYMMETRY\n");
 exit(0);
#endif
  }

 free(vecrot);
 free(integ);
 free(position);

 return(p_cryst->n_mir);

} /* end of function leed_check_mirror_sym */
/*======================================================================*/

