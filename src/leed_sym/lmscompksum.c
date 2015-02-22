/**********************************************************************
  WB/24.08.98
  file contains functions:

  mssumR_p
    summation over all equivalent beams for the function leed_ms_compl_sym
  mssumR_m
    summation over all equivalent beams for the function leed_ms_compl_sym
 Changes:
**********************************************************************/

#include <math.h>
#include <stdio.h>

#include "leed.h"

/*===================================================================*/

mat leed_ms_comp_k_sum(mat Maux,leed_beam *beam,leed_atom *atom,
             int type ,int l_max, int sign)

/*===================================================================*/
{
real faux_r, faux_i, faux1_r, faux1_i;
real exp_k3_r, exp_k3_i;
real *ptr_r, *ptr_i;
real *ptr1_r, *ptr1_i;
int iaux, k, i, l, m;

if(sign <= 0)
{
if(type == BULK)
{
   for(k = 0; k < Maux->cols; k ++)
   {
/* common prefactor exp(i kz * z)/n_eqb_b */
     faux_r = (beam+k)->k_r[3]*atom->pos[3];
     faux_i = (beam+k)->k_i[3]*atom->pos[3];
     cri_expi(&exp_k3_r,&exp_k3_i,faux_r,faux_i);
     exp_k3_r /= cleed_real_sqrt((real)((beam + k)->n_eqb_b));
     exp_k3_i /= cleed_real_sqrt((real)((beam + k)->n_eqb_b));

     ptr_r = Maux->rel + 1 + k;
     ptr_i = Maux->iel + 1 + k;

/* First: m = 0: */

     faux_r = 0.;
     faux_i = 0.;
     for(i = 0; i < (beam + k)->n_eqb_b ; i++)
     {
       faux1_r = +(beam+k)->k_x_sym[i] * atom->pos[1]
                 +(beam+k)->k_y_sym[i] * atom->pos[2];
       cri_expi(&faux1_r, &faux1_i, faux1_r, 0.);
       faux_r += faux1_r;
       faux_i += faux1_i;
     }

     cri_mul(&faux_r, &faux_i, exp_k3_r, exp_k3_i, faux_r, faux_i);
/*
   offset of first occurence of m = 0 is 0,
   increment is 2, 4, 6, ...
*/
     iaux = 0;
     ptr1_r = ptr_r + iaux * Maux->cols;
     ptr1_i = ptr_i + iaux * Maux->cols;
     cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);

     for( i = 2, iaux += i; iaux < Maux->rows ; i += 2, iaux += i)
     {
       ptr1_r = ptr_r + iaux * Maux->cols;
       ptr1_i = ptr_i + iaux * Maux->cols;
       cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);
     }

/* Now: m != 0: */

     for(l = 1; l <= l_max; l ++)
     {
      m = -l;
       faux_r = 0.;
       faux_i = 0.;
       for(i = 0; i < (beam + k)->n_eqb_b ; i++)
       {
         faux1_r = +(beam+k)->k_x_sym[i] * atom->pos[1]
                   +(beam+k)->k_y_sym[i] * atom->pos[2]
                   -(beam+k)->k_p_sym[i] * m;
         cri_expi(&faux1_r, &faux1_i, faux1_r, 0.);
         faux_r += faux1_r;
         faux_i += faux1_i;
       }

       cri_mul(&faux_r, &faux_i, exp_k3_r, exp_k3_i, faux_r, faux_i);

/*
   offset of first occurence of -m is m*m,
   increment is 2|m|+2, 2|m|+4, ..
*/
       iaux = l*l;
       ptr1_r = ptr_r + iaux * Maux->cols;
       ptr1_i = ptr_i + iaux * Maux->cols;
       cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);

       for( i = 2*(l+1), iaux += i; iaux < Maux->rows ; i += 2, iaux += i)
       {
         ptr1_r = ptr_r + iaux * Maux->cols;
         ptr1_i = ptr_i + iaux * Maux->cols;
         cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);
       }

       m = +l;
       faux_r = 0.;
       faux_i = 0.;
       for(i = 0; i < (beam + k)->n_eqb_b ; i++)
       {
         faux1_r = +(beam+k)->k_x_sym[i] * atom->pos[1]
                   +(beam+k)->k_y_sym[i] * atom->pos[2]
                   -(beam+k)->k_p_sym[i] * m;
         cri_expi(&faux1_r, &faux1_i, faux1_r, 0.);
         faux_r += faux1_r;
         faux_i += faux1_i;
       }

       cri_mul(&faux_r, &faux_i, exp_k3_r, exp_k3_i, faux_r, faux_i);

/*
   offset of first occurence of +m is (m+1)*(m+1) -1,
   increment is 2|m|+2, 2|m|+4, ..
*/
       iaux = (l+1)*(l+1) - 1;
       ptr1_r = ptr_r + iaux * Maux->cols;
       ptr1_i = ptr_i + iaux * Maux->cols;
       cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);

       for(i = 2*(l+1), iaux += i; iaux < Maux->rows ; i += 2, iaux += i)
       {
         ptr1_r = ptr_r + iaux * Maux->cols;
         ptr1_i = ptr_i + iaux * Maux->cols;
         cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);
       }

     } /*l*/
   } /*k*/
}/*if bulk*/

 if(type == OVER)
 {
   for(k = 0; k < Maux->cols; k ++)
   {

/* common prefactor exp(i kz * z)/n_eqb_s */
     faux_r = (beam+k)->k_r[3]*atom->pos[3];
     faux_i = (beam+k)->k_i[3]*atom->pos[3];
     cri_expi(&exp_k3_r,&exp_k3_i,faux_r,faux_i);
     exp_k3_r /= cleed_real_sqrt((real)((beam + k)->n_eqb_s));
     exp_k3_i /= cleed_real_sqrt((real)((beam + k)->n_eqb_s));

     ptr_r = Maux->rel + 1 + k;
     ptr_i = Maux->iel + 1 + k;

/* First: m = 0: */

     faux_r = 0.;
     faux_i = 0.;
     for(i = 0; i < (beam + k)->n_eqb_s ; i++)
     {
       faux1_r = +(beam+k)->k_x_sym[i] * atom->pos[1]
                 +(beam+k)->k_y_sym[i] * atom->pos[2];
       cri_expi(&faux1_r, &faux1_i, faux1_r, 0.);
       faux_r += faux1_r;
       faux_i += faux1_i;
     }

     cri_mul(&faux_r, &faux_i, exp_k3_r, exp_k3_i, faux_r, faux_i);
/*
   offset of first occurence of m = 0 is 0,
   increment is 2, 4, 6, ...
*/
     iaux = 0;
     ptr1_r = ptr_r + iaux * Maux->cols;
     ptr1_i = ptr_i + iaux * Maux->cols;
     cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);

     for( i = 2, iaux += i; iaux < Maux->rows ; i += 2, iaux += i)
     {
       ptr1_r = ptr_r + iaux * Maux->cols;
       ptr1_i = ptr_i + iaux * Maux->cols;
       cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);
     }

/* Now: m != 0: */
     for(l = 1; l <= l_max; l ++)
     {
       m = -l;
       faux_r = 0.;
       faux_i = 0.;
       for(i = 0; i < (beam + k)->n_eqb_s ; i++)
       {
         faux1_r = +(beam+k)->k_x_sym[i] * atom->pos[1]
                   +(beam+k)->k_y_sym[i] * atom->pos[2]
                   -(beam+k)->k_p_sym[i] * m;
         cri_expi(&faux1_r, &faux1_i, faux1_r, 0.);
         faux_r += faux1_r;
         faux_i += faux1_i;
       }

       cri_mul(&faux_r, &faux_i, exp_k3_r, exp_k3_i, faux_r, faux_i);

/*
   offset of first occurence of -m is m*m,
   increment is 2|m|+2, 2|m|+4, ..
*/
       iaux = l*l;
       ptr1_r = ptr_r + iaux * Maux->cols;
       ptr1_i = ptr_i + iaux * Maux->cols;
       cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);

       for( i = 2*(l+1), iaux += i; iaux < Maux->rows ; i += 2, iaux += i)
       {
         ptr1_r = ptr_r + iaux * Maux->cols;
         ptr1_i = ptr_i + iaux * Maux->cols;
         cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);
       }

       m = +l;
       faux_r = 0.;
       faux_i = 0.;
       for(i = 0; i < (beam + k)->n_eqb_s ; i++)
       {
         faux1_r = +(beam+k)->k_x_sym[i] * atom->pos[1]
                   +(beam+k)->k_y_sym[i] * atom->pos[2]
                   -(beam+k)->k_p_sym[i] * m;
         cri_expi(&faux1_r, &faux1_i, faux1_r, 0.);
         faux_r += faux1_r;
         faux_i += faux1_i;
       }

       cri_mul(&faux_r, &faux_i, exp_k3_r, exp_k3_i, faux_r, faux_i);

/*
   offset of first occurence of +m is (m+1)*(m+1) -1,
   increment is 2|m|+2, 2|m|+4, ..
*/
       iaux = (l+1)*(l+1) - 1;
       ptr1_r = ptr_r + iaux * Maux->cols;
       ptr1_i = ptr_i + iaux * Maux->cols;
       cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);

       for(i = 2*(l+1), iaux += i; iaux < Maux->rows ; i += 2, iaux += i)
       {
         ptr1_r = ptr_r + iaux * Maux->cols;
         ptr1_i = ptr_i + iaux * Maux->cols;
         cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);
       }

     } /*l*/
   } /*k*/
 }/*type = OVER */

}/*if sign <= 0*/


if(sign > 0)
{
 if(type == BULK)
 {
   for(k = 0; k < Maux->cols; k ++)
   {

/* common prefactor exp(-i kz * z)/n_eqb_b */
     faux_r = -(beam+k)->k_r[3]*atom->pos[3];
     faux_i = -(beam+k)->k_i[3]*atom->pos[3];
     cri_expi(&exp_k3_r,&exp_k3_i,faux_r,faux_i);
     exp_k3_r /= cleed_real_sqrt((real)((beam + k)->n_eqb_b));
     exp_k3_i /= cleed_real_sqrt((real)((beam + k)->n_eqb_b));

     ptr_r = Maux->rel + 1 + k;
     ptr_i = Maux->iel + 1 + k; 

/* First: m = 0: */

     faux_r = 0.;
     faux_i = 0.;
     for(i = 0; i < (beam + k)->n_eqb_b ; i++)
     {
       faux1_r = +(beam+k)->k_x_sym[i] * atom->pos[1]
                 +(beam+k)->k_y_sym[i] * atom->pos[2];
       cri_expi(&faux1_r, &faux1_i, faux1_r, 0.);
       faux_r += faux1_r;
       faux_i += faux1_i;
     }
       
     cri_mul(&faux_r, &faux_i, exp_k3_r, exp_k3_i, faux_r, faux_i);
/*
   offset of first occurence of m = 0 is 0,
   increment is 2, 4, 6, ...
*/
     iaux = 0;
     ptr1_r = ptr_r + iaux * Maux->cols;
     ptr1_i = ptr_i + iaux * Maux->cols;
     cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);

     for( i = 2, iaux += i; iaux < Maux->rows ; i += 2, iaux += i)
     {
       ptr1_r = ptr_r + iaux * Maux->cols;
       ptr1_i = ptr_i + iaux * Maux->cols;
       cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);
     }

/* Now: m != 0: */
     for(l = 1; l <= l_max; l ++)
     {
      m = -l;
       faux_r = 0.;
       faux_i = 0.;
       for(i = 0; i < (beam + k)->n_eqb_b ; i++)
       {
         faux1_r = +(beam+k)->k_x_sym[i] * atom->pos[1]
                   +(beam+k)->k_y_sym[i] * atom->pos[2]
                   -(beam+k)->k_p_sym[i] * m;
         cri_expi(&faux1_r, &faux1_i, faux1_r, 0.);
         faux_r += faux1_r;
         faux_i += faux1_i;
       }

       cri_mul(&faux_r, &faux_i, exp_k3_r, exp_k3_i, faux_r, faux_i);

/*
   offset of first occurence of -m is m*m,
   increment is 2|m|+2, 2|m|+4, ..
*/
       iaux = l*l;
       ptr1_r = ptr_r + iaux * Maux->cols;
       ptr1_i = ptr_i + iaux * Maux->cols;
       cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);

       for( i = 2*(l+1), iaux += i; iaux < Maux->rows ; i += 2, iaux += i)
       {
         ptr1_r = ptr_r + iaux * Maux->cols;
         ptr1_i = ptr_i + iaux * Maux->cols;
         cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);
       }

       m = +l;
       faux_r = 0.;
       faux_i = 0.;
       for(i = 0; i < (beam + k)->n_eqb_b ; i++)
       {
         faux1_r = +(beam+k)->k_x_sym[i] * atom->pos[1]
                   +(beam+k)->k_y_sym[i] * atom->pos[2]
                   -(beam+k)->k_p_sym[i] * m;
         cri_expi(&faux1_r, &faux1_i, faux1_r, 0.);
         faux_r += faux1_r;
         faux_i += faux1_i;
       }

       cri_mul(&faux_r, &faux_i, exp_k3_r, exp_k3_i, faux_r, faux_i);

/*
   offset of first occurence of +m is (m+1)*(m+1) -1,
   increment is 2|m|+2, 2|m|+4, ..
*/
       iaux = (l+1)*(l+1) - 1;
       ptr1_r = ptr_r + iaux * Maux->cols;
       ptr1_i = ptr_i + iaux * Maux->cols;
       cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);

       for(i = 2*(l+1), iaux += i; iaux < Maux->rows ; i += 2, iaux += i)
       {
         ptr1_r = ptr_r + iaux * Maux->cols;
         ptr1_i = ptr_i + iaux * Maux->cols;
         cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);
       }

     } /*l*/
   } /*k*/
 }/*type = BULK */

 if(type == OVER)
 {
   for(k = 0; k < Maux->cols; k ++)
   {

/* common prefactor exp(-i kz * z)/n_eqb_s */
     faux_r = -(beam+k)->k_r[3]*atom->pos[3];
     faux_i = -(beam+k)->k_i[3]*atom->pos[3];
     cri_expi(&exp_k3_r,&exp_k3_i,faux_r,faux_i);
     exp_k3_r /= cleed_real_sqrt((real)((beam + k)->n_eqb_s));
     exp_k3_i /= cleed_real_sqrt((real)((beam + k)->n_eqb_s));

     ptr_r = Maux->rel + 1 + k;
     ptr_i = Maux->iel + 1 + k;

/* First: m = 0: */

     faux_r = 0.;
     faux_i = 0.;
     for(i = 0; i < (beam + k)->n_eqb_s ; i++)
     {
       faux1_r = +(beam+k)->k_x_sym[i] * atom->pos[1]
                 +(beam+k)->k_y_sym[i] * atom->pos[2];
       cri_expi(&faux1_r, &faux1_i, faux1_r, 0.);
       faux_r += faux1_r;
       faux_i += faux1_i;
     }
     
     cri_mul(&faux_r, &faux_i, exp_k3_r, exp_k3_i, faux_r, faux_i);
/*
   offset of first occurence of m = 0 is 0,
   increment is 2, 4, 6, ...
*/
     iaux = 0;
     ptr1_r = ptr_r + iaux * Maux->cols;
     ptr1_i = ptr_i + iaux * Maux->cols;
     cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);

     for( i = 2, iaux += i; iaux < Maux->rows ; i += 2, iaux += i)
     {
       ptr1_r = ptr_r + iaux * Maux->cols;
       ptr1_i = ptr_i + iaux * Maux->cols;
       cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);
     }  

/* Now: m != 0: */
     for(l = 1; l <= l_max; l ++)
     {
       m = -l;
       faux_r = 0.;
       faux_i = 0.;
       for(i = 0; i < (beam + k)->n_eqb_s ; i++)
       {
         faux1_r = +(beam+k)->k_x_sym[i] * atom->pos[1]
                   +(beam+k)->k_y_sym[i] * atom->pos[2]
                   -(beam+k)->k_p_sym[i] * m;
         cri_expi(&faux1_r, &faux1_i, faux1_r, 0.);
         faux_r += faux1_r;
         faux_i += faux1_i;
       }

       cri_mul(&faux_r, &faux_i, exp_k3_r, exp_k3_i, faux_r, faux_i);
/*
   offset of first occurence of -m is m*m,
   increment is 2|m|+2, 2|m|+4, ..
*/
       iaux = l*l;
       ptr1_r = ptr_r + iaux * Maux->cols;
       ptr1_i = ptr_i + iaux * Maux->cols;
       cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);

       for( i = 2*(l+1), iaux += i; iaux < Maux->rows ; i += 2, iaux += i)
       {
         ptr1_r = ptr_r + iaux * Maux->cols;
         ptr1_i = ptr_i + iaux * Maux->cols;
         cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);
       }

       m = +l;
       faux_r = 0.;
       faux_i = 0.;
       for(i = 0; i < (beam + k)->n_eqb_s ; i++)
       {
         faux1_r = +(beam+k)->k_x_sym[i] * atom->pos[1]
                   +(beam+k)->k_y_sym[i] * atom->pos[2]
                   -(beam+k)->k_p_sym[i] * m;
         cri_expi(&faux1_r, &faux1_i, faux1_r, 0.);
         faux_r += faux1_r;
         faux_i += faux1_i;
       }

       cri_mul(&faux_r, &faux_i, exp_k3_r, exp_k3_i, faux_r, faux_i);

/*
   offset of first occurence of +m is (m+1)*(m+1) -1,
   increment is 2|m|+2, 2|m|+4, ..
*/
       iaux = (l+1)*(l+1) - 1;
       ptr1_r = ptr_r + iaux * Maux->cols;
       ptr1_i = ptr_i + iaux * Maux->cols;
       cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);

       for(i = 2*(l+1), iaux += i; iaux < Maux->rows ; i += 2, iaux += i)
       {
         ptr1_r = ptr_r + iaux * Maux->cols;
         ptr1_i = ptr_i + iaux * Maux->cols;
         cri_mul(ptr1_r, ptr1_i, *ptr1_r, *ptr1_i, faux_r, faux_i);
       }

     } /*l*/
   } /*k*/
 }/*type = OVER */
 

}/*if sign > 0*/

return(Maux);
}/*end of function*/
