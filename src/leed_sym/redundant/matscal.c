/*********************************************************************
  GH/12.04.95
mat matscal( mat Mr, mat Mi, real num_r, real num_i )
  Multiply a matrix with a complex number.

Changes:
  GH/12.04.95 - Creation

*********************************************************************/
#include <math.h>   
#include <stdio.h>
#include <stdlib.h>

#include "mat.h"

/*
#define CONTROL
*/
#define ERROR
#define EXIT_ON_ERROR

mat matscal( mat Mr, mat Mi, real num_r, real num_i )

/*********************************************************************
  Multiply a matrix with a complex number: Mr = num * Mi.

  parameters:
  Mr - pointer to the matrix containing the result of the multiplication.
       If NULL, the pointer will be created and returned.

  Mi - pointer to the matrix to be multiplied.

  num_r, num_i - real and imaginary part of the scaling factor.

  return value: Mr

*********************************************************************/

{

/********************************************************************* 
  check input matrix 
*********************************************************************/

/* check validity of the input matrices */
 if (matcheck(Mi) < 1)
 {
#ifdef ERROR
  fprintf(STDERR," *** error (matscal): ivalid input matrix\n");
#endif
#ifdef EXIT_ON_ERROR
  exit(1);
#else
  return(NULL);
#endif
 }

/*********************************************************************
  copy Mi to Mr
*********************************************************************/

 Mr = matcop(Mr, Mi);

/********************************************************************* 
  Perform multiplication 
*********************************************************************/
#ifdef CONTROL
  fprintf(STDCTR," (matscal) start multiplication \n");
#endif
  switch(Mr->num_type)
  {
/*
 real matrix
*/
   case (NUM_REAL):
   { 
    if( num_i == 0.)         /* num is real */
    {
      register real *ptr_end;
      register real *ptrr;

#ifdef CONTROL
      fprintf(STDCTR," (matscal) entering real loop\n");
#endif
      for( ptrr = Mr->rel + 1, ptr_end = Mr->rel + Mr->cols*Mr->rows;
           ptrr <= ptr_end; ptrr ++)
      { *ptrr *= num_r; }
    } /* if num_i == 0. */

    else                    /* num is complex */
    {
      register real *ptr_end;
      register real *ptrr, *ptri;

      Mr->iel = (real*)calloc( Mr->cols*Mr->rows * 1, sizeof(real));
      Mr->num_type = NUM_COMPLEX;

      for( ptrr = Mr->rel + 1, ptri = Mr->iel + 1,
           ptr_end = Mr->rel + Mr->cols*Mr->rows;
           ptrr <= ptr_end; ptrr ++, ptri ++)
      {
        *ptrr = *ptrr * num_r;   /* real part */
        *ptri = *ptrr * num_i;   /* imaginary part */
      }
    }
    break;
   }  /* case REAL */

/*
 complex matrix
*/
   case (NUM_COMPLEX):
   {
#ifdef CONTROL
     fprintf(STDCTR,"(matscal): entering complex loop\n");
#endif
    register real faux;
    register real *ptr_end;
    register real *ptrr, *ptri;

#ifdef CONTROL
    fprintf(STDCTR," (matscal) entering real loop\n");
#endif
    for( ptrr = Mr->rel + 1, ptri = Mr->iel + 1,
         ptr_end = Mr->rel + Mr->cols*Mr->rows;
         ptrr <= ptr_end; ptrr ++, ptri ++)
    { 
      faux  = *ptrr * num_r - *ptri * num_i;   /* real part */
      *ptri = *ptrr * num_i + *ptri * num_r;   /* imaginary part */
      *ptrr = faux;
    }
    break;
   }  /* case COMPLEX */
  }   /* switch */

  return(Mr);

}  /* end of function matscal */
