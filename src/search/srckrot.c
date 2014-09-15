/***********************************************************************
 GH/02.10.95

 file contains function:

  int sr_ckrot(struct sratom_str * atoms, 
               struct search_str * search )

 Check rotation symmetry

 Changes:

GH/21.08.95 - Creation (copy from srmkinp.c)
GH/27.09.95 - More comments

***********************************************************************/

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "search.h"

#define ERROR

#undef  GEO_TOLERANCE
#define GEO_TOLERANCE  0.001

int sr_ckrot(struct sratom_str *atoms, 
             struct search_str *search )

/***********************************************************************
 Check for rotation symmetry
 
INPUT:
 struct sratom_str * atoms - (input, output) information about initial
             atom positions, types, etc.; the function will modify the
             following structure elements:
                  *x/y/z_par (conversion between parameter list and
                              atomic coordinates)
                  ref, n_ref (references to symmetry equivalent atoms).

             The following structure elements (for each atom) must be
             assigned before entering the function:
                  x, y, z (initial atom positions)
                  type    (atom type)

 struct search_str * search - (input, output) information about 
             symmetries, etc.; the function will update the structure
             element:
                  n_par (number of parameters)
             input is needed from the following structure elements:
                  b_lat     (lattice vectors)
                  z_only    (0 for xyz search, 1 for z search only)
                  rot_deg   (degree of rot. symmetry)
                  rot_axis  (x,y of rotational axis)
                  mir_point (x,y of point in mirror plane)
                  mir_dir   (x,y of direction of mirror plane)

DESIGN:
 
 Two atoms at positions xi and xj are symmetry equivalent, if 
 
  xi = R_phi * (xj - axis) + axis + b1*n1 + b2*n2

 where 
 b1/2 are the overlayer lattice vectors and n1/2 integer numbers,
 axis is the position of the rotation axis,
 R_phi is the rotation matrix for phi = n*2pi/n_rot.
  ( 1: cos(phi)  2: -sin(phi) )
  ( 3: sin(phi)  4:  cos(phi) )

 With B = (b1,b2) it follows:
 
 B^(-1)*(R_phi -1)*axis + B^(-1)*(xj - R_phi*xi) = integer vector.

 This condition is tested in this function for all pairs of atoms that
 could qualify to be symmetry equivalent.
 
 
RETURN VALUE:

 number of parameters (n_par).

***********************************************************************/

{

int i_atoms, j_atoms, n_atoms;
int i_rot;
int i_par, n_par;

real R_1[5], R_2[5], R_3[5], R_4[5], R_5[5];
real B_1[5], BR[5];
real *R_n[7];                    /* max degree of rot. symmetry is 6 */

real axis[3];
real xi, yi, xj, yj;
real faux;

/***********************************************************************
 Find number of atoms in list atoms
***********************************************************************/

 for(n_atoms = 0; (atoms + n_atoms)->type != I_END_OF_LIST; n_atoms ++)
 { ; }

 /***********************************************************************
  Symmetry:
*************************************************************************/

 if(search->rot_deg <= 1)
 {
   for(i_atoms = 0; i_atoms != n_atoms; i_atoms ++)
   {
     if( (atoms + i_atoms)->ref == I_END_OF_LIST )
     {
/* First and only occurence of atom i => reference to itself */
       (atoms + i_atoms)->ref  = i_atoms;
       (atoms + i_atoms)->nref = 1;
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

/* 
   Create rot. matries R_n 
   R_n = ( 1: cos(n*phi)  2: -sin(n*phi) )
         ( 3: sin(n*phi)  4:  cos(n*phi) )
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

/* 
   Scan through all rotations and atoms and 
   find symmetry related atoms
*/

   for(i_atoms = 0; i_atoms < n_atoms; i_atoms ++)
   {
     if( (atoms + i_atoms)->ref == I_END_OF_LIST )
     {
/* First occurence of atom i => reference to itself */

#ifdef CONTROL_X
   fprintf(STDCTR,"\n(sr_ckrot): atom: %d ->\trot No n1 n2", i_atoms);
#endif

       (atoms + i_atoms)->ref  = i_atoms;
       (atoms + i_atoms)->nref = 1;
  
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

/* First check if atom i is mapped to itself by rotation (j_atoms = i_atoms) */
         
         faux = B_1[1]*xi + B_1[2]*yi - (BR[1]*xi + BR[2]*yi + axis[1]);
         faux -= R_nint(faux);
         if( fabs(faux) < GEO_TOLERANCE)
         {
           faux = B_1[3]*xi + B_1[4]*yi - (BR[3]*xi + BR[4]*yi + axis[2]);
           faux -= R_nint(faux);
           if( fabs(faux) < GEO_TOLERANCE)
           {
             (atoms + i_atoms)->nref = 0;
           } /* atoms i is symmetry equivalent to itself */
         } /* if fabs ... */

         for(j_atoms = i_atoms+1; j_atoms < n_atoms; j_atoms ++)
         {
           if( ( (atoms + j_atoms)->ref == I_END_OF_LIST ) &&
               ( fabs( (atoms + i_atoms)->z - (atoms + j_atoms)->z )
                 < GEO_TOLERANCE) &&
               ( strcmp((atoms + i_atoms)->name, (atoms + j_atoms)->name)
                 == 0)  )
           {
          
             xj = (atoms + j_atoms)->x;
             yj = (atoms + j_atoms)->y;

             faux = B_1[1]*xj + B_1[2]*yj - (BR[1]*xi + BR[2]*yi + axis[1]);
#ifdef CONTROL_X
             fprintf(STDCTR,"\n\t\t\t %d %d %.3f, ", i_rot, j_atoms, faux);
#endif
             faux -= R_nint(faux);
             if( fabs(faux) < GEO_TOLERANCE)
             {
               faux = B_1[3]*xj + B_1[4]*yj - (BR[3]*xi + BR[4]*yi + axis[2]);
#ifdef CONTROL_X
             fprintf(STDCTR," %.3f ",  faux);
#endif
               faux -= R_nint(faux);
               if( fabs(faux) < GEO_TOLERANCE)
               {
         /*
            Copy reference of atom i into j 
            increment nref(i) (number of atoms which are equivalent to i)
            set nref(j) to i_rot;
         */
                 (atoms + j_atoms)->ref = (atoms + i_atoms)->ref;
                 (atoms + j_atoms)->nref = i_rot;
                 (atoms + i_atoms)->nref ++;
               } /* atoms i and j are symmetry equivalent */
             } /* if fabs ... */
           } /* if ref ... */
         } /* for j_atoms */
       } /* for i_rot */
     } /* if (atoms + i_atoms)->ref == I_END_OF_LIST */
   } /* for i_atoms */
 }  /* search->rot_deg > 1 */

/************************************************************************
  Count search parameters and allocate memory for xyz_par
  Check assignment of atoms
*************************************************************************/

for(i_atoms = 0, n_par = 0; i_atoms != n_atoms; i_atoms ++)
 {
   if( (atoms + i_atoms)->ref == i_atoms )
   {
     if( (atoms + i_atoms)->nref == 0 )
       n_par += 1;
     else if((atoms + i_atoms)->nref == search->rot_deg )
       n_par += ((search->z_only)? 1 : 3);
     else
     {
#ifdef ERROR
       fprintf(STDERR,
"*** error (sr_ckrot): number of atoms does not match with symmetry (%d)\n",
               i_atoms);
#endif
       exit(1);
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

#ifdef CONTROL_X
   fprintf(STDCTR,"(sr_ckrot): No of parameters in search: %d\n", n_par);
#endif

/************************************************************************
  Identify search parameters
*************************************************************************/

for(i_atoms = 0, i_par = 1; i_atoms != n_atoms; i_atoms ++)
 {
   if( (atoms + i_atoms)->ref == i_atoms )
   {

/* 
   x/y variation if atom is not equivalent to itself
   and xy search required
*/
     if( ((atoms + i_atoms)->nref == search->rot_deg) && (!search->z_only) )
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
           i_rot = (atoms + j_atoms)->nref;
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

#ifdef CONTROL_X
   fprintf(STDCTR,"(sr_ckrot): %d parameters, atoms:\n", n_par);
 for(i_atoms = 0; i_atoms != n_atoms; i_atoms ++)
 {
   fprintf(STDCTR,"\n%d \"%s\" (%6.3f, %6.3f, %6.3f) ref: %d nref: %d",
           i_atoms, (atoms + i_atoms)->name,
           (atoms + i_atoms)->x, (atoms + i_atoms)->y, (atoms + i_atoms)->z,
           (atoms + i_atoms)->ref, (atoms + i_atoms)->nref);
   fprintf(STDCTR,"\n\tr_min: %6.3f", (atoms + i_atoms)->r_min);

   if(!search->z_only)
   {
     fprintf(STDCTR,"\n\tx_par: ");
     for(i_par = 1; i_par <= n_par; i_par ++)
     { fprintf(STDCTR,"%6.3f, ", (atoms+i_atoms)->x_par[i_par]); }
     fprintf(STDCTR,"\n\ty_par: ");
     for(i_par = 1; i_par <= n_par; i_par ++)
     { fprintf(STDCTR,"%6.3f, ", (atoms+i_atoms)->y_par[i_par]); }
   }
   fprintf(STDCTR,"\n\tz_par: ");
   for(i_par = 1; i_par <= n_par; i_par ++)
   { fprintf(STDCTR,"%6.3f, ", (atoms+i_atoms)->z_par[i_par]); }
   fprintf(STDCTR,"\n");
 }
#endif

/************************************************************************
  return number of search parameters
*************************************************************************/

 search->n_par = n_par;
 return(n_par);
}
