/**************************************************************************
    File Name: getint.c

**************************************************************************/

/***************************************************************************/
#include "mkiv.h"
#include <stdlib.h>
/***************************************************************************/

int get_int(int nspot,              /* number of spots */
            Spot spot[],            /* list of measurable reflexes */
            ImageMatrix *image,     /* matrix of image data */
            ImageMatrix *imask,     /* matrix of mask image data */
            Vector *scale,          /* half axes of elliptical int. area */
            float angle,            /* angle of int.area versus horizontal */
            float use_cur,          /* normalization factor= beam current */
            int bg,                 /* background subtraction flag */
            float mins2n,           /* minimum signal-to-noise value */
            int verb,               /* verbose output flag */
            float verh,             /* integration area ratios */
            float acci,             /* integration area ratios */
            float accb)             /* integration area ratios */

/****************************************************************************
  GH /25.08.92
  CS /9.8.93

 Purpose:
  getint serves to :
  - integrate over an elliptical spot area ( axes defined as scale->xx/yy )
  - integrate over an elliptical background area (axes = VERH*scale->xx/yy )
  - if ( bg == 1 ) -> subtract background from spot intensity 
  - calculate S/N ratio :
  - set spot[i].control bit SPOT_GOOD_S2N if spot has good s/n ratio
  - set spot[i].control bit SPOT_OUT if spot touches boundaries :
    acci = percentage of spot area that must be within visible LEED-screen
    accb = percentage of background area - " -

****************************************************************************/
{
    register int i, j, k, val, pos;
    int a_back;
    int e_counter=0, *e_count;        /* no.of pixels within spot area      */
    int b_counter=0, *b_count;        /* no.of pixels within background area*/
    float x_fac, y_fac, xy_fac, ellipse, b_norm;
    float sgma, sgmb;
    register int cols = image->rows,        /* define image size */
                 rows = image->cols;
    float h = scale->xx,
          v = scale->yy;
    unsigned short *im = (unsigned short *)image->imagedata;
    unsigned short *mask = (unsigned short *)imask->imagedata;

/***************************************************************************/

    /* Allocate memory for e_count, b_count */
    e_count = (int *)calloc( nspot, sizeof(int) );
    b_count = (int *)calloc( nspot, sizeof(int) );
    if ( e_count == NULL || b_count == NULL ) 
        ERR_EXIT(getint : Memory allocation failed)

    /* Initialize */
    angle *= PI/180.;
    x_fac = PYTH2(cos(angle)/h,sin(angle)/v);
    y_fac = PYTH2(sin(angle)/h,cos(angle)/v);
    xy_fac = sin(angle) * cos(angle) * ( 2./(h*h) - 2./(v*v) );

    for (k = 0; k < nspot; k++) { 
        if ( spot[k].control & SPOT_GOOD_S2N ) continue;
        spot[k].intensity = 0.;
        spot[k].s2n = 0.;
        spot[k].s2u = 0.;
    }

/****************************************************************************

  Search for the points which lie inside the (rotated) ellipses around the 
  spot maxima and sum up the intensities for each spot ( list spot ).

  A point (x,y) being inside an ellipsis with half axes h and v is defined by 
  the following equation:
                  x*x/(h*h) + y*y/(v*v) <= 1.                 (1)
  If the coordinate system is rotated about (-angle),  x and y are transformed:
                  x(new) =  x*cos(angle) + y*sin(angle)       (2)
                  y(new) = -x*sin(angle) + y*cos(angle) 
  The ellipsis equation (1) then transforms as:
                x*x* x_fac + y*y* y_fac + x*y * xy_fac <= 1.  (3)
  
  spot.s2u is temporarily used as background.  
  spot.s2n is temporarily used as SQUARE(background)

****************************************************************************/

    /* loop over relative coordinates */
    a_back = RND( verh * MAX(h,v) );
    for (j = -a_back; j <= a_back; j++) {
        for (i = -a_back; i <= a_back; i++) {
            /* equation (3) - see above */
            ellipse = (i*i* x_fac) + (j*j* y_fac) + (i*j* xy_fac);
            if (ellipse > SQUARE(verh)) continue;    /* outside */
            if (ellipse <= 1.) e_counter++;
            else               b_counter++;
            
            /* loop over spots */
            for (k = 0; k < nspot; k++) {
            
                if( spot[k].control & SPOT_GOOD_S2N )
                    continue; /*already done*/
                    
                pos =((int)spot[k].yy+j)*cols +(int)spot[k].xx+i;
                
                if ( pos < 0 || pos >= cols*rows )
                    continue; /* out of frame */
                    
                if ( imask && !mask[pos] ) 
                    continue; /* outside masked area */
                val = im[ pos ];

                if (ellipse>1.) {
                    b_count[k]++;
                    spot[k].s2u += val;
                    spot[k].s2n += SQUARE(val); 
                }
                else {
                    e_count[k]++;
                    spot[k].intensity += val; 
                }
            }
        }  
     }   

    /* Normalize background.  Subtract background.  calculate s/n ratio */

    for (k = 0; k < nspot; k++) {
        if( spot[k].control & SPOT_GOOD_S2N ) continue;
        if ( (float)e_count[k]/(float)e_counter < acci ||
             (float)b_count[k]/(float)b_counter < accb   ) {
            
            spot[k].control |= SPOT_OUT;
            spot[k].intensity = INT_OUT;
            VPRINT(" (%5.2f,%5.2f)  %3.0f|%3.0f  ***out***\n",
            spot[k].lind1,spot[k].lind2,spot[k].xx,spot[k].yy );
            continue;
        }
	
        b_norm = (float)e_count[k] / (float)b_count[k];
        if ( b_norm < TOLERANCE || b_norm > 1./TOLERANCE ) continue;

        sgma =sqrt( spot[k].s2n -SQUARE(spot[k].s2u)/b_count[k] )/b_count[k];
        sgmb =spot[k].intensity/e_count[k] -spot[k].s2u/b_count[k];
        spot[k].s2n = sqrt( spot[k].s2n*b_count[k] - SQUARE(spot[k].s2u) );
        spot[k].s2n /= spot[k].s2u;

        if ( bg == BG_YES ) {
           spot[k].s2u *= b_norm;
           spot[k].intensity -= spot[k].s2u;
           spot[k].s2u = spot[k].intensity / spot[k].s2u;
           spot[k].intensity /= use_cur;
        }
        else {
           spot[k].s2u *= b_norm;
           spot[k].s2u = spot[k].intensity / spot[k].s2u - 1.;
           spot[k].intensity /= use_cur;
        }
        spot[k].s2n = spot[k].s2u / spot[k].s2n;
        QQ {  
            printf(" (%5.2f,%5.2f)", spot[k].lind1, spot[k].lind2);
            printf("  %3.0f|%3.0f", spot[k].xx, spot[k].yy);
            printf("  i:%7.3f", spot[k].intensity);
            printf("  su:%6.3f  sn:%5.2f", spot[k].s2u, spot[k].s2n );
            printf("  n:%4d", b_count[k]);
            printf("  snn:%5.1f", sgmb/sgma);
        }
        if ( spot[k].s2n > mins2n )
            spot[k].control |= SPOT_GOOD_S2N;
        else 
            QQ printf(" bad");
        QQ printf("\n");
    }
    free( e_count );
    free( b_count );
    
    return 0;
}
/***************************************************************************/
