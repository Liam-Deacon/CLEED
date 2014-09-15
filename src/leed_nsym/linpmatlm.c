/**************************************************************
GH/04.05.2000
file contains function:

     leed_inp_mat_lm

read matrix elements which are specified through their (lm) quantum
numbers and store in natural order.

**************************************************************/
#include<stdio.h>
#include<string.h>
#include<malloc.h>
#include <stdlib.h>
#include <math.h>

#include <mat.h>

/********************************************************************/

   mat leed_inp_mat_lm(mat Mat, int l_max, const char *filename)

/***********************************************************************
   Read in an arry of non diagonal scattering matrices for a singel atom
   or a complex of atoms for each energy.
   These non diagonal t-matrices discribes anisotropic vibritonal
   amplitudes of the atoms.

   input:    l_max        max l quantum number
             *filename    file name of the stored matrix

   output:   Mat          non diagonal T-matrix


   Format of input file:

#
# (optional leading comment lines)
#
<l_max> <energy>
l1 m1 l2 m2 i1 i2  <real part> <imag. part>
l1 m1 l2 m2 i1 i2  <real part> <imag. part>
l1 m1 l2 m2 i1 i2  <real part> <imag. part>
l1 m1 l2 m2 i1 i2  <real part> <imag. part>
l1 m1 l2 m2 i1 i2  <real part> <imag. part>

  .
  .
  .
  .


***********************************************************************/

{

int iaux, iaux1, iaux2;
int i_str;
int l_max_in;
int l1, l2, m1, m2;
int pos_1, pos_2 ;
real eng;
real r_part, i_part;

char linebuffer[STRSZ];

FILE *inp_stream;

/********************************************************************
 Open input file.
********************************************************************/

 if((inp_stream = fopen(filename,"r")) == NULL)
 {
#ifdef ERROR
   fprintf(STDERR,"\n*** error (leed_inp_mat_lm): could not open file \"%s\"\n",
   filename);
#endif
   exit(1);
 } 

/********************************************************************
  Read the first line of the input file which contains the
  energie and the maximum l quantum number (lmax).
********************************************************************/

 while( *fgets(linebuffer, STRSZ, inp_stream) == '#');

 if ( linebuffer == NULL)     /* EOF found */
 {
#ifdef ERROR
   fprintf(STDERR, "\n*** error (leed_inp_mat_lm): unexpected EOF found while reading file \"%s\"\n", filename);
#endif
   exit(1);
 }
   
#ifdef REAL_IS_DOUBLE
 else if( sscanf(linebuffer, "%d %lf", &l_max_in, &eng) < 2)
#endif
#ifdef REAL_IS_FLOAT
 else if( sscanf(linebuffer, "%d %f",  &l_max_in, &eng) < 2)
#endif
 {
#ifdef ERROR
   fprintf(STDERR, 
   "\n*** error (leed_inp_mat_lm): improper input line in file \"%s\":\n%s", 
   filename, linebuffer);
#endif
     exit(1);
 }

#ifdef WARNING
 if (l_max_in > l_max)
   fprintf(STDWAR,
   " * warning (leed_inp_mat_lm): dataset of input file is greater than specified matrix (l_max: input %d, matrix %d)\n",
   l_max_in, l_max);
#endif

 eng = R_sqrt(2* eng);

#ifdef CONTROL
 fprintf(STDCTR, " (leed_inp_mat_lm): l_max_in: %d, eng %f)\n", l_max_in, eng);
#endif


/**********************************************************************
     allocate memory
**********************************************************************/

    iaux = (l_max + 1)*(l_max + 1);
    Mat = matalloc( Mat, iaux, iaux, NUM_COMPLEX);

/**********************************************************************
 Read the next lines and store each matrix element:
 Design:

  (l1, m1)   (l2,m2)   (real part)   (imaginary part)    

  matrixposition 1: (l1 + 1)l1 + m1 + 1 
  matrixposition 2: (l2 + 2)l2 + m2 + 1

**********************************************************************/

 while(fgets(linebuffer,STRSZ,inp_stream) != NULL)
 {
   i_str = 0;
   while(*(linebuffer+i_str) == ' ')i_str++;
#ifdef REAL_IS_DOUBLE
   iaux = sscanf(linebuffer+i_str,"%d %d %d %d %d %d %lf %lf",
#endif
#ifdef REAL_IS_FLOAT
   iaux = sscanf(linebuffer+i_str,"%d %d %d %d %d %d %f %f",
#endif

    &l1, &m1, &l2, &m2, &iaux1, &iaux2, &r_part, &i_part );

    if ((l1 <= l_max) && (l2 <= l_max))
    {
      pos_1 = (l1 + 1)*l1 + m1 + 1;
      pos_2 = (l2 + 1)*l2 + m2 + 1;

      RMATEL(pos_1,pos_2, Mat) = eng * r_part ;
      IMATEL(pos_1,pos_2, Mat) = eng * i_part ;
    }


 }/* Ende while */

 fclose(inp_stream);
   
 return(Mat);

}/* Ende input */
