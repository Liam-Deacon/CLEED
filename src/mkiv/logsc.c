/***************************************************************************/
#include "mkiv.h"
#include "string.h"
#include "math.h"
#include "vinclude.h"

#define MAX_CHAR  256


/***************************************************************************
GH/03.09.92
***************************************************************************/

main(int argc, char *argv[])
{
/* type definitions */
     ImageMatrix *image,           /* resulting LEED-image in matrix format */
                 *readimage();     /* reading routine for matrix images     */
     char *in_file;                /* filename of first leed image          */
     char out_file[STRSZ];         /* filename of first leed image          */
     char string[STRSZ];

     int i;
     
     int cols, rows;
    
     short *pixel, *ip;            /* pointer to short image data */
     unsigned char  *ic;           /* pointer to byte image data */

     short min, max;
   
     long histo[MAX_CHAR];
     long npixel, lowpixel;
   
     double fac, d_val;
    

/* decode arguments */

 switch (argc)
 {
  case(1):
  {
   printf("*** error: missing argument (input file)\n");
   exit(1);
   break;
  }
  case(2):
  {
   in_file = argv[1];
   strcpy(out_file, argv[1]);
   strcat(out_file,".log");
   break;
  }
  case(3):
  {
   in_file = argv[1];
   strcpy(out_file, argv[2]);
   strcat(out_file,".log");
   break;
  }
  default:
  {
   printf("*** error: wrong number of arguments\n");
   exit(1);
   break;
  }
 }  /* switch */

/* Read image from file */
     image = readimage(in_file);

     if ( image == NULL)
     {
          fprintf(stderr,"ERROR:\tcould not read image!");
          exit(1);
     }

     if (writeimage("xtest.img",image) != 1)
     {
          fprintf(stderr,"*** Error!\n\twriteimage failed!");
          exit(1);
     }
    
     rows = image->cols;
     cols = image->rows;
     npixel = (long)(cols*rows);

     printf("Number of pixels = %ld \n", npixel);
     pixel = (short*)(image->imagedata);
    
/* find minimum and maximum */
     for (max = *pixel, min = *pixel, ip = pixel;
          ip < (pixel+npixel); ip ++)
     {
      if (*ip < min) min = *ip;
      if (*ip > max) max = *ip;
     }
/*     printf("min = %d; max = %d\n", min, max); */

/* make histogram */

     if(max > MAX_CHAR) fac = MAX_CHAR / (double)max;
     else               fac = 1.;

     for (ip = pixel; ip < (pixel+npixel); ip ++)
     {
      i =  (int)(*ip * fac);
      histo[i] ++;
     }
/* find pixel value equivalet to lower quater in histogram */
     for (i = 0; lowpixel < npixel/4; i++)
     {
      lowpixel += histo[i];
     }

/* calculate log and store as bytes */
     min = (int)(i*fac) - 1;
     fac = 255./log( (double)(max-min) );
     for (ip = pixel, ic = (unsigned char *)pixel;
          ip < (pixel+npixel); 
          ip ++, ic ++)
     {
      d_val = (double)(*ip - min);
      if (d_val < 1.) *ic = 0;
      else            *ic = (unsigned char) (fac * log(d_val) );
     }

/* store data as single bytes */
     image->data_storage_type = VFF_TYP_1_BYTE;

     if (writeimage(out_file,image) != 1)
     {
          fprintf(stderr,"*** Error!\n\twriteimage failed!");
          exit(1);
     }
     freeimage(image);

     sprintf(string, "putimage -i %s", out_file);
     system (string);
}

/***************************************************************************/


