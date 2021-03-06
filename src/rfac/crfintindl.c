/********************************************************************
GH/30.08.97

Changes:
GH/29.09.92 - Creation
GH/30.08.95 - Adaption to CRFAC
GH/30.08.97 - type = void
********************************************************************/
#include <stdio.h>
#include <strings.h>    /* for strlen */
#include <string.h>
#include <math.h>

#include "crfac.h"       /* rf specific definitions */

#ifdef REAL_IS_DOUBLE
#define REAL_FORM "%lf"
#endif
#ifdef REAL_IS_FLOAT
#define REAL_FORM "%f"
#endif

void cr_intindl( char *command_line, struct rfspot *beam, int n_beam)

/********************************************************************

interpreter for averaging different beams from theoretical input
file.
A scaling factor will be assigned to beams which should be averaged 
(scaling factor : beam.f_val1).

Syntax:
   (<index1>,<index2>) {*<scale> +/- (<index1>,<index2>) * <scale> }

input: command_line,
       *beam list of beam indices from input file
       n_beams  number of beams

output: beam.f_val1: scaling factor for averging over beams.
	Can be 0 (ignore) or a number.

********************************************************************/
{
int i,j;

real test1, test2;
real scale;

/*
 reset all scaling factors in list beam
*/
 for (j=0; j < n_beam; j++)
 {
  (beam+j)->f_val1 = 0.;
 }

 i = 0;
 scale = 1.0;
 test1 = FEND_OF_LIST;
 test2 = FEND_OF_LIST;

 do
 {
  while ( (command_line[i] != '(') && (command_line[i] != '\0') )
	  i++;

/* index */  
  if(command_line[i] == '(')
  {
   i++;
   sscanf(command_line+i, REAL_FORM, &test1);
   while (command_line[i] != ',') i++;
   i++;
   sscanf(command_line+i, REAL_FORM, &test2);
   while (command_line[i] != ')') i++;
   i++;
  }

  while ( (command_line[i] != '+') && (command_line[i] != '-') &&
          (command_line[i] != '*') && (command_line[i] != '\0') )
	  i++;

/* postscript scale factor */  
  if(command_line[i] == '*')
  {
   i++;
   if(scale < 0. )
   {
    sscanf(command_line+i, REAL_FORM, &scale);
    scale *= -1.;
   }
   else sscanf(command_line+i, REAL_FORM, &scale);
   /* scip over possible signs of scaling factor */
   while ( (command_line[i] < '0') || (command_line[i] > '9') ) i++;
  }

/* 
 check compatibility with input indices;
 clear scale.
*/

  for (j=0; ( ( R_fabs( (beam+j)->index1 - test1) + 
                R_fabs( (beam+j)->index2 - test2) ) > IND_TOLERANCE) 
	    && (j < n_beam); j++)   ;

  if (j < n_beam)
  {
   (beam+j)->f_val1 = scale;
#ifdef PRINT  /* control output, if required */
   printf(">rf_intindl: beam:(%5.2f,%5.2f) scale: %5.2f\n",
	  (beam+j)->index1, (beam+j)->index2, (beam+j)->f_val1);
#endif
  }
  else if(test1 > FEND_OF_LIST)
   printf(">rf_intindl: Could not find (%5.2f,%5.2f) in input file\n",
	     test1, test2);
  else 
   printf(">rf_intindl: Error in commandline\n");

/* reset scale and test1/2 for next beam */
  test1 = FEND_OF_LIST;
  test2 = FEND_OF_LIST;
  scale = 1.0;

  while ( (command_line[i] != '+') && (command_line[i] != '-') 
       && (command_line[i] != '\0') )
	  i++;

/* signs */
  if(command_line[i] == '+')
  {
   i++;
  }
  if(command_line[i] == '-')
  {
   scale = -1.;
   i++;
  }

 }  /* do */
 while(i < (int) strlen(command_line) );

}
