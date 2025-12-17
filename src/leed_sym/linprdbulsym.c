/*********************************************************************
GH/29.09.00 
  file contains function:

  leed_inp_read_bul_sym
 
Changes:

GH/17.08.94 - insert REAL_IS_DOUBLE and REAL_IS_FLOAT for sscanf.
GH/20.04.95 - read superstructure matrix
GH/19.07.95 - read vibrational amplitudes (root mean square)
GH/25.07.95 - introduce structure element "layers" in bulk_par
              and eliminate "atoms"
GH/04.09.97 - include symmetry flags.
WB/05.05.98 - raed rotaxis
WB/17.08.98 - include mirror planes
WB/19.08.98 - realloc atoms_rd iatoms+2
WB/26.08.98 - read mirror planes,also more lines
WB/27.08.98 - switch symmetry flags
WB/14.09.98 - read only one mirror plane in one line
GH/29.09.00 - calculate dr2 for dmt input in function leed_inp_debye_temp

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#include "leed.h"
#include "leed_def.h"
#include "leed_func.h"

/*
#define CONTROL_X
#define CONTROL
*/

#ifndef WARNING
#define WARNING
#endif

#ifndef ERROR
#define ERROR
#endif

#define EXIT_ON_ERROR

#ifndef GEO_TOLERANCE          /* should be defined in "leed_def.h" */
#define GEO_TOLERANCE 0.0001
#endif

#ifndef DEF_TEMP               /* should be defined in "leed_def.h" */
#define DEF_TEMP 300.
#endif

#define PREF_DEBWAL 1559.04170632481439  /* prefactor for the evaluation
                                            of <dr^2> from temperature
                                            and Debye temperature */
#define SQRT3    1.73205080756887729352

/********************************************************************/

int leed_inp_read_bul_sym(leed_cryst_t **p_bulk_par, 
                leed_phs_t **p_phs_shifts,
                char *filename)
/*********************************************************************
  Read all the bulk parameters that do not change during a search

  DESIGN

  Letters 'e' - 'k' are reserved as identifiers for parameter input 
  through function leed_inp_leed_read_par

  currently recognized identifier:
  
  'a1','a2','a3': lattice vectors of bulk unit cell
  'b1','b2': superstructure unit cell.
  'c': comment
  
  'm1','m2': superstructure matrix.
  'pb': postion and type of bulk atoms.
  'vi','vr': imag. and real part of optical potential.

  FUNCTION CALLS

   - leed_leed_inp_phase
   - leed_inp_bul_layer

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
int i_plane;                  /*i_plane is only for mirrorplanes
                                do not use it otherwise */  

real faux;                    /* dummy variable */
real a1[4], a2[4], a3[4];     /* vectors: 1=x, 2=y, 3=z, 0 is not used */
real vaux[4];                 /* dummy vector */

leed_cryst_t *bulk_par;   /* use *bulk_par instead of the pointer 
                                 p_bulk_par */

leed_atom_t atom_aux;     /* used for sorting atoms */
leed_atom_t *atoms_rd;    /* this vector of structure atom_str is
                                 used to read and treat the input atomic
                                 properties and will be copied into bulk_par
                                 afterwards */

/********************************************************************
  If bulk_par or phs_shifts is NULL: allocate memory and copy back to;
*********************************************************************/
 bulk_par = NULL;
 if (*p_bulk_par == NULL) 
   bulk_par = *p_bulk_par = 
                (leed_cryst_t *)malloc( sizeof(leed_cryst_t) );

/********************************************************************
  Preset parameters
  - allocate atoms_rd (1 unit)
  - allocate bulk_par->comments (1 unit)
  - set m_trans, m_super, m_recip to identity
  - set ai[j] and bulk_par->b[i] to 0.
  - set ntypes to zero.
  - set temperature (bulk_par->temp) to room temperature (DEF_TEMP = 300K);
  - set symmetry flags to no symmetry.
********************************************************************/

 atoms_rd = (leed_atom_t *)malloc(2 * sizeof(leed_atom_t) );
 i_atoms = 0;

 bulk_par->m_plane = (real *)malloc( sizeof(real));
 i_plane = 0;
 bulk_par->alpha = (real *)malloc( sizeof(real));

 bulk_par->comments = (char * *)malloc( sizeof(char *));
 *(bulk_par->comments) = NULL;
 i_com = 0;

 bulk_par->m_trans[1] = bulk_par->m_trans[4] = 1.;
 bulk_par->m_trans[2] = bulk_par->m_trans[3] = 0.;

 bulk_par->m_super[1] = bulk_par->m_super[4] = 1.;
 bulk_par->m_super[2] = bulk_par->m_super[3] = 0.;

 bulk_par->m_recip[1] = bulk_par->m_recip[4] = 1.;
 bulk_par->m_recip[2] = bulk_par->m_recip[3] = 0.;

 for(i_c = 0; i_c < 4; i_c ++) a1[i_c] = a2[i_c] = a3[i_c] = 0.;
 for(i_c = 0; i_c < 5; i_c ++) bulk_par->b[i_c] = 0.;

 bulk_par->temp = DEF_TEMP;

 bulk_par->ntypes = 0;

 bulk_par->n_rot = 1;
 bulk_par->rot_axis[1] = bulk_par->rot_axis[2] = 0.;
 bulk_par->n_mir = 0;

/********************************************************************
  Open and Read input file
********************************************************************/
 if( (inp_stream = fopen(filename, "r")) == NULL) 
 {
#ifdef ERROR
   fprintf(STDERR,
  "*** error (leed_inp_read_bul): could not open file \"%s\"\n",filename);
#endif
#ifdef EXIT_ON_ERROR
  exit(1);
#else
  return(-1);
#endif
 } 

#ifdef CONTROL
 fprintf(STDCTR,"(leed_inp_read_bul): Reading file \"%s\"\n",filename);
#endif
 while ( fgets(linebuffer, STRSZ, inp_stream) != NULL) 
 {
#ifdef CONTROL_X
   fprintf(STDCTR,"%s", linebuffer);
#endif
   /* find first non blank character */
   for( i_str = 0;  *(linebuffer+i_str) == ' '; i_str ++);
   switch( *(linebuffer+i_str) )
   {
     case ('a'): case ('A'):
   /*********************************** 
     input of bulk unit cell parameters
   ***********************************/
     {
       switch( *(linebuffer+i_str+1) )
       {
         case('1'): 
         {
#ifdef REAL_IS_DOUBLE
           if( sscanf(linebuffer+i_str+3," %lf %lf %lf", a1+1, a1+2, a1+3) < 2)
#endif
#ifdef REAL_IS_FLOAT
           if( sscanf(linebuffer+i_str+3," %f %f %f", a1+1, a1+2, a1+3) < 2)
#endif
           {
#ifdef ERROR
             fprintf(STDERR, 
               "*** error (leed_inp_read_bul): nead at least x/y coordinates of a1\n");
#endif
#ifdef EXIT_ON_ERROR
             exit(1);
#else
             return(-1);
#endif
           }
           for(i=1; i<=3; i++) a1[i] /= BOHR;
           break; 
         } /* a1 */

         case('2'): 
         {
#ifdef REAL_IS_DOUBLE
           if( sscanf(linebuffer+i_str+3," %lf %lf %lf", a2+1, a2+2, a2+3) < 2)
#endif
#ifdef REAL_IS_FLOAT
           if( sscanf(linebuffer+i_str+3," %f %f %f", a2+1, a2+2, a2+3) < 2)
#endif
           { 
#ifdef ERROR
             fprintf(STDERR, 
               "*** error (leed_inp_read_bul): nead at least x/y coordinates of a2\n");
#endif
#ifdef EXIT_ON_ERROR
             exit(1);
#else
             return(-1);
#endif
           }
           for(i=1; i<=3; i++) a2[i] /= BOHR;
           break; 
         } /* a2 */

         case('3'): 
         {
#ifdef REAL_IS_DOUBLE
           sscanf(linebuffer+i_str+3 ," %lf %lf %lf", a3+1, a3+2, a3+3);
#endif
#ifdef REAL_IS_FLOAT
           sscanf(linebuffer+i_str+3 ," %f %f %f", a3+1, a3+2, a3+3);
#endif
           for(i=1; i<=3; i++) a3[i] /= BOHR;
           break; 
         } /* a3 */
       }
       break;
     } /* case 'a' */

     case ('b'): case ('B'):
   /***********************************
     input of super structure unit cell parameters
   ***********************************/
     {
       switch( *(linebuffer+i_str+1) )
       {
         case('1'):
         {
#ifdef REAL_IS_DOUBLE
           if( sscanf(linebuffer+i_str+3," %lf %lf", 
               (bulk_par->b)+1, (bulk_par->b)+3 ) < 2)
#endif
#ifdef REAL_IS_FLOAT
           if( sscanf(linebuffer+i_str+3," %f %f",
               (bulk_par->b)+1, (bulk_par->b)+3 ) < 2)
#endif
           {
#ifdef ERROR
             fprintf(STDERR,
               "*** error (leed_inp_read_bul): nead x/y coordinates of b1\n");
#endif
#ifdef EXIT_ON_ERROR
             exit(1);
#else
             return(-1);
#endif
           }
           bulk_par->b[1] /= BOHR; bulk_par->b[3] /= BOHR;
           break;
         } /* b1 */

         case('2'):
         {
#ifdef REAL_IS_DOUBLE
           if( sscanf(linebuffer+i_str+3," %lf %lf",
               (bulk_par->b)+2, (bulk_par->b)+4 ) < 2)
#endif
#ifdef REAL_IS_FLOAT
           if( sscanf(linebuffer+i_str+3," %f %f",
               (bulk_par->b)+2, (bulk_par->b)+4 ) < 2)
#endif
           {
#ifdef ERROR
             fprintf(STDERR,
               "*** error (leed_inp_read_bul): nead x/y coordinates of b2\n");
#endif
#ifdef EXIT_ON_ERROR
             exit(1);
#else
             return(-1);
#endif
           }
           bulk_par->b[2] /= BOHR; bulk_par->b[4] /= BOHR;
           break;
         } /* b2 */
       }
       break;
     } /* case 'b' */

     case ('c'): case ('C'):
   /*********************************** 
     input of comments to be stored
   ***********************************/
     {
       bulk_par->comments = ( char * * ) realloc( 
                           bulk_par->comments, (i_com+2) * sizeof(char *) ); 

       *(bulk_par->comments + i_com) = (char *)calloc(
            strlen(filename) + strlen(linebuffer) + 2 - i_str, 
            sizeof(char));
       *(bulk_par->comments + i_com+1) = NULL;

       sprintf(*(bulk_par->comments + i_com), "(%s): %s",
               filename, linebuffer+i_str+2);
       
       i_com ++;
       break;
     } /* case 'c' */

     case ('m'): case ('M'):
   /***********************************
     input of super structure matrix
     (use m_recip as temporary storage)
   ***********************************/
     {
       switch( *(linebuffer+i_str+1) )
       {
         case('1'): {
#ifdef REAL_IS_DOUBLE
           sscanf(linebuffer+i_str+3 ," %lf %lf", 
                  bulk_par->m_recip+1, bulk_par->m_recip+2);
#endif
#ifdef REAL_IS_FLOAT
           sscanf(linebuffer+i_str+3 ," %f %f", 
                  bulk_par->m_recip+1, bulk_par->m_recip+2);
#endif
           break; }
         case('2'): {
#ifdef REAL_IS_DOUBLE
           sscanf(linebuffer+i_str+3 ," %lf %lf", 
                  bulk_par->m_recip+3, bulk_par->m_recip+4);
#endif
#ifdef REAL_IS_FLOAT
           sscanf(linebuffer+i_str+3 ," %f %f", 
                  bulk_par->m_recip+3, bulk_par->m_recip+4);
#endif
           break; }
       }
       break;
     } /* case 'm' */

     case ('p'): case ('P'):
   /*********************************** 
     input of atom positions and types
     for bulk through 'pb':
   ***********************************/
     {
     /* go on if 2nd letter is different from 'b' */
       if( (*(linebuffer+i_str+1) != 'b') && (*(linebuffer+i_str+1) != 'B') )
       {
     /* "po" is known to function leed_read_overlayer, all others cause warning */
         if( (*(linebuffer+i_str+1) != 'o') && (*(linebuffer+i_str+1) != 'O') )
#ifdef WARNING
           fprintf(STDWAR,
"* warning (leed_inp_read_bul): could not interpret line \n\t%s\t(in file \"%s\")\n",
           linebuffer, filename);
#endif
         break;
       }

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
       Input of phaseshifts and displacements due to thermal vibrations:

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
             vaux[0] = vaux[1]/BOHR;
             vaux[1] = vaux[2] = vaux[3] = vaux[0]/SQRT3;
             vaux[0] *= vaux[0];
           }

     /* input of root mean square displacements for each direction */

         else if( ( !strncmp(whatnext, "dr3", 3) ) && (iaux >= 8) )
           {
             for(i=1; i<=3; i++) vaux[i] /= BOHR;
             vaux[0] = SQUARE(vaux[1]) + SQUARE(vaux[2]) + SQUARE(vaux[3]);
           }

     /* 
        Input of Debye temperature, atomic mass and temperature:
          vaux[1] = Debye temperature
          vaux[2] = atomic mass
          vaux[3] = temperature (has to be specified only for the first atom; 
                    if not specified, DEF_TEMP K is used)

          <r^2> is calculated in leed_inp_debye_temp

     */
         else if( ( !strncmp(whatnext, "dmt", 3) ) && (iaux >= 7) )
           {
             if(iaux >= 8) bulk_par->temp = vaux[3];

         /* changed GH/29.09.00
             faux = bulk_par->temp/vaux[1];
             vaux[0] = PREF_DEBWAL / (vaux[2] * vaux[1]) *
                       R_sqrt(0.0625 + faux*faux);
             vaux[0] = 0.5 * PREF_DEBWAL / (vaux[2] * vaux[1]) *
                       R_sqrt(0.0625 + faux*faux);

         */
             vaux[0] = leed_inp_debye_temp(vaux[1] , vaux[2] , bulk_par->temp );
             vaux[1] = vaux[2] = vaux[3] = R_sqrt(vaux[0])/SQRT3;

#ifdef CONTROL_X
             fprintf(STDCTR, "(leed_inp_read_bul): temp = %.1f dr = %.3f\n", 
             bulk_par->temp, vaux[1] * SQRT3*BOHR);
#endif
           }
         else
           {
#ifdef WARNING
             fprintf(STDWAR,
                 "* warning (leed_inp_read_bul): Could not interpret input: ");
             fprintf(STDWAR,"%s", whatnext);
             for(i=1; i<=iaux-5; i++) fprintf(STDWAR," %.3f", vaux[i]);
             fprintf(STDWAR,"\n");
#endif
             for(i=0; i<=3; i++) vaux[i] = 0.;
           }
       }

     /* input of atomic phase shifts */
       atoms_rd[i_atoms].type = leed_leed_inp_phase(phaseinp, vaux, p_phs_shifts);
       bulk_par->ntypes = MAX(atoms_rd[i_atoms].type+1, bulk_par->ntypes);

       i_atoms ++;
       break;
     } /* case 'p' */

     case ('v'): case ('V'):
   /*********************************** 
     input of optical potentials
   ***********************************/
     {
       switch( *(linebuffer+i_str+1) )
       {
         case('i'): case('I'): {
#ifdef REAL_IS_DOUBLE
           sscanf(linebuffer+i_str+3 ," %lf", &(bulk_par->vi));
#endif
#ifdef REAL_IS_FLOAT
           sscanf(linebuffer+i_str+3 ," %f", &(bulk_par->vi));
#endif
           (bulk_par->vi) /= HART;
      /* make sure, vi is > 0. */
           if (bulk_par->vi < 0.)
           {
             bulk_par->vi = -bulk_par->vi;
#ifdef WARNING
             fprintf(STDWAR, "* warning (leed_inp_read_bul):");
             fprintf(STDWAR,
               "Vi must be positive, use the negative value of input %.1f\n", 
               bulk_par->vi*HART);
#endif
           }
           break; }

         case('r'): case('R'): {
#ifdef REAL_IS_DOUBLE
           sscanf(linebuffer+i_str+3 ," %lf", &(bulk_par->vr));
#endif
#ifdef REAL_IS_FLOAT
           sscanf(linebuffer+i_str+3 ," %f", &(bulk_par->vr));
#endif
           (bulk_par->vr) /= HART;
      /* make sure, vr is < 0. */
           if (bulk_par->vr > 0.)
           {
             bulk_par->vr = -bulk_par->vr;
#ifdef WARNING
             fprintf(STDWAR, "* warning (leed_inp_read_bul):");
             fprintf(STDWAR,
               "Vr must be negative, use the negative value of input %.1f\n",
               bulk_par->vr*HART);
#endif
           }
           break; }
       }
       break;
     } /* case 'v' */

     case ('s'): case ('S'):
   /***********************************
     input of symmetry
   ***********************************/
     {
     switch( *(linebuffer+i_str+1 ))
      {
      case('r'): case('R'):
        {
#ifdef REAL_IS_DOUBLE
           if( sscanf(linebuffer+i_str+3,"%d %lf %lf",
               &(bulk_par->n_rot),(bulk_par->rot_axis)+1,(bulk_par->rot_axis)+2) < 3)
#endif
#ifdef REAL_IS_FLOAT
           if( sscanf(linebuffer+i_str+3,"%d %f %f",
               &(bulk_par->n_rot),(bulk_par->rot_axis)+1,(bulk_par->rot_axis)+2) < 3)
#endif
           {
#ifdef ERROR
             fprintf(STDERR,
               "*** error (leed_inp_read_bul): need x/y coordinates of rotaxis\n");
#endif
#ifdef EXIT_ON_ERROR
             exit(1);
#else
             return(-1);
#endif
           }
         bulk_par->rot_axis[1] /= BOHR; bulk_par->rot_axis[2] /= BOHR;
         break;
         }

      case('m'): case('M'):
      {
   /**allocate**/  
         i_plane++;
         bulk_par->m_plane = (real *)realloc
                 (bulk_par->m_plane , (2*i_plane + 1) * sizeof(real));
         bulk_par->alpha = (real *)realloc
                 (bulk_par->alpha , (i_plane + 1) * sizeof(real));
 
         i = 2 *i_plane -1;
         j = 2 *i_plane -2; 
#ifdef REAL_IS_DOUBLE
         if(sscanf(linebuffer+i_str+3," %lf %lf ",
            (bulk_par->m_plane)+j ,(bulk_par->m_plane)+i) < 2) 
#endif
#ifdef REAL_IS_FLOAT
         if(sscanf(linebuffer+i_str+i," %f %f ",
            (bulk_par->m_plane)+j ,(bulk_par->m_plane)+i) < 2)
#endif
         {
#ifdef ERROR
             fprintf(STDERR,
               "*** error (leed_inp_read_bul): need x/y coordinates of mirrorplane\n");
#endif
#ifdef EXIT_ON_ERROR
             exit(1);
#else
             return(-1);
#endif
         }
         bulk_par->m_plane[j] /= BOHR; 
         bulk_par->m_plane[i] /= BOHR;
         bulk_par->m_plane[i+1] = F_END_OF_LIST; 
         bulk_par->n_mir = i_plane;

         break;
      }/* case m */

      default:
       {break;}

      }
      break;
     } /* case 's' */


   /***********************************
     comments not to be stored and 
     new line characters
   ***********************************/
     case ('#'): case ('\n'): case('\r'): 
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
  "* warning (leed_inp_read_bul): could not interpret line \n\t%s\t(in file \"%s\")\n",
       linebuffer, filename);
#endif
       break; 
     }
   } /* switch linebuffer */
 }   /* while: read input file */
 
 fclose(inp_stream);
  
/*******************************************
 END OF INPUT
 SORT the mirrorplanes according to the angle
********************************************/
 for(i = 0 ;i < bulk_par->n_mir;i++)
 {
   bulk_par->alpha[i] = R_atan2(bulk_par->m_plane[2*i+1],bulk_par->m_plane[2* i]);
 }

 for(i = 0;i < bulk_par->n_mir;i++)
 {
   for(j = i+1;j< bulk_par->n_mir;j++)
   {
     if(bulk_par->alpha[j] < bulk_par->alpha[i])
     {
       faux = bulk_par->alpha[j];
       bulk_par->alpha[j] = bulk_par->alpha[i];
       bulk_par->alpha[i] = faux;
       faux = bulk_par->m_plane[2* j];
       bulk_par->m_plane[2* j] = bulk_par->m_plane[2* i];
       bulk_par->m_plane[2* i] = faux;
       faux = bulk_par->m_plane[2* j+1];
       bulk_par->m_plane[2* j+1] = bulk_par->m_plane[2* i+1];
       bulk_par->m_plane[2* i+1] = faux;

     }
   }
   bulk_par->alpha[bulk_par->n_mir]= F_END_OF_LIST;
 }

 for(i = 0 ;i < bulk_par->n_mir;i++)
 {
   fprintf(STDOUT,"alpha%d = %f\n",i,bulk_par->alpha[i]/ DEG_TO_RAD);
   fprintf(STDOUT,"mirrorplane (%f %f)\n\n",bulk_par->m_plane[2* i]*BOHR,
                                            bulk_par->m_plane[2*i+1]*BOHR);
 } 

/************************************************************************
 Check the number of bulk atoms. Exit if zero
*************************************************************************/
 if(i_atoms < 1)
 {
#ifdef ERROR
   fprintf(STDERR,
   "*** error (leed_inp_read_bul): could not find any bulk atoms (i_atoms = %d)\n", 
   i_atoms);
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(-1);
#endif
 }

/************************************************************************
 Make shure that the basis vectors for the bulk unit cell specified 
 through a1, a2, a3 form a right-handed system with a3 pointing into
 the crystal (i.e. a3[3] < 0).
 If not: change the directions and the order of a1, a2, a3. 

 Store a1 and a2 and their inverse values in bul_par.a / bul_par.a_1
 2x2 - matrices are stored as:  m[1]  m[2]
                                m[3]  m[4]
*************************************************************************/

/* check if the z-components of a1 and a2 are 0. */
 if( ! IS_EQUAL_REAL(a1[3], 0.0) || ! IS_EQUAL_REAL(a2[3], 0.0) )
 {
#ifdef ERROR
   fprintf(STDERR, " *** error (leed_inp_read_bul):\n");
   fprintf(STDERR,
           " Vectors a1 and a2 are not parallel to the surface (xy plane)\n");
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(-1);
#endif
 }

/* check the direction of a3 */
 if( a3[3] > 0.0 ) a3[3] = - a3[3]; 

/* 
   Check the angle between a1 and a2 (1: must be >= pi/2, i.e. a1*a2 < 0).
   if not: take a1 and -a2 as basic vectors and modify transformation matrix.
*/
 if( (a1[1] * a2[1] + a1[2] * a2[2]) > 0.0)
 {
   for(i=0; i<3; i++) a2[i] = - a2[i];
   bulk_par->m_trans[2] = - bulk_par->m_trans[2];
   bulk_par->m_trans[4] = - bulk_par->m_trans[4];
 }
/* 
   Check the angle between a1 and a2 (2: must be < pi, i.e. a1*a2 > 0) 
   if not: exchange a1 and a2 and modify transformation matrix.
*/
 if( (a1[1] * a2[2] - a1[2] * a2[1]) < 0.0)
 {
   faux = a2[1]; a2[1] = a1[1]; a1[1] = faux;
   faux = a2[2]; a2[2] = a1[2]; a1[2] = faux;

   faux = bulk_par->m_trans[1]; 
   bulk_par->m_trans[1] = bulk_par->m_trans[2];
   bulk_par->m_trans[2] = faux;

   faux = bulk_par->m_trans[3]; 
   bulk_par->m_trans[3] = bulk_par->m_trans[4];
   bulk_par->m_trans[4] = faux;
 }

/*
 Store a1 and a2 and their inverse values in bul_par.a / bul_par.a_1
*/

 bulk_par->a[1] = a1[1]; bulk_par->a[2] = a2[1];
 bulk_par->a[3] = a1[2]; bulk_par->a[4] = a2[2];

 faux = a1[1]*a2[2] - a1[2]*a2[1];
 bulk_par->area = R_fabs(faux);

 faux = 2.* PI/faux;
 bulk_par->a_1[1] =  faux*a2[2]; bulk_par->a_1[2] = -faux*a2[1];
 bulk_par->a_1[3] = -faux*a1[2]; bulk_par->a_1[4] =  faux*a1[1];

/************************************************************************
 Superstructure:
 
 - The superstructure matrix in the input files is defined with respect to
   the unmodified unit cell vectors a:
   b = m_super * a => b = m_super * m_trans * a_prg
   (m_super was stored in m_recip)

 - Calculate inverse transposed of m_super = m_recip.
*************************************************************************/

 if( R_hypot(bulk_par->b[1], bulk_par->b[3]) < GEO_TOLERANCE)
 /*
   There was no input of superstructure lattice vectors 
   => use matrix to calculate them.
 */
 {
   bulk_par->m_super[1] = bulk_par->m_recip[1] * bulk_par->m_trans[1] + 
                          bulk_par->m_recip[2] * bulk_par->m_trans[3];
   bulk_par->m_super[2] = bulk_par->m_recip[1] * bulk_par->m_trans[2] +
                          bulk_par->m_recip[2] * bulk_par->m_trans[4];
   bulk_par->m_super[3] = bulk_par->m_recip[3] * bulk_par->m_trans[1] +
                          bulk_par->m_recip[4] * bulk_par->m_trans[3];
   bulk_par->m_super[4] = bulk_par->m_recip[3] * bulk_par->m_trans[2] +
                          bulk_par->m_recip[4] * bulk_par->m_trans[4];
/* 
   Calculate superstructure lattice vectors b_t = m*a_t 
     b1x = b[1], b2x = b[2]
     b1y = b[3], b2y = b[4]
*/
 
   bulk_par->b[1] = bulk_par->m_super[1] * bulk_par->a[1] +
                    bulk_par->m_super[2] * bulk_par->a[2];
   bulk_par->b[3] = bulk_par->m_super[1] * bulk_par->a[3] +
                    bulk_par->m_super[2] * bulk_par->a[4];
   bulk_par->b[2] = bulk_par->m_super[3] * bulk_par->a[1] +
                    bulk_par->m_super[4] * bulk_par->a[2];
   bulk_par->b[4] = bulk_par->m_super[3] * bulk_par->a[3] +
                    bulk_par->m_super[4] * bulk_par->a[4];
 } /* b was not defined */

 else
 /*
   Superstructure lattice vectors have been supplied
   => use them to determine the superstructure matrix m_super.

   This overwrites even an existing superstructure matrix !!!
 */
 {
   faux = 0.5 / PI;
   bulk_par->m_super[1] = (bulk_par->a_1[1]*bulk_par->b[1] +
                           bulk_par->a_1[2]*bulk_par->b[3]) * faux;
   bulk_par->m_super[3] = (bulk_par->a_1[1]*bulk_par->b[2] +
                           bulk_par->a_1[2]*bulk_par->b[4]) * faux;
   bulk_par->m_super[2] = (bulk_par->a_1[3]*bulk_par->b[1] +
                           bulk_par->a_1[4]*bulk_par->b[3]) * faux;
   bulk_par->m_super[4] = (bulk_par->a_1[3]*bulk_par->b[2] +
                           bulk_par->a_1[4]*bulk_par->b[4]) * faux;
 } /* b was defined */

/*
   Check the matrix for noninteger elements
*/

 if( 
  (R_fabs(bulk_par->m_super[1]-R_nint(bulk_par->m_super[1])) > GEO_TOLERANCE) ||
  (R_fabs(bulk_par->m_super[2]-R_nint(bulk_par->m_super[2])) > GEO_TOLERANCE) ||
  (R_fabs(bulk_par->m_super[3]-R_nint(bulk_par->m_super[3])) > GEO_TOLERANCE) ||
  (R_fabs(bulk_par->m_super[4]-R_nint(bulk_par->m_super[4])) > GEO_TOLERANCE) )
 {
#ifdef ERROR
  fprintf(STDERR, 
          "*** error (leed_inp_read_bul): superstructure is not commensurate \n");
#endif
#ifdef EXIT_ON_ERROR
  exit(1);
#else
  return(-1);
#endif
 }

/*
   Check the angle between b1 and b2 (2: must be < pi, i.e. b1*b2 > 0)
   if not: exchange b1 and b2 and modify transformation matrix.
*/
 if( (bulk_par->b[1]*bulk_par->b[4] - bulk_par->b[3]*bulk_par->b[2]) < 0.0)
 {
   faux=bulk_par->b[2]; bulk_par->b[2]=bulk_par->b[1]; bulk_par->b[1]=faux;
   faux=bulk_par->b[4]; bulk_par->b[4]=bulk_par->b[3]; bulk_par->b[3]=faux;

   faux = bulk_par->m_super[1];
   bulk_par->m_super[1] = bulk_par->m_super[3];
   bulk_par->m_super[3] = faux;

   faux = bulk_par->m_super[2];
   bulk_par->m_super[2] = bulk_par->m_super[4];
   bulk_par->m_super[4] = faux;
 }

#ifdef CONTROL_X
   fprintf(STDCTR,"M_super: %5.2f %5.2f\n", 
                   bulk_par->m_super[1], bulk_par->m_super[2]);
   fprintf(STDCTR,"         %5.2f %5.2f\n", 
                   bulk_par->m_super[3], bulk_par->m_super[4]);
   fprintf(STDCTR,"b1: %5.2f %5.2f\n", bulk_par->b[1]*BOHR,bulk_par->b[3]*BOHR);
   fprintf(STDCTR,"b2: %5.2f %5.2f\n", bulk_par->b[2]*BOHR,bulk_par->b[4]*BOHR);
#endif

/* 
   area of superstructure unit cell in multiples of the (1x1) unit cell 
   = det(m_super)
*/
 faux = bulk_par->m_super[1] * bulk_par->m_super[4] -
        bulk_par->m_super[2] * bulk_par->m_super[3] ;
 bulk_par->rel_area_sup = R_fabs(faux);

/* m_recip = m* = (m_super)t^-1 */
 faux = 1./faux;

 bulk_par->m_recip[1] = +faux * bulk_par->m_super[4];
 bulk_par->m_recip[2] = -faux * bulk_par->m_super[3];
 bulk_par->m_recip[3] = -faux * bulk_par->m_super[2];
 bulk_par->m_recip[4] = +faux * bulk_par->m_super[1];

#ifdef CONTROL_X
   fprintf(STDCTR,"M_recip: %5.2f %5.2f\n", 
                   bulk_par->m_recip[1], bulk_par->m_recip[2]);
   fprintf(STDCTR,"         %5.2f %5.2f\n", 
                   bulk_par->m_recip[3], bulk_par->m_recip[4]);
   fprintf(STDCTR,"area_sup: %5.2f\n", bulk_par->rel_area_sup);
#endif

/* 
  Calculate reciprocal superstructure vectors: b_1 = 2PI * b^-1
    b*1x = b_1[1], b*1y = b_1[2]
    b*2x = b_1[3], b*2y = b_1[4] 
*/

 faux = 2*PI/(bulk_par->b[1]*bulk_par->b[4] - bulk_par->b[2]*bulk_par->b[3]);

 bulk_par->b_1[1] = +faux * bulk_par->b[4];
 bulk_par->b_1[2] = -faux * bulk_par->b[2];
 bulk_par->b_1[3] = -faux * bulk_par->b[3];
 bulk_par->b_1[4] = +faux * bulk_par->b[1];

/************************************************************************
 subtract the coordinates of the rotationaxis(i.e.............mirrorplanes)
 from the atomic positions
 Move all atomic positions specified in atoms.pos into the 2-dim bulk unit 
 cell specified through a1 and a2:

 The vector x = A_1*pos must only have components between 0 and 1.
 => subtract the integer surplus from pos.
*************************************************************************/

 for(i = 0; i < i_atoms; i ++ )
 {
   atoms_rd[i].pos[1] -= bulk_par->rot_axis[1];
   atoms_rd[i].pos[2] -= bulk_par->rot_axis[2];

   vaux[1] = (atoms_rd[i].pos[1] * bulk_par->a_1[1] + 
              atoms_rd[i].pos[2] * bulk_par->a_1[2]) / (2. * PI);
   vaux[2] = (atoms_rd[i].pos[1] * bulk_par->a_1[3] + 
              atoms_rd[i].pos[2] * bulk_par->a_1[4]) / (2. * PI);
   
   if( vaux[1] < - 0.0001) iaux = (int) vaux[1] - 1;
   else              iaux = (int) vaux[1];
   
   atoms_rd[i].pos[1] -= iaux*bulk_par->a[1] ;
   atoms_rd[i].pos[2] -= iaux*bulk_par->a[3] ;
  

   if( vaux[2] < - 0.0001) iaux = (int) vaux[2] - 1;
   else              iaux = (int) vaux[2];
 
   atoms_rd[i].pos[1] -= iaux*bulk_par->a[2] ;
   atoms_rd[i].pos[2] -= iaux*bulk_par->a[4] ;
  
 } 

/************************************************************************
  Sort the atoms specified through atoms.pos according to their z 
  coordinates (largest z first).
*************************************************************************/

 for(i=0; i<i_atoms; i++)
   for(j=i+1; j<i_atoms; j++)
   {
     if( atoms_rd[i].pos[3] < atoms_rd[j].pos[3])
     {
       memcpy(&atom_aux,  atoms_rd+j, sizeof(leed_atom_t));
       memcpy(atoms_rd+j, atoms_rd+i, sizeof(leed_atom_t));
       memcpy(atoms_rd+i, &atom_aux,  sizeof(leed_atom_t));
     }
   }

/*
  Check if the z-coordinates of all atoms are within the unit
  cell. Those which are not, will be neglected.
*/

 for(i=0; i<i_atoms; i++)
 {
   if(atoms_rd[i].pos[3] - atoms_rd[0].pos[3] < a3[3])
   {
#ifdef WARNING
      fprintf(STDWAR,
      "* warning (leed_inp_read_bul): Some coordinates of bulk atoms exceede the\n");
      fprintf(STDWAR,
      "                       bulk unit cell and will not be considered:\n");
      for(j = i; j < i_atoms; j ++)
        fprintf(STDWAR," %s \t %7.4f  %7.4f  %7.4f\n", phaseinp, 
                        atoms_rd[j].pos[1]*BOHR, 
                        atoms_rd[j].pos[2]*BOHR, 
                        atoms_rd[j].pos[3]*BOHR);
#endif
      break;
   }
 }
 i_atoms = i;
 atoms_rd[i_atoms].type = I_END_OF_LIST;
 bulk_par->natoms = i_atoms;
 
/************************************************************************
   ATTENTION 
 - Distribute the atoms to layers.
 - Find the minimum interlayer distance.
 - in the function leed_leed_inp_bul_layer_sym the symmetry is tested,so
   if the test is negative the program break
*************************************************************************/
 if(bulk_par->n_rot >= 1 || bulk_par->n_mir > 0)
    i_layer = leed_leed_inp_bul_layer_sym(bulk_par, atoms_rd, a3);

/**************
 else if(bulk_par->n_rot = 1 && bulk_par->n_mir < 1)
     i_layer = leed_inp_bul_layer(bulk_par, atoms_rd, a3);
***************/
 else
 {
#ifdef ERROR
  fprintf(STDERR,"(leed_inp_read_bul): n_rot is not set to a real positve number\n");
  fprintf(STDERR,"(leed_inp_read_bul): check the inputfile.\n exit now\n");
  exit(0);
#endif
 }
 
 i_layer = i_layer; /* remove unused warning */
 
 free(atoms_rd);

 bulk_par->dmin = R_fabs(bulk_par->layers[0].vec_from_last[3]);
 for(i=0; i < bulk_par->nlayers - 1 /* origin is not relevant */; i++)
 {
   bulk_par->dmin = 
            MIN(bulk_par->dmin, R_fabs(bulk_par->layers[i].vec_to_next[3]) );
 }

/************************************************************************
  Set the symmetry flags
  first number shows the geometry (1=monocline)(2=rec)
                                  (3=hexagonal)(4=quadrat)
  second number shows rotationaxe (2-fold,3-fold,4-fold)
  third number shows mirrorplanes (1,2,3,4)
*************************************************************************/
 vaux[1] = R_atan2(bulk_par->b[3],bulk_par->b[1]); 
 vaux[2] = R_atan2(bulk_par->b[4],bulk_par->b[2]);
 faux = R_fabs(vaux[1] - vaux[2]);

 vaux[0] = bulk_par->b[1]*bulk_par->b[2] + bulk_par->b[3]*bulk_par->b[4];
 vaux[1] = R_hypot(bulk_par->b[1],bulk_par->b[3]);
 vaux[2] = R_hypot(bulk_par->b[2],bulk_par->b[4]);

 if(bulk_par->n_mir > 0 || bulk_par->n_rot > 1)
 {
  if((R_fabs(faux - PI/3) < GEO_TOLERANCE || R_fabs(faux - 2*PI/3) < GEO_TOLERANCE) &&
     (R_fabs(vaux[1] - vaux[2]) < GEO_TOLERANCE)                                      )
  {
     bulk_par->symmetry = 300 + 10 * bulk_par->n_mir +  bulk_par->n_rot;
  }

  else if(R_fabs(vaux[0]) < GEO_TOLERANCE && 
     R_fabs(vaux[1] - vaux[2]) < GEO_TOLERANCE)
  {
    bulk_par->symmetry = 400 + 10 * bulk_par->n_mir +  bulk_par->n_rot;
  }   
 
  else if(R_fabs(vaux[0]) < GEO_TOLERANCE && 
     R_fabs(vaux[1] - vaux[2]) > GEO_TOLERANCE)
  {
    bulk_par->symmetry = 200 + 10 * bulk_par->n_mir +  bulk_par->n_rot;
  }

  else
  {
    bulk_par->symmetry = 100 + 10 * bulk_par->n_mir + bulk_par->n_rot;
  }
 } 

 else
 {
   bulk_par->symmetry = 101;
 }

#ifdef CONTROL
 switch(bulk_par->symmetry)
 {
   case NOSYM:
   { 
     fprintf(STDOUT,"\n(leed_inp_read_bul)SYMMETRY :\tTHE STRUCTURE HAS NO SYMMETRY\n\n");
      break;
   }
   case MONO_2ROT:
   { 
     fprintf(STDOUT,"\n(leed_inp_read_bul)SYMMETRY :\tTHE MONOCLINE STRUCTURE HAS ROTATIONSYMMETRY 2!\n\n");
     break;
   }
   case MONO_1MIR:
   {
     fprintf(STDOUT,"\n(leed_inp_read_bul)SYMMETRY :\tTHE MONOCLINE STRUCTURE HAS ONE MIRRORPLANE\n\n");
     break;
   }
   case REC_2ROT:
   { 
     fprintf(STDOUT,"\n(leed_inp_read_bul)SYMMETRY :\tTHE RECTANGULAR STRUCTURE HAS ROTATIONSYMMETRY 2!\n\n");
     break;
   }
   case REC_1MIR:
   { 
     fprintf(STDOUT,"(leed_inp_read_bul)SYMMETRY :\tTHE RECTANGULAR STRUCTURE HAS ONE MIRRORPLANE\n\n");
     break;
   }
   case REC_2MIR:
   { 
     fprintf(STDOUT,"(leed_inp_read_bul)SYMMETRY :\tTHE RECTANGULAR STRUCTURE HAS TWO MIRRORPLANES\n\n");
     break;
   }
   case HEX_3ROT:
   { 
     fprintf(STDOUT,"(leed_inp_read_bul)SYMMETRY :\tTHE HEXAGONAL STRUCTURE HAS ROTATIONSYMMETRY 3!\n\n");
     break;
   }
   case HEX_1MIR:
   {
     fprintf(STDOUT,"(leed_inp_read_bul)SYMMETRY :\tTHE HEXAGONAL STRUCTURE HAS ONE MIRRORPLANES\n\n");
     break;
   }

   case HEX_3MIR:
   { 
     fprintf(STDOUT,"(leed_inp_read_bul)SYMMETRY :\tTHE HEXAGONAL STRUCTURE HAS THREE MIRRORPLANES\n\n");
     break;
   }
   case SQ_2ROT:
   { 
     fprintf(STDOUT,"(leed_inp_read_bul)SYMMETRY :\tTHE SQUARE STRUCTURE HAS ROTATIONSYMMETRY 2! \n\n");
     break;
   }
   case SQ_4ROT:
   {
     fprintf(STDOUT,"(leed_inp_read_bul)SYMMETRY :\tTHE SQUARE STRUCTURE HAS ROTATIONSYMMETRY 4! \n\n");
     break;
   }
   case SQ_1MIR:
   { 
     fprintf(STDOUT,"(leed_inp_read_bul)SYMMETRY :\tTHE SQUARE STRUCTURE HAS ONE MIRRORPLANE \n\n");
     break;
   }
   case SQ_2MIR:
   { 
     fprintf(STDOUT,"(leed_inp_read_bul)SYMMETRY :\tTHE SQUARE STRUCTURE HAS TWO MIRRORPLANES\n\n");
     break;
   }
   case SQ_4MIR:
   { 
     fprintf(STDOUT,"(leed_inp_read_bul)SYMMETRY :\tTHE SQUARE STRUCTURE HAS FOUR MIRRORPLANES\n\n");
     break;
   }
   default:
   {
     fprintf(STDERR,"(leed_inp_read_bul)SYMMETRY :\tREAD NO SYMMETRY FLAGS\nexit now\n");
     exit(0);
     break;
   }

 }/* switch */
   
#endif

/************************************************************************
 write the structures phs_shifts and bulk_par back.
*************************************************************************/

 *p_bulk_par = bulk_par;

 return(1);
}
