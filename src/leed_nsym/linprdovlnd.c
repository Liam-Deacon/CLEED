/*********************************************************************
GH/29.09.00 
  file contains function:

  leed_read_overlayer
 
Changes:

GH/25.07.95 - Creation (copy from rd_bulpar).
GH/24.08.95 - Accept also input of Vr (vr).
GH/30.08.97 - Fix bug in finding dmin.
WB/05.05.98 - read rotaxis
WB/19.08.98 - atoms_rd realloc i_atoms+2
GH/03.05.00 - read parameters for non-diagonal t matrix
            - fix bug in Debye waller factor (dmt): 0.0625
GH/29.09.00 - calculate dr2 for dmt input in function leed_inp_debye_temp

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>

#include "stddef.h"
#include "leed.h"
#include "leed_def.h"


#ifndef GEO_TOLERANCE           /* should be defined in "leed_def.h" */
#define GEO_TOLERANCE 0.0001
#endif

#define SQRT3   1.73205080756887729352   /* sqrt(3) */

/********************************************************************/

int leed_read_overlayer_nd (leed_cryst_t **p_over_par, 
                  leed_phs_t **p_phs_shifts,
                  leed_cryst_t *bulk_par,
                  char *filename)
/*********************************************************************
  Read all the overlayer parameters that do change during a search

  DESIGN

  Letters 'e' - 'l' are reserved as identifiers for parameter input 
  through function leed_inp_leed_read_par

  currently recognized identifier:
  
  'c': comment
  'po': postion and type of overlayer atoms.

  The atoms are in order of increasing z before they enter leed_inp_overlayer.

  FUNCTION CALLS

   - leed_leed_leed_leed_inp_phase_nd
   - leed_inp_overlayer

  RETURN VALUES

    1 if ok.
   -1 if failed (and EXIT_ON_ERROR is not defined)

*********************************************************************/
{
FILE *inp_stream;

/* input buffers */
char linebuffer[STRSZ];  
char phaseinp[STRSZ];
char whatnext[STRSZ];

int i,j, iaux;                /* counter, dummy  variables */
int i_c, i_str;
int i_com;
int i_atoms;
int i_layer;

real faux;                    /* dummy variable */
real vaux[4];                 /* dummy vector */

leed_cryst_t *over_par;   /* use *over_par instead of the pointer 
                                 p_over_par */

leed_atom_t atom_aux;     /* used for sorting atoms */
leed_atom_t *atoms_rd;    /* this vector of structure atom_str is
                                 used to read and treat the input atomic
                                 properties and will be copied into over_par
                                 afterwards */

/********************************************************************
  If p_over_par is NULL: allocate memory
  Copy contents of bulk_par into p_over_par.
  
*********************************************************************/
 over_par = NULL;             /* initialise */
 if (*p_over_par == NULL) 
 {
   over_par = *p_over_par = 
   (leed_cryst_t *)malloc( sizeof(leed_cryst_t) );
    memcpy(over_par, bulk_par, sizeof(leed_cryst_t) );
 } 
/********************************************************************
  Preset parameters
  - allocate atoms_rd (1 unit)
  - set layers to NULL for now.
  - allocate over_par->comments (1 unit)
  - set temperature (bulk_par->temp) to room temperature (300K);
********************************************************************/

 over_par->layers = NULL;

 atoms_rd = (leed_atom_t *)malloc(2 * sizeof(leed_atom_t));
 i_atoms = 0;

 over_par->comments = (char * *)malloc( sizeof(char *) );
 *(over_par->comments) = NULL;
 i_com = 0;

 over_par->temp = DEF_TEMP;
 over_par->n_rot = bulk_par->n_rot;
 over_par->rot_axis[1] = bulk_par->rot_axis[1];
 over_par->rot_axis[2] = bulk_par->rot_axis[2];

/********************************************************************
  START INPUT
  Open and Read input file
********************************************************************/
 if( (inp_stream = fopen(filename, "r")) == NULL) 
 {
#ifdef ERROR
   fprintf(STDERR,
  " *** error (leed_read_overlayer): could not open file \"%s\"\n",filename);
#endif
#ifdef EXIT_ON_ERROR
  exit(1);
#else
  return(-1);
#endif
 } 

#ifdef CONTROL
 fprintf(STDCTR,"(leed_read_overlayer): Reading file \"%s\"\n",filename);
#endif
 while ( fgets(linebuffer, STRSZ, inp_stream) != NULL) 
 {
#ifdef CONTROL_X
   fprintf(STDCTR,"(leed_read_overlayer): %s", linebuffer);
#endif
   /* find first non blank character */
   for( i_str = 0;  *(linebuffer+i_str) == ' '; i_str ++);
   switch( *(linebuffer+i_str) )
   {
     case ('c'): case ('C'):
   /*********************************** 
     input of comments to be stored
   ***********************************/
     {
       over_par->comments = ( char * * ) realloc( 
                           over_par->comments, (i_com+2) * sizeof(char *) ); 

       *(over_par->comments + i_com) = (char *)calloc(
            strlen(filename) + strlen(linebuffer) + 2 - i_str, 
            sizeof(char));
       *(over_par->comments + i_com+1) = NULL;

       sprintf(*(over_par->comments + i_com), "(%s): %s",
               filename, linebuffer+i_str+2);
       
       i_com ++;
       break;
     } /* case 'c' */

     case ('p'): case ('P'):
   /*********************************** 
     input of atom positions and types
     for bulk through 'po':
   ***********************************/
     {
     /* go on if 2nd letter is different from 'o' */
       if( (*(linebuffer+i_str+1) != 'o') && (*(linebuffer+i_str+1) != 'O') )
         break;

       atoms_rd = ( leed_atom_t *) realloc( 
                   atoms_rd, (i_atoms+2) * sizeof(leed_atom_t) );

#ifdef REAL_IS_DOUBLE
       iaux = sscanf(linebuffer+i_str+3 ," %s %lf %lf %lf %s %lf %lf %lf",
#endif
#ifdef REAL_IS_FLOAT
       iaux = sscanf(linebuffer+i_str+3 ," %s %f %f %f %s %f %f %f",
#endif
              phaseinp, 
              atoms_rd[i_atoms].pos+1, 
              atoms_rd[i_atoms].pos+2, 
              atoms_rd[i_atoms].pos+3,
              whatnext, vaux+1, vaux+2, vaux+3);

       for(i=1; i<=3; i++) atoms_rd[i_atoms].pos[i] /= BOHR;
       
     /**********************************
       Input of phaseshifts and (root) mean square displacements due to 
       thermal vibrations:

       Eventually, the vector vaux will contain

         vaux[0] = <dr^2> = <dx^2> + <dy^2> + <dz^2>;
         vaux[1] = sqrt(<dx^2>), 
         vaux[2] = sqrt(<dy^2>), 
         vaux[3] = sqrt(<dz^2>),

       In the case of isotropic vibrations

         sqrt(<dx^2>) = sqrt(<dy^2>) = sqrt(<dz^2>) = sqrt(<dr^2>/3)
     **********************************/

       if (iaux <= 5) for(i=0; i<=3; i++) vaux[i] = 0.;
       else
       {

     /* input of the isotropic root mean square displacement */
         if( ( !strncmp(whatnext, "dr1", 3) ) && (iaux >= 6) )
           {
             atoms_rd[i_atoms].t_type = T_DIAG;

             vaux[0] = vaux[1]/BOHR;
             vaux[1] = vaux[2] = vaux[3] = vaux[0]/SQRT3;
             vaux[0] *= vaux[0];
           }

     /* input of root mean square displacements for each direction */
         else if( ( !strncmp(whatnext, "dr3", 3) ) && (iaux >= 8) )
           {
             atoms_rd[i_atoms].t_type = T_DIAG;

             for(i=1; i<=3; i++) vaux[i] /= BOHR;
             vaux[0] = SQUARE(vaux[1]) + SQUARE(vaux[2]) + SQUARE(vaux[3]);
           }


     /* input of root mean square displacements for each direction */
         else if( ( !strncmp(whatnext, "nd3", 3) ) && (iaux >= 8) )
           {
             atoms_rd[i_atoms].t_type = T_NOND;

             for(i=1; i<=3; i++) vaux[i] /= BOHR;
             vaux[0] = SQUARE(vaux[1]) + SQUARE(vaux[2]) + SQUARE(vaux[3]);
           }

     /* 
        Input of Debye temperature, atomic mass and temperature:
          vaux[1] = Debye temperature
          vaux[2] = atomic mass
          vaux[3] = temperature (has to be specified only for the first atom; 
                    if not specified, 300 K is used)

          <r^2> is calculated in leed_inp_debye_temp
          
     */
         else if( ( !strncmp(whatnext, "dmt", 3) ) && (iaux >= 7) )
           {
             atoms_rd[i_atoms].t_type = T_DIAG;

             if(iaux >= 8) bulk_par->temp = vaux[3];

         /* changed GH/19.09.00
             faux = bulk_par->temp/vaux[1];
             vaux[0] = PREF_DEBWAL / (vaux[2] * vaux[1]) *
                       R_sqrt(0.0625 + faux*faux);
         */

             vaux[0] = leed_inp_debye_temp(vaux[1] , vaux[2] , bulk_par->temp );
             vaux[1] = vaux[2] = vaux[3] = R_sqrt(vaux[0])/SQRT3;

#ifdef CONTROL_X
             fprintf(STDCTR, "(leed_read_overlayer): temp = %.1f dr = %.3f\n", 
             bulk_par->temp, vaux[1] * SQRT3 * BOHR);
#endif
           }
         else
           {
#ifdef WARNING
             fprintf(STDWAR,
                 "* warning (leed_read_overlayer): Could not interpret input: ");
             fprintf(STDWAR,"%s", whatnext);
             for(i=1; i<=iaux-5; i++) fprintf(STDWAR," %.3f", vaux[i]);
             fprintf(STDWAR,"\n");
#endif
             for(i=0; i<=3; i++) vaux[i] = 0.;
           }
       }

     /* input of atomic phase shifts */
       atoms_rd[i_atoms].type = leed_leed_leed_leed_inp_phase_nd(phaseinp, vaux, 
                                            atoms_rd[i_atoms].t_type, 
                                            p_phs_shifts);
       over_par->ntypes = MAX(atoms_rd[i_atoms].type+1, over_par->ntypes);

       i_atoms ++;
       break;
     } /* case 'p' */

     case ('v'): case ('V'):
   /***********************************
     input of optical potentials
   ***********************************/
     {
/* go on if 2nd letter is different from 'r' */
       if( (*(linebuffer+i_str+1) != 'r') && (*(linebuffer+i_str+1) != 'R') )
         break;

#ifdef REAL_IS_DOUBLE
       sscanf(linebuffer+i_str+3 ," %lf", &(over_par->vr));
#endif
#ifdef REAL_IS_FLOAT
       sscanf(linebuffer+i_str+3 ," %f", &(over_par->vr));
#endif
       (over_par->vr) /= HART;
/* make sure, vr is < 0. */
       if (over_par->vr > 0.)
       {
         over_par->vr = -over_par->vr;
#ifdef WARNING
         fprintf(STDWAR, "* warning (leed_read_overlayer):");
         fprintf(STDWAR,
           "Vr must be negative, use the negative value of input %.1f\n",
           over_par->vr*HART);
#endif
       }
       break;
     } /* case 'v' */

   /***********************************
     comments not to be stored and 
     new line characters
   ***********************************/
     case ('#'): case ('\n'): case('\r'): 
   /***********************************
     identifiers used in leed_inp_read_bul
   ***********************************/
     case ('a'): case ('A'): 
     case ('b'): case ('B'): 
     case ('m'): case ('M'): 
   /***********************************
     identifiers used in leed_inp_leed_read_par
   ***********************************/
     case ('e'): case ('E'): 
     case ('f'): case ('F'): 
     case ('i'): case ('I'): 
     case ('l'): case ('L'): 
     { break; }

     default: 
   /***********************************
     default: print warning for not 
              recognized key words
   ***********************************/
     {
#ifdef WARNING
       fprintf(STDWAR,
  "* warning (leed_read_overlayer): could not interpret line \n\t%s\t(in file \"%s\")\n",
       linebuffer, filename);
#endif
       break; 
     }
   } /* switch linebuffer */
 }   /* while: read input file */
 
 fclose(inp_stream);

/************************************************************************
  END OF INPUT

  Start processing input data if there were any (i_atoms > 0)
*************************************************************************/

#ifdef CONTROL_X
 fprintf(STDCTR, "(leed_read_overlayer): start processing: i_atoms = %d\n", i_atoms);
#endif
 atoms_rd[i_atoms].type = I_END_OF_LIST;
 over_par->natoms = i_atoms;
 
 if(i_atoms > 0)
 {
/************************************************************************
 Move all atomic positions specified in atoms.pos into the 2-dim bulk unit 
 cell specified through b1 and b2:

 The vector x = A_1*pos must only have components between 0 and 1.
 => subtract the integer surplus from pos.
*************************************************************************/

   for(i = 0; i < i_atoms; i ++ )
   {
     vaux[1] = (atoms_rd[i].pos[1] * bulk_par->b_1[1] +
                atoms_rd[i].pos[2] * bulk_par->b_1[2]) / (2. * PI);
     vaux[2] = (atoms_rd[i].pos[1] * bulk_par->b_1[3] +
                atoms_rd[i].pos[2] * bulk_par->b_1[4]) / (2. * PI);

     if( vaux[1] < 0.) iaux = (int) vaux[1] - 1;
     else              iaux = (int) vaux[1];
     {
       atoms_rd[i].pos[1] -= iaux*bulk_par->b[1];
       atoms_rd[i].pos[2] -= iaux*bulk_par->b[3];
     }

     if( vaux[2] < 0.) iaux = (int) vaux[2] - 1;
     else              iaux = (int) vaux[2];
     {
       atoms_rd[i].pos[1] -= iaux*bulk_par->b[2];
       atoms_rd[i].pos[2] -= iaux*bulk_par->b[4];
     }
   } /* for i */

/************************************************************************
  Sort the atoms specified through atoms_rd.pos according to their z 
  coordinates (smallest z first).
*************************************************************************/

#ifdef CONTROL_X
 fprintf(STDCTR, "(leed_read_overlayer): sorting \n");
#endif
   for(i=0; i<i_atoms; i++)
     for(j=i+1; j<i_atoms; j++)
     {
       if( atoms_rd[i].pos[3] > atoms_rd[j].pos[3])
       {
         memcpy(&atom_aux,  atoms_rd+j, sizeof(leed_atom_t));
         memcpy(atoms_rd+j, atoms_rd+i, sizeof(leed_atom_t));
         memcpy(atoms_rd+i, &atom_aux,  sizeof(leed_atom_t));
       }
     } /* for i,j */

/************************************************************************
 - Distribute the atoms to layers.
 - Find the minimum interlayer distance.
*************************************************************************/

    i_layer = leed_inp_overlayer(over_par, atoms_rd);

  free(atoms_rd);
  
/* 
   Find the minimum interlayer distance in bulk and overlayer.
   - The distance between the last bulk layer and the first overlayer is:
     bulk_par->layers[bulk_par->nlayers-1].vec_to_next[3] (bulk - origin)
     + over_par->layers[0].vec_from_last[3] (origin - overlayer)
*/
   over_par->dmin = bulk_par->dmin;

   faux = R_fabs( over_par->layers[0].vec_from_last[3] + 
                  bulk_par->layers[bulk_par->nlayers-1].vec_to_next[3] );
   over_par->dmin = MIN(over_par->dmin, faux);

#ifdef CONTROL
   fprintf(STDCTR, "(leed_read_overlayer): bulk - overlayer distance = %5.2f\n", 
                   faux*BOHR);
#endif

   for(i=1; i < over_par->nlayers; i++)
   {
#ifdef CONTROL
     fprintf(STDCTR, "(leed_read_overlayer): interlayer distance [%d] = %5.2f\n",
             i, over_par->layers[i].vec_from_last[3]*BOHR);
#endif
     over_par->dmin = 
          MIN(over_par->dmin, R_fabs(over_par->layers[i].vec_from_last[3]) );
   }

 }    /* if i_atoms > 0 */
 else /* no atoms in overlayer */
 {
   over_par->nlayers = 0;
   over_par->dmin = bulk_par->dmin;    /* is set to this value anyway ! */
 }

/************************************************************************
 Adjust structure elements dmin, vr, and ntypes in bulk_par.
*************************************************************************/

 bulk_par->dmin = over_par->dmin;
 bulk_par->vr = over_par->vr;
 bulk_par->ntypes = over_par->ntypes;
 bulk_par->n_rot = over_par->n_rot;

#ifdef CONTROL
 printf("***********************(leed_read_overlayer)***********************\n");
 printf("\npositions (overlayer):\n");

 printf("\n\tdmin (bulk and overlayer): %.4f\n", over_par->dmin*BOHR);
 
 for(i=0; i < over_par->nlayers; i++)
 {
   printf("\n->\tvec: (%7.4f  %7.4f  %7.4f) A\n\n", 
            over_par->layers[i].vec_from_last[1]*BOHR,
            over_par->layers[i].vec_from_last[2]*BOHR, 
            over_par->layers[i].vec_from_last[3]*BOHR );

   if( over_par->layers[i].periodic == 0 ) printf("np:");
   else         printf("p: ");

   for( j = 0; j < over_par->layers[i].natoms; j ++)
   {
     printf("\tpos: (%7.4f  %7.4f  %7.4f) A\tlayer: %d type: %d atom: %d\n", 
             over_par->layers[i].atoms[j].pos[1]*BOHR, 
             over_par->layers[i].atoms[j].pos[2]*BOHR, 
             over_par->layers[i].atoms[j].pos[3]*BOHR,
             over_par->layers[i].atoms[j].layer, 
             over_par->layers[i].atoms[j].type, j);
   }
 }

 printf("\n->\tvec: (%7.4f  %7.4f  %7.4f) A\n\n", 
          over_par->layers[over_par->nlayers-1].vec_to_next[1]*BOHR,
          over_par->layers[over_par->nlayers-1].vec_to_next[2]*BOHR, 
          over_par->layers[over_par->nlayers-1].vec_to_next[3]*BOHR );

 printf("comments:\n");

 for( i=0; i<i_com; i++)
 {
   printf("\t%s", *(over_par->comments + i));
 }

 fprintf(STDCTR,"phase shifts:\n");
 fprintf(STDCTR,"\t%d different sets of phase shifts used:\n", 
         over_par->ntypes);
 for(i_c = 0; i_c < over_par->ntypes; i_c ++)
   fprintf(STDCTR,"\t(%d) %s (%d energies, lmax = %d)\tV<dr^2>_T = %.3f A^2\n",
           i_c,
           (*(p_phs_shifts)+i_c)->input_file, 
           (*(p_phs_shifts)+i_c)->neng, 
           (*(p_phs_shifts)+i_c)->lmax,
           R_sqrt( (*(p_phs_shifts)+i_c)->dr[0] ) *BOHR);

 printf("***********************(leed_read_overlayer)***********************\n");
#endif


/************************************************************************
 write the structures phs_shifts and over_par back.
*************************************************************************/

 *p_over_par = over_par;

 return(1);
 i_layer = i_layer * 1;
}
