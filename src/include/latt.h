/*********************************************************************
 *                           LATT.H
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/29.04.14 - Added header for latt.c
**********************************************************************/

#ifndef LATT_H
#define LATT_H

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "gh_stddef.h"
#include "atom.h"
#include "coord.h"
#include "basis.h"
#include "miller_index.h"
#include "lattice.h"

/* additional program information */
#define LATT "latt"
#define LATT_COPYRIGHT "Copyright (C) 1994-2014 Georg Held\n" 	            \
"Copyright (C) 2013-2014 Liam Deacon"
#define LATT_LICENSE "GNU General Public License v3"
#define LATT_VERSION "2014-07-04 Development"
#define LATT_AUTHOR "Georg Held (g.held@reading.ac.uk)"
#define LATT_MAINTAINER "Liam Deacon <liam.deacon@diamond.ac.uk>" 
#define LATT_SHORTDESC "Lattice generator"

/* platform information */
#if defined(WIN32) || defined(_WIN32) \
                   || defined(__WIN32) || defined(__WIN32__)
#define LATT_PLAT "Win32"
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
#define LATT_PLAT "Win32_Cygwin"
#elif defined(__ANDROID__)
#define LATT_PLAT "Android"
#elif defined(__linux__) || defined(__linux)
#define LATT_PLAT "Linux"
#elif defined(__unix__) || defined(__unix)
#define LATT_PLAT "Unix"
#else
#define LATT_PLAT "Unknown"
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

#define ARG_PARSE_INT(x)                                                 	  \
if (i_arg < argc)                                                           \
{                                                                           \
  i_arg++;                                                                  \
  x = (int) atoi(argv[i_arg]);                                              \
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

#define ARG_PARSE_INT(x)                                                    \
if (i_arg < argc)                                                           \
{                                                                           \
  i_arg++;                                                                  \
  x = (int) atoi(argv[i_arg]);                                              \
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
