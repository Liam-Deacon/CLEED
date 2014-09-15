/****************************************************************************
                        LATT.H (Georg Held 01.05.06)
                        
Changes:
 LD/29.04.14 - Added header for latt.c                 
         
****************************************************************************/
#ifndef LATT_H
#define LATT_H

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "atom.h"
#include "coord.h"
#include "basis.h"
#include "miller_index.h"
#include "lattice.h"

/* additional program information */
#define PROG "latt"
#define PROG_COPYRIGHT "Copyright (C) 1994-2014 Georg Held\n" \
"Copyright (C) 2013-2014 Liam Deacon"
#define PROG_LICENSE ""
#define PROG_VERSION "-- "
#define PROG_AUTHOR "Georg Held (g.held@reading.ac.uk)"
#define PROG_MAINTAINER " <>" 
#define PROG_SHORTDESC "Lattice generator"

/* platform information */
#if defined(WIN32) || defined(_WIN32) \
                   || defined(__WIN32) || defined(__WIN32__)
#define PROG_PLAT "Win32"
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
#define PROG_PLAT "Win32_Cygwin"
#elif defined(__ANDROID__)
#define PROG_PLAT "Android"
#elif defined(__linux__) || defined(__linux)
#define PROG_PLAT "Linux"
#elif defined(__unix__) || defined(__unix)
#define PROG_PLAT "Unix"
#else
#define PROG_PLAT "Unknown"
#endif

/* define constants */
#define INVALID_ARGUMENT_ERROR -1

/* preprocessor macros */

#ifndef ARG_IS
#define ARG_IS(x) (strcmp(argv[i_arg], x) == 0)
#endif

#ifndef ARG_ISH
#define ARG_ISH(x,y) (strncmp(argv[i_arg], x, y) == 0)
#endif

#ifdef DEBUG

#define ARG_PARSE_FLOAT(x)                                                  \
if (i_arg < argc)                                                           \
{                                                                           \
  i_arg++;                                                                  \
  x = (float) atof(argv[i_arg]);                                            \
  continue;                                                                 \
}                                                                           \
else                                                                        \
{                                                                           \
  fprintf(stderr, "*** error (latt): invalid argument '%s' for '%s'\n",     \
          argv[i_arg], argv[i_arg-1]);                                      \
  fprintf(stderr, "debug info: argc = %i; i_arg = %i\n", argc, i_arg);      \
  fprintf(stderr, "\n"                                                      \
          "===========================================================\n"   \
          "i\t|\targv[i]\n"                                                 \
          "-----------------------------------------------------------\n"); \
  for (i_arg = 1; i_arg < argc; i_arg++)                                    \
    fprintf(stderr, "%i\t|\t%s\n", i_arg, argv[i_arg]);                     \
  exit(INVALID_ARGUMENT_ERROR);                                             \
}

#define ARG_PARSE_INT(x)                                                  \
if (i_arg < argc)                                                           \
{                                                                           \
  i_arg++;                                                                  \
  x = (int) atoi(argv[i_arg]);                                            \
  continue;                                                                 \
}                                                                           \
else                                                                        \
{                                                                           \
  fprintf(stderr, "*** error (latt): invalid argument '%s' for '%s'\n",     \
          argv[i_arg], argv[i_arg-1]);                                      \
  fprintf(stderr, "debug info: argc = %i; i_arg = %i\n", argc, i_arg);      \
  fprintf(stderr, "\n"                                                      \
          "===========================================================\n"   \
          "i\t|\targv[i]\n"                                                 \
          "-----------------------------------------------------------\n"); \
  for (i_arg = 1; i_arg < argc; i_arg++)                                    \
    fprintf(stderr, "%i\t|\t%s\n", i_arg, argv[i_arg]);                     \
  exit(INVALID_ARGUMENT_ERROR);                                             \
}


#else

#define ARG_PARSE_FLOAT(x)                                                  \
if (i_arg < argc)                                                           \
{                                                                           \
  i_arg++;                                                                  \
  x = (float) atof(argv[i_arg]);                                            \
  continue;                                                                 \
}                                                                           \
else                                                                        \
{                                                                           \
  fprintf(stderr, "*** error (latt): invalid argument for '%s'\n",          \
          argv[i_arg-1]);                                                   \
  exit(INVALID_ARGUMENT_ERROR);                                             \
}

#define ARG_PARSE_INT(x)                                                  \
if (i_arg < argc)                                                           \
{                                                                           \
  i_arg++;                                                                  \
  x = (int) atoi(argv[i_arg]);                                            \
  continue;                                                                 \
}                                                                           \
else                                                                        \
{                                                                           \
  fprintf(stderr, "*** error (latt): invalid argument for '%s'\n",          \
          argv[i_arg-1]);                                                   \
  exit(INVALID_ARGUMENT_ERROR);                                             \
}

#endif
                                        
#endif /* _LATT_H */
