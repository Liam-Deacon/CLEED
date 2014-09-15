/*********************************************************************
  GH/03.06.94
mat matmul( mat Mr, mat M1, mat M2 )
  Matrix multiplication.

Changes:
  GH/16.08.94 - Mr can be equal to  M1 or M2
                (Diagonal matrices are not included)
  GH/26.08.94 - Error in the multiplication for complex matrices
                corrected.

*********************************************************************/
#include <math.h>   
#include <stdio.h>
#include <stdlib.h>

#include "mat.h"


mat matmul( mat Mr, mat M1, mat M2 )

/*********************************************************************
  Multiply two matrices: Mr = M1*M2

  parameters:
  Mr - pointer to the matrix containing the result of the multiplication.
       If NULL, the pointer will be created and returned.

  M1, M2 - pointers to the matrices to multiply.

  return value: Mr

*********************************************************************/

{

long i;
long i_c2, i_r1;
long i_cr1, i_cr2;

mat Maux;

Maux = NULL;

/********************************************************************* 
  check input matrices 
*********************************************************************/

/* check validity of the input matrices */
 if ((matcheck(M1) < 1) || (matcheck(M2) < 1))
 {
#ifdef ERROR
  fprintf(STDERR,"*** error (matmul): ivalid input matrices\n");
#endif
#ifdef EXIT_ON_ERROR
  exit(1);
#else
  return(NULL);
#endif
 }

/* check dimensions of input matrices */
 if (M1->cols != M2->rows)
 {
#ifdef ERROR
  fprintf(STDERR,
  "*** error (matmul): dimensions of input matrices do not match\n");
#endif
#ifdef EXIT_ON_ERROR
  exit(1);
#else
  return(NULL);
#endif
 }

/*********************************************************************
  Create matrix Maux
*********************************************************************/

 if((M1->num_type ==  NUM_REAL) && (M2->num_type ==  NUM_REAL) )
 {
   Maux = matalloc(Maux, M1->rows, M2->cols, NUM_REAL);
 }
 else
   Maux = matalloc(Maux, M1->rows, M2->cols, NUM_COMPLEX);

/********************************************************************* 
  Perform the multiplication 
*********************************************************************/
#ifdef CONTROL
  fprintf(STDCTR," (matmul) start multiplication \n");
#endif
  switch(Maux->num_type)
  {
/*
 real matrix
*/
   case (NUM_REAL):
   { 
    register real rsum;
    register real *ptr1, *ptr2, *ptr_end;
    register real *ptrr;

    i_cr1 = M1->rows * M1->cols;
    i_cr2 = M2->rows * M2->cols;
  
#ifdef CONTROL
    fprintf(STDCTR," (matmul) entering real loop\n");
#endif
    /* 
       i_r1 is the offset in M1 (current row), 
       i_c2 is the offset in M2 (current column)
    */
    for( ptrr = Maux->rel + 1, i_r1 = 1; i_r1 <= i_cr1; i_r1 += M1->cols)
    for( i_c2 = 1; i_c2 <= M2->cols; i_c2 ++, ptrr ++)
    {

      rsum = 0;

      for (ptr1 = M1->rel + i_r1, ptr2 = M2->rel + i_c2, 
           ptr_end = M1->rel + i_r1 + M1->cols;
           ptr1 < ptr_end; ptr1 ++, ptr2 += M2->cols)
        rsum += *ptr1 * *ptr2;

      *ptrr = rsum;
    } /* for ptrr */
    break;
   }  /* case REAL */

/*
 complex matrix
*/
   case (NUM_COMPLEX):
   {
     i_cr1 = M1->rows * M1->cols;
     i_cr2 = M2->rows * M2->cols;
 
#ifdef CONTROL
     fprintf(STDCTR,"(matmul): entering complex loop\n");
#endif

     /*  
      M1 and M2 are complex 
     */

     if((M1->num_type ==  NUM_COMPLEX) && (M2->num_type ==  NUM_COMPLEX) )
     {
     register real rsum, isum;
     register real *ptri1, *ptri2, *ptr_end;
     register real *ptrr1, *ptrr2;
     register real *ptrr, *ptri;

#ifdef CONTROL
     fprintf(STDCTR,"(matmul): M1 and M2 are complex \n");
#endif

     for( ptrr = Maux->rel + 1, ptri = Maux->iel + 1, i_r1 = 1; 
                    i_r1 <= i_cr1; i_r1 += M1->cols)
     for( i_c2 = 1; i_c2 <= M2->cols; i_c2 ++, ptrr ++, ptri ++)
     {
       rsum = 0;
       isum = 0;

       for (ptrr1 = M1->rel + i_r1, ptrr2 = M2->rel + i_c2, 
            ptri1 = M1->iel + i_r1, ptri2 = M2->iel + i_c2, 
            ptr_end = M1->rel + i_r1 + M1->cols;
            ptrr1 < ptr_end; 
            ptrr1 ++, ptrr2 += M2->cols, ptri1 ++, ptri2 += M2->cols)
       {
         rsum += (*ptrr1 * *ptrr2) - (*ptri1 * *ptri2);
         isum += (*ptrr1 * *ptri2) + (*ptri1 * *ptrr2);
       }
 
       *ptrr = rsum;
       *ptri = isum;
     } /* for ptrr */
    }  /* both are complex */

    /*  
     Only M1 is complex 
    */

    else if (M1->num_type ==  NUM_COMPLEX)
    {
     register real rsum, isum;
     register real *ptri1, *ptr_end;
     register real *ptrr1, *ptrr2;
     register real *ptrr, *ptri;

#ifdef CONTROL
    fprintf(STDCTR,"(matmul): only M1 is complex \n");
#endif

     for( ptrr = Maux->rel + 1, ptri = Maux->iel + 1, i_r1 = 1; 
                    i_r1 <= i_cr1; i_r1 += M1->cols)
     for( i_c2 = 1; i_c2 <= M2->cols; i_c2 ++, ptrr ++, ptri ++)
     {
       rsum = 0;
       isum = 0;

       for (ptrr1 = M1->rel + i_r1, ptri1 = M1->iel + i_r1,
            ptrr2 = M2->rel + i_c2, ptr_end = M1->rel + i_r1 + M1->cols;
            ptrr1 < ptr_end; 
            ptrr1 ++, ptrr2 += M2->cols, ptri1 ++)
       {
         rsum += *ptrr1 * *ptrr2; 
         isum += *ptri1 * *ptrr2;
       }
 
       *ptrr = rsum;
       *ptri = isum;
     } /* for ptrr */
    }  /* M1 is complex */

    /*  
     Only M2 is complex
    */

    else
    {
     register real rsum, isum;
     register real *ptrr1, *ptr_end;
     register real *ptri2, *ptrr2;
     register real *ptrr, *ptri;

#ifdef CONTROL
     fprintf(STDCTR,"(matmul) only M2 is complex \n");
#endif

     for( ptrr = Maux->rel + 1, ptri = Maux->iel + 1, i_r1 = 1; 
                    i_r1 <= i_cr1; i_r1 += M1->cols)
     for( i_c2 = 1; i_c2 <= M2->cols; i_c2 ++, ptrr ++, ptri ++)
     {
       rsum = 0;
       isum = 0;

       for (ptrr1 = M1->rel + i_r1, ptrr2 = M2->rel + i_c2, 
            ptri2 = M2->iel + i_c2, ptr_end = M1->rel + i_r1 + M1->cols;
            ptrr1 < ptr_end; 
            ptrr1 ++, ptrr2 += M2->cols, ptri2 += M2->cols)
       {
         rsum += *ptrr1 * *ptrr2;
         isum += *ptrr1 * *ptri2;
       }
 
       *ptrr = rsum;
       *ptri = isum;
     } /* for ptrr */
    }  /* M2 is complex */

    break;
   }  /* case COMPLEX */
  }   /* switch */

/*
  Copy Maux into Mr
*/
  Mr = matcop(Mr, Maux);
  matfree(Maux);
  return(Mr);

}  /* end of function matmul */
