/*********************************************************************
  GH/19.09.00
  matshow
    Show the elements of a matrix:

  Changes:
  
  GH/11.08.94 - Diagonal matrices
  GH/23.01.95 - Does not display 1x1 matrices! (Don't know why)
  GH/19.09.00 - Change CFORM to CFORM_2 (no more abs. value)
  
*********************************************************************/
#include <math.h>  
#include <stdio.h>

#include "mat.h"

#define RFORM     "%6.3f "
#define R_DIAFORM "  *.** "
/*
#define CFORM "%6.2f "
#define CFORM_3 "(%6.3f,%6.3f)[%4.2f]"
*/
#define CFORM_2    "(%7.4f,%7.4f) "
#define CFORM_ZERO "  -.----, -.----  "
#define MIN_VALUE  0.00005

#define CMAXCOL  16
#define C_DIAFORM "( *.**, *.**) "

/*======================================================================*/
/*======================================================================*/

int matshow(mat M)
/*********************************************************************

 Show the elements of a matrix (STDOUT).

 RETURN VALUES: 
   1, if o.k.
   0, if failed.

*********************************************************************/
{
int i_r, i_c;
  
 if (matcheck(M) < 1)
 {
#ifdef ERROR
  fprintf(STDOUT," *** error (matshow): matrix does not exist \n");
#endif
  return(0);
 }
 else
 {
  fprintf(STDOUT,"(%d rows) x (%d columns):\n", M->rows, M->cols);
  switch(M->mat_type)
  {
    case(MAT_NORMAL): case(MAT_SQUARE):
    {
      switch(M->num_type)
      {
       case(NUM_REAL):
       {
        for (i_r = 1; i_r <= M->rows; i_r ++ )
        {
         for (i_c = 1; i_c <= M->cols; i_c ++)
         {
           fprintf(STDOUT,RFORM, RMATEL(i_r,i_c,M));
         }
         fprintf(STDOUT,"\n");
        } 
        fprintf(STDOUT,"\n");
        break;
       } /* case REAL */

       case(NUM_COMPLEX):
       {
        for (i_r = 1; i_r <= M->rows; i_r ++ )
        {
         fprintf(STDOUT,"%3d:    ", i_r);
         for (i_c = 1; i_c <= M->cols; i_c ++)
         {
/*
           fprintf(STDOUT,CFORM_3,RMATEL(i_r,i_c,M),IMATEL(i_r,i_c,M),
                                cri_abs(RMATEL(i_r,i_c,M),IMATEL(i_r,i_c,M)) );
*/
           if( (R_fabs(RMATEL(i_r,i_c,M)) < MIN_VALUE) && 
               (R_fabs(IMATEL(i_r,i_c,M)) < MIN_VALUE) )
             fprintf(STDOUT,CFORM_ZERO);
           else
             fprintf(STDOUT,CFORM_2,RMATEL(i_r,i_c,M),IMATEL(i_r,i_c,M) );
           if( (i_c % CMAXCOL == 0) && (i_c < M->cols) )
             fprintf(STDOUT,"\n  (%3d) ", i_c+1);
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
        for (i_r = 1; i_r <= M->cols; i_r ++ )
        {
         for (i_c = 1; i_c <= M->cols; i_c ++ )
         {
           if(i_r == i_c) fprintf(STDOUT,RFORM, M->rel[i_r]);
           else           fprintf(STDOUT,R_DIAFORM);
         }
         fprintf(STDOUT,"\n");
        }
        fprintf(STDOUT,"\n");
        break;
       } /* case REAL */

       case(NUM_COMPLEX):
       {
        for (i_r = 1; i_r <= M->cols; i_r ++ )
        {
         for (i_c = 1; i_c <= M->cols; i_c ++ )
         {
           if(i_r == i_c) fprintf(STDOUT,CFORM_2, M->rel[i_r], M->iel[i_r]);
           else           fprintf(STDOUT,C_DIAFORM);
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
