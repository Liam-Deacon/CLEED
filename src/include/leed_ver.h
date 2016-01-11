/*********************************************************************
 *                           SEARCH_VER.H
 *
 *  Copyright 1994-2015 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2013-2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

#ifndef LEED_VER_H
#define LEED_VER_H

#ifdef LEED_VERSION
#undef LEED_VERSION
#endif

#ifdef LEED_NAME
#undef LEED_NAME
#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/* current symmetry version */
static const char *LEED_VERSION[] = { "cleed_nsym (2014.07.04 - )" };
static const char LEED_NAME[] = { "CLEED_NSYM" };

/* additional program information */
static const char LEED_PROG[] =  {"cleed_nsym"};
static const char LEED_PROG_COPYRIGHT[] = { "Copyright (C) 1994-2014 Georg Held\n" \
"Copyright (C) 2014 Liam Deacon" };
static const char LEED_PROG_LICENSE[] = { "GNU General Public License v3" };
static const char LEED_PROG_VERSION[] = { ".. " };
static const char LEED_PROG_AUTHOR[] = { "Georg Held (g.held@reading.ac.uk)" };
static const char LEED_PROG_MAINTAINER[] = { "Liam Deacon (liam.m.deacon@gmail.com)" }; 
static const char LEED_PROG_SHORTDESC[] = { "Calculate LEED_PROG intensities for a given geometry" };

/* platform information */
#if defined(WIN32) || defined(_WIN32) \
                   || defined(__WIN32) || defined(__WIN32__)
static const char LEED_PROG_PLAT[] = { "Win32" };
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
static const char LEED_PROG_PLAT[] = "Win32_Cygwin";
#elif defined(__ANDROID__)
static const char LEED_PROG_PLAT[] = "Android";
#elif defined(__linux__) || defined(__linux)
static const char LEED_PROG_PLAT[] = "Linux";
#else
static const char LEED_PROG_PLAT[] = "Unix";
#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* LEED_VER_H */
