/*********************************************************************
  GH/07.08.95 
  file contains functions:

  ms_partinv

    Invert giant scattering matrix by partitioning.

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>

#include "leed.h"

/*======================================================================*/

mat ms_partinv ( mat Minv, mat Mbg, int first_atoms, int l_max)

/************************************************************************

 DESCRIPTION:
 
   Invert giant scattering matrix by partitioning.

 INPUT:

   mat Minv - output: inverse of input matrix
   mat Mbg  - input: giant scattering matrix (dimension N * (l_max +1)^2)

   int first_atoms - number of atoms which are in the same plane.
   int l_max - max angular momentum quantum number.

 DESIGN:

   * General *

 The function follows the formulae given in Num. Rec. (2.7.22 ff/p. 77).
 The inverse of the matrix

           Mbg  = (UL UR)
                  (LL LR)
 Can be written as
           Minv = (P Q)
                  (R S)
 where the submatrices P, Q, R, and S have the same dimensions as UL, UR,
 LL, and LR, respectively. They can be calculated as follows:

        S = [LR - LL*(UL^-1)*UR ]^-1
        R = -S* LL*(UL^-1)
        Q = -(UL^-1)*UR * S
        P = (UL^-1) + (UL^-1)*UR * S * LL*(UL^-1)

 If the first (first_atoms) atoms are in the same plane parallel to the 
 surface, the upper left submatrix UL (dimension: first_atoms *(l_max +1)^2)
 can be blockdiagonalized into two blocks with even (l1+m1), (l2+m2)
 (dimension: first_atoms * (l_max + 1)*(l_max + 2)/2) and odd (l1+m1), 
 (l2+m2) (dimension: first_atoms * (l_max +1 ) * l_max/2). 
 Thus, UL can be inverted by inverting the two blocks which requires less
 cpu time. There are no savings in the other matrix operations of the above 
 formulae, therefore this method saves more time the more atoms are in the 
 same plane.

   * Special *

 First UL is decomposed into two matrices with even (l1+m1), (l2+m2), Maux_a
 and odd (l1+m1), (l2+m2), Maux_b. Maux_a and Maux_b are inverted separately 
 and reinserted into UL which has now become (UL^-1).

 If first_atoms *(l_max +1)^2 is equal to the dimensions of Mbg (i.e. all
 atoms are in the same plane) (UL^-1) is identical to the inverse of Mbg and
 the function will return (UL^-1). If {first_atoms *(l_max +1)^2} is smaller 
 than the dimensions of Mbg (i.e. not all atoms are in the same plane) S, R,
 Q, and P are calculated according to the above formulae, whereby 
 Maux_a = LL*(UL^-1) and Maux_b = (UL^-1)*UR are stored as intermediate 
 results in order to be reused.

 FUNCTION CALLS:
 
  matalloc
  matfree
  matcheck

  matext
  matins

  matinv
  matmul


 RETURN VALUE:

  Minv, if o.k.

*************************************************************************/
{
int iaux;

int i_atoms_1, i_atoms_2;
int l1,m1, l2,m2;

int iev1, iev2;
int iod1, iod2;
int odd1;

real *ptr_1, *ptr_2, *ptr_end;

mat Maux_a, Maux_b;
mat UL, UR, LL, LR;


 UL = UR = LL = LR = NULL;
 Maux_a = Maux_b = NULL;

/*************************************************************************
  Extract UL (upper left) from Mbg.
  Allocate Maux_a, Maux_b.
*************************************************************************/

 iaux = first_atoms * (l_max + 1)*(l_max + 1);
 UL = matext(UL, Mbg, 1, iaux, 1, iaux);

 iaux = first_atoms * (l_max + 1)*(l_max + 2)/2;
 Maux_a = matalloc( Maux_a, iaux, iaux, NUM_COMPLEX);
 iaux = first_atoms * (l_max +1 ) * l_max/2;
 Maux_b = matalloc( Maux_b, iaux, iaux, NUM_COMPLEX);
 
/*************************************************************************
 Loop over (l1,m1),(l2,m2): Set up  Maux_a and Maux_b.

  - UL separates into two blocks with even (l1+m1), (l2+m2) and odd
    (l1+m1), (l2+m2) which are stored in Maux_a and Maux_b, respectively.
*************************************************************************/
 for(i_atoms_1 = 0, iev2 = 1, iod2 = 1, ptr_1 = UL->rel+1, ptr_2 = UL->iel+1;
     i_atoms_1 < first_atoms; i_atoms_1 ++)
 {
   for(l1 = 0; l1 <= l_max; l1 ++)
   {
     for(m1 = -l1; m1 <= l1; m1 ++)
     {
       odd1 = ODD(l1+m1);
       for(i_atoms_2 = 0; i_atoms_2 < first_atoms; i_atoms_2 ++)
       {
         for(l2 = 0; l2 <= l_max; l2 ++)
         {
           for(m2 = -l2; m2 <= l2; m2 ++, ptr_1 ++, ptr_2 ++)
           {
             if(ODD(l2+m2))
             {
               if( odd1)
               {
                 Maux_b->rel[iod2] = *ptr_1;
                 Maux_b->iel[iod2] = *ptr_2;
                 iod2++;
               }
             }
             else
             {
               if(!odd1)
               {
                 Maux_a->rel[iev2] = *ptr_1;
                 Maux_a->iel[iev2] = *ptr_2;
                 iev2++;
               }
             }  /* else */
           }  /* m2 */
         }  /* l2 */
       }  /* i_atoms_2 */
     }  /* m1 */
   }  /* l1 */
 }  /* i_atoms_1 */

#ifdef CONTROL_X
 fprintf(STDCTR,"(ms_partinv): UL, Maux_a/od: \n");
#endif
 
/*************************************************************************
 Matrix inversion: (Maux_a/od)^-1
*************************************************************************/

 Maux_a = matinv(Maux_a, Maux_a);
 Maux_b = matinv(Maux_b, Maux_b);

#ifdef CONTROL_X
 fprintf(STDCTR,"(ms_partinv): (Maux_b)^-1: \n");
#endif
 
/*************************************************************************
  Copy (Maux_a)^-1 and (Maux_b)^-1 back into UL in the natural order.
*************************************************************************/

 for(i_atoms_1 = 0, iev2 = 1, iod2 = 1, ptr_1 = UL->rel+1, ptr_2 = UL->iel+1;
     i_atoms_1 < first_atoms; i_atoms_1 ++)
 {
   for(l1 = 0; l1 <= l_max; l1 ++)
   {
     for(m1 = -l1; m1 <= l1; m1 ++)
     {
       odd1 = ODD(l1+m1);
       for(i_atoms_2 = 0; i_atoms_2 < first_atoms; i_atoms_2 ++)
       {
         for(l2 = 0; l2 <= l_max; l2 ++)
         {
           for(m2 = -l2; m2 <= l2; m2 ++, ptr_1 ++, ptr_2 ++)
           {
             if(ODD(l2+m2))
             {
               if( odd1)
               {
                 *ptr_1 = Maux_b->rel[iod2];
                 *ptr_2 = Maux_b->iel[iod2];
                 iod2++;
               }
               else 
               {
                 *ptr_1 = 0.;
                 *ptr_2 = 0.;
               }
             }
             else
             {
               if(!odd1)
               {
                 *ptr_1 = Maux_a->rel[iev2];
                 *ptr_2 = Maux_a->iel[iev2];
                 iev2++;
               }
               else 
               {
                 *ptr_1 = 0.;
                 *ptr_2 = 0.;
               }
             }  /* else */
           }  /* m2 */
         }  /* l2 */
       }  /* i_atoms_2 */
     }  /* m1 */
   }  /* l1 */
 }  /* i_atoms_1 */

/*************************************************************************
  If all the atoms are in the same plane (i.e. first_atoms * (l_max + 1)^2
  is equal to the dimension of Mbg) the inversion is already complete. 
=> Return here.
*************************************************************************/

 iaux = first_atoms * (l_max + 1)*(l_max + 1);
 if(iaux == Mbg->cols)
 {
#ifdef CONTROL
 fprintf(STDCTR,"(ms_partinv): All atoms are in the same plane.\n");
#endif

   matfree(Maux_a);
   matfree(Maux_b);

   Minv = matcop(Minv, UL);
   matfree(UL);

   return(Minv);
 }

/*************************************************************************
 Calculate submatrices of the inverse of Mbg according to:

 Mbg = (UL UR)  Minv = (Mbg)^-1 = (P Q)
       (LL LR)                    (R S)
      
 where:
        S = (LR - (LL*UL^-1)*UR )^-1
        R = -S*(LL*UL^-1)
        Q = -(UL^-1)*UR * S
        P = (UL^-1) + (UL^-1)*UR * S * (LL*UL^-1)

*************************************************************************/

/*************************************************************************
  First
  Extract UR (upper right), LL (lower left), and LR (lower right) from Mbg.
  Allocate Maux_a, Maux_b.
*************************************************************************/

 iaux = first_atoms * (l_max + 1)*(l_max + 1);
 UR = matext(UR, Mbg, 1, iaux, iaux+1, Mbg->cols);
 LL = matext(LL, Mbg, iaux+1, Mbg->rows, 1, iaux);
 LR = matext(LR, Mbg, iaux+1, Mbg->rows, iaux+1, Mbg->cols);

#ifdef CONTROL
 fprintf(STDCTR,"\n(ms_partinv):\tUL(%d x %d) UR(%d x %d)\n",
                 UL->rows,UL->cols, UR->rows,UR->cols);
 fprintf(STDCTR,"\t\tLL(%d x %d) LR(%d x %d)\n",
                 LL->rows,LL->cols, LR->rows,LR->cols);
#endif

/*  
  Maux_a = (LL*UL^-1) 
  Maux_b = (LL*UL^-1)*UR ) = Maux_a * UR
*/

#ifdef CONTROL_X
 fprintf(STDCTR,"(ms_partinv): Maux_a\n");
#endif

 Maux_a = matmul(Maux_a, LL, UL);
#ifdef CONTROL_X
 fprintf(STDCTR,"(ms_partinv): Maux_b\n");
#endif

 Maux_b = matmul(Maux_b, Maux_a, UR);

/*
   Maux_b = -(LR - (LL*UL^-1)*UR) = Maux_b - LR (first real then imag. part)
   LR = Maux_b^-1 = -S
*/ 

#ifdef CONTROL_X
 fprintf(STDCTR,"(ms_partinv): LR\n");
#endif

 iaux = LR->cols * LR->rows;
 for(ptr_1 = LR->rel+1, ptr_2 = Maux_b->rel+1, ptr_end = LR->rel+iaux; 
     ptr_1 <= ptr_end; ptr_1 ++, ptr_2 ++)
 { *ptr_2 -= *ptr_1; }
   
 for(ptr_1 = LR->iel+1, ptr_2 = Maux_b->iel+1, ptr_end = LR->iel+iaux; 
     ptr_1 <= ptr_end; ptr_1 ++, ptr_2 ++)
 { *ptr_2 -= *ptr_1; }

 LR = matinv(LR, Maux_b);

/*
  Maux_b = (UL^-1)*UR
  R -> LL = - S * (LL*UL^-1) = LR * Maux_a
  Q -> UR = - (UL^-1)*UR * S = Maux_b * LR
*/

#ifdef CONTROL_X
 fprintf(STDCTR,"(ms_partinv): LL, UR\n");
#endif

 Maux_b = matmul(Maux_b, UL, UR);

 LL = matmul(LL, LR, Maux_a);
 UR = matmul(UR, Maux_b, LR);

/*
  P -> UL = (UL^-1) + (UL^-1)*UR * S * (LL*UL^-1) 
          = UL - Maux_b * LL
*/

#ifdef CONTROL_X
 fprintf(STDCTR,"(ms_partinv): UL\n");
#endif

 Maux_b = matmul(Maux_b, Maux_b, LL);
 
 iaux = UL->cols * UL->rows;
 for(ptr_1 = UL->rel+1, ptr_2 = Maux_b->rel+1, ptr_end = UL->rel+iaux;
     ptr_1 <= ptr_end; ptr_1 ++, ptr_2 ++)
 { *ptr_1 -= *ptr_2; }
  
 for(ptr_1 = UL->iel+1, ptr_2 = Maux_b->iel+1, ptr_end = UL->iel+iaux;
     ptr_1 <= ptr_end; ptr_1 ++, ptr_2 ++)
 { *ptr_1 -= *ptr_2; }

/*
  S -> LR = -LR
*/
#ifdef CONTROL_X
 fprintf(STDCTR,"(ms_partinv): S\n");
#endif

 iaux = LR->cols * LR->rows;
 for(ptr_1 = LR->rel+1, ptr_2 = LR->iel+1, ptr_end = LR->rel+iaux;
     ptr_1 <= ptr_end; ptr_1 ++, ptr_2 ++)
 { 
   *ptr_1 = -*ptr_1; 
   *ptr_2 = -*ptr_2;
 }

 matfree(Maux_a);
 matfree(Maux_b);

/*************************************************************************
  insert UL (upper left), UR (upper right), LL (lower left), and
  LR (lower right) into Minv.
*************************************************************************/

#ifdef CONTROL_X
 fprintf(STDCTR,"(ms_partinv): insert\n");
#endif

 Minv = matalloc(Minv, Mbg->rows, Mbg->cols, NUM_COMPLEX);

 iaux = first_atoms * (l_max + 1)*(l_max + 1) + 1;
 Minv = matins(Minv, UL, 1, 1);
 Minv = matins(Minv, UR, 1, iaux);
 Minv = matins(Minv, LL, iaux, 1);
 Minv = matins(Minv, LR, iaux, iaux);

 matfree(UL);
 matfree(UR);
 matfree(LL);
 matfree(LR);

 return(Minv);
} /* end of function ms_partinv */

/*======================================================================*/
