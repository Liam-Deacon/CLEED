/**************************************************************************<
    File Name: mkmask.c
    
**************************************************************************/

/**************************************************************************/
#include "mkiv.h"
#include <stdlib.h>
/**************************************************************************/
 
int mkmask(
        ImageMatrix *image,     /* pointer to matrix structure of image */
        Coord *center,          /* centre of LEED screen */
        float router,           /* outer radius (boundary) of LEED screen */
        float rinner,           /* inner radius (boundary) of LEED screen */
        int write,              /* flag to write mask:
                                      write >= 0 -> write to ima.byte
                                      write <= 0 -> write to mask_file */
        char *mask_path         /* filepath of mask file if writing 
                                   e.g. ima.byte */
    )

/***************************************************************************
  CS/18.08.93
  VJ/06.02.03    
  GH/11.03.03
  LD/16/06/14 - update for ImageMatrix
  
  drawbound draws the boudaries of the LEED-screen,
  i.e. either the mask or router/rinner, if no mask was defined
  write >= 0 : write to ima.byte
  write <= 0 : write to mask_file

***************************************************************************/
{

    int i, n_size;
    float phi, rad;
    int h,v;
    int cols;
    char *fname;
    unsigned short *mask;

/**************************************************************************/

    cols = image->rows;
    h = center->xx; v = center->yy;
    mask = (unsigned short *) image->imagedata;
    fname = (char*) malloc(sizeof(char)*FILENAME_MAX);

    /* no mask -> draw router, rinner */
    n_size = image->rows * image->cols;
    for ( i = 0; i <  n_size; i++)
        mask[i] = 0;

    for (phi=0; phi<2*PI; phi+=.001) {	
        /* draw router and rinner */
        for( rad = rinner; rad <= router; rad += 0.5) {
            mask[cols*(int)(v+rad*sin(phi))+ h+(int)(rad*cos(phi))]= 0xFFFF;
        }
    }

    
    if (write > 0) 
    {
        strcpy(fname, "mask.byte");
    }
    else if(write < 0) {
        strcpy(fname, mask_path);
    }
    else {
        strcpy(fname, "mask_new.byte");
    }
    
    if (write)
    {
        if (out_tif(image, fname)) 
            fprintf(stderr, "***error (out_tif): "
                    "write to '%s' failed!\n", fname);
    }
    else {
        fprintf(stderr, "write mask to '%s'\n", fname);
        if (out_tif(image, fname))
            ERR_EXIT_X("(out_tif): write to '%s' failed!\n", fname);
    }
    return 0;
}
/**************************************************************************/
