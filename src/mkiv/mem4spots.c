
/**************************************************************************<
    >
    >         File Name: mem4spots.c
    >
>**************************************************************************/

/**************************************************************************/
#include "mkiv.h"
#include <stdlib.h>
/**************************************************************************/
 
struct spot *mem4spots(
                int *spot_max, 
                Vector a[], 
                int ndom, 
                Domain *superlat, 
                ImageMatrix *image
            )

/***************************************************************************
  CS/1.9.93     

 Purpose:
  mem4spots allocates memory for the struct spot -array. The amount of
  needed memory is determined by the formula :
    memsize = sizeof(struct spot) * sum( sizeof(screen)/sizeof(bases) )

 Input:
  a         substrat basic vectors
  ndom      number of domains
  superlat  superstructure basic vectors 
  image     LEED-image 

 Output:
  spot_max  number of array members for which memory was allocated
  spot      pointer to the spot structure array

***************************************************************************/
{
   struct spot *spot;
   int i;
   long mem_size;
   float base_area;
   long screen_area = (int)( PI/4. * image->rows * image->cols );

/**************************************************************************/

/* calculate number of substrate spots within screen */

   base_area = fabs( a[1].xx * a[2].yy - a[2].xx * a[1].yy );
   *spot_max = RNDUP( (float)screen_area / base_area );

/* calculate number of superstructure spots within screen */

   for ( i=0; i<ndom; i++)
   { 
      base_area = fabs( a[1].len * superlat[i].lind11 *
		        a[2].len * superlat[i].lind22 -
		        a[1].len * superlat[i].lind12 *
			a[2].len * superlat[i].lind21 );
      *spot_max += RNDUP( (float)screen_area / base_area );
   }

/* allocate memory */

/* changed ..... */
   *spot_max = 1000;
   mem_size = *spot_max * sizeof( struct spot );
   if ( (spot = (struct spot *) malloc( mem_size ) ) == NULL )
      ERR_EXIT(mem4spots : Not enough memory for spot structure array);
   return(spot);
}
/**************************************************************************/
