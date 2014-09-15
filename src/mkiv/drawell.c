/****************************************************************************
GH/27.07.03

  drawell.c

  drawell draws the elliptical integration area boundaries into image.

CHANGES:
  CS /9.8.93
  GH/27.07.03 - circle colour = max_value
****************************************************************************/

#include "mkiv.h"


int drawell(int nspot, Spot spot[], ImageMatrix *image, Vector *scale, 
            float angle, float verh)

/****************************************************************************
  CS /9.8.93
  drawell draws the elliptical integration area boundaries into image.

int nspot;                  number of spots                     
struct spot spot[];         list of measurable reflexes         
ImageMatrix *image;         image matrix structure                
struct vector a[], *scale;  half axes of elliptical int. area   
float angle;                angle of int.area versus horizontal 
float verh;                 ratio bg/integr. ellipses           

****************************************************************************/
{
    register int i,j,k,pos;
    int a_back;
    int cols, rows, n_size;

    float x_fac, y_fac, xy_fac, ell;
    float h = scale->xx,
          v = scale->yy;

    unsigned short max_val;
    unsigned short *im;

/***************************************************************************/

    cols = image->rows;
    rows = image->cols;
    im = (unsigned short *)image->imagedata;

    /* find maximum value in image->imagedata */

    n_size = cols * rows;
    max_val = 1;

    for( i = 0; i < n_size; i ++) 
        max_val = MAX(max_val, im[i]);


    angle *= PI/180.;
    x_fac = PYTH2(cos(angle)/h,sin(angle)/v);
    y_fac = PYTH2(sin(angle)/h,cos(angle)/v);
    xy_fac = sin(angle) * cos(angle) * ( 2./(h*h) - 2./(v*v) );

    a_back = RND( verh * MAX(h,v) );
    for (j = -a_back; j <= a_back; j++) {
        for (i = -a_back; i <= a_back; i++) {
            ell = i*i*x_fac + j*j*y_fac + i*j*xy_fac;

            if (fabs(ell-1.)>.07 && fabs(ell-SQUARE(verh))>.13)
                continue;

            /* loop over spots */
            for (k = 0; k < nspot; k++) {  
                if ( spot[k].control & SPOT_OUT )
                    continue;
                
                pos =((int)spot[k].yy +j)*cols +(int)spot[k].xx +i;
                if( pos < cols*rows && pos > 0 )
                    im[pos] = max_val;
            }
        }
    }
    return 0;
}
/***************************************************************************/
