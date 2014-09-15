/**************************************************************************
     
              File Name: markref.c
     
 **************************************************************************/

/**************************************************************************/
#include "mkiv.h"
#include <stdlib.h>
#include <stdio.h>
/**************************************************************************/
 
int mark_reflex(int nspot, Spot spot[], ImageMatrix *image, float thick, 
                float radius, int color, int ind, char *fname)

/***************************************************************************
  SU/02.07.91 
  GH/07.08.92
  CS/30.7.93    
  VJ/29.01.03
  GH/25.05.05 - adjust norm.
  LD/16.06.14 - ImageMatrix update
  
 Purpose:
  mark_reflex draws a circle around each measurable reflex position
  and plots the corresponding indices above each reflex if desired.
  ind < 0 : don't draw indices, no writing to ima.byte
  ind = 0 : no indices, write to ima.byte
  ind > 0 : draw indices and write to ima.byte



ImageMatrix *image;                      // image matrix structure             
int nspot;                               // length of list                
struct spot spot[];                      // list of reflexes              
float thick, radius;                     // thickness, radius and         
int color,                               // greylevel of circle line      
    ind;                                 // if 1: draw indices            
char *fname;                             // filename to write output e.g. 
                                            'ima.byte'
    
***************************************************************************/
{
    register int k, i, h, v, pos, nncoo; /* number of circle coordinates  */
    int ncoo;                            /* max number of circle coordinates */
    struct coord *coo;                   /* coordinates of circle line    */
    float norm, rad;                     /* radius & color of circle line */
    int  cols, rows, n_size;

    unsigned short *im;
    unsigned short val, max_val;
     
/**************************************************************************/

    cols = image->rows;
    rows = image->cols;
    im = (unsigned short *)image->imagedata;
  
    /* find maximum value in image->imagedata */

    n_size = cols * rows;
    max_val = 1;

    for( i = 0; i < n_size; i ++) 
        max_val = MAX(max_val, im[i]);

    norm = (float) max_val / 256.;

    /* allocate space for the coordinate structure */
    ncoo = 10 + 10 * radius * thick;
    coo = (struct coord *) calloc( ncoo , sizeof(struct coord) );
    if ( coo == NULL )
        ERR_EXIT(markref: No space for array coord -> calloc failed!);
	
    /* calculate coordinates */
    nncoo =0;
    for (h= -(int)radius; h<= (int)radius; h++) {
        for (v= -(int)radius; v<= (int)radius; v++) {
            rad = PYTH(h,v);
            if ( rad>radius || rad<radius-thick ) continue;
            coo[nncoo].xx = h;
            coo[nncoo].yy = v;
            if ( ++nncoo >= ncoo ) ERR_EXIT(too many ncoo...)
        }
    }

    /* draw circles around spots */

    for (k = 0;k < nspot;k++) {
        h = (int) spot[k].x0;
        v = (int) spot[k].y0;
        val = (unsigned short)(100. * norm);
        
        if ( spot[k].control & SPOT_DESI) 
            val = (unsigned short)(180. * norm); /* desired */
        if ( spot[k].control & SPOT_REF ) 
            val = (unsigned short)(255. * norm); /* reference */
        
        if (!(spot[k].control & SPOT_GOOD_S2N)) 
            val = (unsigned short)(100. * norm); /* bad s/n */

        if ( spot[k].control & SPOT_OUT ) 
            val = (unsigned short)(100. * norm); /* touched bounds.*/
        if ( spot[k].control & SPOT_EXCL) 
            val = (unsigned short)(60. * norm);  /* excluded */

        for (i=0;i < nncoo;i++) {
            pos = cols*(v+coo[i].yy) +h+coo[i].xx;
            if ( pos>0 && pos<cols*rows )
                im[pos] = val;
        }
    }

    /* plot indices */
    if(ind>0) plot_indices(image,nspot,spot);

    /* Convert internal matrix into TIFF parameters so that XV is able to display 
       them and write the new data into output TIFF file */

    if (strlen(fname) == 0)
    {
        realloc(fname, sizeof(char)*FILENAME_MAX);
        strcpy(fname, "ima.byte");
    }  
    
    if(ind>=0)
        if (out_tif(image, fname))
            ERR_EXIT(out_tif failed!);

    free (coo);
    
    return 0;
}
/***************************************************************************/
