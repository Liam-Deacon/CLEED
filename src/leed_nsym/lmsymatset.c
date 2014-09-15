/*********************************************************************
  GH/21.04.95 
  file contains functions:

  leed_ms_ymat_set            (21.04.95)
     Create the transformation matrix from angular momentum space 
     into k-space (Ylm(k)) for a given beam set.

Changes:
GH/21.04.95 - copied from leed_ms_ymat

*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "leed.h"

/*======================================================================*/
/*======================================================================*/

static mat Ylm = NULL;

mat leed_ms_ymat_set ( mat Ymat, int l_max, leed_beam_t *beams, int set)

/************************************************************************

 Create the transformation matrix Ymat from angular momentum space into
 k-space: Ylm(k).

 INPUT:

   mat Ymat - (output) transformation matrix.
   int l_max - max l quantum number spanning up the (l,m)-space.
   leed_beam_t *beams - beams spanning up the k-space.
   n_beams - number of beams in list beams.

 RETURN VALUES:

   NULL if failed (and EXIT_ON_ERROR is not defined)

   Ymat (may be different from input parameter).

   The order of Ymat is:

   k / (l,m) (0,0)     (-1,1)    (0,1)   (1,1)   (-2,2) ....

   k1       Y00(k1)   Y-11(k1)  Y01(k1) 
   k2       Y00(k2)   Y-11(k2)  Y01(k2) 
   k3       Y00(k3) 
   ...                                     Y(l,m)(ki) 

   

*************************************************************************/
{
int ll_max;                    /* total number of (l,m) pairs */
int off;                       /* offset in the arrays Ylm and Llm */
int size;
int i_beams, n_beams;

/**************************************************************************
  Check beam list for number of beams in current set (n_beams).
**************************************************************************/
 for( i_beams = 0, n_beams = 0; 
      ! IS_EQUAL_REAL((beams + i_beams)->k_par, F_END_OF_LIST); i_beams ++) 
  {
    if((beams + i_beams)->set == set) n_beams ++;
  }

#ifdef CONTROL
   fprintf(STDCTR,"(leed_ms_ymat_set): No of beams in set %d = %d\n", set, n_beams);
#endif

/**************************************************************************
  Allocate memory for Ymat 
**************************************************************************/
 ll_max = (l_max + 1)*(l_max + 1);
 Ymat = matalloc( Ymat, n_beams, ll_max, NUM_COMPLEX );

/**************************************************************************
  Loop over k's in the current beam set:
  Calculate the sperical harmonics Ylm(k) and copy into Ymat
**************************************************************************/

 size = ll_max*sizeof(real);

 for ( i_beams = 0, off = 1; 
      ! IS_EQUAL_REAL((beams + i_beams)->k_par, F_END_OF_LIST);
       i_beams ++, off += ll_max)
 {
   if( (beams + i_beams)->set == set)
   {
#ifdef CONTROL
     fprintf(STDCTR,"(leed_ms_ymat_set): cos(th): (%.3f %.3f), phi: %.3f\n", 
             (beams+i_beams)->cth_r, 
             (beams+i_beams)->cth_i, (beams+i_beams)->phi);
#endif

     Ylm = c_ylm(Ylm, (beams+i_beams)->cth_r, (beams+i_beams)->cth_i,
                      (beams+i_beams)->phi, l_max);
     memcpy( Ymat->rel+off, Ylm->rel+1, size);
     memcpy( Ymat->iel+off, Ylm->iel+1, size);
   }
 } 

 return(Ymat);
}
/*======================================================================*/
