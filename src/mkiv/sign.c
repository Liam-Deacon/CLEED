/***************************************************************************
GH/27.07.03


         File Name: sign.c

CHANGES:
GH/27.07.03 - include value in parameter list (white).

***************************************************************************/

/**************************************************************************/
#include "mkiv.h"
#include "signs.h"
#include <stdlib.h>
/**************************************************************************/
 
int sign(
      ImageMatrix *image, 
      unsigned char chr,     /* character code */
      int xx,                /* x-coordinate of the upper left corner */
      int yy,                /* y-coordinate of the upper left corner */
      unsigned short value   /* value for white colour */
    )

/***************************************************************************
  SU/4.7.91
  CS/30.7.93    

Purpose:
  sign plots a character (i.e. numbers, fractions, minus and semicolon)

***************************************************************************/
{
    register int j, k, pos;
    int i, cols = image->rows, 
           rows = image->cols;
    unsigned short *im = (unsigned short *)image->imagedata;

/**************************************************************************/

    chr -= 45;
    if (chr > 34 ) /* unsigned so always positive due to wrapping */
        ERR_EXIT((sign): unknown character!);

    pos = cols*yy + xx;
    if ( xx<cols && yy<rows-7 )
        for ( k=0, i=0; i<11; i++, pos+=cols-7 )
            for ( j=0; j<7; j++, k++, pos++)
                im[pos] = a[chr][k] * value;
    
    return 0;
}
/**************************************************************************/
