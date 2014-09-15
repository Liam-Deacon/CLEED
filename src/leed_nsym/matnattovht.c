/*********************************************************************
  GH/17.07.95 
  file contains functions:

  matnattovht
  matline

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>

#include "leed.h"

/*======================================================================*/
/*======================================================================*/

int matnattovht (mat Mbg, int l_max, int n_atoms ) 

/************************************************************************

  Print a matrix in the same order as VHT 

*************************************************************************/
{

int l_max_2;
int i_atoms;
int l1, m1, off, i1, n1;

l_max_2 = (l_max + 1)*(l_max + 1);

 for (i_atoms = 0, off = 1, n1 = 0; i_atoms < n_atoms; 
      i_atoms++, off += l_max_2)
 {
   for(l1 = 0; l1 <= l_max; l1 += 2)
   {
     for(m1 = -l1; m1 <= l1; m1 += 2)
     {
       i1 = off + l1 * (l1+1) + m1;
       n1++;
       fprintf(STDOUT,"%3d:    ", n1);

       matline(Mbg, i1, l_max, l_max_2, n_atoms);
     } /* m1 */
   } /* l1 */

   for(l1 = 1; l1 <= l_max; l1 += 2)
   {
     for(m1 = -l1; m1 <= l1; m1 += 2)
     {
       i1 = off + l1 * (l1+1) + m1;
       n1++;
       fprintf(STDOUT,"%3d:    ", n1);

       matline(Mbg, i1, l_max, l_max_2, n_atoms);
     } /* m1 */
   } /* l1 */

   for(l1 = 1; l1 <= l_max; l1 += 2)
   {
     for(m1 = -l1+1; m1 <= l1; m1 += 2)
     {
       i1 = off + l1 * (l1+1) + m1;
       n1++;
       fprintf(STDOUT,"%3d:    ", n1);

       matline(Mbg, i1, l_max, l_max_2, n_atoms);
     } /* m1 */
   } /* l1 */

   for(l1 = 2; l1 <= l_max; l1 += 2)
   {
     for(m1 = -l1+1; m1 <= l1; m1 += 2)
     {
       i1 = off + l1 * (l1+1) + m1;
       n1++;
       fprintf(STDOUT,"%3d:    ", n1);

       matline(Mbg, i1, l_max, l_max_2, n_atoms);
     } /* m1 */
   } /* l1 */
 } /* i_atoms */
 fprintf(STDOUT,"\n");
 return 0;
} /* end of function matnattovht */
/*======================================================================*/

int matline( mat Mbg, int i1, int l_max, int l_max_2, int n_atoms)

{
int i2, n2, l2, m2, off2, i_atoms2;

 for (i_atoms2 = 0, n2 = 0, off2 = 1; i_atoms2 < n_atoms; 
             i_atoms2 ++, off2 += l_max_2)
 {
   for(l2 = 0; l2 <= l_max; l2 += 2)
   {
     for(m2 = -l2; m2 <= l2; m2 += 2)
     {
       i2 = off2 + l2 * (l2+1) + m2;
       n2 ++;

       fprintf(STDOUT,"(%6.3f,%6.3f)[%4.2f]",
               RMATEL(i1,i2,Mbg),IMATEL(i1,i2,Mbg),
               cri_abs(RMATEL(i1,i2,Mbg),IMATEL(i1,i2,Mbg)) );
       if(n2 % 6 == 0) fprintf(STDOUT,"\n  (%3d) ", n2+1);
     } /* m2 */
   } /* l2 */

   for(l2 = 1; l2 <= l_max; l2 += 2)
   {
     for(m2 = -l2; m2 <= l2; m2 += 2)
     {
       i2 = off2 + l2 * (l2+1) + m2;
       n2 ++;

       fprintf(STDOUT,"(%6.3f,%6.3f)[%4.2f]",
               RMATEL(i1,i2,Mbg),IMATEL(i1,i2,Mbg),
               cri_abs(RMATEL(i1,i2,Mbg),IMATEL(i1,i2,Mbg)) );
       if(n2 % 6 == 0) fprintf(STDOUT,"\n  (%3d) ", n2+1);
     } /* m2 */
   } /* l2 */

   for(l2 = 1; l2 <= l_max; l2 += 2)
   {
     for(m2 = -l2+1; m2 <= l2; m2 += 2)
     {
       i2 = off2 + l2 * (l2+1) + m2;
       n2 ++;

       fprintf(STDOUT,"(%6.3f,%6.3f)[%4.2f]",
               RMATEL(i1,i2,Mbg),IMATEL(i1,i2,Mbg),
               cri_abs(RMATEL(i1,i2,Mbg),IMATEL(i1,i2,Mbg)) );
       if(n2 % 6 == 0) fprintf(STDOUT,"\n  (%3d) ", n2+1);
     } /* m2 */
   } /* l2 */

   for(l2 = 2; l2 <= l_max; l2 += 2)
   {
     for(m2 = -l2+1; m2 <= l2; m2 += 2)
     {
       i2 = off2 + l2 * (l2+1) + m2;
       n2 ++;

       fprintf(STDOUT,"(%6.3f,%6.3f)[%4.2f]",
               RMATEL(i1,i2,Mbg),IMATEL(i1,i2,Mbg),
               cri_abs(RMATEL(i1,i2,Mbg),IMATEL(i1,i2,Mbg)) );
       if(n2 % 6 == 0) fprintf(STDOUT,"\n  (%3d) ", n2+1);
     } /* m2 */
   } /* l2 */
 } /* i_atoms2 */
 fprintf(STDOUT,"\n");
 return 0;
} /* end of function matline */
