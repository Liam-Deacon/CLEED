/***************************************************************************

     Filename: outtif.c


**************************************************************************

VJ 07/02/03
GH: 21.03.03

Purpose: out_tif converts the internal image matrix into TIFF parameters for 
display purposes and then writes the new data into an output 
TIFF file. Used in markref.c and drawbound.c
    
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "mkiv.h"

/**************************************************************************/

int out_tif(ImageMatrix *mat_image, char *filename)
{

    tifvalues *tif_image;

    /* Allocate memory for tifvalues pointer tif_image*/
    tif_image = (tifvalues *)malloc(sizeof(tifvalues));
    if(tif_image == NULL) {
       fprintf(stderr, "failed to allocate memory for tif_image");
       exit(1);
    }
    tif_image->buf = NULL;

    /* Convert ImageMatrix into tifvalues & copy all imagedata to buf*/
    conv_mat2tif(mat_image, tif_image);

    /* Write the 8 bit TIFF to a file */
    writetif(tif_image, filename);

    free(tif_image->buf);
    free(tif_image);

    return(0);
}
