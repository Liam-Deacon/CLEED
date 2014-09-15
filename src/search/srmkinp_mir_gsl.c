/***********************************************************************
 GH/02.04.03

Changes:
GH/30.12.04 - introduce bul for mirrored geometry

 file contains function:

 int sr_mkinp_mir_gsl(real *pos, int i_call, char *filename)

 Prepare (par)input file for CLEED program

 Changes:

GH/22.08.95 - Creation (copy from srmkinp.c)
SRP/31.03.03 - Added a section for the angle search
LD/01.07.2014 - Modified for compatibility with the GNU Scientific Library

***********************************************************************/

#include <stdio.h>
#include <string.h>
#include <gsl/gsl_vector.h>

#include "search.h"

#ifndef FAC_THETA
#define FAC_THETA 5.
#endif

#ifndef FAC_PHI
#define FAC_PHI   50.
#endif

extern struct sratom_str *sr_atoms;
extern struct search_str *sr_search;

char line_buffer[STRSZ];

int sr_mkinp_mir_gsl(gsl_vector *par, int i_call, char *filename)

/***********************************************************************
 - Set up inputfile for IV program
 
INPUT:
 filename: name of IV input file
***********************************************************************/

{
int i_atoms, i_par;
int i_str;

real faux;
real x, y, z;
real theta, phi; /* Added for the angle search (SRP 31.03.03) */

FILE *iv_par;
FILE *iv_bul_in;
FILE *iv_bul_out;
FILE *iv_bul_mir;


/* Open IV parameter file */
 iv_par = fopen(filename, "w"); 

/**********************************************************************/
/* Open IV bulk file for read and write */
 strncpy(line_buffer, filename, STRSZ);
 i_str = strlen(line_buffer) -3;

 sprintf(line_buffer+i_str, "bul");
 iv_bul_in = fopen(line_buffer, "r"); 

 sprintf(line_buffer+i_str, "bul_org");
 iv_bul_out= fopen(line_buffer, "w"); 

 sprintf(line_buffer+i_str, "bul_mir");
 iv_bul_mir= fopen(line_buffer, "w");
/**********************************************************************/

/* 
 Write comment lines to IV parameter file
*/
 fprintf(iv_par, "# overlayer (SEARCH No: %d)\n", i_call);

/* 
 Write all other lines to iv_input
*/

 for( i_atoms = 0; (sr_atoms + i_atoms)->type != I_END_OF_LIST; i_atoms ++)
 {
   x = (sr_atoms + i_atoms)->x;
   y = (sr_atoms + i_atoms)->y;
   z = (sr_atoms + i_atoms)->z;

   for(i_par = 1; i_par <= (sr_search->n_par_geo); i_par ++) 
   {
     faux = gsl_vector_get(par, i_par-1);
     if(!sr_search->z_only)
     {
       x += faux * (sr_atoms + i_atoms)->x_par[i_par];
       y += faux * (sr_atoms + i_atoms)->y_par[i_par];
     }
     z += faux * (sr_atoms + i_atoms)->z_par[i_par];
   }

   fprintf(iv_par, "po: %s %f %f %f dr1 %f\n",
                   (sr_atoms + i_atoms)->name,
                   x, y, z, 
                   (sr_atoms + i_atoms)->dr);
 } /* for i_atoms */


/* angle search */

 if(sr_search->sr_angle)
 {
   phi = sr_search->phi_0 + gsl_vector_get(par, (sr_search->i_par_phi)-1) * FAC_PHI;
   theta = sr_search->theta_0 + gsl_vector_get(par, (sr_search->i_par_theta)-1) * FAC_THETA;
 }
 else
 {
   phi = sr_search->phi_0;
   theta = sr_search->theta_0;
 }


 /* No negative theta angles are allowed, instead change phi by 180 degrees: */  
 if(theta < 0.)
 {
   theta *= -1.;
   phi += 180.;
 }
 

/*  Confine the phi angle between 0 and 360 degrees: */
 while(phi > 360.) { phi -= 360.; }
 while(phi < 0.) { phi += 360.; }


/**********************************************************************/

 while ( fgets(line_buffer, STRSZ, iv_bul_in) != NULL) 
 {
#ifdef CONTROL_X
   fprintf(STDCTR,"%s", line_buffer);
#endif
   /* find first non blank character */
   for( i_str = 0;  *(line_buffer+i_str) == ' '; i_str ++);
   
   if( !strncasecmp(line_buffer+i_str, "ip:",3) )
   {
     fprintf(iv_bul_out, "ip: %.2f\n", phi);
     fprintf(iv_bul_mir, "ip: %.2f\n", 180. - phi);
   }

   else if( !strncasecmp(line_buffer+i_str,"it:",3) )
   {
     fprintf(iv_bul_out, "it: %.2f\n", theta);
     fprintf(iv_bul_mir, "it: %.2f\n", theta);
   }
   else
   {
     fprintf(iv_bul_out, "%s", line_buffer);
     fprintf(iv_bul_mir, "%s", line_buffer);
   }
    
 }

  /* close input streams */
 fclose(iv_bul_in);
 fclose(iv_bul_out);
 fclose(iv_bul_mir);

/**********************************************************************/

 fclose(iv_par);
 return(1);
}
