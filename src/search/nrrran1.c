/*********************************************************************
 * <FILENAME>
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *20.09.95
file contains function:

  real ran1(long *idum)

 random number generator of Park and Miller (From Num. Rec.)

 Changes:
NR/20.09.95 - Copy from Num. Rec. (type in from book !!!)
***********************************************************************/
#include "nrr.h"
#include <math.h>

enum { NTAB = 32 };
static const int IA = 16807;
static const int IM = 2147483647;
static const double AM = (1.0/2147483647);
static const int IQ = 127773;
static const int IR = 2836;
static const double NDIV = (1+(2147483647-1)/32);
static const double EPS = 1.2e-7;
static const double RNMX = (1.0 - 1.2e-7);

real ran1(long *idum)

/************************************************************************

 Minimal random number generator by Park and Miller with Bays-Durham 
 shuffle and added safeguards. Returns a uniform random deviate between
 0.0 and 1.0 (exclusive of the endpointvalues). 

 Call with idum a negative integer to initialize; thereafter do not 
 alter idum inbetween successive deviates in a sequence. RNMX should 
 approximate the largest floating value that is less than 1.0.

************************************************************************/

{
  int j;
  long k;
  static long iy = 0;
  static long iv[NTAB];
  real temp;

  /* Initialize */
  if (*idum < 0 || !iy)
  {
    /* Be sure to prevent idum = 0 */
    if( -(*idum) < 1) *idum = 1;
    else              *idum = -(*idum);
   
    /* Load shuffle table after 8 warm ups */
    for( j = NTAB+7; j >= 0; j --)
    {
      k = (*idum)/IQ;
      *idum = IA * (*idum - k*IQ) - IR*k;
      if(*idum < 0) *idum += IM;
      if(j < NTAB) iv[j] = *idum;
    }

    iy = iv[0];
  }
 
  /* Start here when not initializing: */
  k = (*idum)/IQ;

  /*Compute idum = (IA*idum) % IM  without overflows by Schrage's method */
  *idum = IA * (*idum - k*IQ) - IR*k;
  if(*idum < 0) *idum += IM;

  /* j will be in the range [0..NTAB-1]
   * output previously stored value and refill the shuffle table
   */
  j = (int)round(iy / NDIV);
  iy = iv[j];
  iv[j] = *idum;
 
  /* avoid endpoint values */
  if( (temp=AM*iy) > RNMX) return(RNMX);
  else                     return(temp);

} /* end of function ran1 */
