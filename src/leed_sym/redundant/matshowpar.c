/*********************************************************************
  GH/20.01.95
  matshow
    Show the elements of a matrix:

  Changes:
  
  GH/20.01.95 - Creation
  
*********************************************************************/
#include <math.h>  
#include <stdio.h>

#include "mat.h"

/*
#define ERROR
*/
#define ERROR

/*======================================================================*/

int matshowpar(mat M)
/*********************************************************************

 Show the header elements of a matrix (STDOUT).

 RETURN VALUES: 
   1, if o.k.
   0, if failed.

*********************************************************************/
{
int i_r, i_c;
  
 if (matcheck(M) < 1)
 {
#ifdef ERROR
  fprintf(STDOUT," *** error (matshowpar): matrix does not exist \n");
#endif
  return(0);
 }

 fprintf(STDOUT,"(matshowpar):\n");
 fprintf(STDOUT,"\t(int) mag_no   = 0x%4x\n", M->mag_no);
 fprintf(STDOUT,"\t(int) blk_type = 0x%4x\n", M->blk_type);
 fprintf(STDOUT,"\t(int) mat_type = 0x%4x\n", M->mat_type);
 fprintf(STDOUT,"\t(int) num_type = 0x%4x\n", M->num_type);
 fprintf(STDOUT,"\t(int) rows     =   %4d\n", M->rows);
 fprintf(STDOUT,"\t(int) cols     =   %4d\n", M->cols);
 fprintf(STDOUT,"\t(real *) rel   = 0x%x\n", M->rel);
 fprintf(STDOUT,"\t(real *) iel   = 0x%x\n", M->iel);

 return(1);
}  /* end of function matshowpar */
/*======================================================================*/
