/*********************************************************************
  GH/16.08.94
  matshowabs
    Show the modulus of the elements of a matrix:

  Changes:
  
*********************************************************************/
#include <math.h>  
#include <stdio.h>

#include "mat.h"

/*
#define ERROR
*/
#define ERROR

#define MINVAL  1.e-6
#define ZFORM   " ---  "
#define RMAXCOL  50
#define RFORM     "%5.3f "
#define R_DIAFORM "  *.** "
#define C_DIAFORM "( *.**, *.**) "

/*======================================================================*/
/*======================================================================*/

int matshowabs(mat M)
/*********************************************************************

 Show the modulus of the elements of a matrix (STDOUT).

 RETURN VALUES: 
   1, if o.k.
   0, if failed.

*********************************************************************/
{
int i_r, i_c;
int maxcol;

real faux;
  
 if (matcheck(M) < 1)
 {
#ifdef ERROR
  fprintf(STDOUT," *** error (matshowabs): matrix does not exist \n");
#endif
  return(0);
 }
 else
 {
  maxcol = MIN(M->cols, RMAXCOL);
  fprintf(STDOUT," *** abs ***  (%d rows) x (%d columns); --- < %.0e:\n", 
                   M->rows, M->cols, MINVAL);
  switch(M->mat_type)
  {
    case(MAT_NORMAL): case(MAT_SQUARE):
    {
      switch(M->num_type)
      {
       case(NUM_REAL):
       {
        for (i_r = 1; i_r <= M->rows; ++i_r )
        {
         for (i_c = 1; i_c <= maxcol; ++i_c)
         {
           if( (faux = R_fabs(RMATEL(i_r,i_c,M))) > MINVAL)
             fprintf(STDOUT,RFORM, faux);
           else
             fprintf(STDOUT,ZFORM);
         }
         fprintf(STDOUT,"\n");
        } 
        fprintf(STDOUT,"\n");
        break;
       } /* case REAL */

       case(NUM_COMPLEX):
       {
        for (i_r = 1; i_r <= M->rows; ++i_r )
        {
         for (i_c = 1; i_c <= maxcol; ++i_c)
         {
           if( (faux = cri_abs(RMATEL(i_r,i_c,M),IMATEL(i_r,i_c,M))) > MINVAL)
             fprintf(STDOUT,RFORM, faux);
           else
             fprintf(STDOUT,ZFORM);
         }
         fprintf(STDOUT,"\n");
        }
        fprintf(STDOUT,"\n");
        break;
       } /* case COMPLEX */
      }  /* switch num_type */ 
      break;
    }  /* MAT_NORMAL, MAT_SQUARE */

    case(MAT_DIAG):
    {
      switch(M->num_type)
      {
       case(NUM_REAL):
       {
        for (i_r = 1; i_r <= M->cols; ++i_r )
        {
         for (i_c = 1; i_c <= maxcol; ++i_c)
         {
           if(i_r == i_c) fprintf(STDOUT,RFORM, R_fabs(M->rel[i_r]));
           else           fprintf(STDOUT,R_DIAFORM);
         }
         fprintf(STDOUT,"\n");
        }
        fprintf(STDOUT,"\n");
        break;
       } /* case REAL */

       case(NUM_COMPLEX):
       {
        for (i_r = 1; i_r <= M->cols; ++i_r )
        {
         for (i_c = 1; i_c <= maxcol; ++i_c)
         {
           if(i_r == i_c) fprintf(STDOUT,RFORM, cri_abs(M->rel[i_r], M->iel[i_r]));
           else           fprintf(STDOUT,R_DIAFORM);
         }
         fprintf(STDOUT,"\n");
        }
        fprintf(STDOUT,"\n");
        break;
       } /* case COMPLEX */
      }  /* switch num_type */
      break;
    }  /* MAT_DIAG */
  }  /* switch mat_type */
 }   /* else */
 return(1);
}  /* end of function matshow */
