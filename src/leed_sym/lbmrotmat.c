/*********************************************************************
GH/03.09.97
  file contains function:

  leed_beam_get_rotation_matrices      Create an array of (2x2) rotation matrices
  leed_beam_rotation_matrix_free  Free memory allocated in leed_beam_get_rotation_matrices

Changes:

GH/30.08.97 - Creation
GH/03.09.97 - Add leed_beam_rotation_matrix_free

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>

#include "leed.h"


real ** leed_beam_get_rotation_matrices(int n_rot)

/************************************************************************

 Return an array of (2x2) rotation matrices for all (n_rot-1) rotations
 possible under a n_rot-fold symmetry.

 INPUT:
  int n_rot - degree of rotational symmetry.

 RETURN VALUE:

  real ** R_n - array of matrices where R_n[n] describes the rotation
              about phi = n*2pi/n_rot.
              The matrix elements are stored as follows:
              R11 =  cos(phi) = R_n[n][1]
              R12 = -sin(phi) = R_n[n][2]
              R21 =  sin(phi) = R_n[n][3]
              R22 =  cos(phi) = R_n[n][4]
              
*************************************************************************/
{
int i_rot;

real faux;
real ** R_n;

 if( (n_rot < 1) || (n_rot == 5) || (n_rot > 6) )
 {
#ifdef ERROR
   fprintf(STDERR,"*** error (leed_beam_get_rotation_matrices): wrong argument n_rot = %d.\n", n_rot);
#endif
   return(NULL);
 }
 else
 {
/************************************************************************
 Allocate memory for R_n
*************************************************************************/

   R_n = (real **) malloc( (n_rot + 1) * sizeof(real *) );
   for(i_rot = 1; i_rot < n_rot; i_rot ++)
   {
     *(R_n + i_rot) = (real *) malloc( 5 * sizeof(real) );
   }

/************************************************************************
 Calculate matrices
*************************************************************************/

   switch(n_rot)
   {
     case(1):
     {
       R_n[1] = NULL;
       break;
     } /* n_rot = 1 */

     case(2):
     {
       R_n[1][1] = R_n[1][4] = -1.;
       R_n[1][2] = R_n[1][3] = 0.;

       R_n[2] = NULL;
       break;
     } /* n_rot = 2 */

     case(3):
     {
       faux = 2.*PI / n_rot;

       R_n[1][1] = R_n[1][4] = R_cos(faux);
       R_n[1][2] = - ( R_n[1][3] = R_sin(faux) );

   /* 2*phi = -phi */
       R_n[2][1] = R_n[2][4] = R_n[1][1];
       R_n[2][2] = - R_n[1][2];
       R_n[2][3] = - R_n[1][3];

       R_n[3] = NULL;
       break;
     } /* n_rot = 3 */
     
     case(4):
     {
   /* 90 deg */
       R_n[1][1] = R_n[1][4] = 0.;
       R_n[1][2] = - ( R_n[1][3] = 1.);

   /* 180 deg */
       R_n[2][1] = R_n[2][4] = -1.;
       R_n[2][2] = R_n[2][3] = 0.;

   /* 270 deg = -90 deg */
       R_n[3][1] = R_n[3][4] = 0.;
       R_n[3][2] = - ( R_n[3][3] = -1.);

       R_n[4] = NULL;
       break;
     }  /* n_rot = 4 */

     case(6):
     {
       faux = 2.*PI / n_rot;

       R_n[1][1] = R_n[1][4] = R_cos(faux);
       R_n[1][2] = - ( R_n[1][3] = R_sin(faux) );

   /* 5*phi = -phi */
       R_n[5][1] = R_n[5][4] = R_n[1][1];
       R_n[5][2] = - R_n[1][2];
       R_n[5][3] = - R_n[1][3];

   /* 2*phi */
       faux = PI / n_rot;
       R_n[2][1] = R_n[2][4] = R_cos(faux);
       R_n[2][2] = - ( R_n[2][3] = R_sin(faux) );

   /* 4*phi = -2*phi */
       R_n[4][1] = R_n[4][4] = R_n[2][1];
       R_n[4][2] = - R_n[2][2];
       R_n[4][3] = - R_n[2][3];

   /* 3*phi = 180 deg */
       R_n[3][1] = R_n[3][4] = -1.;
       R_n[3][2] = R_n[3][3] = 0.;
       
       R_n[6] = NULL;
       break;
     } /* n_rot = 6 */
   }  /* switch */

   return(R_n);
 } /* else */
}  /* end of function leed_beam_get_rotation_matrices */

/*************************************************************************/

int leed_beam_rotation_matrix_free(real ** R_n)

/************************************************************************

 Free memory associated with the matrix array R_n created in function
 leed_beam_get_rotation_matrices.

 INPUT:
  real ** R_n matrix array to be freeed

 RETURN VALUE:

  int i_rot: number of matrices found and freeed.
  -1         if failed

*************************************************************************/
{
int i_rot;

 for(i_rot = 1; R_n[i_rot] != NULL; i_rot ++)
 {
   free(*(R_n + i_rot));
 }
 free(R_n);

 return(i_rot);

}  /* end of function leed_beam_rotation_matrix_free */
