/****************************************************************************
GH/28.07.03
LD/02.03.14

File Name: v_writetif.c

Purpose:
    write tiff output images
****************************************************************************/

#include <stdio.h>
#include "mkiv.h"

int writetif (tifvalues *tifimage, char *v_output)
{
    unsigned long row;

    tifimage->tif_out = (TIFF *)TIFFOpen (v_output, "w");

    TIFFSetField (tifimage->tif_out, TIFFTAG_IMAGEWIDTH, tifimage->imagewidth);
    TIFFSetField (tifimage->tif_out, TIFFTAG_IMAGELENGTH, tifimage->imagelength);
    TIFFSetField (tifimage->tif_out, TIFFTAG_BITSPERSAMPLE, tifimage->bitspersample);
    TIFFSetField (tifimage->tif_out, TIFFTAG_COMPRESSION, tifimage->compression);
    TIFFSetField (tifimage->tif_out, TIFFTAG_PHOTOMETRIC, tifimage->photometricinterpretation);
    TIFFSetField (tifimage->tif_out, TIFFTAG_ROWSPERSTRIP, tifimage->rowsperstrip);
    TIFFSetField (tifimage->tif_out, TIFFTAG_XRESOLUTION, tifimage->xresolution);
    TIFFSetField (tifimage->tif_out, TIFFTAG_YRESOLUTION, tifimage->yresolution);
    TIFFSetField (tifimage->tif_out, TIFFTAG_PLANARCONFIG, tifimage->planarconfiguration);
    TIFFSetField (tifimage->tif_out, TIFFTAG_RESOLUTIONUNIT, tifimage->resolutionunit);

    for (row = 0; row < tifimage->imagelength; row++) {
        TIFFWriteScanline( tifimage->tif_out, 
                           (tifimage->buf+row*tifimage->imagewidth), 
                           row, 
                           2*tifimage->imagewidth);
    }

    TIFFClose(tifimage->tif_out);
    
    return 0; /* success */
}
