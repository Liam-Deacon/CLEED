/*********************************************************************
  GH/07.08.95
  
  matwrite
     write matrix to a file.

  Changes:
GH/07.08.95 - Creation (copy from matext)
  
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mat.h"


/*======================================================================*/
/*======================================================================*/

int matwrite(mat M, FILE *file)

/*********************************************************************

  write matrix to a file.
  
  INPUT
    mat M  - input matrix
    FILE * file - (input) pointer to output file.

  RETURN VALUE: 
    total number of bytes if successful
   -1 if not

*********************************************************************/
{
unsigned int n_el;
int tot_size;

/********************************************************************
  Check the input matrix
********************************************************************/

 if (matcheck(M) < 1)
 {
#ifdef ERROR
  fprintf(STDERR," *** error (matwrite): input matrix does not exist \n");
#endif
#ifdef EXIT_ON_ERROR
  exit(1);
#else
  return(-1);
#endif
 }
 
/********************************************************************
 Diagonal Matrix:
********************************************************************/
 if (M->mat_type == MAT_DIAG) 
 {
#ifdef ERROR
  fprintf(STDERR,
   " *** error (matwrite): diagonal input matrix not implemented.\n");
#endif
#ifdef EXIT_ON_ERROR
  exit(1);
#else
  return(-1);
#endif
 }     /* if diagonal */

/********************************************************************
 Other matrix types:
********************************************************************/
 else
 {
/*
  First write matrix header
*/
   if( fwrite(M, sizeof(struct mat_str), 1, file) != 1 )
   {
#ifdef ERROR
 fprintf(STDERR,"*** error (matwrite): output error while writing header\n");
#endif
#ifdef EXIT_ON_ERROR
     exit(1);
#else
     return(-1);
#endif
   }

   tot_size = sizeof(struct mat_str);
   
/*
  Write real parts first (for real and complex matrices)
*/
   n_el = M->cols * M->rows;
   if( fwrite(M->rel+1, sizeof(real), n_el, file) != n_el )
   {
#ifdef ERROR
 fprintf(STDERR,"*** error (matwrite): output error while writing reals\n");
#endif
#ifdef EXIT_ON_ERROR
     exit(1);
#else
     return(-1);
#endif
   }
   tot_size += n_el * sizeof(real);

/*
  For complex matrix copy also imag. parts.
*/
   if(M->num_type == NUM_COMPLEX)
   {
     if( fwrite(M->iel+1, sizeof(real), n_el, file) != n_el )
     {
#ifdef ERROR
 fprintf(STDERR,"*** error (matwrite): output error while writing imags\n");
#endif
#ifdef EXIT_ON_ERROR
       exit(1);
#else
       return(-1);
#endif
     }
     tot_size += n_el * sizeof(real);
   } /* NUM_COMPLEX */
 }     /* else */

#ifdef CONTROL
 fprintf(STDCTR,"(matwrite): %d bytes written\n", tot_size);
#endif

 return(tot_size);
} /* end of function matwrite */
