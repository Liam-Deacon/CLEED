/**************************************************************************<
    
             File Name: calca.c
    
**************************************************************************/

/**************************************************************************/
#include "mkiv.h"
/**************************************************************************/
 
int calca(float *kappa, float *en_old, float energy,
      struct vector a[], float *range, float range_min, float rel_range, 
      struct vector *scale, float scale_min, struct vector rel_scale)

/***************************************************************************
  CS/8.9.93    

 Purpose:
  calca() computes the new values of a, range and scale according to the
  new energy value and given boundaries ( i.e. range_min, scale_min )
  
***************************************************************************/
{
   float fval;

/**************************************************************************/

   *kappa = sqrt(*en_old/energy);
   a[1].xx *= *kappa;
   a[1].yy *= *kappa;
   a[1].len = PYTH( a[1].xx, a[1].yy );
   a[2].xx *= *kappa;
   a[2].yy *= *kappa;
   a[2].len = PYTH( a[2].xx, a[2].yy );
   *range = MAX( range_min, rel_range * a[1].len );

   scale->xx = rel_scale.xx * a[1].len;
   scale->yy = rel_scale.yy * a[1].len;

   if ( ( fval = PYTH(scale->xx,scale->yy)/scale_min/ROOT2 ) < 1. ) {
     scale->xx /= fval;
     scale->yy /= fval;
   }
   *en_old = energy;
   
   return 0;
}
/**************************************************************************/
