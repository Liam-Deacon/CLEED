/***********************************************************************
 GH/20.09.95
***********************************************************************/

#include <stdio.h>
#include <strings.h>
#include <math.h>
#include "csearch.h"

#define CONTROL
#define ERROR

#define EXIT_ON_ERROR

#ifdef EXIT_ON_ERROR
#define OPEN_ERROR(x)      fprintf(STDERR,                      \
        "*** error (SEARCH): could not open file \"%s\"\n", x); \
        exit(1)
#else
#define OPEN_ERROR(x)      fprintf(STDERR,                      \
        "*** error (SEARCH): could not open file \"%s\"\n", x)
#endif

#define NUM_RAN    1000
#define NUM_HIST   100


/**********************************************************************/

main(int argc, char *argv[])
{

long idum;

int  i, hist[NUM_HIST];
real faux1, faux2;
real fac;

 fac = 1.;
 if(argc > 1) fac = (real) atof(argv[1]);

 for(i = 0; i < NUM_HIST; i++) hist[i] = 0;

 idum = -1;
 for(i = 0; i < NUM_RAN; i++)
 {
   faux1 = ran1(&idum);
   faux2 = fac * R_log(faux1);
   printf("%d %f\n", i, faux2);
   *(hist + (int)(NUM_HIST * faux1) ) += 1;
 }

/*
 for(i = 0; i < NUM_HIST; i++) printf("%d %f\n", i, (real)hist[i]/NUM_RAN);
*/

}  /* end of main */

/**********************************************************************/
