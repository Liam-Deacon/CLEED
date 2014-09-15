
/**************************************************************************<
    >
    >         File Name: calcoi.c
    >
>**************************************************************************/

/**************************************************************************/
#include "mkiv.h"
/**************************************************************************/
 
int calcoi(
        int nspot,              /* number of spots in list spot */
        struct spot spot[],     /* list of spots */
        float range,            /* radius of area for computation */
        ImageMatrix *image      /* image as an internal matrix structure */
    )

/***************************************************************************
  CS/9.8.93    

 Purpose:
  calcoi.c recalculates the center of each spot using the
  center of gravity method within the disc: center = position found by fimax
                                            radius = range 
  if spot[i].control -bit SPOT_GOOD_S2N is set, the calculation is suppressed.

 Input:
  list spot with all measurable reflexes

 Output:
  spot.yy/xx = the calculated spot-positions

***************************************************************************/

{
   register int i,v,h,val,pos,        /* auxiliaries */
            hsum,vsum,valsum,v0,h0;
   int lowh,high,lowv,higv;           /* boundaries of area */

   unsigned short *im = (unsigned short *)image->imagedata;
   int cols = image->rows,
       rows = image->cols;

/**************************************************************************/

/* loop over all apots in list spot */

   for( i=0; i<nspot; i++)
   {
      if( spot[i].control & SPOT_GOOD_S2N ) continue;

      h0 = (int) spot[i].xx;
      v0 = (int) spot[i].yy;

/* find boundaries */

      lowh = MAX( h0-(int)range , 0 ); 
      high = MIN( h0+(int)range , cols-1 );

      lowv = MAX( v0-(int)range , 0 );
      higv = MIN( v0+(int)range , rows-1 );

/* loop over integration area rectangle */

      hsum = 0;
      vsum = 0;
      valsum = 1;

      for( v=lowv; v<=higv; v++)
      {
	 for( h=lowh; h<=high; h++)
	 {
	    pos = v*cols+h;
	    if ( pos >= cols*rows || pos < 0 ) continue; /* out of frame */
	    val = im[ pos ];
	    hsum += val * (h-h0);
	    vsum += val * (v-v0);
	    valsum += val;
         }
      }

/* calculate the center of gravity according to the formula :
	   mean(x) = sum(val * x) / sum(val)                    */

      spot[i].xx = h0 + (float)hsum/(float)valsum;
      spot[i].yy = v0 + (float)vsum/(float)valsum;
   }
   
   return 0;
}
/**************************************************************************/
