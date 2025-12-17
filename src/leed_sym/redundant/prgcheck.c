/*********************************************************************
  GH/02.09.97
  file contains function:

  double leed_cpu_time(void)

 Check elapsed cpu time and used memory

Changes:
GH/24.07.95 - Creation
GH/02.09.97 - Add hostname

*********************************************************************/

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>

#if defined(WIN32) || defined(_WIN32)|| \
defined(__WIN32__) || defined(__MINGW__) || defined(_WIN64)

/* alternative headers for WINDOWS */
#include "getrusage_win32.h"
#include <windows.h>

#else

/* use LINUX headers */
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#endif


#include "gh_stddef.h"

/*
#define CONTROL
#define WARNING
#define ERROR
*/

#define CONTROL

#ifndef MBYTE
#define MBYTE 1048576
#endif

double leed_cpu_time(FILE * outp, const char * message)

/************************************************************************

 Check elapsed cpu time.
 
INPUT:

 FILE * outp 1: write cpu time, elapsed cpu time since last call and memory
               requirements to the stream specified by outp.
             NULL: suppress output.

 char * message - message written to outp together with cpu information.
 
FUNCTION CALLS:
  getrusage

RETURN VALUE:

 cpu time elsapsed since last call (in seconds).

*************************************************************************/
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
#if defined(WIN32) || defined(_WIN32)|| \
defined(__WIN32__) || defined(__MINGW__) || defined(_WIN64)
   {
     DWORD hostname_size = (DWORD)STRSZ;
     if (!GetComputerNameA(hostname, &hostname_size))
     {
       strncpy(hostname, "unknown", STRSZ);
       hostname[STRSZ - 1] = '\0';
     }
   }
#else
   gethostname(hostname, STRSZ);
#endif
 }

 getrusage ( RUSAGE_SELF, r_usage );

 new_secs = (double)r_usage->ru_utime.tv_sec + 
        (double)r_usage->ru_utime.tv_usec * 1.e-6;

 if(outp != NULL)
 {
   fprintf(outp,"%s\t", message);
   fprintf(outp,
     "(leed_cpu_time) total time on %s: %10.6f s, diff: %10.6f s\n", 
                hostname, new_secs, new_secs-old_secs);
 }
 old_secs = new_secs;
 return(new_secs - old_secs);
}  /* end of function leed_cpu_time */

/*************************************************************************/
