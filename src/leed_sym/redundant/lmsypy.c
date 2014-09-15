/*********************************************************************
  GH/24.08.94 
  file contains functions:

  leed_ms_yp_yxp            (22.08.94)
     Create the transformation matrix from angular momentum space 
     into k-space: Y*lm(k+).

  leed_ms_yp_yxm            (22.08.94)
     Create the transformation matrix from angular momentum space 
     into k-space: Y*lm(k-).

  leed_ms_yp_ym             (10.04.95)
     Create the transformation matrix from k-space into angular
     momentum space for negative k_z: Ylm(k-).

Changes:
GH/22-24.08.94 - Creation
GH/10.04.95 - correction in leed_ms_yp_ym (remove offm)

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "leed.h"


/*======================================================================*/
/*======================================================================*/

mat leed_ms_yp_yxp ( mat Yxmat, mat Ymat)

/************************************************************************

 Create the transformation matrix from k-space into angular momentum 
 space: Y*lm(k).

 INPUT:

   mat Yxmat - (output) conjugate and transposed matrix to Ymat: Y*lm(k+).
   mat Ymat - (input) transformation matrix from angular momentum space
              into k-space: Ylm(k+).
 DESIGN:

   Use the formula: 

     Y*lm(k+) -> (-1)^m * Yl-m(k+)

   instead of a real conjugation.

 RETURN VALUES:

   NULL if failed (and EXIT_ON_ERROR is not defined)

   Yxmat (may be different from input parameter).

   The order of Ymat is:

   (l,m) / k    k1        k2        k3          ki

    (0,0)     Y00(k1)   Y00(k2)   Y00(k3) 
   (-1,1)    Y-11(k1)  Y-11(k2)  Y-11(k3) 
    (0,1)     Y01(k1) 

    (l,m)                                     Y(l,m)(ki) 

   

*************************************************************************/
{
int l,m;                       /* l,m quantum numbers */
int l_max;
int offl, offm;                /* offsets in the arrays Ymat and Yxmat */
int size;

real *buffer;

real *ptr, *ptr_end;

/*
  Check arguments:
*/

/*
  Copy transposed of Ymat into Yxmat
  - the rows of Yxmat have now the same (l,m) quantum numbers.
    Yxmat->cols = number of k values.
    Yxmat->rows = number of (l,m) pairs.
  - allocate a buffer of the size of one row.
  - find l_max;
*/

 Yxmat = mattrans( Yxmat, Ymat);

 size = Yxmat->cols * sizeof(real);
 buffer = (real*)malloc(size);
 
 l_max = (int)(R_sqrt((real) Yxmat->rows ) + 0.1) - 1;
#ifdef CONTROL
 fprintf(STDCTR,"(leed_ms_yp_yxp): l_max = %2d\n", l_max);
#endif

/*
  Loop over l,m (l>0, m>0):
  - exchange rows of m with -m
*/

 for( l = 1, offl =  2*Yxmat->cols + 1; l <= l_max; 
      l ++,  offl += 2*l* Yxmat->cols)
 {
   for( m = 1, offm = Yxmat->cols; m <= l; m++, offm += Yxmat->cols)
   {
   /* 
     Exchange row m with row -m 
     Change sign, if m is odd.
   */
     memcpy(buffer,               Yxmat->rel+offl+offm, size);
     memcpy(Yxmat->rel+offl+offm, Yxmat->rel+offl-offm, size);
     memcpy(Yxmat->rel+offl-offm, buffer              , size);
    
     memcpy(buffer,               Yxmat->iel+offl+offm, size);
     memcpy(Yxmat->iel+offl+offm, Yxmat->iel+offl-offm, size);
     memcpy(Yxmat->iel+offl-offm, buffer              , size);

     if (ODD(m))
     {
       for( ptr = Yxmat->rel+offl+offm, ptr_end = ptr + Yxmat->cols; 
            ptr < ptr_end; ptr ++) *ptr = -*ptr;
       for( ptr = Yxmat->rel+offl-offm, ptr_end = ptr + Yxmat->cols; 
            ptr < ptr_end; ptr ++) *ptr = -*ptr;

       for( ptr = Yxmat->iel+offl+offm, ptr_end = ptr + Yxmat->cols; 
            ptr < ptr_end; ptr ++) *ptr = -*ptr;
       for( ptr = Yxmat->iel+offl-offm, ptr_end = ptr + Yxmat->cols; 
            ptr < ptr_end; ptr ++) *ptr = -*ptr;
     }
   }  /* m */
 }  /* l */

 free(buffer);
 return(Yxmat);
}
 
/*======================================================================*/
/*======================================================================*/

mat leed_ms_yp_yxm ( mat Yxmat, mat Ymat)

/************************************************************************

 Create the transformation matrix from k-space into angular momentum 
 space: Y*lm(k-).

 INPUT:

   mat Yxmat - (output) conjugate and transposed matrix to Ymat(k): 
               Y*lm(k-).
   mat Ymat - (input) transformation matrix from angular momentum space
              into k-space: Ylm(k+).
 DESIGN:

   Use the formula: 

     Y*lm(k-) -> (-1)^l * Yl-m(k+)

   instead of a real conjugation.

 RETURN VALUES:

   NULL if failed (and EXIT_ON_ERROR is not defined)

   Yxmat (may be different from input parameter).

   The order of Ymat is:

   (l,m) / k    k1-       k2-       k3-         ki-

    (0,0)     Y00(k1-)   Y00(k2-)   Y00(k3-) 
   (-1,1)    Y-11(k1-)  Y-11(k2-)  Y-11(k3-) 
    (0,1)     Y01(k1-) 

    (l,m)                                     Y(l,m)(ki-) 

   

*************************************************************************/
{
int l,m;                       /* l,m quantum numbers */
int l_max;
int offl, offm;                /* offsets in the arrays Ymat and Yxmat */
int size;

real *buffer;

real *ptr, *ptr_end;

/*
  Check arguments:
*/

/*
  Copy transposed of Ymat into Yxmat
  - the rows of Yxmat have now the same (l,m) quantum numbers.
    Yxmat->cols = number of k values.
    Yxmat->rows = number of (l,m) pairs.
  - allocate a buffer of the size of one row.
  - find l_max;
*/
 Yxmat = mattrans( Yxmat, Ymat);

 size = Yxmat->cols * sizeof(real);
 buffer = (real*)malloc(size);
 
 l_max = (int)(R_sqrt((real) Yxmat->rows ) + 0.1) - 1;

#ifdef CONTROL
 fprintf(STDCTR,"(leed_ms_yp_yxm): l_max = %2d\n", l_max);
#endif

/*
  Loop over l,m (l>0, m>0):
  - exchange rows of m with -m
*/

 for( l = 1, offl =  2*Yxmat->cols + 1; l <= l_max; 
      l ++,  offl += 2*l* Yxmat->cols)
 {
   for( m = 1, offm = Yxmat->cols; m <= l; m++, offm += Yxmat->cols)
   {
 /* 
   Exchange row m with row -m 
 */
     memcpy(buffer,               Yxmat->rel+offl+offm, size);
     memcpy(Yxmat->rel+offl+offm, Yxmat->rel+offl-offm, size);
     memcpy(Yxmat->rel+offl-offm, buffer              , size);
    
     memcpy(buffer,               Yxmat->iel+offl+offm, size);
     memcpy(Yxmat->iel+offl+offm, Yxmat->iel+offl-offm, size);
     memcpy(Yxmat->iel+offl-offm, buffer              , size);
   }  /* m */

 /* 
   Change the sign of all rows with the same l, if this is odd.
 */
   if (ODD(l))
   {
     for( ptr = Yxmat->rel+offl-(l*Yxmat->cols), 
          ptr_end = ptr + (2*l + 1)*Yxmat->cols; 
          ptr < ptr_end; ptr ++) 
       *ptr = -*ptr;
     for( ptr = Yxmat->iel+offl-(l*Yxmat->cols), 
          ptr_end = ptr + (2*l + 1)*Yxmat->cols; 
          ptr < ptr_end; ptr ++) 
       *ptr = -*ptr;
   }
 }  /* l */

 free(buffer);
 return(Yxmat);
}
 
/*======================================================================*/
/*======================================================================*/

mat leed_ms_yp_ym ( mat Ymmat, mat Ypmat)

/************************************************************************

 Create the transformation matrix from from angular momentum space into
 k-space: Ylm(k-).

 INPUT:

   mat Ymmat - (output) transformation matrix for negative k_z: Y*lm(k-).
   mat Ypmat - (input) transformation matrix from angular momentum space
              into k-space: Ylm(k+).
 DESIGN:

   Use the formula: 

     Ylm(k-) = (-1)^(l+m) Ylm(k+)


 RETURN VALUES:

   NULL if failed (and EXIT_ON_ERROR is not defined)

   Ymmat (may be different from input parameter).

   The order of Ymmat is:

   k / (l,m) (0,0)      (-1,1)     (0,1)        (l,m)

   k1-      Y00(k1-)   Y-11(k1-)  Y01(k1-) 
   k2-      Y00(k2-)   Y-11(k2-)  Y01(k2-) 
   k3-      Y00(k3-) 

   ki-                                       Y(l,m)(ki-)

   

*************************************************************************/
{
int l,m;                       /* l,m quantum numbers */
int l_max;
int offl;                      /* offsets in the arrays Ymmat and Ypmat */
int size;

real *ptr, *ptr_end;

/*
  Check arguments:
*/

/*
  Copy Ypmat into Ymmat
  - the columns of Ymmat have the same (l,m) quantum numbers.
    Ymmat->rows = number of k values.
    Ymmat->cols = number of (l,m) pairs.
  - find l_max;
*/

 Ymmat = matcop( Ymmat, Ypmat);

 l_max = (int)(R_sqrt((real) Ymmat->cols ) + 0.1) - 1;
#ifdef CONTROL
 fprintf(STDCTR,"(leed_ms_yp_ym): l_max = %2d\n", l_max);
#endif

/*
  Loop over l,m (l>0, m>0):
*/

 for( l = 1, offl = 3; l <= l_max; l ++, offl += 2*l )
 {
   /*
     Special treatment for m = 0
   */
   if(ODD(l))
   {
     ptr_end = Ymmat->rel + Ymmat->cols*Ymmat->rows;
     for( ptr = Ymmat->rel+offl; ptr <= ptr_end; ptr +=Ymmat->cols) 
       *ptr = -*ptr;

     ptr_end = Ymmat->iel + Ymmat->cols*Ymmat->rows;
     for( ptr = Ymmat->iel+offl; ptr <= ptr_end; ptr +=Ymmat->cols) 
       *ptr = -*ptr;
   }

   for( m = 1; m <= l; m++)
   {
   /* 
     Change sign, if (l+m) is odd.
   */
     if(ODD(l+m))
     {
     ptr_end = Ymmat->rel + Ymmat->cols*Ymmat->rows;
     for( ptr = Ymmat->rel+offl+m; ptr <= ptr_end; ptr +=Ymmat->cols) 
       *ptr = -*ptr;
     for( ptr = Ymmat->rel+offl-m; ptr <= ptr_end; ptr +=Ymmat->cols) 
       *ptr = -*ptr;

     ptr_end = Ymmat->iel + Ymmat->cols*Ymmat->rows;
     for( ptr = Ymmat->iel+offl+m; ptr <= ptr_end; ptr +=Ymmat->cols) 
       *ptr = -*ptr;
     for( ptr = Ymmat->iel+offl-m; ptr <= ptr_end; ptr +=Ymmat->cols) 
       *ptr = -*ptr;
     }
   }  /* odd (l+m)'s */
 }  /* l */

 return(Ymmat);
}
 
/*======================================================================*/
/*======================================================================*/
