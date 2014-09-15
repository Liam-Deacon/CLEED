
/**************************************************************************
     
              File Name: fimax4.c
             
 **************************************************************************/

/***************************************************************************/
#include "mkiv.h"
/***************************************************************************/
 
int fimax4(int nspot, Spot spot[], int step, float range, ImageMatrix *image)

/****************************************************************************
  SU/10.07.91
  GH/31.08.92  kernel method 
  CS/5.8.93    all positive kernel
  CS/20.8.93   

 Purpose:
  fimax4 scans for each spot through a disc (radius = range) searching for
  the maximum intensity.  A kernel is used, which is described below.
  If spot->control -bit SPOT_GOOD_S2N is set, no max. search is performed.

 Output: 
  - coordinates of maximum spot[i].xx/yy

int nspot;                      number of measurable spots
struct spot spot[];             list of spots
int step;                       step size for maximum search
float range;                    radius of search area in pixels  
ImageMatrix *image;             pointer to the image structure

****************************************************************************/
{
    register int i, h, v,       /* auxiliaries */
                 sum, adr0;   
    int lowh, lowv, high, higv; /* horiz.and vert.boundaries of search area */
    long max_sum;               /* integration sum, total maximum of all sum*/
    float h0, v0;               /* calculated spot positions */

    /* the kernel that serves for a weighted integration [sum(kernel)=112] */
    int KL_H_SIZE = 3;          /*  half size of kernel */
    int nkernel = 7*7;
    int ad[7*7];
    static long kernel[7*7] ={  0, 1, 1, 1, 1, 1, 0 ,
                                1, 1, 2, 2, 2, 1, 1 ,
                                1, 2, 4, 8, 4, 2, 1 ,
                                1, 2, 8,16, 8, 2, 1 ,
                                1, 2, 4, 8, 4, 2, 1 ,
                                1, 1, 2, 2, 2, 1, 1 ,
                                0, 1, 1, 1, 1, 1, 0 };      

    register int k = 0, cols = image->rows;
    int rows = image->cols;
    unsigned short *im = (unsigned short *)image->imagedata;

/***************************************************************************/
     
    for (v = -KL_H_SIZE; v<= KL_H_SIZE; v++)
        for (h = -KL_H_SIZE; h<= KL_H_SIZE; h++)
            ad[k++] = v*cols + h;

    for (i=0; i<nspot; i++) {
        if( spot[i].control & SPOT_GOOD_S2N )
            continue;

        h0 = spot[i].xx;
        v0 = spot[i].yy;

        /* find boundaries */

        lowh = MAX( (int)(h0-range) , KL_H_SIZE );
        high = MIN( (int)(h0+range) , cols-KL_H_SIZE-1 );

        lowv = MAX( (int)(v0-range) , KL_H_SIZE );
        higv = MIN( (int)(v0+range) , rows-KL_H_SIZE-1 );

        /* loop over searching area */
        /* v and h point to the center of the kernel = 
                            center of integration area */

        max_sum = 0L;
        for (v = lowv; v <= higv; v+=step) {
            for (h = lowh; h <= high; h+=step) {
                if ( PYTH(v-v0,h-h0) > range ) 
                    continue; /* spot out of range */
                adr0 = v*cols +h;
                sum = 0L;
                for (k=0; k<nkernel; k++) 
                    sum += kernel[k] *im[ adr0+ad[k] ];

                /* store coordinates when intensity is max */

                if (sum>max_sum) {
                    spot[i].yy = (float)v;
                    spot[i].xx = (float)h; 
                    max_sum = sum;
                }
            }       /* for(h = ...) */
        }       /* for(v = ...) */
    }       /* for(i < nspot) */
    
    return 0;
}
/****************************************************************************/
