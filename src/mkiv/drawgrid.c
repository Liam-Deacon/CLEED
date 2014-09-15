/**************************************************************************<
    
             File Name: drawgrid.c
    
**************************************************************************/

#include "mkiv.h"

#include <stdlib.h>

/*
#define WHITE 0xFFFF
*/
#define BLACK 0x0000
 
int drawgrid(
        ImageMatrix *image, 
        int write, 
        char *fname
    )

/***************************************************************************
  CS/18.08.93
  VJ/06.02.03    
  drawbound draws the boundaries of the LEED-screen,
  i.e. either the mask or router/rinner, if no mask was defined
  write != 0 : write to ima.byte

  ImageMatrix *image - pointer to image-structure
  int write - =1 -> write to ima.byte 
***************************************************************************/

{
    int i,j,num;
    int cols, rows;

    unsigned short *im;
    char file_path[FILENAME_MAX];

/**************************************************************************/

    cols = image->rows;
    rows = image->cols;

    im = (unsigned short *) image->imagedata ;

    for (i=0; i<rows; i++) {
        for (j=0; j<cols; j+=50) {
            num = i*cols+j;
            im[num] = 0xFFFF - im[num];
        }
    }
    for (i=0; i<rows; i+=50) {
        for (j=0; j<cols; j++) {
            num = i*cols+j;
            im[num] = 0xFFFF - im[num];
        }
    }

    if (write < 0) 
        strcpy(file_path, fname);
    else if (write > 0) 
        strcpy(file_path, "ima.byte");
    
    
    if (write)      /* write to "ima.byte" if desired */ 
    {
        if (out_tif(image, fname))
            ERR_EXIT_X("(drawgrid): writing image '%s' failed!", fname);
    }
    
    return 0;
}
/**************************************************************************/
