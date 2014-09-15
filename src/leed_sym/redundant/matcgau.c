/*********************************************************************
  GH/09.06.94
*********************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mat.h"

/********************************************************************/

#define CAB2RI(a,b)  (a)*(a) + (b)*(b)
#define CABSRI(a,b)  sqrt( (a)*(a) + (b)*(b) )


mat matgausinv(mat A)
	 
/*
C  GH/07.03.90
C	  FUNCTIONAL DESCRIPTION:	
C	  
C	     MATRIX INVERSION (REAL)
C	  
C	  DUMMY ARGUMENTS:
C	  
C	     A   INPUT AND OUTPUT, INVERTED MATRIX (OVERWRITTEN)
C            X   WORKING SPACE (MODIFIED DURING INVERSION)
C	     ND  DIMENSION OF MATRICES
C            NFLAG .LT.0: ERROR HAPPENED
C
C	     H   FOR INTEMEDIATE STORAGE (FOR ND .LE.100 ) 
C	  
C	    [specification_statement]...


	REAL A,X,H,AH,ONE,ZERO
	DIMENSION A(ND,ND), X(ND,ND)
	DIMENSION H(100)
	PARAMETER (ONE=1.0)
	PARAMETER (ZERO=0.0)

	NFLAG = 1
*/
{
int n;
int i_r, i_c, i_i;

real dumi, dumr, aux;
 
real *ptrr1, *ptr_end;
real *invr, *invi;

mat Minv;

/*
  First check type of input matrix
*/
/* check type of input matrix */
 if ( (A->mat_type != MAT_SQUARE) && (A->mat_type != MAT_DIAG) )
 {
#ifdef ERROR
  fprintf(STDERR," *** error (matinv): improper input type of matrix\n");
#endif
  return(NULL);
 }

/* check dimensions of input matrix A */
 else if (A->cols != A->rows)
 {
#ifdef ERROR
  fprintf(STDERR,
          " *** error (matinv): dimensions of input matrix do not match\n");
#endif
  return(NULL);
 }
 n = A->cols;

/*
  Create nxn identity matrix (will be output).
*/

 Minv = (mat)malloc( sizeof( struct mat_str ) );
 invr = Minv->rel = (real *)calloc( n*n + 1, sizeof(real) );
 invi = Minv->iel = (real *)calloc( n*n + 1, sizeof(real) );

 if( (Minv == NULL) || (invr == NULL) || (invi == NULL) )
 {
#ifdef ERROR
   fprintf(STDERR," *** error (c_luinv): allocation error \n");
#endif
   free(invr);
   free(invi);
   free(Minv);
   return(NULL);
 }

 Minv->num_type = NUM_COMPLEX;
 Minv->mat_type = MAT_SQUARE;

 Minv->cols = Minv->rows = n;

 for (ptrr1 = invr + 1, ptr_end = invr + n*n;
      ptrr1 <= ptr_end; ptrr1 += n+1)
   *ptrr1 = 1.0;


/*
C  DOWN:
*/

 for (i_r=1; i_r <= n-1; i_r ++)
 {

   for( i_c=i_r+1;i_c <= n; i_c ++)
   {
     if (CABSRI(RMATEL(i_c,i_r,A), IMATEL(i_c,i_r,A)) > 
         CABSRI(RMATEL(i_r,i_r,A), IMATEL(i_r,i_r,A)) )
/*
  change rows i_c and i_r in A and Minv
*/
     {
       for( i_i = 1; i_i <= n; i_i ++)
       {
         dumr = RMATEL(i_r,i_i,A);
         RMATEL(i_r,i_i,A) = RMATEL(i_c,i_i,A);
         RMATEL(i_c,i_i,A) = dumr;
         dumi = IMATEL(i_r,i_i,A);
         IMATEL(i_r,i_i,A) = IMATEL(i_c,i_i,A);
         IMATEL(i_c,i_i,A) = dumi;
       }
       RMATEL(i_r,i_r,Minv) = 0.;
       RMATEL(i_r,i_c,Minv) = 1.;
       RMATEL(i_c,i_r,Minv) = 1.;
       RMATEL(i_c,i_c,Minv) = 0.;
     }
   }
   if( (RMATEL(i_r,i_r,A) == 0.) && (IMATEL(i_r,i_r,A) == 0.) )
   {
/*
  IF ALL A(i_r,i_r).EQ.ZERO THEN A IS SINGULAR
*/
#ifdef ERROR
     fprintf(STDERR," *** error (ludcmp): Singular matrix \n");
#endif
     return(NULL);
   }


   if (i_r == 1)
   {
     dumr = RMATEL(1,1,A);
     dumi = IMATEL(1,1,A);
     aux = CAB2RI(dumr, dumi);
     dumr /= aux;
     dumi /= -aux;
     
     
     for( i_i = 1; i_i <= n; i_i ++)
     {
       aux = RMATEL(1,i_i,Minv)*dumr - IMATEL(1,i_i,Minv)*dumi;
       IMATEL(1,i_i,Minv) = RMATEL(1,i_i,Minv)*dumi + IMATEL(1,i_i,Minv)*dumr;
       RMATEL(1,i_i,Minv) = aux;

       aux = RMATEL(1,i_i,A)*dumr - IMATEL(1,i_i,A)*dumi;
       IMATEL(1,i_i,A) = RMATEL(1,i_i,A)*dumi + IMATEL(1,i_i,A)*dumr;
       RMATEL(1,i_i,A) = aux;
     }
   }

   for( i_c=i_r+1; i_c <= n; i_c ++)
   {
     dumr = RMATEL(i_c,i_r+1,A) - RMATEL(i_r,i_r+1,A)*RMATEL(i_c,i_r,A) + 
                                  IMATEL(i_r,i_r+1,A)*IMATEL(i_c,i_r,A);
     dumi = IMATEL(i_c,i_r+1,A) - IMATEL(i_r,i_r+1,A)*RMATEL(i_c,i_r,A) - 
                                  RMATEL(i_r,i_r+1,A)*IMATEL(i_c,i_r,A);
     if( (dumr == 0.) && (dumi == 0.) ) dumr = 1.;
     aux = CAB2RI(dumr, dumi);
     dumr /= aux;
     dumi /= -aux;

     for( i_i=i_r+1; i_i<= n; i_i ++)
     {
       RMATEL(i_c,i_i, A) -= RMATEL(i_r,i_i,A)*RMATEL(i_c,i_r,A) + 
                             IMATEL(i_c,i_i,A)*IMATEL(i_c,i_r,A);
       IMATEL(i_c,i_i, A) -= IMATEL(i_r,i_i,A)*RMATEL(i_c,i_r,A) + 
                             RMATEL(i_c,i_i,A)*IMATEL(i_c,i_r,A);

       aux = RMATEL(i_c,i_i, A)*dumr - IMATEL(i_c,i_i, A)*dumi;
       IMATEL(i_c,i_i, A) = RMATEL(i_c,i_i, A)*dumi + IMATEL(i_c,i_i, A)*dumr;
       RMATEL(i_c,i_i, A) = aux;
     }
     for( i_i=i_r+1; i_i<= n; i_i ++)
     {
       RMATEL(i_c,i_i, Minv) -= RMATEL(i_r,i_i,Minv)*RMATEL(i_c,i_r,A) + 
                                IMATEL(i_c,i_i,Minv)*IMATEL(i_c,i_r,A);
       IMATEL(i_c,i_i, Minv) -= IMATEL(i_r,i_i,Minv)*RMATEL(i_c,i_r,A) + 
                                RMATEL(i_c,i_i,Minv)*IMATEL(i_c,i_r,A);

       aux = RMATEL(i_c,i_i, Minv)*dumr - IMATEL(i_c,i_i, Minv)*dumi;
       IMATEL(i_c,i_i, Minv) = 
             RMATEL(i_c,i_i, Minv)*dumi + IMATEL(i_c,i_i, Minv)*dumr;
       RMATEL(i_c,i_i, Minv) = aux;
     }

     RMATEL(i_c,i_r, A) = 0.0; IMATEL(i_c,i_r, A) = 0.0;
   }
 }

/*
 UP:
*/
 for( i_r=n; i_r > 1; i_r --)
 {
   if( (RMATEL(i_r,i_r,A) == 0.) && (IMATEL(i_r,i_r,A) == 0.) )
   {
/*
  IF UPWARDS A(i_r,i_r).EQ.ZERO THEN A IS SINGULAR
*/
#ifdef ERROR
     fprintf(STDERR," *** error (ludcmp): Singular matrix \n");
#endif
     return(NULL);
   }

   for(i_c=i_r-1; i_c >= 1; i_c --)
   {
     dumr = RMATEL(i_c,i_r-1,A);
     dumi = IMATEL(i_c,i_r-1,A);
     
     if( (dumr == 0.) && (dumi == 0.) ) dumr = 1.;
     aux = CAB2RI(dumr, dumi);
     dumr /= aux;
     dumi /= -aux;
 
     for( i_i=i_c; i_i <= i_r-1; i_i ++)
     {
       aux = RMATEL(i_c,i_i,A)*dumr - IMATEL(i_c,i_i,A)*dumi;
       IMATEL(i_c,i_i,A) = RMATEL(i_c,i_i,A)*dumi + IMATEL(i_c,i_i,A)*dumr;
       RMATEL(i_c,i_i,A) = aux;
     }

     for( i_i=1; i_i <= n; i_i ++)
     {
       RMATEL(i_c,i_i, Minv) -= RMATEL(i_r,i_i,Minv)*RMATEL(i_c,i_r,A) + 
                                IMATEL(i_c,i_i,Minv)*IMATEL(i_c,i_r,A);
       IMATEL(i_c,i_i, Minv) -= IMATEL(i_r,i_i,Minv)*RMATEL(i_c,i_r,A) + 
                                RMATEL(i_c,i_i,Minv)*IMATEL(i_c,i_r,A);

       aux = RMATEL(i_c,i_i, Minv)*dumr - IMATEL(i_c,i_i, Minv)*dumi;
       IMATEL(i_c,i_i, Minv) = 
             RMATEL(i_c,i_i, Minv)*dumi + IMATEL(i_c,i_i, Minv)*dumr;
       RMATEL(i_c,i_i, Minv) = aux;
     }

     RMATEL(i_c,i_r, A) = 0.0; IMATEL(i_c,i_r, A) = 0.0;
   }
 }
 return(Minv);
}
