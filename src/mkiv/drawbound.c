/***************************************************************************
GH/27.07.03
 
         File Name: drawbound.c

  Drawbound draws the boudaries of the LEED-screen,
  i.e. either the mask or router/rinner, if no mask was defined
  write != 0 : write to ima.byte

CHANGES
CS/18.08.93
VJ/06.02.03 - Creation
GH/27.07.03 - adjust all values to maximum value in image

**************************************************************************/

#include "mkiv.h"

#include <stdlib.h>
#include <stdio.h>

#ifndef WHITE
#define WHITE 0xFFFF
#endif
#ifndef BLACK
#define BLACK 0x0000
 #endif
 
int drawbound(ImageMatrix *image, ImageMatrix *imask, Coord *center, 
              float router, float rinner, int write, char *fname)

/***************************************************************************
  CS/18.08.93
  VJ/06.02.03    
  LD/16.06.14 - update for ImageMatrix
  drawbound draws the boudaries of the LEED-screen,
  i.e. either the mask or router/rinner, if no mask was defined
  write > 0 : write to ima.byte
  write < 0 : write to fname
***************************************************************************/

/* 
ImageMatrix *image, *imask;        pointer to image-structure
struct coord *center;              center of LEED-screen
float router,rinner;               boundaries of LEED-screen
int write;                         =1 -> write to ima.byte
char *fname; 
*/
{
    int i,j,num;
    int h,v;
    int cols, rows;

    unsigned short max_val;

    float phi;
    unsigned short *mask, *im;
    
    char file_path[FILENAME_MAX];

/**************************************************************************/

    h = center->xx;
    v = center->yy;
    cols = image->rows;
    rows = image->cols;

    im = (unsigned short *) image->imagedata ;

    if (!imask) {                        /* no mask -> draw router, rinner */
        for (i= -20; i<=20; i++) {	/* draw center */
            im[v*cols+(h+i)]= WHITE;
            im[(v+i)*cols+h]= WHITE;
        }
        for (phi=0; phi<2*PI; phi+=.005) {	/* draw router and rinner */
            im[cols*(int)(v+rinner*sin(phi))+ h+(int)(rinner*cos(phi))]= WHITE;
            im[cols*(int)(v+router*sin(phi))+ h+(int)(router*cos(phi))]= WHITE;
        }
    }
    else  {                               /* use mask */
        mask = (unsigned short *) imask->imagedata;

        /* First: find max. value: */
        max_val = 1;
        for (i=0; i<rows; i++) {
            for (j=0; j<cols; j++) {
                num = i*cols+j;
                max_val = MAX(im[num], max_val);
            }
        }

        /* invert all values outside the active area: */
        for (i=0; i<rows; i++) {
            for (j=0; j<cols; j++) {
                num = i*cols+j;
            /*
                  if ( mask[num] == BLACK )   im[num] = ~im[num];
                  if ( mask[num] == BLACK )   im[num] = 0xFFFF - im[num];
                  if ( mask[num] == BLACK )   im[num] = max_val - im[num];
            */
            if ( mask[num] == BLACK ) 
                im[num] = BLACK;
            }
        }
     
        for (i=0; i<rows; i++) {
            for (j=0; j<cols; j+=50) {
                num = i*cols+j;

                im[num] = max_val - im[num];
            }
        }
        
        for (i=0; i<rows; i+=50) {
            for (j=0; j<cols; j++) {
                num = i*cols+j;
                im[num] = max_val - im[num];
            }
        }

    }

    if (write < 0)
        strcpy(file_path, fname);
    else if(write > 0)
        strcpy(file_path, "ima.byte");
    
    if (write)      /* write to "ima.byte" if desired */ 
    {
        if (out_tif(image, file_path))
            ERR_EXIT_X("(drawbound): failed to write to '%s'!", file_path);
    }
    
    return 0;
}
/**************************************************************************/
