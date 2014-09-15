/*********************************************************************
GH/22.09.00 
  file contains functions:

  r_ludcmp
  r_luinv
  r_lubksb

  (modified programs from numerical recipes(NR))

Changes:
GH/20.07.97 - include output (inv) in the parameter list in order
            to avoid unfreed memory allocation.
GH/22.09.00 - include malloc at the top of file

*********************************************************************/

#include <math.h>
#include <stdio.h>
#include <malloc.h>

#include "mat.h"
/*
#define CONTROL
*/
#define ERROR

/********************************************************************/

#define TINY 1.0e-20;

int r_ludcmp( real *a, int * indx, int n )

/*
  Perform LU decomposition (first step of matrix inversion). 

  parameters:
  a - input: pointer to the matrix to be inverted (will be modified).
  indx - output: int vector which records the row permutation effected 
       by the partial pivoting (must be allocated with the right size).
  n  - input: dimension of matrix

  return value:
       +/- 1 depending on whether the number of row interchanges was
       even or odd, respectively.
       0     if failed.
      
  Given a NxN matrix A this routine replaces it by the LU decomposition 
  of a rowwise permutation of itself. A is output in LU arrangement 
  (NR eq. 2.3.14 ); 
  indx is an output vector which records the row permutation effected by 
  the partial pivoting. 
  The return value is +/- 1 depending on whether the number of row 
  interchanges was even or odd, respectively. This routine is used in 
  combination with "lubksb" to solve linear equations.
*/

{
int d;

int i_r, i_c;
int i_max;

real big, dum, sum;
real *vv;                  /* vv stores the implicit scaling of each row */    
real *ptr1, *ptr2, *ptr_end;  /* used as counters in the innermost loops */

 vv = (real *)malloc( (n+1) * sizeof(real) );
 d=1;

/* get implicit scaling information */
 for (i_r = 1; i_r <= n; i_r ++ )  /* loop over rows */
 {
   big = 0.0;
   /* loop over cols */
   for ( ptr1 = a + (i_r - 1)*n + 1, ptr_end = a + (i_r -1)*n + n; 
         ptr1 <= ptr_end; ptr1 ++) 
     if ((dum = fabs(*ptr1)) > big) big = dum;
   if (big == 0.0) 
   {
#ifdef ERROR
     fprintf(STDERR," *** error (ludcmp): Singular matrix \n");
#endif
     return(0);
   }
   vv[i_r]=1.0/big;
 }
 for (i_c = 1; i_c <= n; i_c++)  /* loop over columns in Crout's method */
 {                               /* NR eq 2.3.12 except for i_r == i_c */
   for (i_r = 1; i_r < i_c; i_r ++) 
   {
     sum = *(a + (i_r - 1)*n + i_c);
   
     for (ptr1 = a + (i_r - 1)*n + 1, 
          ptr2 = a + i_c, 
          ptr_end  = a + (i_r - 1)*n + i_r ;
          ptr1 < ptr_end; ptr1 ++, ptr2 += n)
       sum -= (*ptr1) * (*ptr2);
           
     *(a + (i_r - 1)*n + i_c) = sum;
   } /* for i_r */
/*
  Initialize for the search for largest pivot element
  This is i_r = i_c for NR eq 2.3.12 and 
          i_r = i_c +1, ..., n for NR eq 2.3.13
*/
   big = 0.0;
   for (i_r = i_c; i_r <= n; i_r ++) 
   {
     sum = *(a + (i_r - 1)*n + i_c);
 
     for (ptr1 = a + (i_r - 1)*n + 1,
          ptr2 = a + i_c,
          ptr_end  = a + (i_r - 1)*n + i_c;
          ptr1 < ptr_end; ptr1 ++, ptr2 += n)
     sum -= (*ptr1) * (*ptr2);

     *(a + (i_r - 1)*n + i_c) = sum;

/* 
  Figure of the merit for the pivot - is it better than the best so far? 
*/
     if ( ( dum = vv[i_r]*fabs(sum) ) >= big) 
     {
       big = dum;
       i_max = i_r;
     }
   }  /* for i_r */

   if (i_c != i_max)      /* Do we need to interchange rows? */
   {
     for (ptr1 = a + (i_max - 1)*n + 1,
          ptr2 = a + (i_c  - 1)*n + 1,
          ptr_end  = a +  i_max*n;
          ptr1 <= ptr_end; ptr1 ++, ptr2 ++)
     {
       dum = *ptr1;
       *ptr1 = *ptr2;
       *ptr2 = dum;
     }
     d = -d;             /* change the parity of d */
     vv[i_max]=vv[i_c];  /* Also interchange scale factor */
   }  /* if i_c != i_max */
   indx[i_c]=i_max;
/* 
  If the pivot element is zero the matrix is singular (at least to the 
  precision of the algorithm). For some applications on singular matrices 
  it is desirable to substitute TINY to zero.
*/
   if ( *(a + (i_c - 1)*n + i_c) == 0.0 ) *(a + (i_c - 1)*n + i_c) = TINY;

/*
  Now, finally, divide by the pivot element.
*/
   if (i_c != n)
   {
     dum=1.0/(*(a + (i_c - 1)*n + i_c));

     for (ptr1 = a + i_c*n + i_c, ptr_end = a + n*n + i_c;
          ptr1 < ptr_end; ptr1 += n)
       *ptr1 *= dum;
   } /* if i_c != n */
 }   /* for i_c */

 free(vv);
 return(d);
}    /* end of function r_ludcmp */

#undef TINY

/********************************************************************/

/* #include <malloc.h> */

int r_luinv(real * inv, real * lu, int * indx, int n)

/*
 Solves the set of n linear equations A*A_1 = I. 

 parameters:
 
  inv - (output) pointer to the inverse matrix. Enough memory must be
       allocated for this pointer (n*n +1).
  lu - input: pointer to the matrix to be inverted (as LU decomposition
       as returned by r_ludcmp).
  indx - input: int vector which records the row permutation (as returned 
       by ludcmp)
  n  - input: dimension of matrix

  return value:
       1 if o.k.
       0 if failed.

 Here A is input but not as the matrix A but rather as its LU decomposition
 determined by the routine ludcmp. indx is input as the permutation vector
 returned by ludcmp. b is input as the right hand side vector b, and returns 
 with the solution vector X. A and index are not modified by this routine 
 and can be left in place for successive calls with different right-hand 
 sides b. This routine takes into account the possibility that b will begin
 with many zero elements, so it is efficient for the use in matrix inversion.
*/

{
int i_c, i_i, i_r;

real sum;
real *ptr1, *ptr2, *ptr_end;  /* pointers used in innermost loops */

/*
  First, preset inv as nxn identity matrix. 
*/

 for (ptr1 = inv + 1, ptr_end = inv + n*n; ptr1 <= ptr_end; ptr1 ++)
   *ptr1 = 0.;
 for (ptr1 = inv + 1, ptr_end = inv + n*n; ptr1 <= ptr_end; ptr1 += n+1)
   *ptr1 = 1.0;

/*
  Backsubstitution starts here.
*/
 for (i_c = 1; i_c <= n; i_c ++)     /* loop over columns of inv */
 {
   i_i = 0;
   for (i_r = 1; i_r <= n; i_r ++)   
   /* loop through columns of inv (i.e. over rows) */
   {
     sum = *(inv + (indx[i_r] - 1)*n + i_c);
     *(inv + (indx[i_r] - 1)*n + i_c) = *(inv + (i_r - 1)*n + i_c);

     if (i_i)
     {
       for (ptr1 = lu  + (i_r - 1)*n + i_i,
            ptr2 = inv + (i_i - 1)*n + i_c,
            ptr_end = lu + (i_r - 1)*n + i_r;
            ptr1 < ptr_end; ptr1 ++, ptr2 += n )
         sum -= (*ptr1) * (*ptr2);
     }
     else if (sum != 0.0) i_i = i_r;

     *(inv + (i_r - 1)*n + i_c) = sum;
   }

   for (i_r = n; i_r >= 1; i_r --) 
   {
     sum = *(inv + (i_r - 1)*n + i_c);
     for (ptr1 = lu  + (i_r - 1)*n + i_r + 1,
          ptr2 = inv + i_r*n + i_c,
          ptr_end = lu + (i_r - 1)*n + n;
          ptr1 <= ptr_end; ptr1 ++, ptr2 += n )
       sum -= (*ptr1) * (*ptr2);

     *(inv + (i_r - 1)*n + i_c) = sum/(*(lu + (i_r - 1)*n + i_r));
   }
 }   /* for i_c */
 return(1);
}    /* end of function r_luinv */

/********************************************************************/

real * r_lubksb(real * lu, int * indx, real * br, int n)

/*
 Solves the set of n linear equations A*X = b. 

 parameters:
 
  lu - input: pointer to the matrix to be inverted (as LU decomposition
       as returned by r_ludcmp).
  br - input: pointer to the vector to be multiplied with the inverted 
       matrix (will be overwritten).
  indx - input: int vector which records the row permutation (as returned 
       by ludcmp)
  n  - input: dimension of matrix

  return value:
       pointer to the result.
       NULL, if failed.

 Here A is input but not as the matrix A but rather as its LU decomposition
 determined by the routine ludcmp. indx is input as the permutation vector
 returned by ludcmp. b is input as the right hand side vector b, and returns 
 with the solution vector X. A and index are not modified by this routine 
 and can be left in place for successive calls with different right-hand 
 sides b. This routine takes into account the possibility that b will begin
 with many zero elements, so it is efficient for the use in matrix inversion.
*/

{
int i_c, i_i, i_r;

real sum;
real *inv;                    /* output: inverse matrix */
real *ptr1, *ptr2, *ptr_end;  /* pointers used in innermost loops */

 for (i_c = 1; i_c <= n; i_c ++)     /* loop over columns of br */
 {
   i_i = 0;
   for (i_r = 1; i_r <= n; i_r ++)   
   /* loop through columns of br (i.e. over rows) */
   {
     sum = *(br + (indx[i_r] - 1)*n + i_c);
     *(br + (indx[i_r] - 1)*n + i_c) = *(br + (i_r - 1)*n + i_c);

     if (i_i)
     {
       for (ptr1 = lu  + (i_r - 1)*n + i_i,
            ptr2 = br + (i_i - 1)*n + i_c,
            ptr_end = lu + (i_r - 1)*n + i_r;
            ptr1 < ptr_end; ptr1 ++, ptr2 += n )
         sum -= (*ptr1) * (*ptr2);
     }
     else if (sum != 0.0) i_i = i_r;

     *(br + (i_r - 1)*n + i_c) = sum;
   }

   for (i_r = n; i_r >= 1; i_r --) 
   {
     sum = *(br + (i_r - 1)*n + i_c);
     for (ptr1 = lu  + (i_r - 1)*n + i_r + 1,
          ptr2 = br + i_r*n + i_c,
          ptr_end = lu + (i_r - 1)*n + n;
          ptr1 <= ptr_end; ptr1 ++, ptr2 += n )
       sum -= (*ptr1) * (*ptr2);

     *(br + (i_r - 1)*n + i_c) = sum/(*(lu + (i_r - 1)*n + i_r));
   }
 }   /* for i_c */
 return(br);
}    /* end of function r_lubksb */

/********************************************************************/
