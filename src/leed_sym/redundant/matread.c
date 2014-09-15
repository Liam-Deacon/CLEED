/*********************************************************************
  GH/09.08.95
  
  matread
     read matrix from a file.

  Changes:
GH/07.08.95 - Creation (copy from matwrite)
GH/09.08.95 - Allocate mat_str for NULL input
  
*********************************************************************/

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#include "mat.h"

/*
#define ERROR
*/

#define CONTROL
#define ERROR
#define EXIT_ON_ERROR

/*======================================================================*/
/*======================================================================*/

mat matread(mat M, FILE *file)

/*********************************************************************

  Read matrix from a file.
  
  INPUT
    mat M  - (input) pointer to matrix (can be NULL).
    FILE * file - (input) pointer to input file.

  RETURN VALUE: 
    M if successful
    NULL if not

*********************************************************************/
{
int n_el, size;
int check;
int tot_size;

/********************************************************************
  Check the input matrix
********************************************************************/

 if( (check = matcheck(M)) < 0)
 {
#ifdef ERROR
   fprintf(STDERR," *** error (matread): input matrix has improper type\n");
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(NULL);
#endif
 }
 else 
 {
   if(check == 0) 
     M = (mat) malloc( sizeof(struct mat_str) );
   else
   {
     free(M->rel); M->rel = NULL;
     free(M->iel); M->iel = NULL;
   }
 }

/********************************************************************
 First read matrix header
********************************************************************/
   if( fread(M, sizeof(struct mat_str), 1, file) != 1 )
   {
#ifdef ERROR
     fprintf(STDERR,
     "*** error (matread): input error while reading header\n");
#endif
#ifdef EXIT_ON_ERROR
     exit(1);
#else
     return(NULL);
#endif
   } 
   tot_size = sizeof(struct mat_str);
   
   if( M->mag_no != MATRIX )
   {
#ifdef ERROR
     fprintf(STDERR,
     "*** error (matread): input error (magic number)\n");
#endif
#ifdef EXIT_ON_ERROR
     exit(1);
#else
     return(NULL);
#endif
   }
 
/********************************************************************
 Diagonal Matrix: Not implemented
********************************************************************/
 if (M->mat_type == MAT_DIAG) 
 {
#ifdef ERROR
   fprintf(STDERR,
   " *** error (matread): diagonal matrix not implemented.\n");
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(NULL);
#endif
 }     /* if diagonal */

/********************************************************************
 Other matrix types:
********************************************************************/
 else
 {
/*
  Read real parts first (for real and complex matrices)
*/
   n_el = M->cols * M->rows;
   size = (n_el+1) * sizeof(real);

   if( (M->rel = (real *) malloc(size)) == NULL)
   {
#ifdef ERROR
     fprintf(STDERR,"*** error (matread): allocation error (reals)\n");
#endif
#ifdef EXIT_ON_ERROR
     exit(1);
#else
     return(NULL);
#endif
   }

   M->iel = NULL;          /* (for now, will be changed for complex M) */

   if( fread(M->rel+1, sizeof(real), n_el, file) != n_el )
   {
#ifdef ERROR
 fprintf(STDERR,"*** error (matread): input error while reading reals\n");
#endif
#ifdef EXIT_ON_ERROR
     exit(1);
#else
     return(NULL);
#endif
   }
   tot_size += n_el * sizeof(real);

/*
  For complex matrix copy also imag. parts.
*/
   if(M->num_type == NUM_COMPLEX)
   {

     if( (M->iel = (real *) malloc(size)) == NULL)
     {
#ifdef ERROR
       fprintf(STDERR,"*** error (matread): allocation error (imag.)\n");
#endif
#ifdef EXIT_ON_ERROR
       exit(1);
#else
       return(NULL);
#endif
     }

     if( fread(M->iel+1, sizeof(real), n_el, file) != n_el )
     {
#ifdef ERROR
 fprintf(STDERR,"*** error (matread): input error while reading imag. part\n");
#endif
#ifdef EXIT_ON_ERROR
       exit(1);
#else
       return(NULL);
#endif
     }
     tot_size += n_el * sizeof(real);
   } /* NUM_COMPLEX */
 }     /* else */

#ifdef CONTROL
 fprintf(STDCTR,"(matread): %d byte read\n", tot_size);
#endif

 return(M);
} /* end of function matread */
