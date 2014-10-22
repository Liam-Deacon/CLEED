/***********************************************************************
LD/29.06.2014
file contains function:

  real ran1(long *idum)

 random number generator of Park and Miller using minstd 
 function from GSL

 Changes:
LD/29.06.2014 - Creation of open source version of ran1 function
***********************************************************************/
#include "real.h"
#include <stdlib.h>

real ran1_gsl(long *idum)

/************************************************************************

 Minimal random number generator by Park and Miller with Bays-Durham 
 shuffle and added safeguards. Returns a uniform random deviate between
 0.0 and 1.0 (exclusive of the endpointvalues). 

 Call with idum a negative integer to initialize; thereafter do not 
 alter idum inbetween successive deviates in a sequence. RNMX should 
 approximate the largest floating value that is less than 1.0.

************************************************************************/
{   
    /* initialise the random number generator from seed */
    //time_t t;
    //srand((unsigned) time(&t));
    srand((unsigned long) *idum); 
    
    /* produce a random number between 0. and 1. */
    return (real) rand()/RAND_MAX;
}
