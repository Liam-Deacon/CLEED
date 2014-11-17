/*********************************************************************
 *                      PRGCHECK.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/24.07.95 - Creation
 *   GH/02.09.97 - Add hostname
 *   LD/04.06.13 - Add windows headers
 *********************************************************************/

/*! \file
 *
 * Implements leed_cpu_time() function to count seconds since last call.
 *
 * \note "getrusage_win32.c" is required when compiling on Windows systems as
 * the rusage \c struct is not defined on Visual Studio or on MinGW (but
 * is when using Cygwin).
 *
 */

#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#if defined(WIN32) || defined(_WIN32) || \
defined(__WIN32__) || defined(__MINGW__) || defined(_WIN64) 

/* alternative headers for WINDOWS */
#include "getrusage_win32.h"
#include <Winsock2.h>

#else

/* use LINUX headers */
#include <sys/time.h>
#include <sys/resource.h>

#endif

#include "gh_stddef.h"

#ifndef MBYTE
#define MBYTE 1048576
#endif

/*!
 * Checks elapsed cpu time.
 *
 * \param[in] outp File pointer for output stream with output detailing
 * the elapsed cpu time since last call and memory requirements. Note that
 * \p outp can be \c NULL to suppress output.
 * \param[in] message Message written to \p outp together with cpu information.
 * \return cpu time elasped since last call (in seconds).
 */
double leed_cpu_time(FILE * outp, const char * message) 
{
  double new_secs;
  static double old_secs = 0.;
  static struct rusage *r_usage = NULL;  /* stucture defined in sys/resource.h
                                            and sys/time.h (timeval) */
  static char *hostname;

  if (r_usage == NULL)
  {
    r_usage = (struct rusage *) malloc (sizeof(struct rusage));
    hostname = (char *) malloc (STRSZ * sizeof(char));
    gethostname(hostname, STRSZ);
  }

  getrusage ( RUSAGE_SELF, r_usage );

  new_secs = (double)r_usage->ru_utime.tv_sec +
             (double)r_usage->ru_utime.tv_usec * 1.e-6;

  if(outp != NULL)
  {
    fprintf(outp, "%s\t", message);
    fprintf(outp, "(leed_cpu_time) total time on %s: %10.6f s, "
            "diff: %10.6f s\n", hostname, new_secs, new_secs-old_secs);
  }
  old_secs = new_secs;

  return(new_secs - old_secs);

}  /* end of function leed_cpu_time */
