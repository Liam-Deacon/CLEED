/****************************************************************************
VJ/   02.03
GH/28.07.03
LD/02.03.14

File Name: v_readtif.c

History:
VJ/   02.03
GH/19.03.03 - allocate tifvalues inside subroutine.
GH/27.07.03 - allocate buffer according to bitspersample (8/16)
LD/02.03.14 - check image files exist before trying to open & prevent IO error

****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "mkiv.h"

int readtif (tifvalues *tifimage, char *v_input)
{
  
    unsigned long row;
    long imagelength, scanlinesize;
  
   
    /* Check tifimage and allocate memory */

    if(tifimage == NULL) {
        fprintf(stderr, "*** error (v_readtif): structure does not exist\n");
        exit(1);
    }
    else {
        fprintf(stderr, "(v_readtif) free buffer\n");
        free(tifimage->buf);
        tifimage->buf = NULL;
    }

    /* Open input file and read parameters */
    if (!file_exists(v_input)) {
        fprintf(stderr, "*** error: cannot read tiff file '%s'\n", v_input);
        exit(1);
    }
    tifimage->tif_in = (TIFF *)TIFFOpen(v_input, "r");

    TIFFGetField (tifimage->tif_in, TIFFTAG_IMAGEWIDTH, &tifimage->imagewidth);
    TIFFGetField (tifimage->tif_in, TIFFTAG_IMAGELENGTH, &tifimage->imagelength);
    TIFFGetField (tifimage->tif_in, TIFFTAG_BITSPERSAMPLE, &tifimage->bitspersample);
    TIFFGetField (tifimage->tif_in, TIFFTAG_COMPRESSION, &tifimage->compression);
    TIFFGetField (tifimage->tif_in, TIFFTAG_PHOTOMETRIC, &tifimage->photometricinterpretation);
    TIFFGetField (tifimage->tif_in, TIFFTAG_STRIPOFFSETS, &tifimage->stripoffsets);
    TIFFGetField (tifimage->tif_in, TIFFTAG_ROWSPERSTRIP, &tifimage->rowsperstrip);
    TIFFGetField (tifimage->tif_in, TIFFTAG_STRIPBYTECOUNTS, &tifimage->stripbytecounts);
    TIFFGetField (tifimage->tif_in, TIFFTAG_XRESOLUTION, &tifimage->xresolution);
    TIFFGetField (tifimage->tif_in, TIFFTAG_YRESOLUTION, &tifimage->yresolution);
    TIFFGetField (tifimage->tif_in, TIFFTAG_PLANARCONFIG, &tifimage->planarconfiguration);
    TIFFGetField (tifimage->tif_in, TIFFTAG_RESOLUTIONUNIT, &tifimage->resolutionunit);

/*********************************************
  Allocate buffer for image data:
   8 bit:
    imagelength =  tifimage->imagelength;
    scanlinesize = TIFFScanlineSize(tifimage->tif_in);

   16 bit:
    imagelength =  2 * tifimage->imagelength;
    scanlinesize = TIFFScanlineSize(tifimage->tif_in);
*********************************************/
    fprintf(stdout,"(v_readtif): read file %s\n", v_input);

    if(tifimage->bitspersample == 8) {
        fprintf(stdout,"(v_readtif): 8 bit TIFF\n");
        imagelength =  tifimage->imagelength;
        scanlinesize = TIFFScanlineSize(tifimage->tif_in);
    }
    else if(tifimage->bitspersample == 16)
    {
        fprintf(stdout,"(v_readtif): 16 bit TIFF\n" );
        /* before:
            imagelength =  2* tifimage->imagelength;
        */
        imagelength =  tifimage->imagelength;
        scanlinesize = TIFFScanlineSize(tifimage->tif_in);
    }
    else
    {
        fprintf(stderr,
            "*** error (v_readtif): unknown TIFF format: bitspersample = %d\n",
            tifimage->bitspersample);
        exit(1);
    }

  fprintf(stdout,
          "(v_readtif): length = %ld, width = %ld, scanlinesize = %ld, buffersize = %ld\n", 
          imagelength, tifimage->imagewidth, scanlinesize, imagelength * scanlinesize);

  tifimage->buf = (char *)malloc(imagelength * scanlinesize);
  if(tifimage->buf == NULL) 
  {
    fprintf(stderr,"*** error (v_readtif): memory allocation failed (buffer)\n");
    exit(1);
  }

  for (row = 0; row < tifimage->imagelength; row++)
  {
    TIFFReadScanline( tifimage->tif_in, 
                      tifimage->buf+row * scanlinesize,
                      row, scanlinesize );
  }

#ifdef CONTROL
    fprintf(stdout," * v_readtif: *\n");
    fprintf(stdout,"imagewidth = %d\n", tifimage->imagewidth);
    fprintf(stdout,"imagelength = %d\n", tifimage->imagelength);
    fprintf(stdout,"bitspersample = %d\n", tifimage->bitspersample);
    fprintf(stdout,"compression = %d\n", tifimage->compression);
    fprintf(stdout,"photometricinterpretation = %d\n", tifimage->photometricinterpretation);
    fprintf(stdout,"stripoffsets = %d\n", tifimage->stripoffsets);
    fprintf(stdout,"rowsperstrip = %d\n", tifimage->rowsperstrip);
    fprintf(stdout,"stripbytecounts = %d\n", tifimage->stripbytecounts);
    fprintf(stdout,"xresolution = %d\n", tifimage->xresolution);
    fprintf(stdout,"yresolution = %d\n", tifimage->yresolution);
    fprintf(stdout,"planarconfiguration = %d\n", tifimage->planarconfiguration);
    fprintf(stdout,"resolutionunit = %d\n",tifimage->resolutionunit);
#endif
  
    TIFFClose(tifimage->tif_in);
    
    return 0; /* success */
}
