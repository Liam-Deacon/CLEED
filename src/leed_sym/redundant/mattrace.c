/*********************************************************************
  GH/07.06.94

*********************************************************************/
#include <math.h>  
#include <stdio.h>

#include "mat.h"

/*
#define ERROR
*/
#define ERROR

int mattrace(mat tra, mat M)

/* 
  Calculate the trace of a square matrix M

  parameters:
  M - input matrix (must be square)
  tra - output trace

  return values: 1, if o.k.
                 0, if failed.
*/

{
real *ptr, *ptr_end;
int nn;
  
 if ((M == NULL) || (M->mat_type != MAT_SQUARE) )
 {
#ifdef ERROR
   fprintf(STDOUT," *** error (mattra): matrix has the wrong format\n");
#endif
   return(0);
 }
 else
 {

   tra->num_type = M->num_type;
   tra->mat_type = MAT_SCALAR;
   tra->cols = 1;
   tra->rows = 1;

   nn = M->cols * M->cols;

  switch(M->num_type)
  {
    case(NUM_REAL):
    {
      *(tra->rel) = 0.;
      for (ptr = M->rel + 1, ptr_end = M->rel + nn; ptr <= ptr_end; ptr++ )
      {
        *(tra->rel) += *ptr;
      } 
      break;
    } /* case REAL */

    case(NUM_COMPLEX):
    {
      *(tra->rel) = 0.;
      *(tra->iel) = 0.;
      for (ptr = M->rel + 1, ptr_end = M->rel + nn; ptr <= ptr_end; ptr++ )
      {
        *(tra->rel) += *ptr;
      }

      for (ptr = M->iel + 1, ptr_end = M->iel + nn; ptr <= ptr_end; ptr++ )
      {
        *(tra->rel) += *ptr;
      }
      break;
    } /* case COMPLEX */
  }  /* switch */
 }   /* else */
 return(1);
}  /* end of function matshow */
