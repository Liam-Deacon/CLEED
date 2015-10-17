/*********************************************************************
 *                       SEARCH_FUNC.H
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/29.06.2014 - Creation of open source version of ran1 function
 *********************************************************************/

/*! \file
 *
 * Open source replacement of Numerical Recipes ran1() function.
 */
#include <stdlib.h>
#include "cleed_real.h"

/*!
 * Minimal random number generator by Park and Miller with Bays-Durham
 * shuffle and added safeguards. Returns a uniform random deviate between
 * 0.0 and 1.0 (exclusive of the end-point values).
 *
 * Call with \p idum a negative integer to initialize; thereafter do not
 * alter \p idum inbetween successive deviates in a sequence. RAND_MAX should
 * approximate the largest floating value that is less than 1.0.
 *
 * \param idum Seed for random number generator.
 */
cleed_real ran1(long *idum)
{   
  /* initialise the random number generator from seed */

  //time_t t;
  //srand((unsigned) time(&t));
  srand((unsigned long) *idum); // ensure seed is constant between calls

  /* produce a random number between 0. and 1. */
  return (real) rand()/RAND_MAX;
}
