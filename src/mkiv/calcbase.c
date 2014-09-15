
/**************************************************************************
    
             File Name: calcbase.c
    
>**************************************************************************/

/***********************************************************************/
#include "mkiv.h"
/***********************************************************************/

short calcbase(int naux, struct spot aux[], struct vector a[],
               float ratio, int distance, int trip_max, float cor)

/************************************************************************ 
   SU/ 91
   GH/ 11.08.92
   CS/11.8.93

  Purpose:
   determination of origin and two basic vectors from at least 3 spots.
  
  Input:
   naux,aux      number of and reference spots
   a[0]          position of (0,0)-reflex
   ratio         ratio between screen radius and camera distance
   distance      minimum distance for spots used for recalibration
   cor           cor > 0 : used as recorrection factor
		 cor < 0 : no recorrection, no input-origin
		 cor = 0 : recorrection with spot.cos_th

  Output:
   a[1],a[2]     basic rec.lattice vectors
   a[0]          position of (0,0)-reflex

************************************************************************/

{
     register int i, j, k, ntrip;
     float det, x11, x12, x21, x22;          /* Matrix to invert */
     float cor_fac;
     float dposh[2], dposv[2], dind1[2], dind2[2];
     static struct vector V_ZERO = { 0., 0., 0.};

/***********************************************************************/

/* Initialization */

     a[1] = a[2] = V_ZERO;
     cor_fac = 1.;

/* get 'real' spot positions by dividing by the cor_fac */

     if ( cor >= -TOLERANCE )
     {
	if ( cor > TOLERANCE ) cor_fac = cor;
        for (i=0; i<naux; i++)
        {
	   if ( cor < TOLERANCE ) cor_fac = 1. + ratio*( 1. - aux[i].cos_th );
	   aux[i].xx = ( aux[i].xx - a[0].xx ) * cor_fac;   
	   aux[i].yy = ( aux[i].yy - a[0].yy ) * cor_fac;
        }
     }

/* use triples of spots in all combinations */

     ntrip = 0;
     for (i=0; i<naux-2; i++)
     {
	for (j=i+1; j<naux-1 && ntrip<trip_max; j++)
	{ 
	   dposh[1] = aux[j].xx - aux[i].xx;
	   dposv[1] = aux[j].yy - aux[i].yy;
	   dind1[1] = aux[j].lind1 - aux[i].lind1;
	   dind2[1] = aux[j].lind2 - aux[i].lind2;

	   for (k=j+1; k<naux; k++)
	   {
	      dposh[2] = aux[k].xx - aux[i].xx;
	      dposv[2] = aux[k].yy - aux[i].yy;
	      dind1[2] = aux[k].lind1 - aux[i].lind1;
	      dind2[2] = aux[k].lind2 - aux[i].lind2;

	      if ( PYTH( dposh[1]-dposh[2],dposv[1]-dposv[2]) < distance ||
		   PYTH( dposh[1],dposv[1] ) < distance ||
		   PYTH( dposh[2],dposv[2] ) < distance ) continue;

	      det = dind1[1]*dind2[2] - dind2[1]*dind1[2];
	      if ( fabs(det)>.3 && fabs(det)<1./TOLERANCE )
	      {
	         ntrip++;
	         det = 1./det;
                 x11 =  det * dind2[2];
                 x12 = -det * dind2[1];
                 x21 = -det * dind1[2];
                 x22 =  det * dind1[1];

/* Calculate basic vectors a1 and a2 : 

   (a1)       (x11 x12)       (pos1-pos0)
   (  )   =   (       )   *   (         )
   (a2)h/v    (x21 x22)       (pos2-pos0)h/v    */

	         a[1].xx += x11 * dposh[1] + x12 * dposh[2];
	         a[2].xx += x21 * dposh[1] + x22 * dposh[2];
		 a[1].yy += x11 * dposv[1] + x12 * dposv[2];
	         a[2].yy += x21 * dposv[1] + x22 * dposv[2];
	      }
           }
	}
     }
     if (ntrip > 0)
     {
        a[1].xx /= ntrip;
        a[1].yy /= ntrip;
        a[2].xx /= ntrip;
        a[2].yy /= ntrip;
     }
     a[1].len = PYTH( a[1].xx , a[1].yy );
     a[2].len = PYTH( a[2].xx , a[2].yy );

/* Initialize and recalculate orig */

     if ( cor < -TOLERANCE )        /* no input origin */
	a[0] = V_ZERO;
     else                   /* with input origin */
     {
	a[0].xx *= naux;
	a[0].yy *= naux;
     }

     for (i=0; i<naux; i++)
     {
	a[0].xx +=aux[i].xx -aux[i].lind1*a[1].xx -aux[i].lind2*a[2].xx;
        a[0].yy +=aux[i].yy -aux[i].lind1*a[1].yy -aux[i].lind2*a[2].yy;
     }
     a[0].xx /= naux;
     a[0].yy /= naux;
     a[0].len = PYTH( a[0].xx , a[0].yy );
     return(ntrip);
}
/***********************************************************************/
